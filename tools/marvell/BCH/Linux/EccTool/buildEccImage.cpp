#include <memory.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <stdarg.h>

using namespace std;

#ifdef HAS_MRVL_FLASH_LDPC_LIB
#include "../../Src/mrvlFlashBCH.h"
#else
#include "bch.h"
#endif
#include "ndPageInfo.h"
#include "buildEccImage.h"
#ifdef HAS_MRVL_FLASH_LDPC_LIB
int  binary2hex(int *a, int len);
int  binary2hex_rev(int *a, int len);
void bits2symbols_rev(int *bits, int *symbols, int bit_len, int width);
#else
#include "utilities.h"
#define EXP2_CODE_FILE 		"exp2Int15.txt"
#endif

#define DWORD_SIZE 			sizeof(int)
#define BCH_PARAM_T			16
#define NDBF_BUS_WIDTH 		(64/sizeof(char))


bldEccImg::bldEccImg(const char *image) {
	char eccImage[256];
	this->inputFs.open(image, ios::in|ios::binary);
	strcpy(eccImage, image);
	strcat(eccImage, ".out");
	this->outputFs.open(eccImage, ios::out|ios::trunc|ios::binary);
}

bldEccImg::~bldEccImg() {
	this->inputFs.close();
	this->outputFs.close();

#ifndef HAS_MRVL_FLASH_LDPC_LIB
	delete bchNormChnk;
	if (bchLastChnk)
		delete bchLastChnk;
#endif
}

void bldEccImg::buildNandEccImage(ndPageInfo *pageAlloc) {
	int m = 15;	/* GF field degree, Fixed */
	int kNorm, kLast;

	int i = 0;

	ECCTOOL_PRINT("INFO: start building ECC image\n");

	kNorm = (pageAlloc->getChunkSz()+ pageAlloc->getChunkSpr())*8;          /* encoding data length (bits) for normal chunks */
	kLast = (pageAlloc->getLastChunkSz()+ pageAlloc->getLastChunkSpr())*8;  /* encoding data length (bits) for last chunks */

	this->initBch(m, kNorm, kLast);

	while (!this->inputFs.eof()) {
		ECCTOOL_PRINT("INFO: populating page %d\n", i);
		this->dumpPageData(pageAlloc);
		i++;
	}

	ECCTOOL_PRINT("INFO: end building ECC image\n");
}

void bldEccImg::initBch(int m, int kNorm, int kLast) {
#ifdef HAS_MRVL_FLASH_LDPC_LIB
	mrvlCreateFlashBCH();
#else
	int p[16] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1}; /* p(x) - Polynomial, Fixed */
	char *file = EXP2_CODE_FILE;

	GF *gf = new GF2m(m, p, m, file);

	this->bchNormChnk = new BCH(gf, 0, BCH_PARAM_T, 1, kNorm);
	if (kLast)
		this->bchLastChnk = new BCH(gf, 0, BCH_PARAM_T, 1, kLast);
	else
		this->bchLastChnk = NULL;

	delete gf;
#endif
}

void bldEccImg::readInBitStream(int dataSz, int padBytes) {
	char dataBuff;
	int i, j, k;

	ECCTOOL_PRINT("Data In:\n");
	for (i = j = k = 0; i < (dataSz+padBytes); i++) {
		if (i < dataSz) {
			this->inputFs.read(&dataBuff, sizeof(dataBuff));
			if (this->inputFs.eof())
				dataBuff = 0xff; /* padding data area with '1' */
		} else {
			dataBuff = 0xff;     /* padding spare area with '1' */
		}

		ECCTOOL_PRINT("%02x ", (unsigned char)dataBuff);
		if (j++ == 15) {
			ECCTOOL_PRINT("\n");
			j = 0;
		}

		// LittleEndia + LSB
		bitStmIn[k++] = (dataBuff & 0x01) ? 1 : 0;
		bitStmIn[k++] = (dataBuff & 0x02) ? 1 : 0;
		bitStmIn[k++] = (dataBuff & 0x04) ? 1 : 0;
		bitStmIn[k++] = (dataBuff & 0x08) ? 1 : 0;
		bitStmIn[k++] = (dataBuff & 0x10) ? 1 : 0;
		bitStmIn[k++] = (dataBuff & 0x20) ? 1 : 0;
		bitStmIn[k++] = (dataBuff & 0x40) ? 1 : 0;
		bitStmIn[k++] = (dataBuff & 0x80) ? 1 : 0;
	}
	ECCTOOL_PRINT("\n");
}

void bldEccImg::writeOutBitStream(int dataSz, int padBytes) {
	unsigned int *bitStmOutSym = new unsigned int[dataSz/DWORD_SIZE];

	char dataBuff;
	int i, j;

	bits2symbols_rev(bitStmOut, (int *)bitStmOutSym, dataSz, DWORD_SIZE);

	ECCTOOL_PRINT("Data Out:\n");
	for (i = 0, j = 0; i < dataSz/DWORD_SIZE; i = i + 2) {
		dataBuff = ((bitStmOutSym[i] & 0x0F) << 4) | (bitStmOutSym[i + 1]);

		dataBuff = (dataBuff & 0xAA) >> 1 | (dataBuff & 0x55) << 1;
		dataBuff = (dataBuff & 0xCC) >> 2 | (dataBuff & 0x33) << 2;
		dataBuff = (dataBuff & 0xF0) >> 4 | (dataBuff & 0x0F) << 4;

		this->outputFs.write(&dataBuff, sizeof(dataBuff));

		ECCTOOL_PRINT("%02x ", (unsigned char)dataBuff);
		if (j++ == 15) {
			ECCTOOL_PRINT("\n");
			j = 0;
		}
	}

	delete [] bitStmOutSym;

	dataBuff = 0xFF;
	for (i = 0; i < padBytes; i++) {
		this->outputFs.write(&dataBuff, sizeof(dataBuff));

		ECCTOOL_PRINT("%02x ", (unsigned char)dataBuff);
		if (j++ == 15) {
			ECCTOOL_PRINT("\n");
			j = 0;
		}
	}
	ECCTOOL_PRINT("\n");
}

