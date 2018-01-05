// Author: i.scherbiy
// Created on 19 серпня 2016, 12:32

#include "QRCode_Bitmatrix.h"
#include "BCH_Reed_Solomon.h"

typedef boost::dynamic_bitset<> bitset;

const int QRCode_Bitmatrix::mQuitetZone = 4;

const std::unordered_map<char,unsigned> QRCode_Bitmatrix::mAlphanumCharSet =
{
	{'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9},
	{'A', 10}, {'B', 11}, {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15}, {'G', 16}, {'H', 17},
	{'I', 18}, {'J', 19}, {'K', 20}, {'L', 21}, {'M', 22}, {'N', 23},
	{'O', 24}, {'P', 25}, {'Q', 26}, {'R', 27}, {'S', 28}, {'T', 29},
	{'U', 30}, {'V', 31}, {'W', 32}, {'X', 33}, {'Y', 34}, {'Z', 35},
	{' ', 36}, {'$', 37}, {'%', 38}, {'*', 39}, {'+', 40}, {'-', 41}, {'.', 42}, {'/', 43}, {':', 44}
};

const int QRCode_Bitmatrix::mCapacityTbl[][mErrCorrectLevelsQty] =
{
	{  25,   20,   16,   10},
	{  47,   38,   29,   20},
	{  77,   61,   47,   35},
	{ 114,   90,   67,   50},
	{ 154,  122,   87,   64},
	{ 195,  154,  108,   84},
	{ 224,  178,  125,   93},
	{ 279,  221,  157,  122},
	{ 335,  262,  189,  143},
	{ 395,  311,  221,  174},
	{ 468,  366,  259,  200},
	{ 535,  419,  296,  227},
	{ 619,  483,  352,  259},
	{ 667,  528,  376,  283},
	{ 758,  600,  426,  321},
	{ 854,  656,  470,  365},
	{ 938,  734,  531,  408},
	{1046,  816,  574,  452},
	{1153,  909,  644,  493},
	{1249,  970,  702,  557},
	{1352, 1035,  742,  587},
	{1460, 1134,  823,  640},
	{1588, 1248,  890,  672},
	{1704, 1326,  963,  744},
	{1853, 1451, 1041,  779},
	{1990, 1542, 1094,  864},
	{2132, 1637, 1172,  910},
	{2223, 1732, 1263,  958},
	{2369, 1839, 1322, 1016},
	{2520, 1994, 1429, 1080},
	{2677, 2113, 1499, 1150},
	{2840, 2238, 1618, 1226},
	{3009, 2369, 1700, 1307},
	{3183, 2506, 1787, 1394},
	{3351, 2632, 1867, 1431},
	{3537, 2780, 1966, 1530},
	{3729, 2894, 2071, 1591},
	{3927, 3054, 2181, 1658},
	{4087, 3220, 2298, 1774},
	{4296, 3391, 2420, 1852}
};
const QRCode_Bitmatrix::BlocksStruct QRCode_Bitmatrix::mBlocksStructTbl[][mErrCorrectLevelsQty] =
{
	{{ 7,  1,  19,  0}, {10,  1, 16,  0}, {13,  1, 13,  0}, {17,  1,  9,  0}},
	{{10,  1,  34,  0}, {16,  1, 28,  0}, {22,  1, 22,  0}, {28,  1, 16,  0}},
	{{15,  1,  55,  0}, {26,  1, 44,  0}, {18,  2, 17,  0}, {22,  2, 13,  0}},
	{{20,  1,  80,  0}, {18,  2, 32,  0}, {26,  2, 24,  0}, {16,  4,  9,  0}},
	{{26,  1, 108,  0}, {24,  2, 43,  0}, {18,  2, 15,  2}, {22,  2, 11,  2}},
	{{18,  2,  68,  0}, {16,  4, 27,  0}, {24,  4, 19,  0}, {28,  4, 15,  0}},
	{{20,  2,  78,  0}, {18,  4, 31,  0}, {18,  2, 14,  4}, {26,  4, 13,  1}},
	{{24,  2,  97,  0}, {22,  2, 38,  2}, {22,  4, 18,  2}, {26,  4, 14,  2}},
	{{30,  2, 116,  0}, {22,  3, 36,  2}, {20,  4, 16,  4}, {24,  4, 12,  4}},
	{{18,  2,  68,  2}, {26,  4, 43,  1}, {24,  6, 19,  2}, {28,  6, 15,  2}},
	{{20,  4,  81,  0}, {30,  1, 50,  4}, {28,  4, 22,  4}, {24,  3, 12,  8}},
	{{24,  2,  92,  2}, {22,  6, 36,  2}, {26,  4, 20,  6}, {28,  7, 14,  4}},
	{{26,  4, 107,  0}, {22,  8, 37,  1}, {24,  8, 20,  4}, {22, 12, 11,  4}},
	{{30,  3, 115,  1}, {24,  4, 40,  5}, {20, 11, 16,  5}, {24, 11, 12,  5}},
	{{22,  5,  87,  1}, {24,  5, 41,  5}, {30,  5, 24,  7}, {24, 11, 12,  7}},
	{{24,  5,  98,  1}, {28,  7, 45,  3}, {24, 15, 19,  2}, {30,  3, 15, 13}},
	{{28,  1, 107,  5}, {28, 10, 46,  1}, {28,  1, 22, 15}, {28,  2, 14, 17}},
	{{30,  5, 120,  1}, {26,  9, 43,  4}, {28, 17, 22,  1}, {28,  2, 14, 19}},
	{{28,  3, 113,  4}, {26,  3, 44, 11}, {26, 17, 21,  4}, {26,  9, 13, 16}},
	{{28,  3, 107,  5}, {26,  3, 41, 13}, {30, 15, 24,  5}, {28, 15, 15, 10}},
	{{28,  4, 116,  4}, {26, 17, 42,  0}, {28, 17, 22,  6}, {30, 19, 16,  6}},
	{{28,  2, 111,  7}, {28, 17, 46,  0}, {30,  7, 24, 16}, {24, 34, 13,  0}},
	{{30,  4, 121,  5}, {28,  4, 47, 14}, {30, 11, 24, 14}, {30, 16, 15, 14}},
	{{30,  6, 117,  4}, {28,  6, 45, 14}, {30, 11, 24, 16}, {30, 30, 16,  2}},
	{{26,  8, 106,  4}, {28,  8, 47, 13}, {30,  7, 24, 22}, {30, 22, 15, 13}},
	{{28, 10, 114,  2}, {28, 19, 46,  4}, {28, 28, 22,  6}, {30, 33, 16,  4}},
	{{30,  8, 122,  4}, {28, 22, 45,  3}, {30,  8, 23, 26}, {30, 12, 15, 28}},
	{{30,  3, 117, 10}, {28,  3, 45, 23}, {30,  4, 24, 31}, {30, 11, 15, 31}},
	{{30,  7, 116,  7}, {28, 21, 45,  7}, {30,  1, 23, 37}, {30, 19, 15, 26}},
	{{30,  5, 115, 10}, {28, 19, 47, 10}, {30, 15, 24, 25}, {30, 23, 15, 25}},
	{{30, 13, 115,  3}, {28,  2, 46, 29}, {30, 42, 24,  1}, {30, 23, 15, 28}},
	{{30, 17, 115,  0}, {28, 10, 46, 23}, {30, 10, 24, 35}, {30, 19, 15, 35}},
	{{30, 17, 115,  1}, {28, 14, 46, 21}, {30, 29, 24, 19}, {30, 11, 15, 46}},
	{{30, 13, 115,  6}, {28, 14, 46, 23}, {30, 44, 24,  7}, {30, 59, 16,  1}},
	{{30, 12, 121,  7}, {28, 12, 47, 26}, {30, 39, 24, 14}, {30, 22, 15, 41}},
	{{30,  6, 121, 14}, {28,  6, 47, 34}, {30, 46, 24, 10}, {30,  2, 15, 64}},
	{{30, 17, 122,  4}, {28, 29, 46, 14}, {30, 49, 24, 10}, {30, 24, 15, 46}},
	{{30,  4, 122, 18}, {28, 13, 46, 32}, {30, 48, 24, 14}, {30, 42, 15, 32}},
	{{30, 20, 117,  4}, {28, 40, 47,  7}, {30, 43, 24, 22}, {30, 10, 15, 67}},
	{{30, 19, 118,  6}, {28, 18, 47, 31}, {30, 34, 24, 34}, {30, 20, 15, 61}}
};

const int QRCode_Bitmatrix::mFinderPatternSz = 7;
const unsigned QRCode_Bitmatrix::mFinderPattern[mFinderPatternSz] = 
{
	0b1111111,
	0b1000001,
	0b1011101,
	0b1011101,
	0b1011101,
	0b1000001,
	0b1111111
};
const int QRCode_Bitmatrix::mAlignPatternSz = 5;
const unsigned QRCode_Bitmatrix::mAlignPattern[mAlignPatternSz] = 
{
	0b11111,
	0b10001,
	0b10101,
	0b10001,
	0b11111
};
const std::vector<int> QRCode_Bitmatrix::mAlignPatternPos[] =
{
	{6, 18},
	{6, 22},
	{6, 26},
	{6, 30},
	{6, 34},
	{6, 22, 38},
	{6, 24, 42},
	{6, 26, 46},
	{6, 28, 50},
	{6, 30, 54},
	{6, 32, 58},
	{6, 34, 62},
	{6, 26, 46, 66},
	{6, 26, 48, 70},
	{6, 26, 50, 74},
	{6, 30, 54, 78},
	{6, 30, 56, 82},
	{6, 30, 58, 86},
	{6, 34, 62, 90},
	{6, 28, 50, 72,  94},
	{6, 26, 50, 74,  98},
	{6, 30, 54, 78, 102},
	{6, 28, 54, 80, 106},
	{6, 32, 58, 84, 110},
	{6, 30, 58, 86, 114},
	{6, 34, 62, 90, 118},
	{6, 26, 50, 74,  98, 122},
	{6, 30, 54, 78, 102, 126},
	{6, 26, 52, 78, 104, 130},
	{6, 30, 56, 82, 108, 134},
	{6, 34, 60, 86, 112, 138},
	{6, 30, 58, 86, 114, 142},
	{6, 34, 62, 90, 118, 146},
	{6, 30, 54, 78, 102, 126, 150},
	{6, 24, 50, 76, 102, 128, 154},
	{6, 28, 54, 80, 106, 132, 158},
	{6, 32, 58, 84, 110, 136, 162},
	{6, 26, 54, 82, 110, 138, 166},
	{6, 30, 58, 86, 114, 142, 170}
};
const int QRCode_Bitmatrix::mTimingPatternRow = mFinderPatternSz-1;

const int QRCode_Bitmatrix::mLikeFinderPatternSz = QRCode_Bitmatrix::mFinderPatternSz+QRCode_Bitmatrix::mQuitetZone;
const int QRCode_Bitmatrix::mFragmentMaxPos = QRCode_Bitmatrix::mFinderPatternSz-1;

const unsigned QRCode_Bitmatrix::mFormatInfo[mMaskPatternsQty][mErrCorrectLevelsQty] =
{
	{0b111011111000100, 0b101010000010010, 0b011010101011111, 0b001011010001001},
	{0b111001011110011, 0b101000100100101, 0b011000001101000, 0b001001110111110},
	{0b111110110101010, 0b101111001111100, 0b011111100110001, 0b001110011100111},
	{0b111100010011101, 0b101101101001011, 0b011101000000110, 0b001100111010000},
	{0b110011000101111, 0b100010111111001, 0b010010010110100, 0b000011101100010},
	{0b110001100011000, 0b100000011001110, 0b010000110000011, 0b000001001010101},
	{0b110110001000001, 0b100111110010111, 0b010111011011010, 0b000110100001100},
	{0b110100101110110, 0b100101010100000, 0b010101111101101, 0b000100000111011}
};
const uint32_t QRCode_Bitmatrix::mVersionInfo[]
{
	0b000111110010010100, 0b001000010110111100, 0b001001101010011001, 0b001010010011010011, 0b001011101111110110,
	0b001100011101100010, 0b001101100001000111, 0b001110011000001101, 0b001111100100101000, 0b010000101101111000,
	0b010001010001011101, 0b010010101000010111, 0b010011010100110010, 0b010100100110100110, 0b010101011010000011,
	0b010110100011001001, 0b010111011111101100, 0b011000111011000100, 0b011001000111100001, 0b011010111110101011,
	0b011011000010001110, 0b011100110000011010, 0b011101001100111111, 0b011110110101110101, 0b011111001001010000,
	0b100000100111010101, 0b100001011011110000, 0b100010100010111010, 0b100011011110011111, 0b100100101100001011,
	0b100101010000101110, 0b100110101001100100, 0b100111010101000001, 0b101000110001101001
};

int QRCode_Bitmatrix::matrixSize(Version ver)
{
	return 21+4*static_cast<int>(ver);
}

bool QRCode_Bitmatrix::getBestParameters(const std::string &data, Prefer pref, Version &version, ErrCorrect &errCorrect)
{
	auto check = [data, &version, &errCorrect](int v, int ec)
	{
		bool isFit = mCapacityTbl[v][ec]>=data.size();
		if (isFit)
		{
			version = static_cast<Version>(v);
			errCorrect = static_cast<ErrCorrect>(ec);
		}
		return isFit;
	};
	int maxEC = mErrCorrectLevelsQty-1;
	switch (pref)
	{
		case MinVersion:
			for (int v=0; v<mVersionsQty; v++)
				for (int ec=maxEC; ec>=0; ec--)
					if (check(v,ec))
						return true;
			return false;
		case MaxCorrect:
			for (int ec=maxEC; ec>=0; ec--)
				for (int v=0; v<mVersionsQty; v++)
					if (check(v,ec))
						return true;
			return false;
	}
}

QRCode_Bitmatrix::QRCode_Bitmatrix(Mode mode, Version ver, ErrCorrect errCorr) :
	  mMode(mode)
	, mVersion(ver)
	, mErrCorrect(errCorr)
	, mCapacity(capacity(mMode,mVersion,mErrCorrect))
	, mMatrixSize(matrixSize(mVersion))
	, mMatrixBitsQty(mMatrixSize*mMatrixSize)
	, mModeIndicator(1<<static_cast<int>(mMode))
	, mMaxPos(mMatrixSize-1)
	, mTimingPatternColumn(mMaxPos-mTimingPatternRow)
{
	if (mVersion<=Version::V9)
		mCharQtyIndicatorSz = 9;
	else if (mVersion<=Version::V26)
		mCharQtyIndicatorSz = 11;
	else
		mCharQtyIndicatorSz = 13;

	mFunctPatterns.resize(mMatrixSize);
	mUsedArea.resize(mMatrixSize);
	for (int i = 0; i < mMatrixSize; i++)
	{
		mFunctPatterns[i].resize(mMatrixSize);
		mUsedArea[i].resize(mMatrixSize);
	}

	int shift = mMatrixSize - mFinderPatternSz;
	for (int i = 0; i < mFinderPatternSz; i++)
	{
		bitset patternRow(mMatrixSize, mFinderPattern[i]);
		mFunctPatterns[i] |= patternRow;
		mFunctPatterns[i] |= patternRow <<= shift;
		mFunctPatterns[shift+i] |= patternRow;
	}
	bitset finderPatternArea1(mMatrixSize, 0b11111111);
	bitset finderPatternArea2(mMatrixSize, 0b111111111);
	finderPatternArea1 |= finderPatternArea2 <<= mMatrixSize-mFinderPatternSz-2;
	for (int i = 0; i<mFinderPatternSz+1; i++)
	{
		mUsedArea[i] |= finderPatternArea1;
		mUsedArea[mMaxPos - i] |= finderPatternArea2;
	}
	mUsedArea[mFinderPatternSz+1] |= finderPatternArea1;

	if (mVersion>Version::V1)
	{
		const std::vector<int> &alignPatternPos = mAlignPatternPos[static_cast<int>(mVersion)-1];
		const int lastAlignPos = alignPatternPos.size()-1;
		bitset alignPatternArea(mMatrixSize, 0b11111);
		for (int i = 0; i<mAlignPatternSz; i++)
		{
			bitset patternRow(mMatrixSize, mAlignPattern[i]);
			for (int j = 0; j<=lastAlignPos; j++)
			{
				const int shift = mMaxPos-alignPatternPos[j]-mAlignPatternSz/2;
				bitset shiftPatternRow(patternRow), shiftPatternArea(alignPatternArea);
				shiftPatternRow <<= shift;
				shiftPatternArea <<= shift;
				for (int k = 0; k<=lastAlignPos; k++)
					if (!(j==0&&k==0 || j==0&&k==lastAlignPos || j==lastAlignPos&&k==0))
					{
						int row  = alignPatternPos[k]+i-mAlignPatternSz/2;
						mFunctPatterns[row] |= shiftPatternRow;
						mUsedArea[row] |= shiftPatternArea;
					}
			}
		}
	}

	for (int i = mFinderPatternSz+1; i < mMaxPos-mFinderPatternSz; i+=2)
		mFunctPatterns[i][mTimingPatternColumn] = mFunctPatterns[mTimingPatternRow][i] =
		mUsedArea[i][mTimingPatternColumn] = mUsedArea[mTimingPatternRow][i] =
		mUsedArea[i+1][mTimingPatternColumn] = mUsedArea[mTimingPatternRow][i+1]
		= 1;

	mFunctPatterns[mMaxPos-mFinderPatternSz][mMaxPos-mFinderPatternSz-1] = 1;

	if (mVersion>Version::V6)
	{
		bitset versionInfoArea1(mMatrixSize, 0b111), versionInfoArea2(mMatrixSize, 0b111111);
		versionInfoArea1 <<= mFinderPatternSz+1;
		versionInfoArea2 <<= mMatrixSize-6;
		for (int i = 0; i < 6; mUsedArea[i++] |= versionInfoArea1);
		for (int i = 0; i < 3; mUsedArea[mMatrixSize-mFinderPatternSz-2-i++] |= versionInfoArea2);
	}

	const BlocksStruct &blocksStruct = mBlocksStructTbl[static_cast<int>(mVersion)][static_cast<int>(mErrCorrect)];
	mGr2BlockSz = (mGr1BlockSz = blocksStruct.gr1BlockSz)+1;
	mGr1BlocksQty = blocksStruct.gr1BlocksQty;
	mGr2BlocksQty = blocksStruct.gr2BlocksQty;
	mDataCodewordsQty = mGr1BlockSz*mGr1BlocksQty+mGr2BlockSz*mGr2BlocksQty;
	mDataCodeSz = 8*mDataCodewordsQty;
	mErrCorrBlockSz = blocksStruct.errCorrBlockSz;
	mLastErrCorrBlock = mGr1BlocksQty+mGr2BlocksQty-1;
	mGr1LastBlock = mGr1BlocksQty-1;
	mGr2LastBlock = mGr2BlocksQty-1;

	for (int maskN=0; maskN<mMaskPatternsQty; maskN++)
	{
		mMaskPattern[maskN].resize(mMatrixSize);
		for (int i=0; i<mMatrixSize; mMaskPattern[maskN][i++].resize(mMatrixSize));
	}
	for (int x=0; x<mMatrixSize; x++)
	{
		int x0 = mMaxPos-x;
		for (int y=0; y<mMatrixSize; y++)
		{
			int maskPattern[mMaskPatternsQty];
			int xySum = x+y, xyMult = x*y;
			maskPattern[0] = xySum%2;
			maskPattern[1] = y%2;
			maskPattern[2] = x%3;
			maskPattern[3] = xySum%3;
			maskPattern[4] = (y/2+x/3)%2;
			maskPattern[5] = xyMult%2+xyMult%3;
			maskPattern[6] = maskPattern[5]%2;
			maskPattern[7] = (maskPattern[0]+xyMult%3)%2;
			for (int maskN=0; maskN<mMaskPatternsQty; maskN++)
				mMaskPattern[maskN][y][x0] = maskPattern[maskN]==0;
		}
	}
}

QRCode_Bitmatrix::~QRCode_Bitmatrix()
{
}

QRCode_Bitmatrix &QRCode_Bitmatrix::encode(const std::string &dataIn)
{
	// 1.1 Data-in analisys
	if (dataIn.size()>mCapacity)
		throw DataOverflow();

	// 1.2 Data-in analisys
	// 2. Encoding
	typedef boost::dynamic_bitset<uint8_t> bitset8;

	bitset8 dataCode(mDataCodeSz);
	bitset::size_type currPos = mDataCodeSz;

	dataCode |= bitset8(mDataCodeSz, mModeIndicator) <<= currPos-=4;

	dataCode |= bitset8(mDataCodeSz, dataIn.size()) <<= currPos-=mCharQtyIndicatorSz;

	auto alphanumVal = [](char c)
	{
		auto iter = mAlphanumCharSet.find(c);
		if (iter==mAlphanumCharSet.end())
				throw MismatchMode();
		return iter->second;
	};
	for (int i=1; i<dataIn.size(); i+=2)
		dataCode |= bitset8(mDataCodeSz, alphanumVal(dataIn[i-1])*45+alphanumVal(dataIn[i])) <<= currPos-=11;
	if (dataIn.size()%2)
		dataCode |= bitset8(mDataCodeSz, alphanumVal(dataIn[dataIn.size()-1])) <<= currPos-=6;

	uint8_t dataCodeword[mDataCodewordsQty];
	to_block_range(dataCode, dataCodeword);

	const int terminatorSz = 4;
	if (currPos>terminatorSz)
		for (int padBytesQty=(currPos-terminatorSz)/8, i=1; i<=padBytesQty; i++)
			dataCodeword[padBytesQty-i] = i%2 ? 0b11101100 : 0b00010001;

	// 3. Error correction coding
	uint8_t dataCodewordGr1[mGr1BlocksQty][mGr1BlockSz], dataCodewordGr2[mGr2BlocksQty][mGr2BlockSz];
	uint8_t errCorrCodeword[mGr1BlocksQty+mGr2BlocksQty][mErrCorrBlockSz];
	int dataCodewordsN = mDataCodewordsQty-1;
	for (int i = 0; i < mGr1BlocksQty; i++)
		for (int j = 0; j < mGr1BlockSz; j++)
			dataCodewordGr1[i][j] = dataCodeword[dataCodewordsN-i*mGr1BlockSz-j];
	dataCodewordsN -= mGr1BlocksQty*mGr1BlockSz;
	for (int i = 0; i < mGr2BlocksQty; i++)
		for (int j = 0; j < mGr2BlockSz; j++)
			dataCodewordGr2[i][j] = dataCodeword[dataCodewordsN-i*mGr2BlockSz-j];
	BCHReedSolomon rs1(mGr1BlockSz, mErrCorrBlockSz), rs2(mGr2BlockSz, mErrCorrBlockSz);
	for (int i = 0; i < mGr1BlocksQty; i++)
		rs1.Encode(dataCodewordGr1[i], mGr1BlockSz, errCorrCodeword[i], mErrCorrBlockSz);
	for (int i = 0; i < mGr2BlocksQty; i++)
		rs2.Encode(dataCodewordGr2[i], mGr2BlockSz, errCorrCodeword[mGr1BlocksQty+i], mErrCorrBlockSz);

	// 4. Structure final message
	bitset8 finalCode;
	for (int j = mErrCorrBlockSz-1; j>=0; j--)
		for (int i = mLastErrCorrBlock; i>=0; finalCode.append(errCorrCodeword[i--][j]));
	for (int i = mGr2BlocksQty-1; i>=0; finalCode.append(dataCodewordGr2[i--][mGr2BlockSz-1]));
	for (int j = mGr1BlockSz-1; j>=0; j--)
	{
		for (int i = mGr2LastBlock; i>=0; finalCode.append(dataCodewordGr2[i--][j]));
		for (int i = mGr1LastBlock; i>=0; finalCode.append(dataCodewordGr1[i--][j]));
	}

	// 5. Placement in matrix
	// 6.1. Masking
	const int timingPatternColumn = mMaxPos-mTimingPatternRow;
	std::vector<MatrixType> matrix(mMaskPatternsQty, mFunctPatterns);
	MatrixType &usedPos = mUsedArea;
	int x = 0, y = mMaxPos;
	int yDir = -1;
	for (int bitN = finalCode.size()-1; bitN>=0; bitN--)
	{
		for (int maskN=0; maskN<mMaskPatternsQty; maskN++)
			matrix[maskN][y][x] = finalCode[bitN]^mMaskPattern[maskN][y][x];
		do
		{
			bool isXOdd = x%2;
			if (x<timingPatternColumn&&!isXOdd || x>timingPatternColumn&&isXOdd)
				x++;
			else if (yDir==-1&&y>0 || yDir==1&&y<mMaxPos)
			{
				x--;
				y += yDir;
			}
			else
			{
				if (++x==timingPatternColumn)
					x++;
				yDir = -yDir;
			}
		} while (usedPos[y][x]);
	}

	// 6.2 Determining the best mask
	int bestMaskN;
	long minPenalty = std::numeric_limits<long>::max();

	bitset likeFinderPattern1(mLikeFinderPatternSz, mFinderPattern[mFinderPatternSz/2]);
	bitset likeFinderPattern2(likeFinderPattern1<<mQuitetZone);
	bitset fragmentX(mLikeFinderPatternSz), fragmentY(mLikeFinderPatternSz);

	for (int maskN = 0; maskN<mMaskPatternsQty; maskN++)
	{
		const MatrixType &matrixN = matrix[maskN];
		long penalty = 0;

		for (int i=0; i<mMatrixSize; i++)
		{
			bool prevBitX=matrixN[i][0], prevBitY=matrixN[0][i];
			int sameBitsQtyX=1, sameBitsQtyY=1;
			for (int j=1; j<mMatrixSize; j++)
			{
				auto count = [&penalty](bool bit, bool &prevBit, int &sameBitsQty)
				{
					if (bit==prevBit)
						sameBitsQty++;
					else
					{
						if (sameBitsQty>=5)
							penalty+=sameBitsQty-2;
						prevBit=bit;
						sameBitsQty=1;
					}
				};
				count(matrixN[i][j], prevBitX, sameBitsQtyX);
				count(matrixN[j][i], prevBitY, sameBitsQtyY);
			}
		}

		for (int i=0, i1=1; i1<mMatrixSize; i++, i1++)
			for (int j=0, j1=1; j1<mMatrixSize; j++, j1++)
				if (matrixN[i][j]==matrixN[i][j1] && matrixN[i][j]==matrixN[i1][j] && matrixN[i][j]==matrixN[i1][j1])
					penalty+=3;

		for (int i=0; i<mMatrixSize; i++)
		{
			int j;
			for (j=0; j<mLikeFinderPatternSz; j++)
			{
				fragmentX[j] = matrixN[i][j];
				fragmentY[j] = matrixN[j][i];
			}
			do
			{
				if (fragmentX==likeFinderPattern1 || fragmentX==likeFinderPattern2)
					penalty+=40;
				if (fragmentY==likeFinderPattern1 || fragmentY==likeFinderPattern2)
					penalty+=40;
				if (++j<mMatrixSize)
				{
					(fragmentX>>=1)[mFragmentMaxPos] = matrixN[i][j];
					(fragmentY>>=1)[mFragmentMaxPos] = matrixN[j][i];
				}
			} while (j<mMatrixSize);
		}

		long setBits = 0;
		for (const bitset &row : matrixN)
			setBits += row.count();
		(setBits *= 100) /= mMatrixBitsQty;
		int value1 = setBits-setBits%5;
		int value2 = value1+5;
		value1 = abs(value1-50)/5;
		value2 = abs(value2-50)/5;
		penalty += std::min(value1,value2)*10;

		if (penalty<minPenalty)
		{
			bestMaskN = maskN;
			minPenalty = penalty;
		}
	}
	mMatrix = matrix[bestMaskN];

	// 7. Format && version info
	const int formatInfoSz = 15;
	const bitset formatInfo(formatInfoSz, mFormatInfo[bestMaskN][static_cast<int>(mErrCorrect)]);
	const int formatInfoRow = mFinderPatternSz+1, formatInfoColumn = mMaxPos-mFinderPatternSz-1;
	for (int i = 0; i<=5; i++)
	{
		mMatrix[i][formatInfoColumn] = formatInfo[i];
		mMatrix[formatInfoRow][mMaxPos-i] = formatInfo[formatInfoSz-1-i];
	}
	mMatrix[formatInfoRow-1][formatInfoColumn]	= formatInfo[6];
	mMatrix[formatInfoRow][formatInfoColumn]	= formatInfo[7];
	mMatrix[formatInfoRow][formatInfoColumn+1]	= formatInfo[8];
	int i;
	for (i=0; i<=7; i++)
		mMatrix[formatInfoRow][i] = formatInfo[i];
	for (; i<formatInfoSz; i++)
		mMatrix[mMatrixSize-formatInfoSz+i][formatInfoColumn] = formatInfo[i];

	if (mVersion>Version::V6)
	{
		const bitset versionInfo(3*6, mVersionInfo[static_cast<int>(mVersion)-6]);
		for (int i = 0; i<3; i++)
			for (int j = 0; j<6; j++)
				mMatrix[mMaxPos-mFinderPatternSz-3+i][mMaxPos-j] =
				mMatrix[j][mFinderPatternSz+3-i] =
					versionInfo[i+j*3];
	}

	return *this;
}

QRCode_Bitmatrix &QRCode_Bitmatrix::encode(const void *dataIn, size_t sz)
{
	assert(dataIn);
	return encode(std::string(static_cast<const char*>(dataIn), sz));
}

bool QRCode_Bitmatrix::matrix(int x, int y) const
{
	return mMatrix[y][mMaxPos-x];
}

int QRCode_Bitmatrix::minImageSize(int matrixSize)
{
	return matrixSize+mQuitetZone*2;
}