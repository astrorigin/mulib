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

#include <p_vector.h>

#include <p_mempool.h>

#undef P_TRACE
#if defined( P_TRACE_MODE ) && defined( P_TRACE_VECTOR )
#define P_TRACE P_TRACER
#else
#define P_TRACE( moo, ... )
#endif

P_VOID
p_vector_new( p_Vector** v,
    P_SZ len,
    P_SZ unit,
    P_SZ (*csfn)( p_Vector*, P_SZ ) )
{
    P_TRACE( "-- VECTOR -- new ("P_PTR_FMT")\n", *v )
    *v = P_MALLOC( sizeof( p_Vector ) );
    P_ASSERT( *v )
    p_vector_init( *v, len, unit, csfn );
}

P_VOID
p_vector_delete( p_Vector** v )
{
    P_ASSERT( *v )
    P_TRACE( "-- VECTOR -- delete ("P_PTR_FMT")\n", *v )
    p_vector_fini( *v );
    P_FREE( *v, sizeof( p_Vector ) );
    *v = NULL;
}

P_VOID
p_vector_init( p_Vector* v,
        P_SZ len,
        P_SZ unit,
        P_SZ (*csfn)( p_Vector*, P_SZ ) )
{
    P_SZ space_req;

    P_ASSERT( v )
    P_TRACE( "-- VECTOR -- init ("P_PTR_FMT")\n", v )

    v->data = NULL;
    v->len = 0;
    v->unit = unit;
    v->capacity = 0;
    v->calc_space_fn = csfn;

    space_req = csfn ? (*csfn)( v, len ) : len;
    if ( space_req )
    {
        v->data = P_MALLOC( space_req );
        P_ASSERT( v->data )
        v->len = len;
        v->capacity = space_req;
    }
    P_ASSERT( space_req > 0 || len == 0 );
}

P_VOID
p_vector_fini( p_Vector* v )
{
    P_ASSERT( v )
    P_TRACE( "-- VECTOR -- fini ("P_PTR_FMT")\n", v )

    if ( v->data )
    {
        P_FREE( v->data, v->capacity );
        v->data = NULL;
        v->len = 0;
        v->capacity = 0;
    }
}

P_SZ
p_vector_default_space_fn( p_Vector* self,
        P_SZ len )
{
    P_UNUSED( self );
    return len * 2;
}

#ifndef NDEBUG
P_PTR
p_vector_get( const p_Vector* v,
        const P_ID index )
{
    P_ASSERT( v )
    P_TRACE( "-- VECTOR -- get ("P_PTR_FMT") index ("P_PTR_FMT")\n", v, index );

    return index > v->len ? NULL : v->data + ( index * v->unit );
}
#endif /* NDEBUG */

P_PTR
p_vector_set( p_Vector* v,
        const P_ID index,
        const P_PTR ptr )
{
    P_PTR dest;

    P_ASSERT( v )
    P_TRACE( "-- VECTOR -- set ("P_PTR_FMT") index ("P_PTR_FMT")"
             " ptr ("P_PTR_FMT")\n", v, index, ptr )

    if ( index > v->len || ptr == NULL )
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

P_PTR
p_vector_reserve( p_Vector* v,
        const P_SZ len )
{
    P_SZ req;

    P_ASSERT( v )
    P_TRACE( "-- VECTOR -- reserve ("P_PTR_FMT")\n", v )

    req = v->calc_space_fn ? (*v->calc_space_fn)( v, len ) : len;
    if ( v->capacity != req )
    {
        v->data = P_REALLOC( v->data, req, v->capacity );
        if ( v->data == NULL )
            P_FATAL_ERROR( "realloc failed ("P_PTR_FMT")", v );
        v->capacity = req;
    }
    return v->data;
}

P_VOID
p_vector_append( p_Vector* v,
        const P_PTR ptr,
        const P_SZ len )
{
    P_ASSERT( v )
    P_ASSERT( ptr )
    P_TRACE( "-- VECTOR -- append ("P_PTR_FMT")\n", v )

    if ( len == 0 )
        return;
    p_vector_reserve( v, v->len + len );
    memcpy( v->data + ( v->len * v->unit ), ptr, len );
    v->len += len;
    P_ASSERT( v->len * v->unit <= v->capacity )
}

P_VOID
p_vector_traverse( p_Vector* v,
        P_VOID (*traverse_fn)( P_PTR, P_PTR ),
        P_PTR userdata )
{
    P_SZ i;
    const P_SZ vlen = v->len;

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

struct p_vector_test_t
{
    P_INT32 v1;
    P_INT32 v2;
};

P_SZ
p_vector_test_csfn( p_Vector* v,
        P_SZ sz )
{
    P_UNUSED( v );
    return sz * sizeof( struct p_vector_test_t ) * 2;
}

P_INT32
p_vector_test( P_VOID )
{
    

    p_MemPool mp;
    struct p_vector_test_t data[] =
    {
    { 1, 2 },
    { 3, 4 },
    { 5, 6 }
    };
    int i;
    p_Vector vec;
    
    p_mempool_set( &mp );
    p_mempool_init( &mp, 0 );
    
    p_vector_init( &vec, 3, sizeof( struct p_vector_test_t ), &p_vector_test_csfn );
    P_ASSERT( vec.capacity == 3 * sizeof( struct p_vector_test_t ) * 2 )
    
    p_vector_fill( &vec, data, sizeof( struct p_vector_test_t ) * 3 );
    p_vector_debug( &vec );
    
    for ( i = 0; i < 3; i++ )
    {
        P_PTR p = p_vector_get( &vec, i );
        P_ASSERT( p )
        P_ASSERT( ((struct p_vector_test_t*)p)->v1 == (i+(i+1)) )
    }
    
    p_vector_fini( &vec );
    
    p_mempool_debug( &mp );
    p_mempool_fini( &mp );
    p_mempool_debug( &mp );
    
    return 0;
}

#endif /* NDEBUG */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
