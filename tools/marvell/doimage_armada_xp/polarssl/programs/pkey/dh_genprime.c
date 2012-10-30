/*
 *  Diffie-Hellman-Merkle key exchange (prime generation)
 *
 *  Based on XySSL: Copyright (C) 2006-2008  Christophe Devine
 *
 *  Copyright (C) 2009  Paul Bakker <polarssl_maintainer at polarssl dot org>
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of PolarSSL or XySSL nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include <stdio.h>

#include "polarssl/bignum.h"
#include "polarssl/config.h"
#include "polarssl/havege.h"

/*
 * Note: G = 4 is always a quadratic residue mod P,
 * so it is a generator of order Q (with P = 2*Q+1).
 */
#define DH_P_SIZE 1024
#define GENERATOR "4"

int main( void )
{
    int ret = 1;

#if defined(POLARSSL_GENPRIME)
    mpi G, P, Q;
    havege_state hs;
    FILE *fout;

    mpi_init( &G, &P, &Q, NULL );
    mpi_read_string( &G, 10, GENERATOR );

    printf( "\n  . Seeding the random number generator..." );
    fflush( stdout );

    havege_init( &hs );

    printf( " ok\n  . Generating the modulus, please wait..." );
    fflush( stdout );

    /*
     * This can take a long time...
     */
    if( ( ret = mpi_gen_prime( &P, DH_P_SIZE, 1,
                               havege_rand, &hs ) ) != 0 )
    {
        printf( " failed\n  ! mpi_gen_prime returned %d\n\n", ret );
        goto exit;
    }

    printf( " ok\n  . Verifying that Q = (P-1)/2 is prime..." );
    fflush( stdout );

    if( ( ret = mpi_sub_int( &Q, &P, 1 ) ) != 0 )
    {
        printf( " failed\n  ! mpi_sub_int returned %d\n\n", ret );
        goto exit;
    }

    if( ( ret = mpi_div_int( &Q, NULL, &Q, 2 ) ) != 0 )
    {
        printf( " failed\n  ! mpi_div_int returned %d\n\n", ret );
        goto exit;
    }

    if( ( ret = mpi_is_prime( &Q, havege_rand, &hs ) ) != 0 )
    {
        printf( " failed\n  ! mpi_is_prime returned %d\n\n", ret );
        goto exit;
    }

    printf( " ok\n  . Exporting the value in dh_prime.txt..." );
    fflush( stdout );

    if( ( fout = fopen( "dh_prime.txt", "wb+" ) ) == NULL )
    {
        ret = 1;
        printf( " failed\n  ! Could not create dh_prime.txt\n\n" );
        goto exit;
    }

    if( ( ret = mpi_write_file( "P = ", &P, 16, fout ) != 0 ) ||
        ( ret = mpi_write_file( "G = ", &G, 16, fout ) != 0 ) )
    {
        printf( " failed\n  ! mpi_write_file returned %d\n\n", ret );
        goto exit;
    }

    printf( " ok\n\n" );
    fclose( fout );

exit:

    mpi_free( &Q, &P, &G, NULL );
#else
    printf( "\n  ! Prime-number generation is not available.\n\n" );
#endif

#ifdef WIN32
    printf( "  Press Enter to exit this program.\n" );
    fflush( stdout ); getchar();
#endif

    return( ret );
}
