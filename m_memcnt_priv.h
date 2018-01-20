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
 *  \file m_memcnt_priv.h
 *  \brief Basic memory checks (private).
 *  \author Stanislas Marquis <stan@astrorigin.com>
 */

#ifndef M_MEMCNT_PRIV_H
#define M_MEMCNT_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NDEBUG

/**
 *  \brief Acquire the global lock.
 */
#define m_MemCnt_lock() m_Mutex_lock(_m_MemCnt_mutex)

/**
 *  \brief Release the global lock.
 */
#define m_MemCnt_unlock() m_Mutex_unlock(_m_MemCnt_mutex)

/**
 *  \brief A global mutex (private).
 */
extern M_MUTEX
_m_MemCnt_mutex;

/**
 *  \typedef m_MemCntNode
 */
typedef struct _m_MemCntNode m_MemCntNode;

/**
 *  \struct _m_MemCntNode
 *  \brief A node with information about allocated memory.
 */
struct _m_MemCntNode
{
  m_MemCntNode* next;
  M_SZ sz;
  M_PTR data[];
};

/**
 *  \brief The list of nodes (private).
 */
extern m_MemCntNode*
_m_MemCnt_nodes;

/**
 *  \brief Malloc replacement.
 *  \param sz Size requested.
 *  \return Memory location, or NULL on error.
 */
M_DLLAPI M_PTR
m_MemCnt_malloc(const M_SZ sz);

/**
 *  \brief Free replacement.
 *  \param p Pointer to free.
 */
M_DLLAPI M_VOID
m_MemCnt_free(const M_PTR p);

/**
 *  \brief Realloc replacement.
 *  \param p Pointer to realloc.
 *  \param sz Size requested.
 *  \return Memory location, or NULL on error.
 */
M_DLLAPI M_PTR
m_MemCnt_realloc(const M_PTR p,
        const M_SZ sz);

/**
 *  \brief Free all the memory.
 */
M_DLLAPI M_VOID
m_MemCnt_fini(M_VOID);

/**
 *  \brief Print some stats to stdout.
 */
M_DLLAPI M_VOID
m_MemCnt_debug(M_VOID);

#endif /* !NDEBUG */

#ifdef __cplusplus
}
#endif
#endif /* !M_MEMCNT_PRIV_H */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
