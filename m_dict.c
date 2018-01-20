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

#include "m_dict.h"

#include "m_sllist.h"

#undef M_TRACE
#if defined(M_TRACE_MODE) && defined(M_TRACE_DICT)
#define M_TRACE(msg, ...) _M_TRACER("-- Dict -- "msg, __VA_ARGS__)
#else
#define M_TRACE(moo, ...)
#endif

M_BOOL
m_Dict_new(m_Dict** const d)
{
  assert(d);
  M_TRACE("new ("M_PTR_FMT")", d);
  if (!d) return M_FALSE;

  return m_BTree_new(d);
}

M_BOOL
m_Dict_init(m_Dict* const d)
{
  assert(d);
  M_TRACE("init ("M_PTR_FMT")", d);
  if (!d) return M_FALSE;

  return m_BTree_init((m_BTree*)d);
}

M_VOID
m_Dict_fini(m_Dict* const d)
{
  assert(d);
  M_TRACE("fini ("M_PTR_FMT")", d);
  if (!d) return;

  if (d->finalize_fn)
  {
    m_Dict_traverse(d, d->finalize_fn);
    d->finalize_fn = NULL;
  }
  m_BTree_traverse((m_BTree*)d, (M_VOID(*)(M_PTR))&m_DictNode_list_delete);
  m_BTree_fini((m_BTree*)d);
}

M_VOID
m_Dict_delete(m_Dict** const d)
{
  assert(d && *d);
  M_TRACE("delete ("M_PTR_FMT")", *d);
  if (!d || !*d) return;

  m_Dict_fini(*d);
  M_FREE(*d, sizeof(m_Dict));
  *d = NULL;
}

M_ID
m_Dict_hash(const M_PTR k,
        const M_SZ len)
{ /* this function is public domain */
  M_UCHAR* p = (M_UCHAR*) k;
  M_ID h = 0;
  M_SZ i;

  for (i = 0; i < len; i++)
  {
    h += p[i];
    h += ( h << 10 );
    h ^= ( h >> 6 );
  }
  h += ( h << 3 );
  h ^= ( h >> 11 );
  h += ( h << 15 );
  return h;
}

M_PTR
m_Dict_get(const m_Dict* const d,
        const M_CHAR* const key)
{
  m_DictNode* nd;
  M_ID k;

  assert(d);
  assert(key && *key);
  M_TRACE("get ("M_PTR_FMT") key ("M_STR_FMT")", d, key);
  if (!d || !key || !*key) return NULL;

  k = m_Dict_hash(key, strlen(key));
  nd = m_BTree_get((m_BTree*)d, k);
  for (; nd; nd = nd->next)
  {
    if (!strcmp(nd->key.data, key)) return nd->val;
  }
  return NULL;
}

M_BOOL
m_Dict_set(m_Dict* const d,
        const M_CHAR* const key,
        const M_PTR const val,
        M_PTR* const prev)
{
  M_ID k;
  m_DictNode* nd;

  assert(d);
  assert(key && *key);
  M_TRACE("set ("M_PTR_FMT") key ("M_STR_FMT") val ("M_PTR_FMT")", d, key, val);
  if (!d || !key || !*key) return M_FALSE;

  k = m_Dict_hash(key, strlen(key));
  nd = m_BTree_get((m_BTree*)d, k);
  if (nd == NULL)
  {
    if (!m_DictNode_new(&nd, key, val)) return M_FALSE;
    if (m_BTree_insert((m_BTree*)d, k, nd) != 1) return M_FALSE;
    if (prev) *prev = (M_PTR) val;
  }
  else
  {
    m_DictNode* last = NULL;
    for (; nd; nd = nd->next)
    {
      if (!strcmp(nd->key.data, key)) break;
      last = nd;
    }
    if (nd == NULL)
    {
      assert(last);
      if (!m_DictNode_new(&nd, key, val)) return M_FALSE;
      last->next = nd;
      if (prev) *prev = (M_PTR) val;
    }
    else
    {
      if (prev) *prev = nd->val;
      nd->val = (M_PTR) val;
    }
  }
  return M_TRUE;
}

M_PTR
m_Dict_unset(m_Dict* const d,
        const M_CHAR* const key)
{
  m_DictNode* nd, *first, *prev = NULL;
  M_ID k;

  assert(d);
  assert(key && *key);
  M_TRACE("unset ("M_PTR_FMT") key ("M_STR_FMT")", d, key);
  if (!d || !key || !*key) return NULL;

  k = m_Dict_hash(key, strlen(key));
  nd = m_BTree_get((m_BTree*)d, k);
  if (!nd) return NULL;
  if (!nd->next)
  {
    if (!strcmp(nd->key.data, key))
    {
      M_PTR val = nd->val;
      m_BTree_remove((m_BTree*)d, k, NULL);
      m_DictNode_delete(&nd);
      return val;
    }
    return NULL;
  }
  first = nd;
  for (; nd; nd = nd->next)
  {
    if (!strcmp(nd->key.data, key))
    {
      M_PTR val = nd->val;
      if (nd == first) m_BTree_set((m_BTree*)d, k, nd->next, NULL);
      m_SLList_TAKE(&first, nd, prev);
      m_DictNode_delete(&nd);
      return val;
    }
    prev = nd;
  }
  return NULL;
}

