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
 *  \file p_i18n.c
 *  \brief Internationalization of source code.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 *
 *  This should be replaced by a real scanner (xml?).
 *
 *  Produce a text file suitable for a translator to work on.
 *
 *  Usage: cat <input files> | p_i18n > <output file>
 *         p_i18n <input file> > <output file>
 */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef P_NO_MEMPOOL
#include "p_mempool.h"
#endif
#include "p_string.h"

const char templ[] =
"***CONTEXT***\n%s"
"*TEXT*\n%s"
"*PLURAL*\n%s"
"*TRANSLATION TEXT*\n"
"*TRANSLATION PLURAL*\n";

void
stop( const char* msg )
{
    fprintf( stderr, "\nERROR %s\n", msg );
    exit( EXIT_FAILURE );
}

char
get_string( char** p, p_String* tmp )
{
    char flag = 0;
    char* q = *p;

    while ( isspace( *q )) ++q;
    if ( *q != '"' )
        goto finish;
    while ( *++q )
    {
        if ( *q == '"' && *( q - 1 ) != '\\' )
        {
            ++q;
            flag = 1;
            if ( !p_string_cat_len( tmp, "\n", 1 ))
                stop( "nomem" );
            break;
        }
        if ( !p_string_cat_len( tmp, q, 1 ))
            stop( "nomem" );
    }
    finish:
    *p = q;
    return flag;
}

char
get_string2( char** p, p_String* s1, p_String* s2 )
{
    if ( !get_string( p, s1 ))
        return 0;
    while( isspace( *(*p) )) ++(*p);
    if ( *(*p) != ',' )
        return 0;
    ++(*p);
    if ( !get_string( p, s2 ))
        return 0;
    return 1;
}

char
get_string3( char** p, p_String* s1, p_String* s2, p_String* s3 )
{
    if ( !get_string( p, s1 ))
        return 0;
    while( isspace( *(*p) )) ++(*p);
    if ( *(*p) != ',' )
        return 0;
    ++(*p);
    if ( !get_string( p, s2 ))
        return 0;
    while( isspace( *(*p) )) ++(*p);
    if ( *(*p) != ',' )
        return 0;
    ++(*p);
    if ( !get_string( p, s3 ))
        return 0;
    return 1;
}

int
main( int argc, char* argv[] )
{
#ifndef P_NO_MEMPOOL
    p_MemPool mp;
#endif
    p_String tmp;
    char* p;
#if 0
    p_l10n_Translation t;
#endif

    if ( argc > 1 && argv[1][0] != '-' )
    {
        if ( !freopen( argv[1], "r", stdin ))
            stop( "Invalid input file" );
    }
#if 0
    if ( argc > 2 )
    {
        if ( !p_l10n_init( &t ))
            stop( "nomem" );
        // TODO
    }
#endif

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

    p = tmp.data;
    while ( ( p = strstr( p, "p_i18n" )))
    {
        p += 6;
        while ( isspace( *p )) ++p;
        if ( *p == '(' ) /* i18n( txt ) */
        {
            p_String txt;
            if ( !p_string_init( &txt, "" ))
                stop( "nomem" );
            ++p;
            if ( !get_string( &p, &txt ))
            {
                p_string_fini( &txt );
                continue;
            }
            fprintf( stdout, templ, "", (P_CHAR*)txt.data, "" );
            p_string_fini( &txt );
        }
        else if ( *p == '_' )
        {
            ++p;
            if ( *p == 'p' )
            {
                ++p;
                while ( isspace( *p )) ++p;
                if ( *p == '(' ) /* i18n_p( txt, plural ) */
                {
                    p_String txt;
                    p_String plural;
                    if ( !p_string_init( &txt, "" )
                        || !p_string_init( &plural, "" ))
                        stop( "nomem" );
                    ++p;
                    if ( !get_string2( &p, &txt, &plural ))
                    {
                        p_string_fini( &txt );
                        p_string_fini( &plural );
                        continue;
                    }
                    fprintf( stdout, templ, "",
                        (P_CHAR*)txt.data, (P_CHAR*)plural.data );
                    p_string_fini( &txt );
                    p_string_fini( &plural );
                }
                else continue;
            }
            else if ( *p == 'c' )
            {
                ++p;
                while ( isspace( *p )) ++p;
                if ( *p == '(' ) /* i18n_c( ctxt, txt ) */
                {
                    p_String ctxt;
                    p_String txt;
                    if ( !p_string_init( &ctxt, "" )
                        || !p_string_init( &txt, "" ))
                        stop( "nomem" );
                    ++p;
                    if ( !get_string2( &p, &ctxt, &txt ))
                    {
                        p_string_fini( &ctxt );
                        p_string_fini( &txt );
                        continue;
                    }
                    fprintf( stdout, templ,
                        (P_CHAR*)ctxt.data, (P_CHAR*)txt.data, "" );
                    p_string_fini( &ctxt );
                    p_string_fini( &txt );
                }
                else if ( *p == 'p' )
                {
                    ++p;
                    while ( isspace( *p )) ++p;
                    if ( *p == '(' ) /* i18n_cp( ctxt, txt, plural ) */
                    {
                        p_String ctxt;
                        p_String txt;
                        p_String plural;
                        if ( !p_string_init( &ctxt, "" )
                            || !p_string_init( &txt, "" )
                            || !p_string_init( &plural, "" ))
                            stop( "nomem" );
                        ++p;
                        if ( !get_string3( &p, &ctxt, &txt, &plural ))
                        {
                            p_string_fini( &ctxt );
                            p_string_fini( &txt );
                            p_string_fini( &plural );
                            continue;
                        }
                        fprintf( stdout, templ, (P_CHAR*)ctxt.data,
                            (P_CHAR*)txt.data, (P_CHAR*)plural.data );
                        p_string_fini( &ctxt );
                        p_string_fini( &txt );
                        p_string_fini( &plural );
                    }
                }
                else continue;
            }
            else continue;
        }
        else continue;
    }

    p_string_fini( &tmp );
#ifndef P_NO_MEMPOOL
    p_mempool_fini( &mp );
#endif
    return EXIT_SUCCESS;
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
