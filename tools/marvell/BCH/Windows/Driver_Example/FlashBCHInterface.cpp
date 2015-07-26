#include "FlashBCHInterface.h"

DEF( mrvlFlashBCHVersion )
DEF( mrvlCreateFlashBCH )
DEF( mrvlReleaseFlashBCH )
DEF( mrvlInitializeFlashBCH )
DEF( mrvlGetParityLength )
DEF( mrvlEncodeBCH )
DEF( mrvlDecodeBCH )
DEF( mrvlGenerateRandomBits )
DEF( mrvlAddGaussianNoise )
DEF( mrvlSwapData )


HINSTANCE SetupMRVLFlashBCHLibrary( void )
{
#ifdef _DEBUG
	HINSTANCE handle = ::LoadLibrary( TEXT( "O:\\Library\\FlashBCH\\mrvlFlashBCH.dll" ) );
#else
	HINSTANCE handle = ::LoadLibrary( TEXT( "O:\\Library\\FlashBCH\\mrvlFlashBCH.dll" ) );
#endif
	if ( !handle )
	{
			return NULL;
	}

	GETPROCADDRESS( mrvlFlashBCHVersion )
	GETPROCADDRESS( mrvlCreateFlashBCH )
	GETPROCADDRESS( mrvlReleaseFlashBCH )
	GETPROCADDRESS( mrvlInitializeFlashBCH )
	GETPROCADDRESS( mrvlGetParityLength )
	GETPROCADDRESS( mrvlEncodeBCH )
	GETPROCADDRESS( mrvlDecodeBCH )
	GETPROCADDRESS( mrvlGenerateRandomBits )
	GETPROCADDRESS( mrvlAddGaussianNoise )
	GETPROCADDRESS( mrvlSwapData )

	return handle;
}
