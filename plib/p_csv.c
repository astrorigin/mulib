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

#include "p_csv.h"

P_VOID
p_csv_init( p_CsvContext* csv,
        const P_CHAR* data,
        const P_CHAR* sepchars,
        const P_CHAR* comchars )
{
    P_ASSERT( csv )
    csv->data = (P_CHAR*)data;
    csv->sepchars = sepchars ? (P_CHAR*)sepchars : (P_CHAR*)" \t\r";
    csv->comchars = comchars ? (P_CHAR*)comchars : (P_CHAR*)"#";
    csv->p = NULL;
}

P_INT32
p_csv_parse( p_CsvContext* csv )
{
    /*
     *  If EOL is met, put pointer just after it. Else stay there.
     */

    P_CHAR c;

    P_ASSERT( csv->data )

    if ( !csv->p )
        csv->p = csv->data;

    c = *csv->p;

    if ( c == '\0' )
    {
        csv->p = NULL;
        return P_CSV_EOF;
    }

    if ( strchr( csv->sepchars, c ) )
        do c = *++(csv->p);
        while ( strchr( csv->sepchars, c ) );

    if ( c == '\n' )
    {
        csv->p++;
        return P_CSV_EOL;
    }

    if ( strchr( csv->comchars, c ) )
    {
        csv->p = (P_CHAR*)strchr( csv->p, '\n' );
        if ( !csv->p )
        {
            csv->p = NULL;
            return P_CSV_EOF;
        }
        csv->p++;
        return P_CSV_EOL;
    }

    return P_CSV_WORD;
}

P_VOID
p_csv_word( p_CsvContext* csv,
        P_CHAR** start,
        P_CHAR** end )
{
    P_CHAR* p = csv->p;
    *start = p;
    do ++p;
    while ( *p && *p != '\n' && !strchr( csv->sepchars, *p ) );
    *end = p-1;
    csv->p = p;
}

P_VOID
p_csv_word_copy( p_CsvContext* csv,
        P_CHAR* buf,
        const P_SZ limit )
{
    P_CHAR *st, *nd;
    P_ASSERT( limit > 0 )
    p_csv_word( csv, &st, &nd );
    strncpy( buf, st, P_LIMIT( nd-st+1, (P_INT32)limit-1 ) );
    buf[ P_LIMIT( nd-st+1, (P_INT32)limit-1 ) ] = '\0';
}

P_VOID
p_csv_line_copy( p_CsvContext* csv,
        P_CHAR* buf,
        const P_SZ limit )
{
    P_CHAR *st, *nd;
    P_ASSERT( limit > 0 )
    st = nd = csv->p;
    do ++nd;
    while ( *nd && *nd != '\n' && !strchr( csv->comchars, *nd ) );
    strncpy( buf, st, P_LIMIT( nd-st, (P_INT32)limit-1 ) );
    buf[ P_LIMIT( nd-st, (P_INT32)limit-1 ) ] = '\0';
    csv->p = nd;
}

P_BOOL
p_csv_get_word( p_CsvContext* csv,
        P_CHAR* buf,
        const P_SZ limit,
        P_INT32* i )
{
    P_INT32 j;
    if ( limit == 0 )
        return P_FALSE;
    j = p_csv_parse( csv );
    if ( j != P_CSV_WORD )
        return P_FALSE;
    p_csv_word_copy( csv, buf, limit );
    *i = p_csv_parse( csv );
    return P_TRUE;
}

P_BOOL
p_csv_get_line( p_CsvContext* csv,
        P_CHAR* buf,
        const P_SZ limit,
        P_INT32* i )
{
    P_INT32 j;
    if ( limit == 0 )
        return P_FALSE;
    j = p_csv_parse( csv );
    if ( j != P_CSV_WORD )
        return P_FALSE;
    p_csv_line_copy( csv, buf, limit );
    *i = p_csv_parse( csv );
    return P_TRUE;
}

#ifndef NDEBUG

P_INT32
p_csv_test()
{
    P_CHAR* txt = (P_CHAR*)"aa b \nd e\tf\n";
    p_CsvContext csv;
    P_INT32 res;
    P_CHAR* st, *nd;
    P_CHAR buf[24];

    p_csv_init( &csv, txt, NULL, NULL );

    res = p_csv_parse( &csv );
    P_ASSERT( res == P_CSV_WORD )

    p_csv_word( &csv, &st, &nd );
    strncpy( buf, st, nd-st+1 );
    buf[ nd-st+1 ] = '\0';
    P_ASSERT( !strcmp( "aa", buf ) )

    res = p_csv_parse( &csv );
    P_ASSERT( res == P_CSV_WORD )

    p_csv_word_copy( &csv, buf, 24 );
    P_ASSERT( !strcmp( buf, "b" ) )

    res = p_csv_parse( &csv );
    P_ASSERT( res == P_CSV_EOL )

    res = p_csv_parse( &csv );
    P_ASSERT( res == P_CSV_WORD )

    p_csv_line_copy( &csv, buf, 24 );
    P_ASSERT( !strcmp( buf, "d e\tf" ) )

    return 0;
}

#endif /* NDEBUG */
/* vim: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
