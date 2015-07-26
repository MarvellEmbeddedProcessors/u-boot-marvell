#ifndef BUILDECCIMG_H
#define BUILDECCIMG_H

//#define ECCTOOL_DEBUG

#ifdef ECCTOOL_DEBUG
#define ECCTOOL_PRINT(format, ...) printf (format, ##__VA_ARGS__)
#else
#define ECCTOOL_PRINT
#endif

class bldEccImg{
public:
	bldEccImg(const char *image);
	~bldEccImg();
	void buildNandEccImage(ndPageInfo *pageAlloc);
private:
	void initBch(int m, int kNorm, int kLast);
	void readInBitStream(int dataSz, int padBytes);
	void writeOutBitStream(int dataSz, int padBytes);
	void dumpPageData(ndPageInfo *pageAlloc);

	fstream inputFs;
	fstream outputFs;

	int *bitStmIn;
	int	*bitStmOut;

#ifndef HAS_MRVL_FLASH_LDPC_LIB
	BCH *bchNormChnk;
	BCH *bchLastChnk;
#endif
};

#endif
