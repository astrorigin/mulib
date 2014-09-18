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

#include "p_vector.h"

#include "p_mempool.h"

#undef P_TRACE
#if defined( P_TRACE_MODE ) && defined( P_TRACE_VECTOR )
#define P_TRACE P_TRACER
#else
#define P_TRACE( moo, ... )
#endif

P_BOOL
p_vector_new( p_Vector** v,
    P_SZ len,
    P_SZ unit,
    P_SZ (*csfn)( p_Vector*, P_SZ ) )
{
    P_TRACE( "-- VECTOR -- new ("P_PTR_FMT")\n", *v )
    *v = P_MALLOC( sizeof( p_Vector ) );
    P_ASSERT( *v )
#ifdef NDEBUG
    if ( !*v )
        return P_FALSE;
#endif
    return p_vector_init( *v, len, unit, csfn );
}

P_VOID
p_vector_delete( p_Vector** v )
{
    P_ASSERT( *v )
#ifdef NDEBUG
    if ( !*v )
        return;
#endif
    P_TRACE( "-- VECTOR -- delete ("P_PTR_FMT")\n", *v )
    p_vector_fini( *v );
    P_FREE( *v, sizeof( p_Vector ) );
    *v = NULL;
}

P_BOOL
p_vector_init( p_Vector* v,
        P_SZ len,
        P_SZ unit,
        P_SZ (*csfn)( p_Vector*, P_SZ ) )
{
    P_SZ space_req;

    P_ASSERT( v )
#ifdef NDEBUG
    if ( !v )
        return P_FALSE;
#endif
    P_TRACE( "-- VECTOR -- init ("P_PTR_FMT")\n", v )

    v->data = NULL;
    v->len = 0;
    v->unit = unit;
    v->capacity = 0;
    v->calc_space_fn = csfn;
    v->finalize_fn = NULL;

    space_req = csfn ? (*csfn)( v, len ) : len * unit;
    if ( space_req )
    {
        v->data = P_MALLOC( space_req );
        P_ASSERT( v->data )
#ifdef NDEBUG
        if ( !v->data )
            return P_FALSE;
#endif
        v->capacity = space_req;
    }
    P_ASSERT( space_req > 0 || len == 0 );
    return P_TRUE;
}

P_VOID
p_vector_fini( p_Vector* v )
{
    P_ASSERT( v )
#ifdef NDEBUG
    if ( !v )
        return;
#endif
    P_TRACE( "-- VECTOR -- fini ("P_PTR_FMT")\n", v )

    if ( v->data )
    {
        if ( v->finalize_fn )
            p_vector_traverse( v, v->finalize_fn );
        if ( v->capacity )
            P_FREE( v->data, v->capacity );
        v->data = NULL;
    }
    v->len = 0;
    v->unit = 0;
    v->capacity = 0;
    v->calc_space_fn = NULL;
    v->finalize_fn = NULL;
}

P_SZ
p_vector_default_space_fn( p_Vector* self,
        P_SZ len )
{
    return len * 2 * self->unit;
}

P_PTR
p_vector_get( const p_Vector* v,
        const P_ID index )
{
    P_ASSERT( v )
    P_TRACE( "-- VECTOR -- get ("P_PTR_FMT") index ("P_ID_FMT")\n", v, index );

    return index >= v->len ? NULL : v->data + ( index * v->unit );
}

P_PTR
p_vector_get_next( p_Vector* v )
{
    P_ASSERT( v )
    P_TRACE( "-- VECTOR -- get next ("P_PTR_FMT")\n", v );

    if ( !p_vector_reserve( v, v->len + 1 ))
        return NULL;
    P_ASSERT( v->len * v->unit < v->capacity )
    return v->data + ( v->len * v->unit );
}

P_PTR
p_vector_set( p_Vector* v,
        const P_ID index,
        const P_PTR ptr )
{
    P_PTR dest;

    P_ASSERT( v )
#ifdef NDEBUG
    if ( !v )
        return NULL;
#endif
    P_TRACE( "-- VECTOR -- set ("P_PTR_FMT") index ("P_ID_FMT")"
             " ptr ("P_PTR_FMT")\n", v, index, ptr )

    if ( index >= v->len || ptr == NULL )
        return NULL;
    dest = v->data + ( index * v->unit );
    if ( dest == ptr )
        return dest;
#ifndef NDEBUG
    /* check memory does not overlap */
    if ( ( ((P_PTR)v->unit) < ptr && dest <= ptr - v->unit )
        || ( dest >= ptr + v->unit ))
        memcpy( dest, ptr, v->unit );
    else
        P_FATAL_ERROR( "memory overlap ("P_PTR_FMT") ("P_PTR_FMT")", ptr, dest );
#else
    memcpy( dest, ptr, v->unit );
#endif
    return dest;
}

