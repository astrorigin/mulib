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
 *  \file mempool.h
 *  \brief Memory pool module.
 *  \author Stanislas Marquis <stan@astrorigin.com>
 */

#ifndef M_MEMPOOL_H
#define M_MEMPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \def M_NO_MEMPOOL
 *  \brief Define this to disable internal mempools (and use memcnt instead).
 */
#ifdef M_NO_MEMPOOL

#include "m_memcnt.h"

#define M_MALLOC          _M_MALLOC
#define M_REALLOC(x,y,z)  _M_REALLOC(x,y)
#define M_FREE(x,y)       _M_FREE(x)

#define M_MALLOC_REF  _M_MALLOC_REF
#define M_REALLOC_REF _M_REALLOC_REF
#define M_FREE_REF    _M_FREE_REF

#define M_MEMPOOL_INIT()
#define M_MEMPOOL_FINI()
#define M_MEMPOOL_STATUS()

#else /* using memory pool */

#include "m_h.h"
#include "m_btree.h"

/**
 *  \brief Request memory from the pool.
 *  \see m_MemPool_malloc
 */
#define M_MALLOC m_MemPool_malloc

/**
 *  \brief Reallocate memory taken from the pool.
 *  \see m_MemPool_realloc
 */
#define M_REALLOC m_MemPool_realloc

/**
 *  \brief Recycle memory taken from the pool.
 *  \see m_MemPool_free
 */
#define M_FREE m_MemPool_free

#define M_MALLOC_REF  &m_MemPool_malloc
#define M_REALLOC_REF &m_MemPool_realloc
#define M_FREE_REF    &m_MemPool_free

/**
 *  \typedef m_MemPool
 */
typedef struct _m_MemPool m_MemPool;

/**
 *  \struct _m_MemPool
 *  \brief The memory pool.
 */
struct _m_MemPool
{
  M_SZ max;
  M_SZ used;
  M_SZ record;
  m_BTree buckets;
};

#include "m_mempool_priv.h"

/**
 *  \brief Get the local mempool.
 */
M_DLLAPI m_MemPool**
m_MemPool_get(M_VOID);

/**
 *  \brief Set the memory pool in use in the current thread (or NULL).
 *  \param mp The memory pool to set for the current thread.
 */
M_DLLAPI M_VOID
m_MemPool_set(const m_MemPool* const mp);

/**
 *  \brief Allocate a memory pool.
 *  \param pool The memory pool.
 *  \param max Maximum amount of memory allowed to use (0 for no limits)
 *  \return M_TRUE, or M_FALSE on error.
 */
M_DLLAPI M_BOOL
m_MemPool_new(m_MemPool** const pool,
        const M_SZ max);

/**
 *  \brief Deallocate a memory pool created by m_MemPool_new.
 *  \param pool The memory pool.
 */
M_DLLAPI M_VOID
m_MemPool_delete(m_MemPool** const pool);

/**
 *  \brief Boilerplate to create and set a global mempool.
 */
#define M_MEMPOOL_INIT() \
  do { \
    if (!m_MemPool_new(m_MemPool_get(), 0)) \
      M_FATAL_ERRMSG("-- MemPool -- unable to create mempool ("M_PTR_FMT")", \
      *m_MemPool_get()); \
  } while (0)

/**
 *  \brief Shortcut to delete the global mempool.
 */
#define M_MEMPOOL_FINI() m_MemPool_delete(m_MemPool_get())

#ifndef NDEBUG
/**
 *  \brief Print mempool status.
 */
#define M_MEMPOOL_STATUS() m_MemPool_debug(*m_MemPool_get())

#else
#define M_MEMPOOL_STATUS()
#endif /* !NDEBUG */

#ifdef __cplusplus
}
#endif
#endif /* M_NO_MEMPOOL */
#endif /* !M_MEMPOOL_H */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
