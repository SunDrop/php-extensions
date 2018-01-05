// Author: i.scherbiy
// Created on 19 серпня 2016, 14:49

#include "CV_QRCode.h"

using CV_QRCode::QRCode_Mat;

cv::Mat CV_QRCode::getQRCode(const std::string &dataIn, QRCode_Bitmatrix::Prefer pref, int imageSz)
{
	QRCode_Bitmatrix::Mode mode = QRCode_Bitmatrix::Mode::Alphanum;
	QRCode_Bitmatrix::Version version;
	QRCode_Bitmatrix::ErrCorrect errCorrect;
	if (QRCode_Bitmatrix::getBestParameters(dataIn, pref, version, errCorrect))
		return QRCode_Mat(mode, version, errCorrect, imageSz).encode(dataIn).mat().clone();
	else
		throw QRCode_Bitmatrix::DataOverflow();
}

const int QRCode_Mat::mRoundingRatio = 2;

QRCode_Mat::QRCode_Mat(Mode mode, Version ver, ErrCorrect errCorr, int declarSize)
	: QRCode_Bitmatrix(mode, ver, errCorr)
{
	int matSize;
	const int baseSize = minImageSize();
	if (declarSize<=baseSize)
	{
		matSize = baseSize;
		mScale = 1;
	}
	else
	{
		mScale = declarSize/baseSize;
		if (const int r = declarSize%baseSize)
		{
			if (baseSize/r < mRoundingRatio)
				mScale++;
			matSize = baseSize*mScale;
		}
		else
			matSize = declarSize;
	}
	mMat = cv::Mat::ones(matSize,matSize,CV_8U)*0xFF;
	mMatQuitetZone = mQuitetZone*mScale;
}

QRCode_Mat::~QRCode_Mat()
{
}

QRCode_Mat &QRCode_Mat::encode(const std::string &dataIn)
{
	QRCode_Bitmatrix::encode(dataIn);
	update();
	return *this;
}

QRCode_Mat &QRCode_Mat::encode(const void *dataIn, size_t sz)
{
	QRCode_Bitmatrix::encode(dataIn, sz);
	update();
	return *this;
}

void QRCode_Mat::update()
{
	for (int bitY=0, moduleY=mMatQuitetZone; bitY<matrixSize(); bitY++, moduleY+=mScale)
	{
		for (int bitX=0, moduleX=mMatQuitetZone; bitX<matrixSize(); bitX++, moduleX+=mScale)
		{
			unsigned bitColor = matrix(bitX,bitY) ? 0 : 0xFF;
			for (int y=0; y<mScale; y++)
			{
				int pixelY = moduleY+y;
				for (int x=0; x<mScale; x++)
					mMat.at<uchar>(cv::Point(moduleX+x, pixelY)) = bitColor;
			}
		}
	}
}

void QRCode_Mat::toFile(std::string fileName) const
{
	imwrite(fileName, mMat);
}

cv::Mat QRCode_Mat::mat() const
{
	return mMat.clone();
}

