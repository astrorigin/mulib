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

#include "p_utf8.h"

P_INT8
p_utf8_octetnum( const P_INT32 c )
{
    return
        /* prohibited range */
        c >= 0xD800 && c <= 0xDFFF ? 0 :
        /* allowed ranges */
        c >= 0x0 && c <= 0x7F ? 1 :
        c >= 0x80 && c <= 0x7FF ? 2 :
        c >= 0x800 && c <= 0xFFFF ? 3 :
        c >= 0x10000 && c <= 0x10FFFF ? 4 :
        /* out of range */
        0;
}

P_VOID
p_utf8_octetprepare( P_CHAR* octets,
        const P_INT8 num )
{
    P_ASSERT( octets )
    P_ASSERT( num >= 1 && num <= 4 )

    switch ( num )
    {
    case 1:
        octets[0] = 0;
        return;
    case 2:
        octets[0] = -64;
        octets[1] = -128;
        return;
    case 3:
        octets[0] = -32;
        octets[1] = -128;
        octets[2] = -128;
        return;
    case 4:
        octets[0] = -16;
        octets[1] = -128;
        octets[2] = -128;
        octets[3] = -128;
        return;
    }
}

P_CHAR*
p_utf8_encode( const P_INT32 c,
        P_CHAR* buf )
{
    P_INT8 num;

    P_ASSERT( buf )

    memset( buf, 0, 5 );

    if ( ( num = p_utf8_octetnum( c )) == 0 )
        return buf;
    if ( num > 1 )
        p_utf8_octetprepare( buf, num );
    switch ( num )
    {
    case 1:
        buf[0] |= c;
        break;
    case 2:
        buf[0] |= (char) ( c >> 6 );
        buf[1] |= ((char)c) & ~-64;
        break;
    case 3:        
        buf[0] |= (char) ( c >> 12 );
        buf[1] |= ((char)( c >> 6 )) & ~-64;
        buf[2] |= ((char)c) & ~-64;
        break;
    case 4:
        buf[0] |= (char) ( c >> 19 );
        buf[1] |= ((char)( c >> 12 )) & ~-64;
        buf[2] |= ((char)( c >> 6 )) & ~-64;
        buf[3] |= ((char)c) & ~-64;
        break;
    }
    return buf;
}

P_SZ
p_utf8_strlen( const P_CHAR* s )
{
    P_CHAR c;
    P_CHAR* p;
    P_SZ cnt = 0;

    P_ASSERT( s )

    p = (P_CHAR*) s;
    while ( ( c = *p++ ))
    {
        if ( c >= 0 )
        {
            ++cnt;
        }
        else
        if ( c >= -64 && c < -32 )
        {
            if ( !( c = *p++ ) || !( c < -64 ))
                return (P_SZ) -1;
            ++cnt;
        }
        else
        if ( c >= -32 && c < -16 )
        {
            P_SZ i;
            for ( i = 0; i < 2; ++i )
            {
                if ( !( c = *p++ ) || !( c < -64 ))
                    return (P_SZ) -1;
            }
            ++cnt;
        }
        else
        if ( c >= -16 && c < -8 )
        {
            P_SZ i;
            for ( i = 0; i < 3; ++i )
            {
                if ( !( c = *p++ ) || !( c < -64 ))
                    return (P_SZ) -1;                
            }
            ++cnt;
        }
        else
            return (P_SZ) -1;
    }
    return cnt;
}

#ifndef NDEBUG

static void
showbits(int x)
{
    int i; 
    for(i=(sizeof(int)*8)-1; i>=0; i--)
        (x&(1<<i))?putchar('1'):putchar('0');

	printf("\n");
}

static void
showbitsc(char x)
{
    int i; 
    for(i=(sizeof(char)*8)-1; i>=0; i--)
        (x&(1<<i))?putchar('1'):putchar('0');

	printf("\n");
}

static void 
showbitsstr( char* s )
{
    size_t i;
    size_t n = strlen( s );
    for ( i = 0; i < n; ++i )
        showbitsc( *s++ );
}

P_INT32
p_utf8_test( P_VOID )
{
    char buf[5];

    P_ASSERT( sizeof( char ) == 1 )
    P_ASSERT( sizeof( int ) == 4 )

    /* ascii */
    P_ASSERT( p_utf8_octetnum( 0x45 ) == 1 )
    printf( "ASCII 'E': %s\n", p_utf8_encode( 0x45, buf ));
    showbitsstr( buf );
    P_ASSERT( p_utf8_strlen( buf ) == 1 )

    /* Some latin-1 */
    P_ASSERT( p_utf8_octetnum( 0xE2 ) == 2 )
    printf( "French 'â': %s\n", p_utf8_encode( 0xE2, buf ));
    showbitsstr( buf );
    P_ASSERT( p_utf8_strlen( buf ) == 1 )

    /* hebrew */
    P_ASSERT( p_utf8_octetnum( 0x5D0 ) == 2 )
    printf( "Hebrew aleph: %s\n", p_utf8_encode( 0x5D0, buf ));
    showbitsstr( buf );
    P_ASSERT( p_utf8_strlen( buf ) == 1 )

    /* samaritan */
    P_ASSERT( p_utf8_octetnum( 0x800 ) == 3 )
    printf( "Samaritan alaf: %s\n", p_utf8_encode( 0x800, buf ));
    showbitsstr( buf );
    P_ASSERT( p_utf8_strlen( buf ) == 1 )

    /* Linear b syllabe KA */
    P_ASSERT( p_utf8_octetnum( 0x1000F ) == 4 )
    printf( "Syllabe KA: %s\n", p_utf8_encode( 0x1000F, buf ));
    showbitsstr( buf );
    P_ASSERT( p_utf8_strlen( buf ) == 1 )

    return 0;
}

#endif /* NDEBUG */

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
