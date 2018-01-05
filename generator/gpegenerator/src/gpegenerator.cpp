#include "../include/gpegenerator.h"
#include "../include/BMP.h"
#include "../../gpelib/include/GPE.h"
#include "../../gpelib/include/FCS_HASH.h"
#include "../../qrlib/include/CV_QRCode.h"

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <vector>

#ifndef NULL
#define NULL 0
#endif

extern "C" uint32_t GPE_generate(uint32_t dwMaskSeed
                                ,uint32_t dwFixedSeed
                                ,uint32_t dwGlobalSeed
                                ,const uint8_t* pQRData
                                ,uint32_t dwQRDataSize
                                ,const uint8_t* pGPEData
                                ,uint32_t dwGPEDataSize
                                ,uint8_t* pOutBitmap
                                ,uint32_t* pdwOutBitmapSize
                                ,uint8_t bQRInternal)       
{
    uint32_t dwBitmapSize=0;
    uint32_t dwPadding=0x01000000;
    
    GPE_BYTE cbKey[20];
    HASH_CTX SHA1;
    
    CGPEMask GPEMask;
    CGPEProtectionLayer GPEProtectionLayer;
    CGPELayer GPELayer;
    CMonoBitmap Bmp;
    
    std::vector<std::vector<bool>> BmpPix;
    
    // ***************************************************************
    // Check input variables
    // ***************************************************************
    if(dwMaskSeed==0)   return GPE_ERROR_INVALID_MASK_SEED;
    if(dwFixedSeed==0)  return GPE_ERROR_INVALID_FIXED_SEED;
    if(dwGlobalSeed==0) return GPE_ERROR_INVALID_GLOBAL_SEED;
    
    if(pQRData==NULL||dwQRDataSize==0)   return GPE_ERROR_INVALID_QR_DATA;
    if(pGPEData==NULL||dwGPEDataSize==0) return GPE_ERROR_INVALID_GPE_DATA;
    
    if(pOutBitmap==NULL)               return GPE_ERROR_INVALID_BMP_BUFFER;
    if(pdwOutBitmapSize==NULL)         return GPE_ERROR_INVALID_BMP_SIZE_PTR;
    if(*pdwOutBitmapSize==0)           return GPE_ERROR_INVALID_BMP_BUFF_SIZE;
    
    if(dwGPEDataSize>31)               return GPE_ERROR_INVALID_GPE_DATA_LENGTH;
    // ***************************************************************
    // Prepare key material for encryption key calculation
    // ***************************************************************
    memcpy(cbKey,   &dwMaskSeed,  4);
    memcpy(cbKey+4, &dwFixedSeed, 4);
    memcpy(cbKey+8, &dwGlobalSeed,4);
    memcpy(cbKey+12,&dwPadding,   4);
    // ***************************************************************
    // Generate encryption key
    // ***************************************************************
    FCS_SHA1_Starts(&SHA1);
    FCS_SHA1_Update(&SHA1,cbKey,16);
    FCS_SHA1_Finish(&SHA1,cbKey);
    
    // ***************************************************************
    // Uncoment for _DEBUG    
    // for(int i=0;i<16;i++) printf("%0.2X",cbKey[i]);
    // printf("\n");
    // ***************************************************************
    
    GPEMask.Generate(dwMaskSeed);
    GPEProtectionLayer.Generate(GPEMask,dwFixedSeed,dwGlobalSeed);
    GPELayer.Generate(GPEMask,GPEProtectionLayer
		     ,pGPEData,dwGPEDataSize
		     ,cbKey,128);
    // ***************************************************************
    // QR-code generation
    // QR code matrix byte per pixel.
    // ***************************************************************
    try
    {
	CV_QRCode::QRCode_Mat QR(QRCode_Bitmatrix::Mode::Alphanum,QRCode_Bitmatrix::Version::V5,QRCode_Bitmatrix::ErrCorrect::H,600);
	CV_QRCode::QRCode_Mat QRInternal(QRCode_Bitmatrix::Mode::Alphanum,QRCode_Bitmatrix::Version::V2,QRCode_Bitmatrix::ErrCorrect::H,25);
	
	QR.encode((char*)pQRData);
	QRInternal.encode((char*)pQRData);
	
	cv::Mat QRMat(QR.mat());
	int nRows=0;
	int nCols=0;
	//****************************************************************
	// Debug only
	//****************************************************************
	//printf("QR[%d][%d]\n",nRows,nCols);
	// ***************************************************************
	// Output bitmap generation
        // ***************************************************************
        #define GPE_BORDER_OFFSET (8)
	GPELayer.BitsToBmpVector(BmpPix);
	// ***************************************************************
	// Internal QR code generation
	// 57(include) - 84 (not include)
	// ***************************************************************
	if(bQRInternal!=0)
	{
	    cv::Mat QRIntMat(QRInternal.mat());
	    nRows=QRIntMat.rows;
    	    nCols=QRIntMat.cols;
    	    for(int y=57,QRy=3;y<84;y++,QRy++)
    	    {
    		for(int x=57,QRx=3;x<84;x++,QRx++)
    		{
    		    BmpPix[y][x]=(QRIntMat.at<uint8_t>(QRy,QRx)==0xFF);
    		}
    	    }
    	    //printf("QR internal Rows=%d\n",nRows);
    	    //printf("QR internal Cols=%d\n",nCols);
	}
	nRows=QRMat.rows;
	nCols=QRMat.cols;
	
	int nGpeHeight=BmpPix.size();
	int nGpeWidth=BmpPix[0].size();
	int nStartRow=(nRows-nGpeHeight)/2;
	int nStartCol=(nCols-nGpeWidth)/2;
	int nBorderRow=nStartRow-GPE_BORDER_OFFSET;
	int nBorderCol=nStartCol-GPE_BORDER_OFFSET; 
	std::vector<bool> Row;
	std::vector<std::vector<bool>> QRPix;	
	for(int y=0;y<nRows;y++)
	{
	    Row.clear();
	    for(int x=0;x<nCols;x++)
	    {
		if(y>=nBorderRow&&y<nBorderRow+nGpeHeight+GPE_BORDER_OFFSET*2
		 &&x>=nBorderCol&&x<nBorderCol+nGpeWidth+GPE_BORDER_OFFSET*2) Row.push_back(0xFF);
		else                                                          Row.push_back(QRMat.at<uint8_t>(y,x)==0xFF);
	    }
	    QRPix.push_back(Row);
	}
	for(int y=0,nQRRow=nStartRow-1;y<nGpeHeight;y++,nQRRow++)
	{
	    for(int x=0,nQRCol=nStartCol-1;x<nGpeWidth;x++,nQRCol++)
	    {
		QRPix[nQRRow][nQRCol]=BmpPix[y][x];
	    }
	}
        Bmp.Create(QRPix);
	dwBitmapSize=Bmp.GetSize();
	//printf("Output bitmap size: %d\n",dwBitmapSize);

	dwBitmapSize=nRows*nCols;
        if(*pdwOutBitmapSize<dwBitmapSize)
	{
	    *pdwOutBitmapSize=dwBitmapSize;
	    return GPE_ERROR_BUFFER_SMALL;
        }
	*pdwOutBitmapSize=dwBitmapSize;
        Bmp.Copy(pOutBitmap);
    }
    catch(std::exception& ex)
    {
	//printf("EXCEPTION GPE_ERROR_QR_GENERATION_FAILURE detected\n");
	return GPE_ERROR_QR_GENERATION_FAILURE;
    }
    catch(...)
    {
	//printf("EXCEPTION GPE_ERROR_SEGMENTATION_FAULT detected\n");
    	return GPE_ERROR_SEGMENTATION_FAULT;
    }
    
    return GPE_ERROR_SUCCESS;
}