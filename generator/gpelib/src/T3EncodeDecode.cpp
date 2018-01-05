#include "T3EncodeDecode.h"
#include "GPE_base.h"
#include "FCS_HASH.h"
#include <algorithm>
#include <math.h>
#include <vector>

#include <string.h>
#include <memory.h>


#define HASHBITS     24
#define ITERATIONS   ((unsigned int)12)
#define No           ((float)1.0f)
CT3Coder::CT3Coder(int nNumberOfBits)
{
	m_MessageSizeBit=nNumberOfBits;
    m_N=nNumberOfBits+HASHBITS;

	memset(m_Output0,0,MACHINE_STATES_COUNT*sizeof(m_Output0[0]));
    memset(m_Output1,0,MACHINE_STATES_COUNT*sizeof(m_Output1[0]));
    memset(m_FromState0,0,MACHINE_STATES_COUNT*sizeof(m_FromState0[0]));
    memset(m_FromState1,0,MACHINE_STATES_COUNT*sizeof(m_FromState1[0]));
    memset(m_ToState0,0,MACHINE_STATES_COUNT*sizeof(m_ToState0[0]));
    memset(m_ToState1,0,MACHINE_STATES_COUNT*sizeof(m_ToState1[0]));

	m_InterleaveArray.resize(m_N);
    m_DeinterleaveArray.resize(m_N);
    CreateInterleave(m_N);
    CreateEncodeTable();
}
CT3Coder::~CT3Coder(void)
{
}
void CT3Coder::CreateInterleave(int nSize)
{
	unsigned int uFlagIdx=0;
	GPEBASE::CGpeRND RND(-1);
    double dSize=(double)nSize;
    std::vector<bool> FlagArr(nSize);
    for(unsigned int i=0;i<nSize;i++)
    {
		do
        {
			double dRndReal=RND.RandReal();
            uFlagIdx=(unsigned int)((double)dRndReal*dSize);
        } while(FlagArr[uFlagIdx]);
        FlagArr[uFlagIdx]=true;
        m_InterleaveArray[i]=uFlagIdx;
        m_DeinterleaveArray[uFlagIdx]=i;
    }
}
void CT3Coder::CreateEncodeTable(void)
{
	bool bFlagArr1[4];
    bool bFlagArr2[4];

	for(int i=0;i<2;i++)
    {
		for(int c=0;c<MACHINE_STATES_COUNT;c++)
        {
			bool flag=!(i==0);
            IntToBool(c,bFlagArr1,4);
            
			bFlagArr2[3]=bFlagArr1[2];
            bFlagArr2[2]=bFlagArr1[1];
            bFlagArr2[1]=bFlagArr1[0];
            bFlagArr2[0]=Add(Add(flag,bFlagArr1[0]),bFlagArr1[3]);

            if(i==0)
            {
				m_Output0[c]=Add(flag,bFlagArr1[0]);
                m_Output0[c]=Add(m_Output0[c],bFlagArr1[3]);
                m_Output0[c]=Add(m_Output0[c],bFlagArr1[1]);
                m_Output0[c]=Add(m_Output0[c],bFlagArr1[2]);
                m_Output0[c]=Add(m_Output0[c],bFlagArr1[3]);
                
				BoolToInt(bFlagArr2,4,m_ToState0[c]);
				m_FromState0[m_ToState0[c]]=c;
            }
            else
            {
				m_Output1[c]=Add(flag,bFlagArr1[0]);
                m_Output1[c]=Add(m_Output1[c],bFlagArr1[3]);
                m_Output1[c]=Add(m_Output1[c],bFlagArr1[1]);
                m_Output1[c]=Add(m_Output1[c],bFlagArr1[2]);
                m_Output1[c]=Add(m_Output1[c],bFlagArr1[3]);
                
				BoolToInt(bFlagArr2,4,m_ToState1[c]);
                m_FromState1[m_ToState1[c]]=c;
            }
		}
	}
}
// ******************************************************************************************
// Вспомогательные примитивы
// ******************************************************************************************
std::vector<bool> CT3Coder::ByteArrayToBooleanVector(unsigned char* pData,int nSize)
{
	std::vector<bool> Out;
	for(int i=0;i<nSize;i++)
	{
		Out.push_back((pData[i]&0x80)!=0);
		Out.push_back((pData[i]&0x40)!=0);
		Out.push_back((pData[i]&0x20)!=0);
		Out.push_back((pData[i]&0x10)!=0);
		Out.push_back((pData[i]&0x08)!=0);
		Out.push_back((pData[i]&0x04)!=0);
		Out.push_back((pData[i]&0x02)!=0);
		Out.push_back((pData[i]&0x01)!=0);
	}
	return Out;
}
void CT3Coder::IntToBool(int nState,bool* pArray,int nSize)
{
	for(int i=0;i<nSize;i++)
    {
		int nTmp=(nState>>1);
		if((nTmp<<1)==nState) pArray[i]=false;
		else                  pArray[i]=true;
		nState=nTmp;
    }
}
void CT3Coder::BoolToInt(bool* pArray,int nSize,int& nState)
{
	nState=0;
    for(int i=0;i<nSize;i++)
    {
		if(pArray[i]) nState|=(1<<(i&31));
    }
}
bool CT3Coder::Add(bool a,bool b)
{
	return (a!=b);
}
std::vector<unsigned char> CT3Coder::BoolArraytoByteArray(std::vector<bool> Msg,int nOffset,int nCount)
{
	std::vector<unsigned char> bArr(((Msg.size()/8)+((nCount%8)>0?1:0)));
    int i1=0;
    int i2=std::min<int>(Msg.size(),nOffset+nCount);
    int i3=nOffset;

    while(i3<(nOffset+nCount))
    {
		unsigned char b=0;
        if((i3<i2)&&Msg[i3]) b+=128;
        i3++;
        if((i3<i2)&&Msg[i3]) b+=64;
        i3++;
        if((i3<i2)&&Msg[i3]) b+=32;
        i3++;
        if((i3<i2)&&Msg[i3]) b+=16;
        i3++;
        if((i3<i2)&&Msg[i3]) b+=8;
        i3++;
        if((i3<i2)&&Msg[i3]) b+=4;
        i3++;
        if((i3<i2)&&Msg[i3]) b+=2;
        i3++;
        if((i3<i2)&&Msg[i3]) b++;
        i3++;
        
		bArr[i1]=b;
        i1++;
	}
    return bArr;
}
bool CT3Coder::Decode(float* pMesg,std::vector<float>& Parity1,std::vector<float>& Parity2,int nSize,bool** ppDecodedMsg)
{
	/**ppDecodedMsg=NULL;

	// ******************************************************************************************
	// 1
	// ******************************************************************************************
	std::vector<std::vector<float>> fArr7;
	std::vector<std::vector<float>> fArr8;
	std::vector<std::vector<float>> fArr6;

	fArr7.resize(nSize);
	for(int i=0;i<nSize;i++) fArr7[i].resize(MACHINE_STATES_COUNT);

	fArr8.resize(nSize);
	for(int i=0;i<nSize;i++) fArr8[i].resize(MACHINE_STATES_COUNT);

	fArr6.resize(nSize);
	for(int i=0;i<nSize;i++) fArr6[i].resize(MACHINE_STATES_COUNT);
	// ******************************************************************************************
	// 2
	// ******************************************************************************************
	std::vector<std::vector<float>> fArr15;
	std::vector<std::vector<float>> fArr12;
	std::vector<std::vector<float>> fArr11;
	std::vector<std::vector<float>> fArr10;

	fArr15.resize(nSize);
	for(int i=0;i<nSize;i++) fArr15[i].resize(MACHINE_STATES_COUNT);

	fArr12.resize(nSize);
	for(int i=0;i<nSize;i++) fArr12[i].resize(MACHINE_STATES_COUNT);

	fArr11.resize(nSize);
	for(int i=0;i<nSize;i++) fArr11[i].resize(MACHINE_STATES_COUNT);

	fArr10.resize(nSize);
	for(int i=0;i<nSize;i++) fArr10[i].resize(MACHINE_STATES_COUNT);
	// ******************************************************************************************
	// 3
	// ******************************************************************************************
	std::vector<std::vector<float>> fArr14;
	std::vector<std::vector<float>> fArr13;
	std::vector<std::vector<float>> fArr9;
	std::vector<std::vector<float>> fArr5;

	fArr14.resize(nSize);
	for(int i=0;i<nSize;i++) fArr14[i].resize(MACHINE_STATES_COUNT);

	fArr13.resize(nSize);
	for(int i=0;i<nSize;i++) fArr13[i].resize(MACHINE_STATES_COUNT);

	fArr9.resize(nSize);
	for(int i=0;i<nSize;i++) fArr9[i].resize(MACHINE_STATES_COUNT);

	fArr5.resize(nSize);
	for(int i=0;i<nSize;i++) fArr5[i].resize(MACHINE_STATES_COUNT);


    std::vector<std::vector<float>> fArr;
	fArr.resize(nSize);
	for(int i=0;i<nSize;i++) fArr[i].resize(MACHINE_STATES_COUNT);


    std::vector<float> fArr1(nSize);
    std::vector<float> fArr2(nSize);

    //std::vector<float> fArr3=Msg;

    InterleaveFloat(fArr3,nSize);

    float f34=1.0F;
    fArr13[0][0]=1.0F;
    fArr9[nSize-1][0]=f34;

    fArr14[0][0]=f34;
    float f1 = 4.0F;

	for(unsigned int ui=0;(int)ui<12;ui++)
    {
        // *************************************************************************************
		// 2.
		// *************************************************************************************
	    for(int i1=0;i1<nSize;i1++)
        {
			double d1=0.5*(double)f1*(double)Parity1[i1];

            float f2=(float)exp(d1);
            float f3=(float)exp(-d1);
            float f4=fArr2[i1]+(f1*pMesg[i1]);
            float f5=(float)exp((double)(-0.5F*f4));
            float f6=f5*f2;
            float f7=f5*f3;

			for(int i2=0;i2<MACHINE_STATES_COUNT;i2++)
            {
	            if(m_Output0[i2])
				{
					fArr15[i1][i2]=f2;
                    fArr7[i1][i2]=f6;
                }
                else
                {
					fArr15[i1][i2]=f3;
                    fArr7[i1][i2]=f7;
                }
            }
            
			f5=(float)exp((double)(0.5F*f4));
            f6=f5*f2;
            f7=f5*f3;
            
			for(int i3=0;i3<MACHINE_STATES_COUNT;i3++)
            {
				if(m_Output1[i3])
                {
					fArr12[i1][i3]=f2;
                    fArr8[i1][i3]=f6;
                }
                else
                {
					fArr12[i1][i3]=f3;
                    fArr8[i1][i3]=f7;
                }
            }
            
			if(i1>=1)
            {
				float f8=0.0F;
                for(int i4=0;i4<MACHINE_STATES_COUNT;i4++)
                {
					float f9=(fArr14[i1 - 1][m_FromState0[i4]]
						    *fArr7[i1-1][m_FromState0[i4]])
						    +(fArr14[i1-1][m_FromState1[i4]]
							*fArr8[i1-1][m_FromState1[i4]]);

                    f8+=f9;
                    fArr14[i1][i4]=f9;
                }
                for(int i5=0;i5<MACHINE_STATES_COUNT;i5++)
                {
					fArr14[i1][i5]=fArr14[i1][i5]/f8;
				}
            }
		}
        // *************************************************************************************
		// 2.
		// *************************************************************************************
		for(int i6=nSize-1;i6>=1;i6--)
        {
			float f10=0.0F;
            for(int i7=0;i7<MACHINE_STATES_COUNT;i7++)
            {
				f10+=(fArr14[i6][m_FromState0[i7]]*fArr7[i6][m_FromState0[i7]])
					+(fArr14[i6][m_FromState1[i7]]*fArr8[i6][m_FromState1[i7]]);
            }
			
			for(int i8=0;i8<MACHINE_STATES_COUNT;i8++)
            {
				fArr9[i6-1][i8]=((fArr9[i6][m_ToState0[i8]]*fArr7[i6][i8])
					           +(fArr9[i6][m_ToState1[i8]]*fArr8[i6][i8]))/f10;
			}
		}
        for(int i9=0;i9<nSize;i9++)
        {
			float f11=0.0F,f12=0.0F,f13=3.402823E+38F;

            for(int i10=0;i10<MACHINE_STATES_COUNT;i10++)
			{
				float f14=fArr14[i9][i10];
                float f15=f14*fArr15[i9][i10]*fArr9[i9][m_ToState0[i10]];
                if((f15!=0.0F)&&(f15<f13)) f13=f15;
                f11+=f15;
                
				float f16=f14*fArr12[i9][i10]*fArr9[i9][m_ToState1[i10]];
                
				if((f16!=0.0F)&&(f16<f13)) f13=f16;
                f12+=f16;
            }
            if(f13>1.0F)       f13=0.0F;
            if(f11==0.0F)      f11=f13;
            else if(f12==0.0F) f12=f13;
            
			fArr1[i9]=(float)log((double)(f12/f11));
        }
        
		InterleaveFloat(fArr1,nSize);
        
        // *************************************************************************************
		// 3.
		// *************************************************************************************
		for(int i11=0;i11<nSize;i11++)
        {
			double d2=0.5*(double)f1*(double)Parity2[i11];

            float f17=(float)exp(d2);
            float f18=(float)exp(-d2);
            float f19=fArr1[i11]+(f1*fArr3[i11]);
            float f20=(float)exp(-0.5*(double)f19);
            float f21=f20*f17;
            float f22=f20*f18;

            for(int i12=0;i12<MACHINE_STATES_COUNT;i12++)
            {
				if(m_Output0[i12])
                {
					fArr11[i11][i12]=f17;
                    fArr6[i11][i12]=f21;
                }
                else
                {
					fArr11[i11][i12]=f18;
                    fArr6[i11][i12]=f22;
                }
            }
            
			f20=(float)exp((double)(0.5F*f19));
            f21=f20*f17;
            f22=f20*f18;

            for(int i13=0;i13<MACHINE_STATES_COUNT;i13++)
			{
				if(m_Output1[i13])
                {
					fArr10[i11][i13]=f17;
                    fArr[i11][i13]=f21;
                }
                else
                {
					fArr10[i11][i13]=f18;
                    fArr[i11][i13]=f22;
                }
            }
            if(i11>=1)
            {
				float f23=0.0F;
                for(int i14=0;i14<MACHINE_STATES_COUNT;i14++)
                {
					float f24=(fArr13[i11-1][m_FromState0[i14]]
						     *fArr6[i11-1][m_FromState0[i14]])
						     +(fArr13[i11-1][m_FromState1[i14]]
							 *fArr[i11-1][m_FromState1[i14]]);

                    f23+=f24;
                    fArr13[i11][i14]=f24;
                }
                for(int i15=0;i15<MACHINE_STATES_COUNT;i15++)
                {
					fArr13[i11][i15]=fArr13[i11][i15]/f23;
                }
            }
        }
        if((int)ui==0)
        {
			float f25=0.0F;
            for(int i16=0;i16<MACHINE_STATES_COUNT;i16++)
            {
				float f26=(fArr13[nSize-1][m_FromState0[i16]]
					     *fArr6[nSize-1][m_FromState0[i16]])
					     +(fArr13[nSize-1][m_FromState1[i16]]
						 *fArr[nSize-1][m_FromState1[i16]]);

                f25+=f26;
                fArr5[nSize-1][i16]=f26;
            }
            for(int i17=0;i17<MACHINE_STATES_COUNT;i17++)
            {
				fArr5[nSize-1][i17]=fArr5[nSize-1][i17]/f25;
            }
        }
        for(int i18=nSize-1;i18>=1;i18--)
        {
			float f27=0.0F;
            for(int i19=0;i19<MACHINE_STATES_COUNT;i19++)
            {
				f27+=(fArr13[i18][m_FromState0[i19]]
					*fArr6[i18][m_FromState0[i19]])
					+(fArr13[i18][m_FromState1[i19]]
					*fArr[i18][m_FromState1[i19]]);
            }
            for(int i20=0;i20<MACHINE_STATES_COUNT;i20++)
            {
				fArr5[i18-1][i20]=((fArr5[i18][m_ToState0[i20]]*fArr6[i18][i20])
					             +(fArr5[i18][m_ToState1[i20]]*fArr[i18][i20]))/f27;
            }
        }
        for(int i21=0;i21<nSize;i21++)
        {
			float f28=0.0F,f29=0.0F,f30=3.402823E+38F;
            for(int i22=0;i22<MACHINE_STATES_COUNT;i22++)
			{
				float f31=fArr13[i21][i22];
                float f32=f31*fArr11[i21][i22]*fArr5[i21][m_ToState0[i22]];
                if((f32!=0.0F)&&(f32<f30)) f30=f32;
                f28+=f32;
                float f33=f31*fArr10[i21][i22]*fArr5[i21][m_ToState1[i22]];
                if((f33!=0.0F)&&(f33<f30)) f30=f33;
                f29+=f33;
            }
            if(f30>1.0F)       f30=0.0F;
            if(f28==0.0F)      f28=f30;
            else if(f29==0.0F) f29=f30;
            fArr2[i21]=log((double)(f29/f28));
        }
        
		DeinterleaveFloat(fArr2,nSize);
        DeinterleaveFloat(fArr1,nSize);

        std::vector<float> fArr4(nSize);
        for(int i23=0;i23<nSize;i23++)
        {
			if(((f1*Msg[i23])+fArr1[i23]+fArr2[i23])>0.0F) fArr4[i23]=1.0F;
            else                                           fArr4[i23]=-1.0F;
        }
        
		byte[] bArr1 = FloatArraytoByteArray(fArr4,0,size-24);
        byte[] bArr2 = FloatArraytoByteArray(fArr4,size-24,24);
        byte[] bArr3 = SVLCLib.PortableSHA1.ComputeHash(bArr1);

                if ((bArr2[0] == bArr3[0]) && (bArr2[1] == bArr3[1]) && (bArr2[2] == bArr3[2]))
                {
                    decodedMsg = new bool[(N - 24)];
                    for (int i24 = 0; i24 < (N - 24); i24++)
                    {
                        decodedMsg[i24] = fArr4[i24] > 0.0F;
                    }
                    return true;
                }
            }
            return false;*/
	return false;
}
void CT3Coder::InterleaveFloat(std::vector<float>& mesg,int size)
{
	std::vector<float> fArr=mesg;
    for(int i=0;i<size;i++)
    {
		mesg[m_InterleaveArray[i]]=fArr[i];
    }
}
void CT3Coder::Encode(std::vector<bool>& Msg,std::vector<bool>& Parity,int nSize,bool bForce)
{
	int i1 = 0;
    for(int i2=0;i2<nSize;i2++)
    {
		if((i2>=(nSize-4))&&bForce)
        {
			if((m_ToState0[i1]&1)!=0) Msg[i2]=true;
            else                      Msg[i2]=false;
        }
        if(Msg[i2])
        {
			Parity[i2]=m_Output1[i1];
            i1=m_ToState1[i1];
        }
        else
        {
			Parity[i2]=m_Output0[i1];
            i1=m_ToState0[i1];
        }
	}
}
std::vector<unsigned char> CT3Coder::ComputeHash(std::vector<unsigned char>& Data)
{
	std::vector<unsigned char> Hash;
	HASH_CTX Stx;
	unsigned char cbHash[20];
	unsigned char* pMessage=new unsigned char[Data.size()];
	for(int i=0;i<Data.size();i++) pMessage[i]=Data[i];
	::FCS_SHA1_Starts(&Stx);
	::FCS_SHA1_Update(&Stx,pMessage,Data.size());
	::FCS_SHA1_Finish(&Stx,cbHash);
	for(int i=0;i<20;i++) Hash.push_back(cbHash[i]);
	delete [] pMessage;
	return Hash;
}
void CT3Coder::EncodeMessage(std::vector<bool>& RawMsg,std::vector<bool>& Msg,std::vector<bool>& Parity1,std::vector<bool>& Parity2)
{
    Parity1.resize(m_N);
    Parity2.resize(m_N);

	Msg=RawMsg;
	Msg.resize(m_N);

	std::vector<unsigned char> bArr1=BoolArraytoByteArray(RawMsg,0,RawMsg.size());
    std::vector<unsigned char> bArr2=ComputeHash(bArr1);

    int i1=RawMsg.size();
    for(int i2=0;i2<3;i2++)
	{
		unsigned char b=bArr2[i2];
        if((b&128)!=0) Msg[i1]=true;
        i1++;
        if((b&64)!=0)  Msg[i1]=true;
        i1++;
        if((b&32)!=0)  Msg[i1]=true;
        i1++;
        if((b&16)!=0)  Msg[i1]=true;
        i1++;
        if((b&8)!=0)   Msg[i1]=true;
        i1++;
        if((b&4)!=0)   Msg[i1]=true;
        i1++;
        if((b&2)!=0)   Msg[i1]=true;
        i1++;
        if((b&1)!=0)   Msg[i1]=true;
        i1++;
    }
    Encode(Msg,Parity1,m_N,false);
    Interleave(Msg,m_N);
    Encode(Msg,Parity2,m_N,false);
    Deinterleave(Msg,m_N);
}
void CT3Coder::Interleave(std::vector<bool>& Msg,int nSize)
{
	std::vector<bool> flagArr(nSize);
    for(int i=0;i<nSize;i++) flagArr[i]=Msg[i];
    for(int i=0;i<nSize;i++) Msg[m_InterleaveArray[i]]=flagArr[i];
}
void CT3Coder::Deinterleave(std::vector<bool>& Msg,int nSize)
{
	std::vector<bool> FlagArr(nSize);
    for(int i=0;i<nSize;i++) FlagArr[i]=Msg[i];
    for(int i=0;i<nSize;i++) Msg[m_DeinterleaveArray[i]]=FlagArr[i];
}
std::vector<bool> CT3Coder::EncodeMessage(std::vector<bool>& RawMsg)
{
	std::vector<bool> FlagArr1,FlagArr2,FlagArr3;

    EncodeMessage(RawMsg,FlagArr1,FlagArr2,FlagArr3);
	
	std::vector<bool> FlagArr4((FlagArr1.size()*3));
    int i1=0,i2=0;
    for(int i3=0;i3<FlagArr4.size();i3++)
    {
		if(i1==0)
        {
			FlagArr4[i3]=FlagArr1[i2];
            i1++;
        }
        else
        {
			if(i1==1)
            {
				FlagArr4[i3]=FlagArr2[i2];
                i1++;
                continue;
			}
            if(i1==2)
            {
				FlagArr4[i3]=FlagArr3[i2];
                i1=0;
                i2++;
            }
        }
    }
    return FlagArr4;
}

