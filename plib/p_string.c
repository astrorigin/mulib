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

#include <p_string.h>

#include <p_mempool.h>

#undef P_TRACE
#if defined( P_TRACE_MODE ) && defined( P_TRACE_STRING )
#define P_TRACE P_TRACER
#else
#define P_TRACE( moo, ... )
#endif

P_VOID
p_string_new_len( p_String** s,
        const P_CHAR* content,
        const P_SZ len )
{
    P_ASSERT( *s )

    p_vector_new( (p_Vector**)s, len + 1, sizeof( P_CHAR ),
                  (P_SZ(*)(p_Vector*, P_SZ)) &p_string_calc_space_fn );
    if ( content )
        strncpy( (*s)->data, content, len );
    ((char*)(*s)->data)[ len ] = '\0';
}

P_VOID
p_string_init_len( p_String* s,
        const P_CHAR* content,
        const P_SZ len )
{
    P_ASSERT( s )

    p_vector_init( (p_Vector*)s, len + 1, sizeof( P_CHAR ),
                   (P_SZ(*)(p_Vector*, P_SZ)) &p_string_calc_space_fn );
    if ( content )
        strncpy( s->data, content, len );
    ((char*)s->data)[ len ] = '\0';
}

P_SZ
p_string_calc_space_fn( p_String* s,
        P_SZ len )
{
    P_UNUSED( s );
    return P_STRING_QUANTA * ((( len + 1 ) / P_STRING_QUANTA) + 1 );
}

P_CHAR*
p_string_set_len( p_String* s,
        const P_CHAR* content,
        const P_SZ len )
{
    P_ASSERT( s )

    if ( len == 0 || content == NULL || content[0] == '\0' )
    {
        p_vector_reserve( (p_Vector*)s, 1 );
        ((P_CHAR*)s->data)[0] = '\0';
        s->len = 1;
    }
    else
    {
        if ( s->len == len && !strncmp( s->data, content, len ))
            return s->data;
        p_vector_reserve( (p_Vector*)s, len + 1 );
        strncpy( s->data, content, len );
        s->len = len + 1;
        ((P_CHAR*)s->data)[ len ] = '\0';
    }
    P_ASSERT( ((P_CHAR*)s->data)[ s->len -1 ] == '\0' )
    return s->data;
}

P_CHAR*
p_string_cat( p_String* s,
        const P_CHAR* content )
{
    P_ASSERT( s )

    if ( content == NULL || content[0] == '\0' )
        return s->data;
    if ( s->len ) /* it should always be */
        s->len -= 1; /* trick to overwrite trailing null */
    p_vector_append( (p_Vector*)s, (P_PTR)content, strlen( content ) + 1 );
    P_ASSERT( ((P_CHAR*)s->data)[ s->len -1 ] == '\0' )
    return s->data;
}

P_CHAR*
p_string_cat_len( p_String* s,
        const P_CHAR* content,
        const P_SZ len )
{
    P_ASSERT( s )

    if ( len == 0 || content == NULL || content[0] == '\0' )
        return s->data;
    p_vector_reserve( (p_Vector*)s, s->len + len );
    memcpy( s->data + ( s->unit * s->len -1 ), content, len );
    ((P_CHAR*)s->data)[ s->len + len ] = '\0';
    s->len += len;
    P_ASSERT( ((P_CHAR*)s->data)[ s->len -1 ] == '\0' )
    return s->data;
}

P_SZ
p_string_replace( p_String* s,
        const P_CHAR* this,
        const P_CHAR* that )
{
    P_CHAR* p, *b, *d;
    P_SZ sz, cnt = 0;
    P_SZ len1, len2;

    P_ASSERT( s )
    P_ASSERT( this )
    P_ASSERT( that )

    len1 = strlen( this );
    if ( p_string_len( s ) == 0 || len1 == 0 || p_string_len( s ) < len1 )
        return cnt;

    p = strstr( s->data, this );
    if ( p == NULL )
        return cnt;
    len2 = strlen( that );

    if ( len1 == len2 )
    {
        do
        {
            strncpy( p, that, len1 );
            p += len1;
            cnt += 1;
            p = strstr( p, this );
        }
        while ( p );
    }
    else
    if ( len1 > len2 )
    {
        b = p;
        do
        {
            strncpy( b, that, len2 );
            p += len1;
            b += len2;
            d = p;
            cnt += 1;
            p = strstr( p, this );
            if ( p )
            {
                while ( d != p )
                    *b++ = *d++;
            }
        }
        while ( p );
        while ( *d )
            *b++ = *d++;
        *b = '\0';
        s->len -= cnt * ( len1 - len2 );
        p_vector_reserve( (p_Vector*)s, s->len );
    }
    else
    {
        P_SZ cap_req;
        do
        {
            cnt += 1;
            p += len1;
            p = strstr( p, this );
        }
        while ( p );
        sz = s->len + (( len2 - len1 ) * cnt );
        cap_req = s->calc_space_fn ? (*s->calc_space_fn)( s, sz ) : sz; 
        b = P_MALLOC( cap_req );
        d = p = s->data;
        p = strstr( p, this );
        do
        {
            while ( d != p )
                *b++ = *d++;
            strncpy( b, that, len2 );
            b += len2;
            p += len1;
            d = p;
            p = strstr( p, this );
        }
        while ( p );
        while ( *d )
            *b++ = *d++;
        *b = '\0';
        /* switch data */
        p = s->data;
        s->data = b;
        P_FREE( p, s->capacity );
        s->len += cnt * ( len2 - len1 );
        s->capacity = cap_req;
    }
    return cnt;
}

#ifndef NDEBUG

P_INT32
p_string_test( P_VOID )
{
#ifndef P_NO_MEMPOOL
    p_MemPool mp;
    p_mempool_init( &mp, 0 );
    p_mempool_set( &mp );
#endif

    p_String s;
    p_string_init( &s , "" );
    /*p_string_debug( &s );*/
    P_ASSERT( s.len == 1 );

    p_string_set( &s, "abcdefgabcdefg" );
    P_ASSERT( !strcmp( s.data, "abcdefgabcdefg" ))

    p_string_replace( &s, "d", "x" );
    P_ASSERT( !strcmp( s.data, "abcxefgabcxefg" ))

    p_string_replace( &s, "fg", "z" );
    P_ASSERT( !strcmp( s.data, "abcxezabcxez" ));

    p_string_replace( &s, "c", "yy" );
    P_ASSERT( !strcmp( s.data, "abyyxezabyyxez" ));

    p_string_cat( &s, "1234" );
    P_ASSERT( !strcmp( s.data, "abyyxezabyyxez1234" ));

    p_string_cat_len( &s, "5678", 3 );
    P_ASSERT( !strcmp( s.data, "abyyxezabyyxez1234567" ));
    
    
    p_string_fini( &s );

#ifndef P_NO_MEMPOOL
    p_mempool_debug( &mp );
    p_mempool_fini( &mp );
#endif
    return 0;
}

#endif /* NDEBUG */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
