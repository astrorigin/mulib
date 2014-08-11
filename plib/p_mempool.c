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

#ifndef P_NO_MEMPOOL

#include <p_mempool.h>

#include <p_memlist.h>
#include <p_sllist.h>

#undef P_TRACE
#if defined( P_TRACE_MODE ) && defined( P_TRACE_MEMPOOL )
#define P_TRACE P_TRACER
#else
#define P_TRACE( moo, ... )
#endif

P_TLS p_MemPool*
_p_mempool_global = NULL;

P_VOID
p_mempool_set( const p_MemPool* mp )
{
    P_TRACE( "-- MEMPOOL -- setting global memory pool ("P_PTR_FMT")\n", mp )
    P_ASSERT( mp )
    _p_mempool_global = (p_MemPool*) mp;
}

P_BOOL
p_mempool_init( p_MemPool* mp,
        const P_SZ max )
{
    P_TRACE( "-- MEMPOOL -- init ("P_PTR_FMT") max: "P_SZ_FMT"\n", mp, max )
    P_ASSERT( mp )

    mp->max = max;
    mp->used = 0;
#ifndef NDEBUG
    mp->record = 0;
#endif
    return p_btree_init( &mp->buckets, _P_MALLOC_REF, _P_FREE_REF );
}

P_VOID
p_mempool_fini( p_MemPool* mp )
{
    P_TRACE( "-- MEMPOOL -- fini ("P_PTR_FMT")\n", mp )
    P_ASSERT( mp )
    p_btree_traverse( &mp->buckets, &_p_mempool_buckets_fini, NULL );
    p_btree_fini( &mp->buckets );
}

P_VOID
_p_mempool_buckets_fini( P_PTR bucket,
        P_PTR userdata )
{
    p_MemChunk* nxt;
    p_MemChunk* chk = ((p_MemBucket*)bucket)->alive;
    while ( chk )
    {
        nxt = chk->next;
        _P_FREE( chk );
        chk = nxt;
    }
    chk = ((p_MemBucket*)bucket)->trash;
    while ( chk )
    {
        nxt = chk->next;
        _P_FREE( chk );
        chk = nxt;
    }
    _P_FREE( bucket );
    P_UNUSED( userdata );
}

P_PTR
p_mempool_malloc( const P_SZ sz )
{
    p_MemBucket* bkt;
    p_MemChunk* chk;

    P_TRACE( "-- MEMPOOL -- malloc ("P_SZ_FMT")\n", sz )
    P_ASSERT( _p_mempool_global )

    bkt = (p_MemBucket*) p_btree_get( &_p_mempool_global->buckets, sz );

    if ( !bkt )
    {
        P_TRACE( "-- MEMPOOL -- new bucket ("P_SZ_FMT")\n", sz )
        bkt = _P_MALLOC( sizeof( p_MemBucket ) );
        if ( !bkt )
            return NULL;
        bkt->alive = NULL;
        bkt->trash = NULL;
        if ( p_btree_insert( &_p_mempool_global->buckets, sz, bkt ) < 0 )
            return NULL;
        /*p_btree_debug(_p_mempool_global->buckets.root);*/
    }

    if ( bkt->trash )
    {
        p_sllist_pull( bkt->trash, chk );
    }
    else
    {
        P_TRACE( "-- MEMPOOL -- new chunk ("P_SZ_FMT")\n", sz )
        chk = _P_MALLOC( sizeof( p_MemChunk ) + sz );
        if ( !chk )
            return NULL;
        _p_mempool_global->used += sz;

#ifndef NDEBUG
        /* update record */
        if ( _p_mempool_global->used > _p_mempool_global->record )
            _p_mempool_global->record = _p_mempool_global->used;
#endif

        /* check limit */
        if ( _p_mempool_global->max &&
            _p_mempool_global->used > _p_mempool_global->max )
        {
            p_mempool_purge( _p_mempool_global, 0 );
        }
    }

    p_sllist_push( bkt->alive, chk );
    P_TRACE( "-- MEMPOOL -- malloced ("P_PTR_FMT")\n", chk->chunk )

    return (P_PTR) chk->chunk;
}

