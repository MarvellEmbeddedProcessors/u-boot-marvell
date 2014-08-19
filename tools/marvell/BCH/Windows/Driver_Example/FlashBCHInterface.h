#pragma once
#include <windows.h>
//////////////////////////////////////////////////////////////////////////////////////////
// defines
//

#define DEFTYPEDEF(a,r)  typedef r ( *LPFNDLL##a )
#define DEFEXTERNN(a)	 extern LPFNDLL##a a;
#define DEF(a)			LPFNDLL##a a;
#define GETPROCADDRESS(a) a	= ( LPFNDLL##a ) GetProcAddress( handle, #a );	\
				if ( ! a ) { ::FreeLibrary( handle );	return NULL; }

DEFTYPEDEF( mrvlFlashBCHVersion, void )( char version[100]);
DEFTYPEDEF( mrvlCreateFlashBCH, int )( void );
DEFTYPEDEF( mrvlReleaseFlashBCH, int )( void );

DEFTYPEDEF( mrvlInitializeFlashBCH, int )(
						int _num_codewords,
						int* _codeword_lengths_bch,
						int _bch_t
						);

DEFTYPEDEF( mrvlGetParityLength, int )( void );

DEFTYPEDEF( mrvlEncodeBCH, int )(
						int* _input,
						int* _output
						);

DEFTYPEDEF( mrvlDecodeBCH, int )(
						int* _input,
						int* _output
						);

DEFTYPEDEF( mrvlGenerateRandomBits, void )(
						int* _random_array,
						int _length,
						int _seed
						);

DEFTYPEDEF( mrvlAddGaussianNoise, void )(
						int* _user_data,
						double* _noisy_data,
						int _length,
						double _snr,
						int _noise_seed
						);

DEFTYPEDEF( mrvlSwapData, void )(
						int* _input,
						int _bit_length,
						int* _output,
						int _word_size,
						int _byte_size,
						int _byte_swap_flag,
						int _bit_swap_flag
						);

HINSTANCE SetupMRVLFlashBCHLibrary( void );
