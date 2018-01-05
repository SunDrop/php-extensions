#include "FCS_HASH.h"
#include <string.h>

#define BIG_ENDIAN
#ifdef BIG_ENDIAN

#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)          \
{                                     \
	(n)=((unsigned int)(b)[(i)]<<24)   \
     |((unsigned int)(b)[(i)+1]<<16) \
     |((unsigned int)(b)[(i)+2]<<8)  \
     |((unsigned int)(b)[(i)+3]);    \
}
#endif

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)           \
{                                      \
	(b)[(i)]=(unsigned char)((n)>>24);   \
	(b)[(i)+1]=(unsigned char)((n)>>16); \
	(b)[(i)+2]=(unsigned char)((n)>>8);  \
	(b)[(i)+3]=(unsigned char)((n));     \
}
#endif

#endif // BIG_ENDIAN

void FCS_SHA1_Starts(HASH_CTX *ctx)
{
	ctx->total[0]=0;
	ctx->total[1]=0;

	ctx->state[0]=0x67452301;
	ctx->state[1]=0xEFCDAB89;
	ctx->state[2]=0x98BADCFE;
	ctx->state[3]=0x10325476;
	ctx->state[4]=0xC3D2E1F0;
}
static unsigned int W[16],_A,_B,_C,_D,_E,_K;
unsigned int _S_(unsigned int x,unsigned int n)
{
	return (unsigned int)((x<<n)|((x&0xFFFFFFFF)>>(32-n)));
}
unsigned int _R_(unsigned int t)
{
	unsigned int tmp=W[(t-3)&0x0F]^W[(t-8)&0x0F]^W[(t-14)&0x0F]^W[t&0x0F];
	W[t&0x0F]=_S_(tmp,1);
	return (unsigned int)W[t&0x0F];
}
unsigned int _F1(unsigned int x,unsigned int y,unsigned int z)
{
	return (unsigned int)(z^(x&(y^z)));
}
unsigned int _F2(unsigned int x,unsigned int y,unsigned int z)
{
	return (x^y^z);
}
unsigned int _F3(unsigned int x,unsigned int y,unsigned int z)
{	
	return ((x&y)|(z&(x|y)));
}
unsigned int _F4(unsigned int x,unsigned int y,unsigned int z)
{
	return (x^y^z);
}
typedef unsigned int(*_F_PTR)(unsigned int,unsigned int,unsigned int);
void _P_(unsigned int a,unsigned int* b,unsigned int c,unsigned int d,unsigned int* e,unsigned int x,_F_PTR _F_)
{
	(*e)+=_S_(a,5)+_F_(*b,c,d)+_K+x;
	(*b)=_S_(*b,30);
}
static void SHA1_process(HASH_CTX *ctx,unsigned char output[64])
{
	unsigned char i;
#ifdef BIG_ENDIAN
	for(i=0;i<16;i++) GET_UINT32_BE(W[i],output,i*4);
#else
	for(i=0;i<16;i++) W[i]=*(unsigned int*)(output+i*4);
#endif

	_A=ctx->state[0];
	_B=ctx->state[1];
	_C=ctx->state[2];
	_D=ctx->state[3];
	_E=ctx->state[4];

	_K=((unsigned int)0x5A827999);

	_P_(_A,&_B,_C,_D,&_E,W[0],&_F1);
	_P_(_E,&_A,_B,_C,&_D,W[1],&_F1);
	_P_(_D,&_E,_A,_B,&_C,W[2],&_F1);
	_P_(_C,&_D,_E,_A,&_B,W[3],&_F1);
	_P_(_B,&_C,_D,_E,&_A,W[4],&_F1);
	_P_(_A,&_B,_C,_D,&_E,W[5],&_F1);
	_P_(_E,&_A,_B,_C,&_D,W[6],&_F1);
	_P_(_D,&_E,_A,_B,&_C,W[7],&_F1);
	_P_(_C,&_D,_E,_A,&_B,W[8],&_F1);
	_P_(_B,&_C,_D,_E,&_A,W[9],&_F1);
	_P_(_A,&_B,_C,_D,&_E,W[10],&_F1);
	_P_(_E,&_A,_B,_C,&_D,W[11],&_F1);
	_P_(_D,&_E,_A,_B,&_C,W[12],&_F1);
	_P_(_C,&_D,_E,_A,&_B,W[13],&_F1);
	_P_(_B,&_C,_D,_E,&_A,W[14],&_F1);
	_P_(_A,&_B,_C,_D,&_E,W[15],&_F1);
	_P_(_E,&_A,_B,_C,&_D,_R_(16),&_F1);
	_P_(_D,&_E,_A,_B,&_C,_R_(17),&_F1);
	_P_(_C,&_D,_E,_A,&_B,_R_(18),&_F1);
	_P_(_B,&_C,_D,_E,&_A,_R_(19),&_F1);

	_K=((unsigned int)0x6ED9EBA1);

	_P_(_A,&_B,_C,_D,&_E,_R_(20),&_F2);
	_P_(_E,&_A,_B,_C,&_D,_R_(21),&_F2);
	_P_(_D,&_E,_A,_B,&_C,_R_(22),&_F2);
	_P_(_C,&_D,_E,_A,&_B,_R_(23),&_F2);
	_P_(_B,&_C,_D,_E,&_A,_R_(24),&_F2);
	_P_(_A,&_B,_C,_D,&_E,_R_(25),&_F2);
	_P_(_E,&_A,_B,_C,&_D,_R_(26),&_F2);
	_P_(_D,&_E,_A,_B,&_C,_R_(27),&_F2);
	_P_(_C,&_D,_E,_A,&_B,_R_(28),&_F2);
	_P_(_B,&_C,_D,_E,&_A,_R_(29),&_F2);
	_P_(_A,&_B,_C,_D,&_E,_R_(30),&_F2);
	_P_(_E,&_A,_B,_C,&_D,_R_(31),&_F2);
	_P_(_D,&_E,_A,_B,&_C,_R_(32),&_F2);
	_P_(_C,&_D,_E,_A,&_B,_R_(33),&_F2);
	_P_(_B,&_C,_D,_E,&_A,_R_(34),&_F2);
	_P_(_A,&_B,_C,_D,&_E,_R_(35),&_F2);
	_P_(_E,&_A,_B,_C,&_D,_R_(36),&_F2);
	_P_(_D,&_E,_A,_B,&_C,_R_(37),&_F2);
	_P_(_C,&_D,_E,_A,&_B,_R_(38),&_F2);
	_P_(_B,&_C,_D,_E,&_A,_R_(39),&_F2);

	_K=((unsigned int)0x8F1BBCDC);

	_P_(_A,&_B,_C,_D,&_E,_R_(40),&_F3);
	_P_(_E,&_A,_B,_C,&_D,_R_(41),&_F3);
	_P_(_D,&_E,_A,_B,&_C,_R_(42),&_F3);
	_P_(_C,&_D,_E,_A,&_B,_R_(43),&_F3);
	_P_(_B,&_C,_D,_E,&_A,_R_(44),&_F3);
	_P_(_A,&_B,_C,_D,&_E,_R_(45),&_F3);
	_P_(_E,&_A,_B,_C,&_D,_R_(46),&_F3);
	_P_(_D,&_E,_A,_B,&_C,_R_(47),&_F3);
	_P_(_C,&_D,_E,_A,&_B,_R_(48),&_F3);
	_P_(_B,&_C,_D,_E,&_A,_R_(49),&_F3);
	_P_(_A,&_B,_C,_D,&_E,_R_(50),&_F3);
	_P_(_E,&_A,_B,_C,&_D,_R_(51),&_F3);
	_P_(_D,&_E,_A,_B,&_C,_R_(52),&_F3);
	_P_(_C,&_D,_E,_A,&_B,_R_(53),&_F3);
	_P_(_B,&_C,_D,_E,&_A,_R_(54),&_F3);
	_P_(_A,&_B,_C,_D,&_E,_R_(55),&_F3);
	_P_(_E,&_A,_B,_C,&_D,_R_(56),&_F3);
	_P_(_D,&_E,_A,_B,&_C,_R_(57),&_F3);
	_P_(_C,&_D,_E,_A,&_B,_R_(58),&_F3);
	_P_(_B,&_C,_D,_E,&_A,_R_(59),&_F3);

	_K=((unsigned int)0xCA62C1D6);

	_P_(_A,&_B,_C,_D,&_E,_R_(60),_F4);
	_P_(_E,&_A,_B,_C,&_D,_R_(61),_F4);
	_P_(_D,&_E,_A,_B,&_C,_R_(62),_F4);
	_P_(_C,&_D,_E,_A,&_B,_R_(63),_F4);
	_P_(_B,&_C,_D,_E,&_A,_R_(64),_F4);
	_P_(_A,&_B,_C,_D,&_E,_R_(65),_F4);
	_P_(_E,&_A,_B,_C,&_D,_R_(66),_F4);
	_P_(_D,&_E,_A,_B,&_C,_R_(67),_F4);
	_P_(_C,&_D,_E,_A,&_B,_R_(68),_F4);
	_P_(_B,&_C,_D,_E,&_A,_R_(69),_F4);
	_P_(_A,&_B,_C,_D,&_E,_R_(70),_F4);
	_P_(_E,&_A,_B,_C,&_D,_R_(71),_F4);
	_P_(_D,&_E,_A,_B,&_C,_R_(72),_F4);
	_P_(_C,&_D,_E,_A,&_B,_R_(73),_F4);
	_P_(_B,&_C,_D,_E,&_A,_R_(74),_F4);
	_P_(_A,&_B,_C,_D,&_E,_R_(75),_F4);
	_P_(_E,&_A,_B,_C,&_D,_R_(76),_F4);
	_P_(_D,&_E,_A,_B,&_C,_R_(77),_F4);
	_P_(_C,&_D,_E,_A,&_B,_R_(78),_F4);
	_P_(_B,&_C,_D,_E,&_A,_R_(79),_F4);

	ctx->state[0]+=_A;
	ctx->state[1]+=_B;
	ctx->state[2]+=_C;
	ctx->state[3]+=_D;
	ctx->state[4]+=_E;
}
void FCS_SHA1_Update(HASH_CTX *ctx,unsigned char* input,unsigned int ilen)
{
	unsigned int fill;
	unsigned int left;

	if(ilen<=0) return;

	left=ctx->total[0]&0x3F;
	fill=64-left;

	ctx->total[0]+=(unsigned int)ilen;
	ctx->total[0]&=0xFFFFFFFF;

	if(ctx->total[0]<(unsigned int)ilen) ctx->total[1]++;
	if(left&&ilen>=fill)
	{
		memcpy(ctx->buffer+left,input,(unsigned short)fill);
		SHA1_process(ctx,ctx->buffer);
		input+=fill;
		ilen-=fill;
		left=0;
	}
	while(ilen>=64)
	{
		SHA1_process(ctx,(unsigned char*)input);
		input+=64;
		ilen-=64;
	}
	if(ilen>0) memcpy(ctx->buffer+left,input,(unsigned short)ilen);
}
static unsigned char sha1_padding[64]=
{
  0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
void FCS_SHA1_Finish(HASH_CTX *ctx,unsigned char output[20])
{
	unsigned char i;
	unsigned int last,padn;
	unsigned int high,low;
	unsigned char msglen[8];

	high=(ctx->total[0]>>29)|(ctx->total[1]<<3);
	low=(ctx->total[0]<<3);

#ifdef BIG_ENDIAN
	PUT_UINT32_BE(high,msglen,0);
	PUT_UINT32_BE(low,msglen,4);
#else
	*(unsigned int*)(msglen)=high;
	*(unsigned int*)(msglen+4)=low;
#endif
	last=ctx->total[0]&0x3F;
	padn=(last<56)?(56-last):(120-last);

	FCS_SHA1_Update(ctx,sha1_padding,padn);
	FCS_SHA1_Update(ctx,msglen,8);

#ifdef BIG_ENDIAN
	for(i=0;i<5;i++) PUT_UINT32_BE(ctx->state[i],output,i*4);
#else
	for(i=0;i<5;i++) *(unsigned int*)(output+i*4)=ctx->state[i];
#endif
	memset(ctx,0,sizeof(HASH_CTX));
}
