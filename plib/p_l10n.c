/*
    Plib
    Copyright (C) 2014 Stanislas Marquis <smarquis@astrorigin.ch>

    This program is free software: you can redistribute it and/or modify it
    under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  \file p_l10n.c
 *  \brief Localization of source code.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 *
 *  This should be replaced by a real scanner (xml?).
 *
 *  Produce a C file for a programmer.
 *
 *  Usage: p_l10n <input file> <varname> > <output file>
 *         cat <input files> | p_l10n -- <varname> > <output file>
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef P_NO_MEMPOOL
#include "p_mempool.h"
#endif
#include "p_string.h"

const char templ_1[] =
"/*\n\tFile generated by p_l10n - Do not edit\n*/\n\n"
"#include <stddef.h>\n\n"
"const char %s[] = {\n";

const char templ_2[] =
"\n};\n\n"
"const size_t sizeof_%s = %lu;\n\n";

void
stop( const char* msg )
{
    fprintf( stderr, "\nERROR %s\n", msg );
    exit( EXIT_FAILURE );
}

void
echo( char* p, size_t sz, const char islast )
{
    size_t i;
    for ( i = 0; i < sz; ++i )
    {
        /*fprintf( stdout, "%c,", *p );*/
        fprintf( stdout, "0x%hhX,", *p );
        ++p;
    }
    fprintf( stdout, islast ? "0x%hhX" : "0x%hhX,", '\0' );
}

int main( int argc, char* argv[] )
{
#ifndef P_NO_MEMPOOL
    p_MemPool mp;
#endif
    p_String tmp;
    char* p, *q;
    size_t sz = 0;

    if ( argc < 3 )
        stop( "Missing arguments" );

    if ( argv[1][0] != '-' )
    {
        if ( !freopen( argv[1], "r", stdin ))
            stop( "Invalid input file" );
    }

#ifndef P_NO_MEMPOOL
    if ( !p_mempool_init( &mp, 0 ))
        stop( "nomem" );
    p_mempool_set( &mp );
#endif

    if ( !p_string_init( &tmp, "" ))
        stop( "nomem" );

    while ( !feof( stdin ))
    {
        char buf[512];
        size_t z = fread( buf, 1, 512, stdin );
        if ( !p_string_cat_len( &tmp, buf, z ))
            stop( "nomem" );
    }

    fprintf( stdout, templ_1, argv[2] );

    p = tmp.data;
    while ( *p && ( p = strstr( p, "***CONTEXT***\n" )))
    {
        char* ctxt;
        size_t ctxtsz;
        char* txt;
        size_t txtsz;
        char* plural;
        size_t pluralsz;
        char* tr1;
        size_t tr1sz;
        char* tr2;
        size_t tr2sz;
        char islast = 0;

        p += 14;
        if ( ( q = strstr( p, "*TEXT*\n" )) == p )
            ctxt = NULL;
        else
        {
            ctxt = p;
            ctxtsz = q - p - 1;
        }
        q += 7;
        if ( ( p = strstr( q, "*PLURAL*\n" )) == q )
            stop( "Missing text" );
        else
        {
            txt = q;
            txtsz = p - q - 1;
        }
        p += 9;
        if ( ( q = strstr( p, "*TRANSLATION TEXT*\n" )) == p )
            plural = NULL;
        else
        {
            plural = p;
            pluralsz = q - p - 1;
        }
        q += 19;
        if ( ( p = strstr( q, "*TRANSLATION PLURAL*\n" )) == q )
            stop( "Missing translation" );
        else
        {
            tr1 = q;
            tr1sz = p - q - 1;
        }
        p += 21;
        if ( ( q = strstr( p, "***CONTEXT***\n" )) == p )
            tr2 = NULL;
        else
        {
            if ( q == NULL )
            {
                islast = 1;
                q = strchr( p, '\0' );
            }
            tr2 = p;
            tr2sz = q - p - 1;
        }
        if ( ctxt )
        {
            sz += ctxtsz + 1;
            echo( ctxt, ctxtsz, 0 );
        }
        else
        {
            sz += 1;
            fprintf( stdout, "0x%hhX,", '\0' );
        }
        sz += txtsz + 1;
        echo( txt, txtsz, 0 );
        if ( plural )
        {
            sz += pluralsz + 1;
            echo( plural, pluralsz, 0 );
        }
        else
        {
            sz += 1;
            fprintf( stdout, "0x%hhX,", '\0' );
        }
        sz += tr1sz + 1;
        echo( tr1, tr1sz, tr2 ? 0 : islast );
        if ( tr2 )
        {
            sz += tr2sz + 1;
            echo( tr2, tr2sz, islast );
        }
        else
        {
            sz += 1;
            fprintf( stdout, islast ? "0x%hhX" : "0x%hhX,", '\0' );
        }
    }

    fprintf( stdout, templ_2, argv[2], sz );

    p_string_fini( &tmp );
#ifndef P_NO_MEMPOOL
    p_mempool_fini( &mp );
#endif
    return EXIT_SUCCESS;
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
