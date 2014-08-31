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
get_string( char** p, char** ret )
{
    char flag = 0;
    char* q = *p;
    FILE* tmpf;
    char* tmp = NULL;
    size_t tmpsz;
    while ( isspace( *q )) ++q;
    if ( *q != '"' )
        goto finish;
    if ( !( tmpf = open_memstream( &tmp, &tmpsz )))
        stop( strerror( errno ));
    while ( *++q )
    {
        if ( *q == '"' && *( q - 1 ) != '\\' )
        {
            ++q;
            flag = 1;
            if ( fputc( '\n', tmpf ) == EOF )
                stop( "nomem" );
            break;
        }
        if ( fputc( *q, tmpf ) == EOF )
            stop( "nomem" );
    }
    if ( fclose( tmpf ) == EOF )
        stop( strerror( errno ));

    finish:
    *p = q;
    if ( flag )
        *ret = tmp;
    else if ( tmp )
        free( tmp );
    return flag;
}

char
get_string2( char** p, char** s1, char** s2 )
{
    if ( !get_string( p, s1 ))
        return 0;
    while( isspace( *(*p) )) ++(*p);
    if ( *(*p) != ',' )
    {
        if ( *s1 )
            free( *s1 );
        return 0;
    }
    ++(*p);
    if ( !get_string( p, s2 ))
    {
        if ( *s1 )
            free( *s1 );
        return 0;
    }
    return 1;
}

char
get_string3( char** p, char** s1, char** s2, char** s3 )
{
    if ( !get_string( p, s1 ))
        return 0;
    while( isspace( *(*p) )) ++(*p);
    if ( *(*p) != ',' )
    {
        if ( *s1 )
            free( *s1 );
        return 0;
    }
    ++(*p);
    if ( !get_string( p, s2 ))
    {
        if ( *s1 )
            free( *s1 );
        return 0;
    }
    while( isspace( *(*p) )) ++(*p);
    if ( *(*p) != ',' )
    {
        if ( *s1 )
            free( *s1 );
        if ( *s2 )
            free( *s2 );
        return 0;
    }
    ++(*p);
    if ( !get_string( p, s3 ))
    {
        if ( *s1 )
            free( *s1 );
        if ( *s2 )
            free( *s2 );
        return 0;
    }
    return 1;
}

int
main( int argc, char* argv[] )
{
    FILE* tmpf;
    char* tmp;
    size_t tmpsz;
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
    if ( !( tmpf = open_memstream( &tmp, &tmpsz )))
        stop( strerror( errno ));

    while ( !feof( stdin ))
    {
        char buf[512];
        size_t z = fread( buf, 1, 512, stdin );
        if ( fwrite( buf, 1, z, tmpf ) != z )
            stop( "nomem" );
    }

    if ( fclose( tmpf ) == EOF )
        stop( strerror( errno ));

    p = tmp;
    while ( ( p = strstr( p, "p_i18n" )))
    {
        p += 6;
        while ( isspace( *p )) ++p;
        if ( *p == '(' ) /* i18n( txt ) */
        {
            char* txt = NULL;
            ++p;
            if ( !get_string( &p, &txt ))
                continue;
            fprintf( stdout, templ, "", txt, "" );
            free ( txt );
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
                    char* txt = NULL, *plural = NULL;
                    ++p;
                    if ( !get_string2( &p, &txt, &plural ))
                        continue;
                    fprintf( stdout, templ, "", txt, plural );
                    free( txt );
                    free( plural );
                }
                else continue;
            }
            else if ( *p == 'c' )
            {
                ++p;
                while ( isspace( *p )) ++p;
                if ( *p == '(' ) /* i18n_c( ctxt, txt ) */
                {
                    char* ctxt = NULL, *txt = NULL;
                    ++p;
                    if ( !get_string2( &p, &ctxt, &txt ))
                        continue;
                    fprintf( stdout, templ, ctxt, txt, "" );
                    free( ctxt );
                    free( txt );
                }
                else if ( *p == 'p' )
                {
                    ++p;
                    while ( isspace( *p )) ++p;
                    if ( *p == '(' ) /* i18n_cp( ctxt, txt, plural ) */
                    {
                        char* ctxt = NULL, *txt = NULL, *plural = NULL;
                        ++p;
                        if ( !get_string3( &p, &ctxt, &txt, &plural ))
                            continue;
                        fprintf( stdout, templ, ctxt, txt, plural );
                        free( ctxt );
                        free( txt );
                        free( plural );
                    }
                }
                else continue;
            }
            else continue;
        }
        else continue;
    }

    if ( tmp )
        free( tmp );
    return EXIT_SUCCESS;
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
