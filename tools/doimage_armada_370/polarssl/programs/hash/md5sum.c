/*
 *  md5sum demonstration program
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

#include <string.h>
#include <stdio.h>

#include "polarssl/md5.h"

static int md5_wrapper( char *filename, unsigned char *sum )
{
    int ret = md5_file( filename, sum );

    if( ret == 1 )
        fprintf( stderr, "failed to open: %s\n", filename );

    if( ret == 2 )
        fprintf( stderr, "failed to read: %s\n", filename );

    return( ret );
}

static int md5_print( char *filename )
{
    int i;
    unsigned char sum[16];

    if( md5_wrapper( filename, sum ) != 0 )
        return( 1 );

    for( i = 0; i < 16; i++ )
        printf( "%02x", sum[i] );

    printf( "  %s\n", filename );
    return( 0 );
}

static int md5_check( char *filename )
{
    int i;
    size_t n;
    FILE *f;
    int nb_err1, nb_err2;
    int nb_tot1, nb_tot2;
    unsigned char sum[16];
    char buf[33], line[1024];

    if( ( f = fopen( filename, "rb" ) ) == NULL )
    {
        printf( "failed to open: %s\n", filename );
        return( 1 );
    }

    nb_err1 = nb_err2 = 0;
    nb_tot1 = nb_tot2 = 0;

    memset( line, 0, sizeof( line ) );

    n = sizeof( line );

    while( fgets( line, n - 1, f ) != NULL )
    {
        n = strlen( line );

        if( n < 36 )
            continue;

        if( line[32] != ' ' || line[33] != ' ' )
            continue;

        if( line[n - 1] == '\n' ) { n--; line[n] = '\0'; }
        if( line[n - 1] == '\r' ) { n--; line[n] = '\0'; }

        nb_tot1++;

        if( md5_wrapper( line + 34, sum ) != 0 )
        {
            nb_err1++;
            continue;
        }

        nb_tot2++;

        for( i = 0; i < 16; i++ )
            sprintf( buf + i * 2, "%02x", sum[i] );

        if( memcmp( line, buf, 32 ) != 0 )
        {
            nb_err2++;
            fprintf( stderr, "wrong checksum: %s\n", line + 34 );
        }

        n = sizeof( line );
    }

    if( nb_err1 != 0 )
    {
        printf( "WARNING: %d (out of %d) input files could "
                "not be read\n", nb_err1, nb_tot1 );
    }

    if( nb_err2 != 0 )
    {
        printf( "WARNING: %d (out of %d) computed checksums did "
                "not match\n", nb_err2, nb_tot2 );
    }

    return( nb_err1 != 0 || nb_err2 != 0 );
}

int main( int argc, char *argv[] )
{
    int ret, i;

    if( argc == 1 )
    {
        printf( "print mode:  md5sum <file> <file> ...\n" );
        printf( "check mode:  md5sum -c <checksum file>\n" );

#ifdef WIN32
        printf( "\n  Press Enter to exit this program.\n" );
        fflush( stdout ); getchar();
#endif

        return( 1 );
    }

    if( argc == 3 && strcmp( "-c", argv[1] ) == 0 )
        return( md5_check( argv[2] ) );

    ret = 0;
    for( i = 1; i < argc; i++ )
        ret |= md5_print( argv[i] );

    return( ret );
}
