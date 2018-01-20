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

#ifndef M_NO_MEMPOOL

#include "m_mempool.h"

#include "m_memcnt.h"
#include "m_sllist.h"

#undef M_TRACE
#if defined(M_TRACE_MODE) && defined(M_TRACE_MEMPOOL)
#define M_TRACE(msg, ...) _M_TRACER("-- MemPool -- "msg, __VA_ARGS__)
#else
#define M_TRACE(moo, ...)
#endif

M_TLS m_MemPool*
_m_MemPool_global = NULL;

m_MemPool**
m_MemPool_get(M_VOID)
{
  return &_m_MemPool_global;
}

M_VOID
m_MemPool_set(const m_MemPool* const mp)
{
  M_TRACE("setting global memory pool ("M_PTR_FMT")", mp);
  _m_MemPool_global = (m_MemPool*) mp;
}

M_BOOL
m_MemPool_new(m_MemPool** const mp,
        const M_SZ max)
{
  assert(mp);
  M_TRACE("new ("M_PTR_FMT") max: ("M_SZ_FMT")", mp, max);
  *mp = _M_MALLOC(sizeof(m_MemPool));
  assert(*mp);
  if (!*mp) return M_FALSE;
  return m_MemPool_init(*mp, max);
}

M_BOOL
m_MemPool_init(m_MemPool* const mp,
        const M_SZ max)
{
  assert(mp);
  M_TRACE("init ("M_PTR_FMT") max: ("M_SZ_FMT")", mp, max);
  mp->max = max;
  mp->used = 0;
  mp->record = 0;
  return m_BTree_init2(&mp->buckets, _M_MALLOC_REF, (M_VOID(*)(M_PTR))_M_FREE_REF);
}

M_DLLAPI M_VOID
m_MemPool_delete(m_MemPool** const mp)
{
  assert(mp);
  M_TRACE("delete ("M_PTR_FMT")", mp);
  m_MemPool_fini(*mp);
  _M_FREE(*mp);
  *mp = NULL;
}

M_VOID
m_MemPool_fini(m_MemPool* const mp)
{
  assert(mp);
  M_TRACE("fini ("M_PTR_FMT")", mp);
  m_BTree_traverse(&mp->buckets, &_m_MemPool_buckets_fini);
  m_BTree_fini(&mp->buckets);
}

M_VOID
_m_MemPool_buckets_fini(M_PTR bucket)
{
  m_MemChunk* nxt;
  m_MemChunk* chk = ((m_MemBucket*)bucket)->alive;
  for (; chk; chk = nxt)
  {
    nxt = chk->next;
    _M_FREE(chk);
  }
  chk = ((m_MemBucket*)bucket)->trash;
  for (; chk; chk = nxt)
  {
    nxt = chk->next;
    _M_FREE(chk);
  }
  _M_FREE(bucket);
}

M_PTR
m_MemPool_malloc(const M_SZ sz)
{
  m_MemBucket* bkt;
  m_MemChunk* chk;

  assert(_m_MemPool_global);
  M_TRACE("malloc ("M_SZ_FMT")", sz);

  if (sz == 0) return NULL;

  bkt = (m_MemBucket*) m_BTree_get(&_m_MemPool_global->buckets, sz);
  if (!bkt)
  {
    M_TRACE("new bucket ("M_SZ_FMT")", sz);
    bkt = _M_MALLOC(sizeof(m_MemBucket));
    if (!bkt) return NULL;
    bkt->alive = NULL;
    bkt->trash = NULL;
    if (m_BTree_insert(&_m_MemPool_global->buckets, sz, bkt) < 0) return NULL;
    /*m_BTree_debug(_m_MemPool_global->buckets.root);*/
  }

  if (bkt->trash)
  {
    chk = (m_MemChunk*) m_SLList_PULL(&bkt->trash);
  }
  else
  {
    M_TRACE("new chunk ("M_SZ_FMT")", sz);
    chk = _M_MALLOC(sizeof(m_MemChunk) + sz);
    if (!chk) return NULL;
    _m_MemPool_global->used += sz;

    /* update record */
    if (_m_MemPool_global->used > _m_MemPool_global->record)
      _m_MemPool_global->record = _m_MemPool_global->used;

    /* check limit */
    if (_m_MemPool_global->max
        && _m_MemPool_global->used > _m_MemPool_global->max)
    {
      m_MemPool_purge(_m_MemPool_global, 0);
    }
  }

  assert(chk);
  m_SLList_PUSH(&bkt->alive, chk);
  M_TRACE("malloced ("M_PTR_FMT")", chk->chunk);

  return (M_PTR) chk->chunk;
}

