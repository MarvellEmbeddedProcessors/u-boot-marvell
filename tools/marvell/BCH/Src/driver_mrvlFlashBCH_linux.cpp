//*******************************************************************************************
// driver_mrvlFlashBCH_linux.cpp
//-------------------------------------------------------------------------------------------
// M A R V E L L   C O N F I D E N T I A L
//-------------------------------------------------------------------------------------------
// This script demonstrates how to call the FlashBCH library.
//-------------------------------------------------------------------------------------------
// Shashi Kiran Chilappagari (shashic@marvell.com) and Phong Sy Nguyen (psnguyen@marvell.com)
// DSP Group
// Marvell Semiconductor
//*******************************************************************************************

#include <iostream>
#include <fstream>
#include <ctime>
#include "mrvlFlashBCH.h"


int main()
{

	char version[100];

	using std::cout;
	using namespace std;

	mrvlFlashBCHVersion( version );

	std::cout << "mrvlFlashLDPC library version is " << version << std::endl;

	int function_status;
	int decoder_status;

	function_status = mrvlCreateFlashBCH( );

	int num_codewords		= 1;
	int codeword_length_bch = 17888;
	int t_bch				= 119;

	function_status = mrvlInitializeFlashBCH( num_codewords, &codeword_length_bch, t_bch );

	int parity_length_bch;
	parity_length_bch = mrvlGetParityLength( );

	int* user_bits			= new int[codeword_length_bch];
	int* encoded_bits		= new int[codeword_length_bch+parity_length_bch];
	double*	noisy_data		= new double[codeword_length_bch+parity_length_bch];
	int* corrupted_bits		= new int[codeword_length_bch+parity_length_bch];
	int* decoded_bits		= new int[codeword_length_bch];

	int data_seed = 123;
	double SNR = 14.0;
	int noise_seed = 12345;

	mrvlGenerateRandomBits( user_bits, codeword_length_bch, data_seed);

	function_status =  mrvlEncodeBCH( user_bits, encoded_bits );

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

	std::cout<<"Before decoding, there are "<<num_bit_errors<<" bit errors!"<<std::endl<<std::endl;
	std::cout<<"After decoding, there are "<<num_decoded_bit_errors<<" bit errors!"<<std::endl<<std::endl;

	function_status = mrvlReleaseFlashBCH( );

	delete [] user_bits;
	delete [] encoded_bits;
	delete [] corrupted_bits;
	delete [] decoded_bits;
	delete [] noisy_data;

	return 0;

}
