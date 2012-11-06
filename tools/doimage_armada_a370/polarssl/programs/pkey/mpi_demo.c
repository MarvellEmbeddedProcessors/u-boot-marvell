/*
 *  Simple MPI demonstration program
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

int main( void )
{
    mpi E, P, Q, N, H, D, X, Y, Z;

    mpi_init( &E, &P, &Q, &N, &H,
              &D, &X, &Y, &Z, NULL );

    mpi_read_string( &P, 10, "2789" );
    mpi_read_string( &Q, 10, "3203" );
    mpi_read_string( &E, 10,  "257" );
    mpi_mul_mpi( &N, &P, &Q );

    printf( "\n  Public key:\n\n" );
    mpi_write_file( "  N = ", &N, 10, NULL );
    mpi_write_file( "  E = ", &E, 10, NULL );

    printf( "\n  Private key:\n\n" );
    mpi_write_file( "  P = ", &P, 10, NULL );
    mpi_write_file( "  Q = ", &Q, 10, NULL );

    mpi_sub_int( &P, &P, 1 );
    mpi_sub_int( &Q, &Q, 1 );
    mpi_mul_mpi( &H, &P, &Q );
    mpi_inv_mod( &D, &E, &H );

    mpi_write_file( "  D = E^-1 mod (P-1)*(Q-1) = ",
                    &D, 10, NULL );

    mpi_read_string( &X, 10, "55555" );
    mpi_exp_mod( &Y, &X, &E, &N, NULL );
    mpi_exp_mod( &Z, &Y, &D, &N, NULL );

    printf( "\n  RSA operation:\n\n" );
    mpi_write_file( "  X (plaintext)  = ", &X, 10, NULL );
    mpi_write_file( "  Y (ciphertext) = X^E mod N = ", &Y, 10, NULL );
    mpi_write_file( "  Z (decrypted)  = Y^D mod N = ", &Z, 10, NULL );
    printf( "\n" );

    mpi_free( &Z, &Y, &X, &D, &H,
              &N, &Q, &P, &E, NULL );

#ifdef WIN32
    printf( "  Press Enter to exit this program.\n" );
    fflush( stdout ); getchar();
#endif

    return( 0 );
}