P_VOID
p_mempool_free( P_PTR p,
        const P_SZ sz )
{
    p_MemBucket* bkt;
    p_MemChunk* chk;
    p_MemChunk* prev = NULL;

    P_TRACE( "-- MEMPOOL -- free ("P_PTR_FMT") ("P_SZ_FMT")\n", p, sz )
    P_ASSERT( _p_mempool_global )

    if ( !p )
        return;

    bkt = (p_MemBucket*) p_btree_get( &_p_mempool_global->buckets, sz );
#ifndef NDEBUG
    if ( !bkt )
    {
        P_FATAL_ERROR( "unable to find bucket ("P_SZ_FMT")", sz );
    }
    else
    if ( !bkt->alive )
    {
        P_FATAL_ERROR( "unable to find living chunk in bucket ("P_SZ_FMT")", sz );
    }
#endif /* NDEBUG */
    chk = bkt->alive;

    do
    {
        if ( &chk->chunk == p )
        {
            p_sllist_take( bkt->alive, chk, prev );
            p_sllist_push( bkt->trash, chk );
            return;
        }
        prev = chk;
        chk = chk->next;
    }
    while ( chk );
    /* not reached */
    P_FATAL_ERROR( "unable to find chunk in bucket ("P_SZ_FMT")", sz );
}

P_PTR
p_mempool_realloc( P_PTR p,
        const P_SZ sz,
        const P_SZ oldsz )
{
    P_PTR tmp;

    if ( !p )
        return p_mempool_malloc( sz );

    if ( sz == oldsz )
        return p;

    P_TRACE( "-- MEMPOOL -- realloc ("P_PTR_FMT") ("P_SZ_FMT" <- "P_SZ_FMT")\n",
             p, sz, oldsz )

    tmp = p_mempool_malloc( sz );
    memcpy( tmp, p, P_LIMIT( oldsz, sz ));
    p_mempool_free( p, oldsz );
    return tmp;
}

P_VOID
p_mempool_purge( p_MemPool* mp,
        const P_SZ bucket )
{
    P_TRACE( "-- MEMPOOL -- purge "P_SZ_FMT"\n", bucket )
    if ( bucket )
    {
        p_MemBucket* bkt = (p_MemBucket*)
            p_btree_get( &mp->buckets, bucket );
        if ( !bkt )
            return;
        p_mempool_purge_bucket( bkt, NULL );
    }
    else
        p_btree_traverse( &mp->buckets,
                (P_VOID(*)(P_PTR,P_PTR))&p_mempool_purge_bucket, NULL );
}

P_VOID
p_mempool_purge_bucket( p_MemBucket* bkt,
        P_PTR userdata )
{
    P_ASSERT( bkt )

    if ( bkt->trash )
    {
        p_MemChunk* nxt;
        p_MemChunk* chk = bkt->trash;
        do
        {
            nxt = chk->next;
            _P_FREE( chk );
            chk = nxt;
        }
        while ( chk );
        bkt->trash = NULL;
    }
    P_UNUSED( userdata );
}

#ifndef NDEBUG

P_VOID
p_mempool_debug( const p_MemPool* mp )
{
    p_MemBucket* bkt;
    p_MemChunk* chk;
    P_SZ numAlive;
    P_SZ numTrash;
    P_SZ numBuckets = 0;
    p_BTNode* bktnd;

    P_ASSERT( mp )

    printf( "-- MEMPOOL -- debug ("P_PTR_FMT"):\n", (P_PTR)mp );

    if ( !mp->buckets.root )
    {
        printf( "--     Total buckets = 0\n"
                "-- end mempool debug\n" );
        return;
    }

    bktnd = p_btree_least( &mp->buckets );

    for ( ; bktnd; bktnd = p_btree_next( bktnd ) )
    {
        bkt = (p_MemBucket*) bktnd->val;
        numAlive = 0;
        numTrash = 0;

        for ( chk = bkt->alive; chk; chk = chk->next )
            numAlive += 1;
        for ( chk = bkt->trash; chk; chk = chk->next )
            numTrash += 1;

        printf( "--     Bucket ("P_ID_FMT"): "P_SZ_FMT" alive, "P_SZ_FMT" trash\n",
            bktnd->key, numAlive, numTrash );

        numBuckets += 1;
    }

    printf( "--     Total buckets = "P_SZ_FMT"\n",
        numBuckets );

    printf( "-- end mempool debug\n" );
}

P_INT32
p_mempool_test( P_VOID )
{
    printf( "-- MEMPOOL -- test\n"
            "-- sizeof( chunk ) = "P_SZ_FMT"\n"
            "-- sizeof( bucket ) = "P_SZ_FMT"\n"
            "-- sizeof( mempool ) = "P_SZ_FMT"\n",
        sizeof( p_MemChunk ),
        sizeof( p_MemBucket ),
        sizeof( p_MemPool ) );

    p_memlist_debug();
    /* TODO find something here */
    p_memlist_debug();

    printf( "-- end mempool test\n" );
    return 0;
}

#endif /* NDEBUG */

#endif /* P_NO_MEMPOOL */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
