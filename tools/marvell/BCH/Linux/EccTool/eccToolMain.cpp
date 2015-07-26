//*******************************************************************************************
// eccToolMain.cpp
//-------------------------------------------------------------------------------------------
// M A R V E L L   C O N F I D E N T I A L
//-------------------------------------------------------------------------------------------
// This script demonstrates how to encode the binary file with FlashBCH library.
//
//  modify "codeword_length_bch" for codeword size (number of bits input to the encoder)
//  modify "t_bch" for BCH T value
//
//-------------------------------------------------------------------------------------------
// Wilson Ding (dingwei@marvell.com	)
// NCD
// Marvell Semiconductor
//*******************************************************************************************

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <ctime>
#include <getopt.h>

using namespace std;

#ifdef HAS_MRVL_FLASH_LDPC_LIB
#include "../../Src/mrvlFlashBCH.h"
#else
#include "bch.h"
#endif
#include "ndPageInfo.h"
#include "buildEccImage.h"


void usage(void) {
	cout << "USAGE: " << endl;
	cout << "  ecc_tool -b crct_cap -p page_size -i input_file " << endl;
	cout << "  ecc_tool -h " << endl;
	cout << endl;
	cout << "DESCRIPTION: " << endl;
	cout << "  \'ecc_tool\' generates the binary image with ECC bytes. " << endl;
	cout << "  The file name of the output image is appended with suffix \'.out\'. " << endl;
	cout << "  The ECC bytes are calculated by the data of fix chunk size, " << endl;
	cout << "  which is defined in Marvell NFC driver. " << endl;
	cout << endl;
	cout << "OPTIONS: " << endl;
	cout << "	-b <crct_cap>      -- BCH bits correction capability (4/8) " << endl;
	cout << "	-h                 -- ecc_tool help " << endl;
	cout << "	-i <input_file>    -- input image name " << endl;
	cout << "	-p <page_size>     -- page size (2048/4096/8192) " << endl;
	cout << endl;
}

int main(int argc, char ** argv) {
	char ver[100];
	char *inputFn = NULL;
	pageSize_e pageSz = PAGE_SIZE_2K;
	eccMode_e eccMode = ECC_MODE_4BITS;
	ndPageInfo *pageInfo = NULL;
	bldEccImg  *eccImg  = NULL;
	int enLog = 0;
	int ch, opt;

#ifdef HAS_MRVL_FLASH_LDPC_LIB
	mrvlFlashBCHVersion(ver);
	cout << "Marvell Flash LDPC Library Version " << ver << endl;
#endif

	opterr = 0;
	while((ch = getopt(argc, argv, "b:p:i:h")) != -1)  {
		switch (ch) {
			case 'b':
				opt = atoi(optarg);
				switch(opt) {
					case 4:
						eccMode = ECC_MODE_4BITS;
						break;
					case 8:
						eccMode = ECC_MODE_8BITS;
						break;
					default:
						cout << "ERROR: invalid ECC mode " << endl;
						return -1;
				}
				ECCTOOL_PRINT("DEBUG: get option - ECC mode = %d bits\n", opt);
				break;
			case 'p':
				opt = atoi(optarg);
				switch(opt) {
					case 2048:
						pageSz = PAGE_SIZE_2K;
						break;
					case 4096:
						pageSz = PAGE_SIZE_4K;
						break;
					case 8192:
						pageSz = PAGE_SIZE_8K;
						break;
					default:
						cout << "ERROR: invalid page size " << endl;
						return -1;
				}
				ECCTOOL_PRINT("DEBUG: get option - page size = %d\n", opt);
				break;
			case 'i':
				inputFn = optarg;
				ECCTOOL_PRINT("DEBUG: get option - image name = %s\n", inputFn);
				break;
			case 'h':
				usage();
				return 0;
			default:
				cout << "ERROR: unrecognized option: " << optopt << endl;
				return -1;
		}
	}

	if (inputFn != NULL) {
		ifstream ifs(inputFn);
		if (!ifs) {
			cout << "ERROR: cannot open input image file " << endl;
			return -1;
		}
		ifs.close();
	} else {
		cout << "ERROR: input image file not specified " << endl;
		usage();
		return -1;
	}

	pageInfo = new ndPageInfo(pageSz, eccMode);

	if (pageInfo->isPageInfoValid()) {
		eccImg = new bldEccImg(inputFn);
		eccImg->buildNandEccImage(pageInfo);
	} else {
		cout << "ERROR: ECC mode not support " << endl;
		return -1;
	}

	delete eccImg;
	delete pageInfo;

	return 0;
}
