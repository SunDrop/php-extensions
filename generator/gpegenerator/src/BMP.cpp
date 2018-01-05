#include "BMP.h"

#include <string.h>
#include <stdio.h>
#include <vector>
#include <inttypes.h>

#ifndef NULL
#define NULL
#endif

#ifndef ZeroMemory
#define ZeroMemory(a,s) memset(a,0,s)
#endif

#ifndef CopyMemory
#define CopyMemory(a,b,s) memcpy(a,b,s)
#endif

CMonoBitmap::CMonoBitmap(void)
: m_pBmpBuffer(NULL)
, m_dwBufferSize(0)
{
}
CMonoBitmap::~CMonoBitmap(void)
{
	Destroy();
}
void CMonoBitmap::Destroy(void)
{
	if (m_pBmpBuffer != NULL)
	{
		m_dwBufferSize=0;
		delete [] m_pBmpBuffer;
		m_pBmpBuffer=NULL;
	}
}
void SetBit(uint8_t* pArray,uint32_t dwBitPos)
{
	uint32_t dwByteIdx=dwBitPos/8;
	uint32_t dwBitInByte=dwBitPos%8;
	uint8_t cbMask=0x80;
	cbMask>>=dwBitInByte;
	pArray[dwByteIdx]|=cbMask;
}
void CMonoBitmap::Create(std::vector<std::vector<bool>>& BmpBits)
{
	Destroy();

	uint8_t cbClr[8]={0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00};
	BITMAPFILEHEADER BmpFileHeader;
	BITMAPINFOHEADER BmpInfoHeader;
	ZeroMemory(&BmpFileHeader,sizeof(BITMAPFILEHEADER));
	ZeroMemory(&BmpInfoHeader,sizeof(BITMAPINFOHEADER));

	int nBmpWidth=BmpBits[0].size();
	int nBmpHeight=BmpBits.size();
	int nBitRowLengthInBytes=(nBmpWidth/8)+(nBmpWidth%8?1:0);
	nBitRowLengthInBytes=((nBitRowLengthInBytes/4)*4+4);

	m_dwBufferSize=sizeof(BITMAPFILEHEADER);
	m_dwBufferSize+=sizeof(BITMAPINFOHEADER);
	m_dwBufferSize+=sizeof(cbClr);
	m_dwBufferSize+=nBitRowLengthInBytes*nBmpHeight;

	// ******************************************************************
	// Bitmap file header
	// ******************************************************************
	BmpFileHeader.bfType=0x4D42;           // BMP ID
	BmpFileHeader.bfSize=m_dwBufferSize;   // BMP file size
	BmpFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)
		                   +sizeof(BITMAPINFOHEADER)
		                   +sizeof(cbClr); // Bits offset
	// ******************************************************************
	// Bitmap info header
	// ******************************************************************
	BmpInfoHeader.biSize=sizeof(BITMAPINFOHEADER);
	BmpInfoHeader.biWidth=nBmpWidth;
	BmpInfoHeader.biHeight=nBmpHeight;
	BmpInfoHeader.biPlanes=1;
	BmpInfoHeader.biBitCount=1;
	BmpInfoHeader.biCompression=0;

	m_dwBufferSize=sizeof(BITMAPFILEHEADER);
	m_dwBufferSize+=sizeof(BITMAPINFOHEADER);
	m_dwBufferSize+=sizeof(cbClr);
	m_dwBufferSize+=nBitRowLengthInBytes*nBmpHeight;

	BmpInfoHeader.biSizeImage=m_dwBufferSize-(sizeof(BITMAPFILEHEADER)
						 +sizeof(BITMAPINFOHEADER)
						 +sizeof(cbClr));
	BmpInfoHeader.biXPelsPerMeter=0x0000B88C;
	BmpInfoHeader.biYPelsPerMeter=0x0000B88C;
	BmpInfoHeader.biClrUsed=0;
	BmpInfoHeader.biClrImportant=0;

	m_pBmpBuffer=new BYTE[m_dwBufferSize];
	ZeroMemory(m_pBmpBuffer,m_dwBufferSize);

	uint8_t* pPtr=m_pBmpBuffer;
	CopyMemory(pPtr,&BmpFileHeader,sizeof(BITMAPFILEHEADER));pPtr+=sizeof(BITMAPFILEHEADER);
	CopyMemory(pPtr,&BmpInfoHeader,sizeof(BITMAPINFOHEADER));pPtr+=sizeof(BITMAPINFOHEADER);
	CopyMemory(pPtr,cbClr,sizeof(cbClr));pPtr+=sizeof(cbClr);
	uint8_t* pRow=new BYTE[nBitRowLengthInBytes];
	for (int y = nBmpHeight-1; y>=0; y--)
	{
		ZeroMemory(pRow,nBitRowLengthInBytes);
		for (int x = 0; x < nBmpWidth; x++)
		{
			if(BmpBits[y][x]) SetBit(pRow,x);
		}
		CopyMemory(pPtr,pRow,nBitRowLengthInBytes);
		pPtr+=nBitRowLengthInBytes;
	}
	delete [] pRow;
}
void CMonoBitmap::Create3x3(std::vector<std::vector<bool>>& BmpBits)
{
	Destroy();

	uint8_t cbClr[8]={0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00};
	BITMAPFILEHEADER BmpFileHeader;
	BITMAPINFOHEADER BmpInfoHeader;
	ZeroMemory(&BmpFileHeader,sizeof(BITMAPFILEHEADER));
	ZeroMemory(&BmpInfoHeader,sizeof(BITMAPINFOHEADER));

	int nBmpWidth=(BmpBits[0].size()*3);
	int nBmpHeight=(BmpBits.size()*3);

	int nBitRowLengthInBytes=(nBmpWidth/8)+(nBmpWidth%8?1:0);
	nBitRowLengthInBytes=((nBitRowLengthInBytes/4)*4+4);

	m_dwBufferSize=sizeof(BITMAPFILEHEADER);
	m_dwBufferSize+=sizeof(BITMAPINFOHEADER);
	m_dwBufferSize+=sizeof(cbClr);
	m_dwBufferSize+=nBitRowLengthInBytes*nBmpHeight;

	// ******************************************************************
	// Bitmap file header
	// ******************************************************************
	BmpFileHeader.bfType=0x4D42;           // BMP ID
	BmpFileHeader.bfSize=m_dwBufferSize;   // BMP file size
	BmpFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)
		                   +sizeof(BITMAPINFOHEADER)
		                   +sizeof(cbClr); // Bits offset
	// ******************************************************************
	// Bitmap info header
	// ******************************************************************
	BmpInfoHeader.biSize=sizeof(BITMAPINFOHEADER);
	BmpInfoHeader.biWidth=nBmpWidth;
	BmpInfoHeader.biHeight=nBmpHeight;
	BmpInfoHeader.biPlanes=1;
	BmpInfoHeader.biBitCount=1;
	BmpInfoHeader.biCompression=0;
	BmpInfoHeader.biSizeImage=m_dwBufferSize-(sizeof(BITMAPFILEHEADER)
											 +sizeof(BITMAPINFOHEADER)
											 +sizeof(cbClr));
	BmpInfoHeader.biXPelsPerMeter=0x0000B88C;
	BmpInfoHeader.biYPelsPerMeter=0x0000B88C;
	BmpInfoHeader.biClrUsed=0;
	BmpInfoHeader.biClrImportant=0;

	m_pBmpBuffer=new uint8_t[m_dwBufferSize];
	ZeroMemory(m_pBmpBuffer,m_dwBufferSize);

	uint8_t* pPtr=m_pBmpBuffer;
	CopyMemory(pPtr,&BmpFileHeader,sizeof(BITMAPFILEHEADER));pPtr+=sizeof(BITMAPFILEHEADER);
	CopyMemory(pPtr,&BmpInfoHeader,sizeof(BITMAPINFOHEADER));pPtr+=sizeof(BITMAPINFOHEADER);
	CopyMemory(pPtr,cbClr,sizeof(cbClr));pPtr+=sizeof(cbClr);
	uint8_t* pRow1=new BYTE[nBitRowLengthInBytes];
	uint8_t* pRow2=new BYTE[nBitRowLengthInBytes];
	uint8_t* pRow3=new BYTE[nBitRowLengthInBytes];
	for (int y = BmpBits.size()-1; y>=0; y--)
	{
		ZeroMemory(pRow1,nBitRowLengthInBytes);
		ZeroMemory(pRow2,nBitRowLengthInBytes);
		ZeroMemory(pRow3,nBitRowLengthInBytes);
		for (int x = 0; x <BmpBits[0].size(); x++)
		{
			if(!BmpBits[y][x])
			{
				if(y>3&&y<BmpBits.size()-4&&x>3&&x<BmpBits[0].size()-4)
				{
					SetBit(pRow1,x*3);
					SetBit(pRow1,x*3+2);
					SetBit(pRow3,x*3);
					SetBit(pRow3,x*3+2);
				}
			}
			else
			{
				SetBit(pRow1,x*3);
				SetBit(pRow1,x*3+1);
				SetBit(pRow1,x*3+2);

				SetBit(pRow2,x*3);
				SetBit(pRow2,x*3+1);
				SetBit(pRow2,x*3+2);

				SetBit(pRow3,x*3);
				SetBit(pRow3,x*3+1);
				SetBit(pRow3,x*3+2);
			}
		}
		CopyMemory(pPtr,pRow1,nBitRowLengthInBytes);pPtr+=nBitRowLengthInBytes;
		CopyMemory(pPtr,pRow2,nBitRowLengthInBytes);pPtr+=nBitRowLengthInBytes;
		CopyMemory(pPtr,pRow3,nBitRowLengthInBytes);pPtr+=nBitRowLengthInBytes;
	}
	delete [] pRow1;
	delete [] pRow2;
	delete [] pRow3;
}
void CMonoBitmap::Dump(const char* lpszFileName)
{
    FILE* dump=0;
    dump=fopen(lpszFileName,"wb");
    if(dump==0) return;
    fwrite(m_pBmpBuffer,m_dwBufferSize,1,dump);
    fclose(dump);
}
void CMonoBitmap::Copy(uint8_t* pBuffer)
{
    memcpy(pBuffer,m_pBmpBuffer,m_dwBufferSize);
}