void bldEccImg::dumpPageData(ndPageInfo *pageAlloc) {
	int chnkSz, sprSz, chnkCnt;
	int bitStmSzIn, bitStmSzOut;
	int padSz;
	int i;
	int last = (pageAlloc->getLastChunkSz()+ pageAlloc->getLastChunkSpr())*8;

	chnkSz  = pageAlloc->getChunkSz();
	sprSz   = pageAlloc->getChunkSpr();
	chnkCnt = pageAlloc->getChunkCnt();

	bitStmSzIn  = (chnkSz + sprSz)*8;
	ECCTOOL_PRINT("DEBUG: Code Len = %d \n", bitStmSzIn);
#ifdef HAS_MRVL_FLASH_LDPC_LIB
	mrvlInitializeFlashBCH(1, &bitStmSzIn, BCH_PARAM_T);
	bitStmSzOut = mrvlGetParityLength();
	bitStmSzOut += bitStmSzIn;
	ECCTOOL_PRINT("DEBUG: Parity Len = %d \n", bitStmSzOut);
#else
	bitStmSzOut = this->bchNormChnk->getN();
#endif

	this->bitStmIn  = new int[bitStmSzIn];
	this->bitStmOut = new int[bitStmSzOut];

	padSz = NDBF_BUS_WIDTH - ((bitStmSzOut / (DWORD_SIZE * 2)) % NDBF_BUS_WIDTH);
	padSz = (padSz == NDBF_BUS_WIDTH) ? 0 : padSz;
	ECCTOOL_PRINT("DEBUG: padding size = %d \n", padSz);

	for (i = 0; i < chnkCnt; i++) {
		ECCTOOL_PRINT("INFO: calculate ECC bytes - chunk %d/%d \n", i+1, chnkCnt);
		this->readInBitStream(chnkSz, sprSz);

#ifdef HAS_MRVL_FLASH_LDPC_LIB
		if (mrvlEncodeBCH(this->bitStmIn, this->bitStmOut) == 1)
			ECCTOOL_PRINT("ERROR: failed to encode the input bitstream \n");
#else
		this->bchNormChnk->encode(this->bitStmIn, this->bitStmOut);
#endif

#ifdef HAS_MRVL_FLASH_LDPC_LIB
		if ((i == (chnkCnt - 1)) && (!last))
#else
		if ((i == (chnkCnt - 1)) && (this->bchLastChnk == NULL))
#endif
			this->writeOutBitStream(bitStmSzOut, padSz);
		else
			this->writeOutBitStream(bitStmSzOut, 0);
	}

	delete [] this->bitStmIn;
	delete [] this->bitStmOut;

	if (last) {
		ECCTOOL_PRINT("INFO: calculate ECC bytes - last chunk \n");

		bitStmSzIn  = last;
		ECCTOOL_PRINT("DEBUG: Code Len = %d \n", bitStmSzIn);
#ifdef HAS_MRVL_FLASH_LDPC_LIB
		mrvlInitializeFlashBCH(1, &bitStmSzIn, BCH_PARAM_T);
		bitStmSzOut = mrvlGetParityLength();
		bitStmSzOut += bitStmSzIn;
		ECCTOOL_PRINT("DEBUG: Parity Len = %d \n", bitStmSzOut);
#else
		bitStmSzOut = this->bchLastChnk->getN();
#endif

		this->bitStmIn  = new int[bitStmSzIn];
		this->bitStmOut = new int[bitStmSzOut];

		padSz = NDBF_BUS_WIDTH - ((bitStmSzOut / (DWORD_SIZE * 2)) % NDBF_BUS_WIDTH);
		padSz = (padSz == NDBF_BUS_WIDTH) ? 0 : padSz;
		ECCTOOL_PRINT("DEBUG: padding size = %d \n", padSz);

		this->readInBitStream(chnkSz, sprSz);
#ifdef HAS_MRVL_FLASH_LDPC_LIB
		if (mrvlEncodeBCH(this->bitStmIn, this->bitStmOut) == 1)
			ECCTOOL_PRINT("ERROR: failed to encode the input bitstream \n");
#else
		this->bchLastChnk->encode(this->bitStmIn, this->bitStmOut);
#endif
		this->writeOutBitStream(bitStmSzOut, padSz);

		delete [] this->bitStmIn;
		delete [] this->bitStmOut;
	}
}


#ifdef HAS_MRVL_FLASH_LDPC_LIB
int binary2hex(int *a, int len) {
	int h = 0, i;

	for (i = 0; i < len; i++)
		h += a[i] * (1 << i);
	return h;
}

int binary2hex_rev(int *a, int len) {
	int i, s;
	int *tmp = new int[len];

	for(i = 0; i < len; i++)
		tmp[i] = a[len-1-i];

	s = binary2hex(tmp,len);
	delete [] tmp;

	return s;
}

void bits2symbols_rev(int *bits, int *symbols, int bit_len, int width) {
	int i;

	for(i = 0; i < bit_len/width; i++)
		symbols[i] = binary2hex_rev(bits + i*width, width);
}
#endif