M_VOID
m_Dict_traverse(m_Dict* const d,
        M_VOID (* const func)(M_PTR))
{
  m_BTNode* nd;
  m_DictNode* n;

  assert(d);
  assert(func);
  if (!d || !func) return;

  nd = m_BTree_least((m_BTree*)d);
  for (; nd; nd = m_BTree_next(nd))
  {
    n = (m_DictNode*) nd->val;
    for (; n; n = n->next)
    {
      (*func)(n->val);
    }
  }
}

M_VOID
m_Dict_traverse2(m_Dict* const d,
        M_VOID (* const func)(M_PTR, M_PTR),
        M_PTR const userdata)
{
  m_BTNode* nd;
  m_DictNode* n;

  assert(d);
  assert(func);
  if (!d || !func) return;

  nd = m_BTree_least((m_BTree*)d);
  for (; nd; nd = m_BTree_next(nd))
  {
    n = (m_DictNode*) nd->val;
    for (; n; n = n->next)
    {
      (*func)(n->val, userdata);
    }
  }
}

M_VOID
m_Dict_traverse_keyval(m_Dict* const d,
        M_VOID (* const func)(m_String*, M_PTR))
{
  m_BTNode* nd;
  m_DictNode* n;

  assert(d);
  assert(func);
  if (!d || !func) return;

  nd = m_BTree_least((m_BTree*)d);
  for (; nd; nd = m_BTree_next(nd))
  {
    n = (m_DictNode*) nd->val;
    for (; n; n = n->next)
    {
      (*func)(&n->key, n->val);
    }
  }
}

M_VOID
m_Dict_traverse_keyval2(m_Dict* const d,
        M_VOID (* const func)(m_String*, M_PTR, M_PTR),
        M_PTR const userdata)
{
  m_BTNode* nd;
  m_DictNode* n;

  assert(d);
  assert(func);
  if (!d || !func) return;

  nd = m_BTree_least((m_BTree*)d);
  for (; nd; nd = m_BTree_next(nd))
  {
    n = (m_DictNode*) nd->val;
    for (; n; n = n->next)
    {
      (*func)(&n->key, n->val, userdata);
    }
  }
}

M_BOOL
m_DictNode_new(m_DictNode** const nd,
        const M_CHAR* const key,
        const M_PTR const val)
{
  assert(nd);
  if (!nd) return M_FALSE;

  *nd = M_MALLOC(sizeof(m_DictNode));
  assert(*nd);
  if (!*nd) return M_FALSE;

  return m_DictNode_init(*nd, key, val);
}

M_VOID
m_DictNode_delete(m_DictNode** const nd)
{
  assert(nd && *nd);
  if (!nd || !*nd) return;

  m_DictNode_fini(*nd);
  M_FREE(*nd, sizeof(m_DictNode));
  *nd = NULL;
}

M_VOID
m_DictNode_list_delete(m_DictNode* nd)
{
  m_DictNode* next;

  assert(nd);
  if (!nd) return;

  for (;nd; nd = next)
  {
    next = nd->next;
    m_DictNode_delete(&nd);
  }
}

M_BOOL
m_DictNode_init(m_DictNode* const nd,
        const M_CHAR* const key,
        const M_PTR const val)
{
  assert(nd);
  if (!nd) return M_FALSE;

  nd->next = NULL;
  if (!m_String_init(&nd->key, key)) return M_FALSE;
  nd->val = (M_PTR) val;
  return M_TRUE;
}

M_VOID
m_DictNode_fini(m_DictNode* const nd)
{
  assert(nd);
  if (!nd) return;

  nd->next = NULL;
  m_String_fini(&nd->key);
  nd->val = NULL;
}

#ifndef NDEBUG

M_VOID
m_Dict_debug(const m_Dict* const d)
{
  m_BTNode* btn;
  m_DictNode* dn;

  assert(d);

  printf("-- Dict -- debug ("M_PTR_FMT"):\n", d);

  btn = m_BTree_least((m_BTree*)d);
  for (; btn; btn = m_BTree_next(btn))
  {
    dn = btn->val;
    for (; dn; dn = dn->next)
    {
      printf("--     \"%s\": ("M_PTR_FMT")\n",
          (char*)dn->key.data, dn->val);
    }
  }

  printf("-- end dict debug\n");
}

#endif /* NDEBUG */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
