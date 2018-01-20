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

#include "m_bdict.h"

#include "m_sllist.h"

#undef M_TRACE
#if defined(M_TRACE_MODE) && defined(M_TRACE_DICT)
#define M_TRACE(msg, ...) _M_TRACER("-- BDict -- "msg, __VA_ARGS__)
#else
#define M_TRACE(moo, ...)
#endif

M_BOOL
m_BDict_new(m_BDict** const d)
{
  assert(d);
  M_TRACE("new ("M_PTR_FMT")", d);
  if (!d) return M_FALSE;

  return m_BTree_new(d);
}

M_BOOL
m_BDict_init(m_BDict* const d)
{
  assert(d);
  M_TRACE("init ("M_PTR_FMT")", d);
  if (!d) return M_FALSE;

  return m_BTree_init((m_BTree*)d, M_MALLOC_REF, _m_BTree_node_free_ref);
}

M_VOID
m_BDict_fini(m_BDict* const d)
{
  assert(d);
  M_TRACE("fini ("M_PTR_FMT")", d);
  if (!d) return;

  m_BTree_traverse((m_BTree*)d, (M_VOID(*)(M_PTR))&m_BDict_node_list_delete);
  m_BTree_fini((m_BTree*)d);
}

M_VOID
m_BDict_delete(m_BDict** const d)
{
  assert(d && *d);
  M_TRACE("delete ("M_PTR_FMT")", *d);
  if (!d || !*d) return;

  m_BDict_fini(*d);
  M_FREE(*d, sizeof(m_BDict));
  *d = NULL;
}

M_PTR
m_BDict_get(const m_BDict* const d,
        const m_Array* const key)
{
  m_BDictNode* nd;
  M_ID k;

  assert(d);
  assert(key && key->data);
  M_TRACE("get ("M_PTR_FMT") key ("M_PTR_FMT")", d, key);
  if (!d || !key || !key->data) return NULL;

  k = m_BDict_hash(key->data, key->len);
  nd = m_BTree_get((m_BTree*)d, k);
  for (; nd; nd = nd->next)
  {
    if (nd->key.len == key->len && !memcmp(nd->key.data, key->data, key->len))
      return nd->val;
  }
  return NULL;
}

