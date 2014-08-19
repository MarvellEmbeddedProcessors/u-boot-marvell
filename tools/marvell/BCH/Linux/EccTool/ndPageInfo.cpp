#include "ndPageInfo.h"

typedef struct {
	int chnkSz;
	int chnkCnt;
	int chnkSpr;
	int lastChnkSz;
	int lastChnkSpr;
} pageInfo_t;

typedef struct {
	int			isValid;
	pageInfo_t	pageInfo;
} nandFlashDB_t;

const nandFlashDB_t nandFlashDbTbl[PAGE_SIZE_MAX][ECC_MODE_MAX] = {
	{
		{1, {2048,  1, 32,   0,   0}},
		{0, {0}},
		{0, {0}},
		{0, {0}},
	},
	{
		{1,	{2048,  2, 32,   0,   0}},
		{1,	{1024,  4,  0,   0,  64}},
		{0, {0}},
		{0, {0}},
	},
	{
		{1,	{2048,  4, 32,   0,   0}},
		{1,	{1024,  8,  0,   0, 160}},
		{0, {0}},
		{0, {0}},
	},
	{
		{0, {0}},
		{0, {0}},
		{0, {0}},
		{0, {0}},
	},
};

ndPageInfo::ndPageInfo(pageSize_e pageSize, eccMode_e eccMode) {
	const nandFlashDB_t *nandFlash = &nandFlashDbTbl[pageSize][eccMode];
	this->isValid = nandFlash->isValid;

	if (this->isValid) {
		const pageInfo_t *pageInfo = &(nandFlash->pageInfo);

		this->pageSz  = pageSize;
		this->eccMode = eccMode;

		this->chnkSz  = pageInfo->chnkSz;
		this->chnkCnt = pageInfo->chnkCnt;
		this->chnkSpr = pageInfo->chnkSpr;
		this->lastChnkSz  = pageInfo->lastChnkSz;
		this->lastChnkSpr = pageInfo->lastChnkSpr;
	}
}

ndPageInfo::ndPageInfo(ndPageInfo *pageInfo) {
	this->isValid = pageInfo->isPageInfoValid();

	this->pageSz  = pageInfo->getPageSz();
	this->eccMode = pageInfo->getEccMode();

	this->chnkSz  = pageInfo->getChunkSz();
	this->chnkCnt = pageInfo->getChunkCnt();
	this->chnkSpr = pageInfo->getChunkSpr();

	this->lastChnkSz  = pageInfo->getLastChunkSz();
	this->lastChnkSpr = pageInfo->getLastChunkSpr();
}

int ndPageInfo::isPageInfoValid() {
	return this->isValid;
}

pageSize_e ndPageInfo::getPageSz() {
	return this->pageSz;
}

eccMode_e ndPageInfo::getEccMode() {
	return this->eccMode;
}

int ndPageInfo::getChunkSz() {
	return this->chnkSz;
}

int ndPageInfo::getChunkCnt() {
	return this->chnkCnt;
}

int ndPageInfo::getChunkSpr() {
	return this->chnkSpr;
}

int ndPageInfo::getLastChunkSz() {
	return this->lastChnkSz;
}

int ndPageInfo::getLastChunkSpr() {
	return this->lastChnkSpr;
}
