/*
    mulib
    Copyright 2014-2018 Stanislas Marquis <stan@astrorigin.com>

MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

 */

/**
 *  \file mempool_priv.h
 *  \brief Memory pool module (private).
 *  \author Stanislas Marquis <stan@astrorigin.com>
 */

#ifndef M_MEMPOOL_PRIV_H
#define M_MEMPOOL_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef M_NO_MEMPOOL

/**
 *  \typedef m_MemChunk
 */
typedef struct _m_MemChunk m_MemChunk;

/**
 *  \struct _m_MemChunk
 *  \brief A chunk of "reusable" memory.
 */
struct _m_MemChunk
{
  m_MemChunk* next;
  M_PTR chunk[];
};

/**
 *  \typedef m_MemBucket
 */
typedef struct _m_MemBucket m_MemBucket;

/**
 *  \struct _m_MemBucket
 *  \brief A memory "bucket".
 */
struct _m_MemBucket
{
  m_MemChunk* alive;
  m_MemChunk* trash;
};

/**
 *  \brief One global memory pool per thread (private).
 *  \see m_MemPool_set (TLS data may not be exported).
 */
extern M_TLS m_MemPool*
_m_MemPool_global;

/**
 *  \brief Initialize a memory pool.
 *  \param pool The memory pool.
 *  \param max Maximum amount of memory allowed to use (0 for no limits)
 *  \return M_TRUE, or M_FALSE on error.
 */
M_DLLAPI M_BOOL
m_MemPool_init(m_MemPool* const pool,
        const M_SZ max);

/**
 *  \brief Finalize a memory pool (free all chunks).
 *  \param pool The memory pool.
 */
M_DLLAPI M_VOID
m_MemPool_fini(m_MemPool* const pool);

/**
 *  \brief Function to release buckets and all chunks.
 *  \param bucket The bucket.
 */
M_DLLAPI M_VOID
_m_MemPool_buckets_fini(M_PTR bucket);

/**
 *  \brief Let the memory pool find some memory to use.
 *  \param sz Size of memory requested.
 *  \return Pointer to memory, or null on error.
 */
M_DLLAPI M_PTR
m_MemPool_malloc(const M_SZ sz);

/**
 *  \brief Let the memory pool recycle a chunk.
 *  \param p Pointer to memory previously taken from the pool.
 *  \param sz Size of chunk.
 */
M_DLLAPI M_VOID
m_MemPool_free(const M_PTR p,
        const M_SZ sz);

/**
 *  \brief Resize memory taken from the memory pool.
 *  \param p Pointer to memory previously taken from the pool.
 *  \param sz Size of memory requested.
 *  \param oldsz Current size of memory.
 *  \return Pointer to memory.
 */
M_DLLAPI M_PTR
m_MemPool_realloc(const M_PTR p,
        const M_SZ sz,
        const M_SZ oldsz);

/**
 *  \brief Really frees one or all buckets of memory.
 *  \param mp The memory pool.
 *  \param bucket The bucket size, or 0 for all buckets.
 */
M_DLLAPI M_VOID
m_MemPool_purge(m_MemPool* const mp,
        const M_SZ bucket);

/**
 *  \brief Empty the trash of the bucket.
 *  \param bucket The memory bucket.
 */
M_DLLAPI M_VOID
m_MemPool_purge_bucket(m_MemBucket* const bucket);

#ifndef NDEBUG

/**
 *  \brief Print some stats to stdout.
 *  \param pool The memory pool to debug.
 */
M_DLLAPI M_VOID
m_MemPool_debug(const m_MemPool* const pool);

#endif /* NDEBUG */

#ifdef __cplusplus
}
#endif
#endif /* !M_NO_MEMPOOL */
#endif /* !M_MEMPOOL_PRIV_H */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
