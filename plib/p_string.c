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

#include "p_string.h"

#include "p_mempool.h"

#undef P_TRACE
#if defined( P_TRACE_MODE ) && defined( P_TRACE_STRING )
#define P_TRACE P_TRACER
#else
#define P_TRACE( moo, ... )
#endif

P_BOOL
p_string_new( p_String** s,
        const P_CHAR* content )
{
    P_ASSERT( s )

    return p_string_new_len( s, content, content ? strlen( content ) : 0 );
}

P_BOOL
p_string_new_len( p_String** s,
        const P_CHAR* content,
        const P_SZ len )
{
    P_ASSERT( s )

    if ( !p_vector_new( (p_Vector**)s, len + 1, sizeof( P_CHAR ),
        (P_SZ(*)(p_Vector*, P_SZ)) &p_string_calc_space_fn ))
        return P_FALSE;
    if ( content )
        strncpy( (*s)->data, content, len );
    ((char*)(*s)->data)[ len ] = '\0';
    (*s)->len = len + 1;
    return P_TRUE;
}

P_BOOL
p_string_init( p_String* s,
        const P_CHAR* content )
{
    P_ASSERT( s )

    return p_string_init_len( s, content, content ? strlen( content ) : 0 );
}

P_BOOL
p_string_init_len( p_String* s,
        const P_CHAR* content,
        const P_SZ len )
{
    P_ASSERT( s )

    if ( !p_vector_init( (p_Vector*)s, len + 1, sizeof( P_CHAR ),
        (P_SZ(*)(p_Vector*, P_SZ)) &p_string_calc_space_fn ))
        return P_FALSE;
    if ( content )
        strncpy( s->data, content, len );
    ((char*)s->data)[ len ] = '\0';
    s->len = len + 1;
    return P_TRUE;
}

P_SZ
p_string_calc_space_fn( p_String* s,
        P_SZ len )
{
    P_UNUSED( s );
    return P_STRING_QUANTA * ((( len + 1 ) / P_STRING_QUANTA) + 1 );
}

P_CHAR*
p_string_set( p_String* s,
        const P_CHAR* content )
{
    P_ASSERT( s )

    return p_string_set_len( s, content, content ? strlen( content ) : 0 );
}

P_CHAR*
p_string_set_len( p_String* s,
        const P_CHAR* content,
        const P_SZ len )
{
    P_ASSERT( s )

    if ( len == 0 || content == NULL || content[0] == '\0' )
    {
        if ( !p_vector_reserve( (p_Vector*)s, 1 ))
            return NULL;
        ((P_CHAR*)s->data)[0] = '\0';
        s->len = 1;
    }
    else
    {
        if ( s->len == len && !strncmp( s->data, content, len ))
            return s->data;
        if ( !p_vector_reserve( (p_Vector*)s, len + 1 ))
            return NULL;
        strncpy( s->data, content, len );
        s->len = len + 1;
        ((P_CHAR*)s->data)[ len ] = '\0';
    }
    P_ASSERT( ((P_CHAR*)s->data)[ s->len -1 ] == '\0' )
    return s->data;
}

P_CHAR*
p_string_copy( p_String* s,
        const p_String* s2 )
{
    P_ASSERT( s )
    P_ASSERT( s2 )

    return p_string_set_len( s, (P_CHAR*) s2->data, s2->len - 1 );
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
    if ( !p_vector_append( (p_Vector*)s, (P_PTR)content, strlen( content ) + 1 ))
        return NULL;
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
    if ( !p_vector_reserve( (p_Vector*)s, s->len + len ))
        return NULL;
    memcpy( s->data + s->len - 1, content, len );
    ((P_CHAR*)s->data)[ s->len + len - 1 ] = '\0';
    s->len += len;
    P_ASSERT( ((P_CHAR*)s->data)[ s->len -1 ] == '\0' )
    return s->data;
}

P_BOOL
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
        return P_TRUE;

    p = strstr( s->data, this );
    if ( p == NULL )
        return P_TRUE;
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
        if ( !p_vector_reserve( (p_Vector*)s, s->len ))
            return P_FALSE;
    }
    else
    {
        P_CHAR* buf;
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
        buf = b = P_MALLOC( cap_req );
        if ( !b )
            return P_FALSE;
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
        s->data = buf;
        P_FREE( p, s->capacity );
        s->len += cnt * ( len2 - len1 );
        s->capacity = cap_req;
    }
    return P_TRUE;
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
