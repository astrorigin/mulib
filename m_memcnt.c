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

#include "m_memcnt.h"

#ifndef NDEBUG

#include "m_sllist.h"

#undef M_TRACE
#if defined(M_TRACE_MODE) && defined(M_TRACE_MEMCNT)
#define M_TRACE(msg, ...) _M_TRACER("-- MemCnt -- "msg, __VA_ARGS__)
#else
#define M_TRACE(moo, ...)
#endif

M_MUTEX
_m_MemCnt_mutex = M_MUTEX_INITIALIZER;

m_MemCntNode*
_m_MemCnt_nodes = NULL;

M_PTR
m_MemCnt_malloc(const M_SZ sz)
{
  m_MemCntNode* nd;

#ifdef _MSC_VER
  m_Mutex_init(_m_MemCnt_mutex);
#endif

  M_TRACE("allocating "M_SZ_FMT" bytes:", sz);

  assert(sz);
  if (sz == 0) return NULL;

  nd = malloc(sizeof(m_MemCntNode) + sz);
  if (!nd) return NULL;
  nd->sz = sz;

  m_MemCnt_lock();
  m_SLList_PUSH(&_m_MemCnt_nodes, nd);
  m_MemCnt_unlock();

  M_TRACE("now tracking ("M_PTR_FMT")", nd->data);
  return nd->data;
}

M_VOID
m_MemCnt_free(const M_PTR p)
{
  m_MemCntNode* nd;
  m_MemCntNode* prev = NULL;

  M_TRACE("freeing tracked ("M_PTR_FMT"):", p);

  assert(_m_MemCnt_nodes);
  assert(p);
  if (!p) return;

  m_MemCnt_lock();
  for (nd = _m_MemCnt_nodes; nd; nd = nd->next)
  {
    if (nd->data == p)
    {
      m_SLList_TAKE(&_m_MemCnt_nodes, nd, prev);
      m_MemCnt_unlock();
      M_TRACE("now freeing "M_SZ_FMT" bytes", nd->sz);
      free(nd);
      return;
    }
    prev = nd;
  }

  M_FATAL_ERRMSG("-- MemCnt -- pointer ("M_PTR_FMT") untracked", p);
}

M_PTR
m_MemCnt_realloc(const M_PTR p,
        const M_SZ sz)
{
  m_MemCntNode* nd;
  m_MemCntNode* prev = NULL;

  M_TRACE("reallocating tracked ("M_PTR_FMT"):", p);

  assert(_m_MemCnt_nodes);
  assert(p);
  assert(sz);
  if (!p) return m_MemCnt_malloc(sz);
  if (sz == 0)
  {
    m_MemCnt_free(p);
    return NULL;
  }

  m_MemCnt_lock();
  for (nd = _m_MemCnt_nodes; nd; nd = nd->next)
  {
    if (nd->data == p)
    {
      m_SLList_TAKE(&_m_MemCnt_nodes, nd, prev);
      m_MemCnt_unlock();
      M_TRACE("reallocating "M_SZ_FMT" to "M_SZ_FMT" bytes", nd->sz, sz);
      nd = realloc(nd, sizeof(m_MemCntNode) + sz);
      if (!nd) return NULL;
      nd->sz = sz;

      m_MemCnt_lock();
      m_SLList_PUSH(&_m_MemCnt_nodes, nd);
      m_MemCnt_unlock();

      return nd->data;
    }
    prev = nd;
  }

  M_FATAL_ERRMSG("-- MemCnt -- pointer ("M_PTR_FMT") untracked", p);
  return NULL; /* dummy */
}

M_VOID
m_MemCnt_fini(M_VOID)
{
  m_MemCntNode* nd;

  M_TRACE("purge memory");

  assert(_m_MemCnt_nodes);

  m_MemCnt_lock();
  while (_m_MemCnt_nodes)
  {
    nd = (m_MemCntNode*) m_SLList_PULL(&_m_MemCnt_nodes);
    free(nd);
  }
  m_MemCnt_unlock();

  M_TRACE("finishing mutex");
  m_Mutex_fini(_m_MemCnt_mutex);
}

M_VOID
m_MemCnt_debug(M_VOID)
{
  M_SZ cnt = 0;
  M_SZ used = 0;
  m_MemCntNode* nd;

  m_MemCnt_lock();
  for (nd = _m_MemCnt_nodes; nd; nd = nd->next)
  {
    cnt += 1;
    used += nd->sz;
  }
  m_MemCnt_unlock();

  printf("-- MemCnt -- debug:\n"
         "--     Count:  "M_SZ_FMT"\n"
         "--     Using:  "M_SZ_FMT"\n"
         "--     (Self:  "M_SZ_FMT")\n"
         "-- end memcnt debug\n",
    cnt, used, cnt * sizeof(m_MemCntNode));
}

#endif /* !NDEBUG */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
