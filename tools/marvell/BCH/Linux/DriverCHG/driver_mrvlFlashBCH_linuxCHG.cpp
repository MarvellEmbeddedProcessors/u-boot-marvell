//*******************************************************************************************
// driver_mrvlFlashBCH_linux.cpp
//-------------------------------------------------------------------------------------------
// M A R V E L L   C O N F I D E N T I A L
//-------------------------------------------------------------------------------------------
// This script demonstrates how to call the FlashBCH library.
//
// <modified by gchen@marvell.com
// (1) takes input "bchEncInput.txt", which includes the data input to encoder
// (2) output "encodedOut.txt", which includes the data and parity.
//
//  modify "codeword_length_bch" for codeword size (number of bits input to the encoder)
//  modify "t_bch" for BCH T value
//
//-------------------------------------------------------------------------------------------
// Shashi Kiran Chilappagari (shashic@marvell.com) and Phong Sy Nguyen (psnguyen@marvell.com)
// DSP Group
// Marvell Semiconductor
//*******************************************************************************************

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <ctime>
#include "mrvlFlashBCH.h"

using namespace std;

void my_input ( int* iInput,
		const char* fileName,
		int size
		);

void my_print ( int* iToPrint,
		const char* fileName,
		int size
		);
void my_print_rev ( int* iToPrint,
		    const char* fileName,
		    int size
		    );

int main()
{
    char version[100];

    mrvlFlashBCHVersion( version );
    cout<<"mrvlFlashLDPC library version is "<<version<<endl;

    int function_status;
    int decoder_status;

    function_status = mrvlCreateFlashBCH( );

    int num_codewords		= 1;
    int codeword_length_bch	= 512*8;
    int t_bch			= 32;

    function_status = mrvlInitializeFlashBCH( num_codewords, &codeword_length_bch, t_bch );

    int parity_length_bch;
    parity_length_bch = mrvlGetParityLength( );

    int* user_bits		= new int[codeword_length_bch];
    int* encoded_bits		= new int[codeword_length_bch+parity_length_bch];
    double*	noisy_data	= new double[codeword_length_bch+parity_length_bch];
    int* corrupted_bits		= new int[codeword_length_bch+parity_length_bch];
    int* decoded_bits		= new int[codeword_length_bch];

    int data_seed = 123;
    double SNR = 14.0;
    int noise_seed = 12345;

    my_input( user_bits, "bchEncInput.txt", codeword_length_bch);
    //mrvlGenerateRandomBits( user_bits, codeword_length_bch, data_seed);

    function_status =  mrvlEncodeBCH( user_bits, encoded_bits );

    if( function_status == 1 ){
	cout<<"  sorry, mrvlEncodeBCH failed..."<<endl;
    } else {
	//my_print_rev( user_bits, "userdata", codeword_length_bch);
	my_print_rev( encoded_bits, "encodedOut", codeword_length_bch + parity_length_bch);
    }
    /*
    mrvlAddGaussianNoise( encoded_bits, noisy_data, codeword_length_bch+parity_length_bch, SNR, noise_seed );
    int num_bit_errors = 0;
    for (int i=0; i<codeword_length_bch + parity_length_bch; i++)
	{
	    corrupted_bits[i] = noisy_data[i] > 0.0 ? 0 : 1;
	    if (corrupted_bits[i] != encoded_bits[i])
		num_bit_errors++;
	}

    decoder_status = mrvlDecodeBCH( corrupted_bits, decoded_bits );
    int num_decoded_bit_errors = 0;
    for (int i=0; i<codeword_length_bch; i++)
	{
	    if (decoded_bits[i] != user_bits[i])
		num_decoded_bit_errors++;
	}

    cout<<"Before decoding, there are "<<num_bit_errors<<" bit errors!"<<endl<<endl;
    cout<<"After decoding, there are "<<num_decoded_bit_errors<<" bit errors!"<<endl<<endl;

    function_status = mrvlReleaseFlashBCH( );
    */

    delete [] user_bits;
    delete [] encoded_bits;
    delete [] corrupted_bits;
    delete [] decoded_bits;
    delete [] noisy_data;

    return 0;

}

void my_input ( int* iInput,
		const char* filename,
		int size
		)
{
    ifstream ifs;
    string line;
    char line_c[100];

    ifs.open(filename);
    if (ifs.is_open()) {
	unsigned long ul_temp;
	int line_cnt = size/4;
	for( int i=0; i<line_cnt; i++){
	    getline (ifs, line);
	    strcpy(line_c, line.c_str());
	    sscanf(line_c, "%x", &ul_temp);
	    for( int j=0; j<4; j++ ){
		iInput[ i*4 + j ] = 1 & (ul_temp >> (3-j));
	    }
	}
    } else {
	cout<<" error: file "<<filename<<" does not exit."<<endl;
    }
}

void my_print ( int* iToPrint,
		const char* fileName,
		int size
		)
{
    ofstream ofs;
    char cfilename [100];
    sprintf(cfilename,"%s.txt", fileName);
    ofs.open(cfilename);

    for( int i=0; i<size; i++ ){
	int i_char;
	if( i%4 == 0 ) {
	    i_char = iToPrint[ i ];
	} else if( i%4 == 1 ) {
	    i_char += (iToPrint[ i ] << 1 );
	} else if( i%4 == 2 ) {
	    i_char += (iToPrint[ i ] << 2 );
	} else if( i%4 == 3 ) {
	    i_char += (iToPrint[ i ] << 3 );
	    ofs << hex << i_char << endl;
	}
    }
}

void my_print_rev ( int* iToPrint,
		    const char* fileName,
		    int size
		    )
{
    ofstream ofs;
    char cfilename [100];
    sprintf(cfilename,"%s.txt", fileName);
    ofs.open(cfilename);

    for( int i=0; i<size; i++ ){
	int i_char;
	if( i%4 == 0 ) {
	    i_char = (iToPrint[ i ] << 3 );
	} else if( i%4 == 1 ) {
	    i_char += (iToPrint[ i ] << 2 );
	} else if( i%4 == 2 ) {
	    i_char += (iToPrint[ i ] << 1 );
	} else if( i%4 == 3 ) {
	    i_char += iToPrint[ i ];
	    ofs << hex << i_char << endl;
	}
    }
}
