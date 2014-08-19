#ifndef NDPAGEINFO_H
#define NDPAGEINFO_H

typedef enum {
	PAGE_SIZE_2K,
	PAGE_SIZE_4K,
	PAGE_SIZE_8K,
	PAGE_SIZE_16K,
	PAGE_SIZE_MAX,
} pageSize_e;

typedef enum {
	ECC_MODE_4BITS,
	ECC_MODE_8BITS,
	ECC_MODE_12BITS,
	ECC_MODE_16BITS,
	ECC_MODE_MAX,
} eccMode_e;

class ndPageInfo{
public:
	ndPageInfo(pageSize_e pageSize, eccMode_e eccMode);
	ndPageInfo(ndPageInfo *pageInfo);
	~ndPageInfo(){}
	ndPageInfo *clone(){return new ndPageInfo(this);}

	int        isPageInfoValid();
	pageSize_e getPageSz();
	eccMode_e  getEccMode();
	int        getChunkSz();
	int        getChunkCnt();
	int        getChunkSpr();
	int        getLastChunkSz();
	int        getLastChunkSpr();

private:
	int        isValid;
	pageSize_e pageSz;
	eccMode_e  eccMode;
	int        chnkSz;
	int        chnkCnt;
	int        chnkSpr;
	int        lastChnkSz;
	int        lastChnkSpr;
};

#endif