P_BOOL
p_vector_unset( p_Vector* v,
        const P_ID index )
{
    P_PTR dest;

    P_ASSERT( v )
#ifdef NDEBUG
    if ( !v )
        return P_FALSE;
#endif
    P_TRACE( "-- VECTOR -- unset ("P_PTR_FMT") index ("P_ID_FMT")\n", v, index )

    if ( index >= v->len )
        return P_FALSE;
    dest = v->data + ( index * v->unit );
    if ( index != v->len - 1 )
        memmove( dest, dest + v->unit, ( v->len - index - 1 ) * v->unit );
    if ( !p_vector_reserve( v, v->len - 1 ))
        return P_FALSE;
    v->len -= 1;
    return P_TRUE;
}

P_PTR
p_vector_reserve( p_Vector* v,
        const P_SZ len )
{
    P_SZ req;

    P_ASSERT( v )
#ifdef NDEBUG
    if ( !v )
        return NULL;
#endif
    P_TRACE( "-- VECTOR -- reserve ("P_PTR_FMT")\n", v )

    req = v->calc_space_fn ? (*v->calc_space_fn)( v, len ) : len * v->unit;
    if ( v->capacity != req )
    {
        v->data = P_REALLOC( v->data, req, v->capacity );
        if ( !v->data )
            return NULL;
        v->capacity = req;
    }
    return v->data;
}

P_BOOL
p_vector_append( p_Vector* v,
        const P_PTR ptr,
        const P_SZ len )
{
    P_ASSERT( v )
    P_ASSERT( ptr )
#ifdef NDEBUG
    if ( !v || !ptr )
        return P_FALSE;
#endif
    P_TRACE( "-- VECTOR -- append ("P_PTR_FMT")\n", v )

    if ( len == 0 )
        return P_TRUE;
    if ( !p_vector_reserve( v, v->len + len ))
        return P_FALSE;
    memcpy( v->data + ( v->len * v->unit ), ptr, len * v->unit );
    v->len += len;
    P_ASSERT( v->len * v->unit <= v->capacity )
    return P_TRUE;
}

P_BOOL
p_vector_prepend( p_Vector* v,
        const P_PTR ptr,
        const P_SZ len )
{
    P_ASSERT( v )
    P_ASSERT( ptr )
#ifdef NDEBUG
    if ( !v || !ptr )
        return P_FALSE;
#endif
    P_TRACE( "-- VECTOR -- prepend ("P_PTR_FMT")\n", v )

    if ( len == 0 )
        return P_TRUE;
    if ( !p_vector_reserve( v, v->len + len ))
        return P_FALSE;
    memmove( v->data + ( len * v->unit ), v->data, v->len * v->unit );
    memcpy( v->data, ptr, len * v->unit );
    v->len += len;
    P_ASSERT( v->len * v->unit <= v->capacity )
    return P_TRUE;
}

P_VOID
p_vector_traverse( p_Vector* v,
        P_VOID (*traverse_fn)( P_PTR ) )
{
    P_SZ i;
    const P_SZ vlen = v->len;

    P_ASSERT( v )
    P_ASSERT( traverse_fn )
#ifdef NDEBUG
    if ( !v || !traverse_fn )
        return;
#endif
    P_TRACE( "-- VECTOR -- traverse ("P_PTR_FMT") func("P_PTR_FMT")\n",
        v, traverse_fn )

    for ( i = 0; i < vlen; ++i )
    {
        (*traverse_fn)( v->data + ( i * v->unit ));
    }
}

P_VOID
p_vector_traverse2( p_Vector* v,
        P_VOID (*traverse_fn)( P_PTR, P_PTR ),
        P_PTR userdata )
{
    P_SZ i;
    const P_SZ vlen = v->len;

    P_ASSERT( v )
    P_ASSERT( traverse_fn )
#ifdef NDEBUG
    if ( !v || !traverse_fn )
        return;
#endif
    P_TRACE( "-- VECTOR -- traverse2 ("P_PTR_FMT") func("P_PTR_FMT") udata("P_PTR_FMT")\n",
        v, traverse_fn, userdata )

    for ( i = 0; i < vlen; ++i )
    {
        (*traverse_fn)( v->data + ( i * v->unit ), userdata );
    }
}

#ifndef NDEBUG

P_VOID
p_vector_debug( const p_Vector* v )
{
    P_ASSERT( v );
    printf( "-- VECTOR -- debug ("P_PTR_FMT"):\n"
            "--     data = ("P_PTR_FMT")\n"
            "--     len = "P_SZ_FMT"\n"
            "--     unit = "P_SZ_FMT"\n"
            "--     capacity = "P_SZ_FMT"\n"
            "--     calc_space_fn = ("P_PTR_FMT")\n"
            "-- end vector debug\n",
        (P_PTR)v, v->data, v->len, v->unit, v->capacity, (P_PTR)v->calc_space_fn );
}

#endif /* NDEBUG */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
