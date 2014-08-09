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
 *  \file mempool.h
 *  \brief Memory pool module.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 */

#ifndef P_MEMPOOL_H
#define P_MEMPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \def P_NO_MEMPOOL
 *  \brief Define this to forget about the mempools.
 */
#ifdef P_NO_MEMPOOL

#include "p_memlist.h"

#define P_MALLOC            _P_MALLOC
#define P_REALLOC(x,y,z)    _P_REALLOC(x,y)
#define P_FREE(x,y)         _P_FREE(x)

#define P_MALLOC_REF    _P_MALLOC_REF
#define P_REALLOC_REF   _P_REALLOC_REF
#define P_FREE_REF      _P_FREE_REF

#else /* using memory pool */

#include "p_h.h"
#include "p_btree.h"

/**
 *  \brief Request memory from the pool.
 *  \see p_mempool_malloc
 */
#define P_MALLOC    p_mempool_malloc

/**
 *  \brief Recycle memory taken from the pool.
 *  \see p_mempool_free
 */
#define P_FREE      p_mempool_free

/**
 *  \brief Reallocate memory taken from the pool.
 *  \see p_mempool_realloc
 */
#define P_REALLOC   p_mempool_realloc

#define P_MALLOC_REF    &p_mempool_malloc
#define P_REALLOC_REF   &p_mempool_realloc
#define P_FREE_REF      &p_mempool_free

/**
 *  \typedef p_MemChunk
 */
typedef struct _p_memchunk_t p_MemChunk;

/**
 *  \struct _p_memchunk_t
 *  \brief A chunk of reusable memory.
 */
struct _p_memchunk_t
{
    p_MemChunk*     next;
    P_PTR           chunk[];
};

/**
 *  \typedef p_MemBucket
 */
typedef struct _p_membucket_t p_MemBucket;

/**
 *  \struct _p_membucket_t
 *  \brief A memory bucket.
 */
struct _p_membucket_t
{
    p_MemChunk*     alive;
    p_MemChunk*     trash;
};

/**
 *  \typedef p_MemPool
 */
typedef struct _p_mempool_t p_MemPool;

/**
 *  \struct _p_mempool_t
 *  \brief The memory pool.
 */
struct _p_mempool_t
{
    P_SZ        max;
    P_SZ        used;
    p_BTree     buckets;
#ifndef NDEBUG
    P_SZ        record;
#endif /* NDEBUG */
};

/**
 *  \brief Set the memory pool in use in the current thread.
 *  \param mp The memory pool to set for the current thread.
 */
P_EXPORT P_VOID
p_mempool_set( const p_MemPool* mp );

/**
 *  \brief One global memory pool per thread (private).
 *  \see p_mempool_set (TLS data may not be exported).
 */
extern P_TLS p_MemPool*
_p_mempool_global;

/**
 *  \brief Initialize a memory pool.
 *  \param pool The memory pool.
 *  \param max Maximum amount of memory allowed to use (0 for no limits)
 */
P_EXPORT P_VOID
p_mempool_init( p_MemPool* pool,
        const P_SZ max );

/**
 *  \brief Finalize a memory pool (free all chunks).
 *  \param pool The memory pool.
 */
P_EXPORT P_VOID
p_mempool_fini( p_MemPool* pool );

/**
 *  \brief Function to release buckets and all chunks.
 *  \param bucket The bucket.
 */
P_EXPORT P_VOID
_p_mempool_buckets_fini( P_PTR bucket,
        P_PTR userdata );

/**
 *  \brief Let the memory pool find some memory to use.
 *  \param sz Size of memory requested.
 *  \return Pointer to memory, or null on error.
 */
P_EXPORT P_PTR
p_mempool_malloc( const P_SZ sz );

/**
 *  \brief Let the memory pool recycle a chunk.
 *  \param p Pointer to memory previously taken from the pool.
 *  \param sz Size of chunk.
 */
P_EXPORT P_VOID
p_mempool_free( P_PTR p,
        const P_SZ sz );

/**
 *  \brief Resize memory taken from the memory pool.
 *  \param p Pointer to memory previously taken from the pool.
 *  \param sz Size of memory requested.
 *  \param oldsz Current size of memory.
 *  \return Pointer to memory.
 */
P_EXPORT P_PTR
p_mempool_realloc( P_PTR p,
        const P_SZ sz,
        const P_SZ oldsz );

/**
 *  \brief Really frees one or all buckets of memory.
 *  \param mp The memory pool.
 *  \param bucket The bucket size, or 0 for all buckets.
 */
P_EXPORT P_VOID
p_mempool_purge( p_MemPool* mp,
        const P_SZ bucket );

/**
 *  \brief Empty the trash of the bucket.
 *  \param bucket The memory bucket.
 */
P_EXPORT P_VOID
p_mempool_purge_bucket( p_MemBucket* bucket,
        P_PTR userdata );

#ifndef NDEBUG

/**
 *  \brief Print some stats to stdout.
 *  \param pool The memory pool to debug.
 */
P_EXPORT P_VOID
p_mempool_debug( const p_MemPool* pool );

/**
 *  \brief Memory pool unittest
 *  \return 0 if all checks were ok.
 */
P_EXPORT P_INT32
p_mempool_test( P_VOID );

#endif /* NDEBUG */

#ifdef __cplusplus
}
#endif

#endif /* P_NO_MEMPOOL */
#endif /* P_MEMPOOL_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
