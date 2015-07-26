#ifndef _MRVL_IFLASHBCH_H
#define _MRVL_IFLASHBCH_H

#ifdef WIN32
	#ifdef MRVLFLASH_EXPORTS
		#ifndef _LIB
			#define MRVLFLASHBCH_API
		#else
			#define MRVLFLASHBCH_API __declspec(dllexport)
		#endif
	#else
		#define MRVLFLASHBCH_API __declspec(dllimport)       // for dll
	#endif
#else
		#define MRVLFLASHBCH_API __attribute__ ((visibility ("default")))
#endif

MRVLFLASHBCH_API void	mrvlFlashBCHVersion( char version[100] );

MRVLFLASHBCH_API int mrvlCreateFlashBCH( void );

MRVLFLASHBCH_API int mrvlReleaseFlashBCH( void );

MRVLFLASHBCH_API int mrvlInitializeFlashBCH( int _num_codewords, int * _codeword_lengths_bch, int _bch_t );

MRVLFLASHBCH_API int mrvlGetParityLength( void );

MRVLFLASHBCH_API int mrvlEncodeBCH( int* _input, int* _output);

MRVLFLASHBCH_API int mrvlDecodeBCH( int* _input, int* _output );

MRVLFLASHBCH_API void mrvlGenerateRandomBits( int* randomArray, int length, int seed );

MRVLFLASHBCH_API void mrvlAddGaussianNoise( int* user_data,double *noisy_data, int length, double snr, int seed );

MRVLFLASHBCH_API void mrvlSwapData( int* input, int bit_len, int* output, int wordSize, int byteSize, int byte_swap_flag, int bit_swap_flag );

#endif
