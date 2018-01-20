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
 *  \file m_memcnt.h
 *  \brief Basic memory checks.
 *  \author Stanislas Marquis <stan@astrorigin.com>
 *
 *  This module provides substitutes for malloc, realloc and free,
 *  and keeps track of the amount of allocated memory by maintaining
 *  a singly-linked list of information (thread-safely).
 *
 *  That is completely disabled when compiled with NDEBUG.
 */

#ifndef M_MEMCNT_H
#define M_MEMCNT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "m_h.h"
#include "m_mutex.h"

#include "m_memcnt_priv.h"

#ifndef NDEBUG

#define _M_MALLOC   m_MemCnt_malloc
#define _M_REALLOC  m_MemCnt_realloc
#define _M_FREE     m_MemCnt_free

#define _M_MALLOC_REF   &m_MemCnt_malloc
#define _M_REALLOC_REF  &m_MemCnt_realloc
#define _M_FREE_REF     &m_MemCnt_free

#define M_MEMCNT_FINI() m_MemCnt_fini()

#define M_MEMCNT_DEBUG() m_MemCnt_debug()

#else /* Release mode */

#define _M_MALLOC   malloc
#define _M_REALLOC  realloc
#define _M_FREE     free

#define _M_MALLOC_REF   &malloc
#define _M_REALLOC_REF  &realloc
#define _M_FREE_REF     &free

#define M_MEMCNT_FINI()

#define M_MEMCNT_DEBUG()

#endif /* !NDEBUG */

#ifdef __cplusplus
}
#endif
#endif /* !M_MEMCNT_H */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