M_VOID
m_MemPool_free(const M_PTR p,
        const M_SZ sz)
{
  m_MemBucket* bkt;
  m_MemChunk* chk;
  m_MemChunk* prev = NULL;

  assert(_m_MemPool_global);
  assert(p);
  assert(sz);
  M_TRACE("free ("M_PTR_FMT") ("M_SZ_FMT")", p, sz);

  if (!p || sz == 0) return;

  bkt = (m_MemBucket*) m_BTree_get(&_m_MemPool_global->buckets, sz);
  if (!bkt)
  {
    M_FATAL_ERRMSG("-- MemPool -- unable to find bucket ("M_SZ_FMT")", sz);
  }
  else
  if (!bkt->alive)
  {
    M_FATAL_ERRMSG("-- MemPool -- unable to find living chunk ("M_PTR_FMT")"
        " in bucket ("M_SZ_FMT")", p, sz);
  }
  chk = bkt->alive;

  do
  {
    if (&chk->chunk == p)
    {
      m_SLList_TAKE(&bkt->alive, chk, prev);
      m_SLList_PUSH(&bkt->trash, chk);
      return;
    }
    prev = chk;
    chk = chk->next;
  }
  while (chk);
  /* not reached */
  M_FATAL_ERRMSG("-- MemPool -- unable to find chunk in bucket ("M_SZ_FMT")", sz);
}

M_PTR
m_MemPool_realloc(const M_PTR p,
        const M_SZ sz,
        const M_SZ oldsz)
{
  M_PTR tmp = NULL;

  M_TRACE("realloc ("M_PTR_FMT") ("M_SZ_FMT" <- "M_SZ_FMT")",
      p, sz, oldsz);

  assert(p);
  assert(sz);
  assert(sz != oldsz);
  if (!p) return m_MemPool_malloc(sz);
  if (sz == 0)
  {
    m_MemPool_free(p, oldsz);
    return NULL;
  }
  if (sz == oldsz) return (M_PTR) p;

  tmp = m_MemPool_malloc(sz);
  assert(tmp);
  if (!tmp) return NULL;
  memcpy(tmp, p, M_LIMIT(oldsz, sz));
  m_MemPool_free(p, oldsz);
  return tmp;
}

M_VOID
m_MemPool_purge(m_MemPool* const mp,
        const M_SZ bucket)
{
  M_TRACE("purge ("M_SZ_FMT")", bucket);
  if (bucket)
  {
    m_MemBucket* bkt = (m_MemBucket*) m_BTree_get(&mp->buckets, bucket);
    if (!bkt) return;
    m_MemPool_purge_bucket(bkt);
  }
  else
    m_BTree_traverse(&mp->buckets, (M_VOID(*)(M_PTR))&m_MemPool_purge_bucket);
}

M_VOID
m_MemPool_purge_bucket(m_MemBucket* const bkt)
{
  assert(bkt);

  if (bkt->trash)
  {
    m_MemChunk* nxt;
    m_MemChunk* chk = bkt->trash;
    do
    {
      nxt = chk->next;
      _M_FREE(chk);
      chk = nxt;
    }
    while (chk);
    bkt->trash = NULL;
  }
}

#ifndef NDEBUG

M_VOID
m_MemPool_debug(const m_MemPool* const mp)
{
  m_MemBucket* bkt;
  m_MemChunk* chk;
  M_SZ numAlive;
  M_SZ numTrash;
  M_SZ numBuckets = 0;
  m_BTNode* bktnd;

  printf("-- MemPool -- debug ("M_PTR_FMT"):\n", (M_PTR)mp);
  if (!mp || !mp->buckets.root)
  {
    printf("--     Total buckets = 0\n"
           "-- end mempool debug\n");
    return;
  }

  bktnd = m_BTree_least(&mp->buckets);
  for (; bktnd; bktnd = m_BTree_next(bktnd))
  {
    bkt = (m_MemBucket*) bktnd->val;
    numAlive = 0;
    numTrash = 0;

    for (chk = bkt->alive; chk; chk = chk->next)
      numAlive += 1;
    for (chk = bkt->trash; chk; chk = chk->next)
      numTrash += 1;

    printf("--     Bucket ("M_ID_FMT"): "M_SZ_FMT" alive, "M_SZ_FMT" trash\n",
          bktnd->key, numAlive, numTrash);

    numBuckets += 1;
  }
  printf("--     Total buckets = "M_SZ_FMT"\n", numBuckets);
  printf("-- end mempool debug\n");
}

#endif /* !NDEBUG */
#endif /* !M_NO_MEMPOOL */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