M_BOOL
m_BDict_set(m_BDict* const d,
        const m_Array* const key,
        const M_PTR const val,
        M_PTR* const prev)
{
  M_ID k;
  m_BDictNode* nd;

  assert(d);
  assert(key && key->data);
  M_TRACE("set ("M_PTR_FMT") key ("M_PTR_FMT") val ("M_PTR_FMT")", d, key, val);
  if (!d || !key || !key->data) return M_FALSE;

  k = m_BDict_hash(key->data, key->len);
  nd = m_BTree_get((m_BTree*)d, k);
  if (nd == NULL)
  {
    if (!m_BDict_node_new(&nd, key, val)) return M_FALSE;
    if (m_BTree_insert((m_BTree*)d, k, nd) != 1) return M_FALSE;
    if (prev) *prev = (M_PTR) val;
  }
  else
  {
    m_BDictNode* last = NULL;
    for (; nd; nd = nd->next)
    {
      if (nd->key.len == key->len && !memcmp(nd->key.data, key->data, key->len))
        break;
      last = nd;
    }
    if (nd == NULL)
    {
      assert(last);
      if (!m_BDict_node_new(&nd, key, val)) return M_FALSE;
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
m_BDict_unset(m_BDict* const d,
        const m_Array* const key)
{
  m_BDictNode* nd, *first, *prev = NULL;
  M_ID k;

  assert(d);
  assert(key && key->data);
  M_TRACE("unset ("M_PTR_FMT") key ("M_PTR_FMT")", d, key);
  if (!d || !key || !key->data) return NULL;

  k = m_BDict_hash(key->data, key->len);
  nd = m_BTree_get((m_BTree*)d, k);
  if (!nd) return NULL;

  if (!nd->next)
  {
    if (nd->key.len == key->len && !memcmp(nd->key.data, key->data, key->len))
    {
      M_PTR val = nd->val;
      m_BTree_remove((m_BTree*)d, k);
      m_BDict_node_delete(&nd);
      return val;
    }
    return NULL;
  }
  first = nd;
  for (; nd; nd = nd->next)
  {
    if (nd->key.len == key->len && !memcmp(nd->key.data, key->data, key->len))
    {
      M_PTR val = nd->val;
      if (nd == first) m_BTree_set((m_BTree*)d, k, nd->next);
      m_SLList_take(first, nd, prev);
      m_BDict_node_delete(&nd);
      return val;
    }
    prev = nd;
  }
  return NULL;
}

M_VOID
m_BDict_traverse(m_BDict* const d,
        M_VOID (* const func)(M_PTR))
{
  return m_Dict_traverse((m_Dict*)d, func);
}

M_VOID
m_BDict_traverse2(m_BDict* const d,
        M_VOID (* const traverse_fn)(M_PTR val, M_PTR udata),
        M_PTR const userdata)
{
  return m_Dict_traverse2((m_Dict*)d, func, userdata);
}

M_VOID
m_BDict_traverse_keyval(m_BDict* const d,
        M_VOID (* const func)(m_Array*, M_PTR))
{
  p_BTNode* nd;
  m_BDictNode* n;

  assert(d);
  assert(func);
  M_TRACE("traverse_keyval ("M_PTR_FMT") func ("M_PTR_FMT")", d, func);
  if (!d || !func) return;

  nd = m_BTree_least((m_BTree*)d);
  for (; nd; nd = m_BTree_next(nd))
  {
    n = (m_BDictNode*) nd->val;
    for (; n; n = n->next)
    {
      (*func)(&n->key, n->val);
    }
  }
}

M_VOID
m_BDict_traverse_keyval2(m_BDict* const d,
        M_VOID (* const func)(m_Array*, M_PTR, M_PTR),
        M_PTR const userdata)
{
  p_BTNode* nd;
  m_BDictNode* n;

  assert(d);
  assert(func);
  M_TRACE("traverse_keyval2 ("M_PTR_FMT") func ("M_PTR_FMT")", d, func);
  if (!d || !func) return;

  nd = m_BTree_least((m_BTree*)d);
  for (; nd; nd = m_BTree_next(nd))
  {
    n = (m_BDictNode*) nd->val;
    for (; n; n = n->next)
    {
      (*func)(&n->key, n->val, userdata);
    }
  }
}

M_BOOL
m_BDict_node_new( m_BDictNode** nd,
        const m_Array* key,
        const M_PTR val )
{
    assert( nd )
    *nd = M_MALLOC( sizeof( m_BDictNode ));
    assert( *nd )
#ifdef NDEBUG
    if ( !*nd )
        return M_FALSE;
#endif
    return m_BDict_node_init( *nd, key, val );
}

M_VOID
m_BDict_node_delete( m_BDictNode** nd )
{
    assert( *nd )
    m_BDict_node_fini( *nd );
    M_FREE( *nd, sizeof( m_BDictNode ));
    *nd = NULL;
}

M_VOID
m_BDict_node_list_delete( m_BDictNode* nd )
{
    m_BDictNode* next;
    assert( nd )
    while ( nd )
    {
        next = nd->next;
        m_BDict_node_delete( &nd );
        nd = next;
    }
}

M_BOOL
m_BDict_node_init( m_BDictNode* nd,
        const m_Array* key,
        const M_PTR val )
{
    assert( nd )
    nd->next = NULL;
    if ( !p_vector_init( &nd->key, key->len, key->unit, key->calc_space_fn ))
        return M_FALSE;
    if ( key->len )
        memcpy( nd->key.data, key->data, key->len );
    nd->key.len = key->len;
    nd->val = (M_PTR) val;
    return M_TRUE;
}

M_VOID
m_BDict_node_fini( m_BDictNode* nd )
{
    assert( nd )
    nd->next = NULL;
    p_vector_fini( &nd->key );
    nd->val = NULL;
}

#if 0
M_BOOL
m_BDict_node_ro_new( m_BDictNode** nd,
        const M_PTR data,
        const M_SZ len,
        const M_PTR val )
{
    assert( nd )
    *nd = M_MALLOC( sizeof( m_BDictNode ));
    assert( *nd )
#ifdef NDEBUG
    if ( !*nd )
        return M_FALSE;
#endif
    return m_BDict_node_ro_init( *nd, data, len, val );
}

M_VOID
m_BDict_node_ro_delete( m_BDictNode** nd )
{
    assert( *nd )
    m_BDict_node_ro_fini( *nd );
    M_FREE( *nd, sizeof( m_BDictNode ));
    *nd = NULL;
}

M_VOID
m_BDict_node_ro_list_delete( m_BDictNode* nd,
        M_PTR userdata )
{
    m_BDictNode* next;
    assert( nd )
    while ( nd )
    {
        next = nd->next;
        m_BDict_node_ro_delete( &nd );
        nd = next;
    }
    M_UNUSED( userdata );
}

M_BOOL
m_BDict_node_ro_init( m_BDictNode* nd,
        const M_PTR data,
        const M_SZ len,
        const M_PTR val )
{
    assert( nd )
    assert( data )
    nd->next = NULL;
    nd->key.data = (M_PTR) data;
    nd->key.len = (M_SZ) len;
    nd->key.unit = 1;
    nd->key.capacity = 0;
    nd->key.calc_space_fn = NULL;
    nd->val = (M_PTR) val;
    return M_TRUE;
}

M_BOOL
m_BDict_ro_set( m_BDict* d,
        const M_PTR data,
        const M_SZ len,
        const M_PTR val,
        M_PTR* old )
{
    M_ID k;
    m_BDictNode* nd;
    assert( d )

    if ( data == NULL || len == 0 )
        return M_TRUE;
    k = m_BDict_hash( data, len );
    nd = m_BTree_get( (m_BTree*)d, k );
    if ( nd == NULL )
    {
        if ( !m_BDict_node_ro_new( &nd, data, len, val ))
            return M_FALSE;
        if ( m_BTree_insert( (m_BTree*)d, k, nd ) != 1 )
            return M_FALSE;
        if ( old )
            *old = (M_PTR) val;
    }
    else
    {
        m_BDictNode* last = NULL;
        for ( ; nd; nd = nd->next )
        {
            if ( nd->key.len == len
                && !memcmp( nd->key.data, data, len ))
                break;
            last = nd;
        }
        if ( nd == NULL )
        {
            assert( last )
            if ( !m_BDict_node_ro_new( &nd, data, len, val ))
                return M_FALSE;
            last->next = nd;
            if ( old )
                *old = (M_PTR) val;
        }
        else
        {
            if ( old )
                *old = nd->val;
            nd->val = (M_PTR) val;
        }
    }
    return M_TRUE;
}
#endif

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