/*
        public bool DecodeMessage(bool[] noiseMsg, out bool[] rawMsg)
        {
            float f;

            float[] fArr1 = new float[N];
            float[] fArr2 = new float[N];
            float[] fArr3 = new float[N];
            int i1 = 0, i2 = 0;
            for (int i3 = 0; i3 < noiseMsg.Length; i3++)
            {
                if (noiseMsg[i3])
                    f = 1.0F;
                else
                    f = -1.0F;
                if (i1 == 0)
                {
                    fArr1[i2] = f;
                    i1++;
                }
                else
                {
                    if (i1 == 1)
                    {
                        fArr2[i2] = f;
                        i1++;
                        continue;
                    }
                    if (i1 == 2)
                    {
                        fArr3[i2] = f;
                        i1 = 0;
                        i2++;
                    }
                }
            }
            return decode(fArr1, fArr2, fArr3, N, out rawMsg);
        }

        public bool DecodeMessage(float[] channelmesg, float[] channelparity1, float[] channelparity2, out bool[] rawMsg)
        {
            if ((channelmesg == null) || (channelmesg.Length != N) || (channelparity1 == null) || (channelparity1.Length != N) || (channelparity2 == null) || (channelparity2.Length != N))
                throw new System.ArgumentException();
            return decode(channelmesg, channelparity1, channelparity2, N, out rawMsg);
        }

        private void deinterleave(ref bool[] mesg, int size)
        {
            bool[] flagArr = new bool[size];
            for (int i1 = 0; i1 < size; i1++)
            {
                flagArr[i1] = mesg[i1];
            }
            for (int i2 = 0; i2 < size; i2++)
            {
                mesg[deinterleavearray[i2]] = flagArr[i2];
            }
        }

        private void deinterleavefloat(ref float[] mesg, int size)
        {
            float[] fArr = (float[])mesg.Clone();
            for (int i = 0; i < size; i++)
            {
                mesg[deinterleavearray[i]] = fArr[i];
            }
        }

        private void encode(bool[] mesg, ref bool[] parity, int size, bool force)
        {
            int i1 = 0;
            for (int i2 = 0; i2 < size; i2++)
            {
                if ((i2 >= (size - memory)) && force)
                {
                    if ((tostate0[i1] & 1) != null)
                        mesg[i2] = true;
                    else
                        mesg[i2] = false;
                }
                if (mesg[i2])
                {
                    parity[i2] = output1[i1];
                    i1 = tostate1[i1];
                }
                else
                {
                    parity[i2] = output0[i1];
                    i1 = tostate0[i1];
                }
            }
        }

        public void EncodeMessage(bool[] rawMsg, out bool[] mesg, out bool[] parity1, out bool[] parity2)
        {
            mesg = new bool[N];
            parity1 = new bool[N];
            parity2 = new bool[N];
            System.Array.Copy(rawMsg, mesg, rawMsg.Length);
            byte[] bArr1 = BoolArraytoByteArray(rawMsg, 0, rawMsg.Length);
            byte[] bArr2 = SVLCLib.PortableSHA1.ComputeHash(bArr1);
            int i1 = rawMsg.Length;
            for (int i2 = 0; i2 < 3; i2++)
            {
                byte b = bArr2[i2];
                if ((b & 128) != 0)
                    mesg[i1] = true;
                i1++;
                if ((b & 64) != 0)
                    mesg[i1] = true;
                i1++;
                if ((b & 32) != 0)
                    mesg[i1] = true;
                i1++;
                if ((b & 16) != 0)
                    mesg[i1] = true;
                i1++;
                if ((b & 8) != 0)
                    mesg[i1] = true;
                i1++;
                if ((b & 4) != 0)
                    mesg[i1] = true;
                i1++;
                if ((b & 2) != 0)
                    mesg[i1] = true;
                i1++;
                if ((b & 1) != 0)
                    mesg[i1] = true;
                i1++;
            }
            encode(mesg, ref parity1, N, false);
            interleave(ref mesg, N);
            encode(mesg, ref parity2, N, false);
            deinterleave(ref mesg, N);
        }

        public bool[] EncodeMessage(bool[] rawMsg)
        {
            bool[] flagArr1, flagArr2, flagArr3;

            EncodeMessage(rawMsg, out flagArr1, out flagArr2, out flagArr3);
            bool[] flagArr4 = new bool[(flagArr1.Length * 3)];
            int i1 = 0, i2 = 0;
            for (int i3 = 0; i3 < flagArr4.Length; i3++)
            {
                if (i1 == 0)
                {
                    flagArr4[i3] = flagArr1[i2];
                    i1++;
                }
                else
                {
                    if (i1 == 1)
                    {
                        flagArr4[i3] = flagArr2[i2];
                        i1++;
                        continue;
                    }
                    if (i1 == 2)
                    {
                        flagArr4[i3] = flagArr3[i2];
                        i1 = 0;
                        i2++;
                    }
                }
            }
            return flagArr4;
        }

        private byte[] FloatArraytoByteArray(float[] msg, int offset, int count)
        {
            byte[] bArr = new byte[((count / 8) + ((count % 8) > 0 ? 1 : 0))];
            int i1 = System.Math.Min(msg.Length, offset + count);
            int i2 = 0;
            int i3 = offset;
            while (i3 < (offset + count))
            {
                byte b = 0;
                if ((i3 < i1) && ((double)msg[i3] == 1.0))
                    b += 128;
                i3++;
                if ((i3 < i1) && ((double)msg[i3] == 1.0))
                    b += 64;
                i3++;
                if ((i3 < i1) && ((double)msg[i3] == 1.0))
                    b += 32;
                i3++;
                if ((i3 < i1) && ((double)msg[i3] == 1.0))
                    b += 16;
                i3++;
                if ((i3 < i1) && ((double)msg[i3] == 1.0))
                    b += 8;
                i3++;
                if ((i3 < i1) && ((double)msg[i3] == 1.0))
                    b += 4;
                i3++;
                if ((i3 < i1) && ((double)msg[i3] == 1.0))
                    b += 2;
                i3++;
                if ((i3 < i1) && ((double)msg[i3] == 1.0))
                    b++;
                i3++;
                bArr[i2] = b;
                i2++;
            }
            return bArr;
        }

        private void interleave(ref bool[] mesg, int size)
        {
            bool[] flagArr = new bool[size];
            for (int i1 = 0; i1 < size; i1++)
            {
                flagArr[i1] = mesg[i1];
            }
            for (int i2 = 0; i2 < size; i2++)
            {
                mesg[interleavearray[i2]] = flagArr[i2];
            }
        }

        private void interleavefloat(ref float[] mesg, int size)
        {
            float[] fArr = (float[])mesg.Clone();
            for (int i = 0; i < size; i++)
            {
                mesg[interleavearray[i]] = fArr[i];
            }
        }

        internal void inttobool(int state, ref bool[] array, int size)
        {
            for (uint ui = 0; ui < size; ui++)
            {
                int i = state >> 1;
                if ((i << 1) == state)
                    array[ui] = false;
                else
                    array[ui] = true;
                state = i;
            }
        }

    } // class Turbo3
	*/