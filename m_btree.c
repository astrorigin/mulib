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

#include "m_btree.h"

#include "m_mempool.h"

#undef M_TRACE
#if defined(M_TRACE_MODE) && defined(M_TRACE_BTREE)
#define M_TRACE(msg, ...) _M_TRACER("-- BTree -- "msg, __VA_ARGS__)
#else
#define M_TRACE(moo, ...)
#endif

#define parent(x) (x)->parent
#define left_child(x) (x)->less
#define right_child(x) (x)->more
#define BalanceFactor(x) (x)->bfactor

M_BOOL
m_BTree_new(m_BTree** const bt)
{
  assert(bt && !*bt);
  M_TRACE("new ("M_PTR_FMT")", bt);
  if (!bt) return M_FALSE;

  *bt = M_MALLOC(sizeof(m_BTree));
  assert(*bt);
  if (!*bt) return M_FALSE;
  return m_BTree_init2(*bt, M_MALLOC_REF, _m_BTNode_free_ref);
}

M_VOID
m_BTree_delete(m_BTree** const bt)
{
  assert(bt && *bt);
  M_TRACE("delete ("M_PTR_FMT")", *bt);
  if (!bt || !*bt) return;

  m_BTree_fini(*bt);
  M_FREE(*bt, sizeof(m_BTree));
  *bt = NULL;
}

M_BOOL
m_BTree_init2(m_BTree* const bt,
        M_PTR (* const mallocdoer)(M_SZ),
        M_VOID (* const freedoer)(M_PTR))
{
  assert(bt);
  M_TRACE("init2 ("M_PTR_FMT")", bt);
  if (!bt) return M_FALSE;

  bt->root = NULL;
  bt->mallocdoer = mallocdoer ? mallocdoer : &malloc;
  bt->freedoer = freedoer ? freedoer : &free;
  bt->finalize_fn = NULL;
  return M_TRUE;
}

M_BOOL
m_BTree_init(m_BTree* const bt)
{
  assert(bt);
  M_TRACE("init ("M_PTR_FMT")", bt);
  if (!bt) return M_FALSE;

  return m_BTree_init2(bt, M_MALLOC_REF, _m_BTNode_free_ref);
}

M_VOID
m_BTree_fini(m_BTree* const bt)
{
  assert(bt);
  M_TRACE("fini ("M_PTR_FMT")", bt);
  if (!bt) return;

  if (bt->finalize_fn) m_BTree_traverse(bt, bt->finalize_fn);
  m_BTNode_delete_all(&bt->root, bt->freedoer);
  bt->mallocdoer = NULL;
  bt->freedoer = NULL;
  bt->finalize_fn = NULL;
}

M_BOOL
m_BTNode_new(m_BTNode** const bt,
        const M_ID key,
        const M_PTR val,
        const m_BTNode* const parent,
        M_PTR (* const mallocdoer)(M_SZ))
{
  assert(bt && !*bt);
  M_TRACE("Node -- new key ("M_ID_FMT") value ("M_PTR_FMT") parent ("M_ID_FMT")",
          key, val, parent ? parent->key : 0);
  if (!bt) return M_FALSE;

  *bt = (*mallocdoer)(sizeof(m_BTNode));
  assert(*bt);
  if (!*bt) return M_FALSE;

  (*bt)->key = key;
  (*bt)->val = (M_PTR) val;
  (*bt)->parent = (m_BTNode*) parent;
  (*bt)->less = NULL;
  (*bt)->more = NULL;
  (*bt)->bfactor = 0;
  return M_TRUE;
}

M_VOID
m_BTNode_delete_all(m_BTNode** const bt,
        M_VOID (* const freedoer)(M_PTR))
{
  m_BTNode* nd, *next;

  assert(bt);
  M_TRACE("delete_all ("M_PTR_FMT")", *bt);
  if (!*bt) return; /* Dont assert that */

  next = m_BTNode_least(*bt);
  while (next)
  {
    nd = next;
    next = m_BTNode_next(nd);
    m_BTNode_delete(&nd, freedoer);
  }
}

M_VOID
m_BTNode_delete(m_BTNode** const bt,
        M_VOID (* const freedoer)(M_PTR))
{
  assert(bt);
  M_TRACE("Node -- delete ("M_PTR_FMT")", *bt);
  if (!*bt) return; /* Dont assert that */

  (*freedoer)(*bt);
  *bt = NULL;
}

#ifndef M_NO_MEMPOOL
M_VOID
_m_BTNode_free(M_PTR p)
{
  M_TRACE("Node -- free ("M_PTR_FMT")", p);
  M_FREE(p, sizeof(m_BTNode));
}
#endif

m_BTNode*
m_BTNode_rotate_Left(m_BTNode* const X,
        m_BTNode* const Z)
{
  m_BTNode* t23;

  M_TRACE("Node -- rotate_Left ("M_PTR_FMT") ("M_PTR_FMT")", X, Z);

  // Z is by 2 higher than its sibling
  t23 = left_child(Z); // Inner child of Z
  right_child(X) = t23;
  if (t23 != NULL)
    parent(t23) = X;
  left_child(Z) = X;
  parent(X) = Z;
  // 1st case, BalanceFactor(Z) == 0, only happens with deletion, not insertion:
  if (BalanceFactor(Z) == 0) { // t23 has been of same height as t4
    BalanceFactor(X) = +1;   // t23 now higher
    BalanceFactor(Z) = -1;   // t4 now lower than X
  } else { // 2nd case happens with insertion or deletion:
    BalanceFactor(X) = 0;
    BalanceFactor(Z) = 0;
  }
  return Z; // return new root of rotated subtree
}

m_BTNode*
m_BTNode_rotate_Right(m_BTNode* const X,
        m_BTNode* const Z)
{
  m_BTNode* t23;

  M_TRACE("Node -- rotate_Right ("M_PTR_FMT") ("M_PTR_FMT")", X, Z);

  // Z is by 2 higher than its sibling
  t23 = right_child(Z); // Inner child of Z
  left_child(X) = t23;
  if (t23 != NULL)
    parent(t23) = X;
  right_child(Z) = X;
  parent(X) = Z;
  // 1st case, BalanceFactor(Z) == 0, only happens with deletion, not insertion:
  if (BalanceFactor(Z) == 0) { // t23 has been of same height as t4
    BalanceFactor(X) = -1;   // t23 now higher
    BalanceFactor(Z) = +1;   // t4 now lower than X
  } else { // 2nd case happens with insertion or deletion:
    BalanceFactor(X) = 0;
    BalanceFactor(Z) = 0;
  }
  return Z; // return new root of rotated subtree
}

m_BTNode*
m_BTNode_rotate_RightLeft(m_BTNode* const X,
        m_BTNode* const Z)
{
  m_BTNode* Y, *t2, *t3;

  M_TRACE("Node -- rotate_RightLeft ("M_PTR_FMT") ("M_PTR_FMT")", X, Z);

  // Z is by 2 higher than its sibling
  Y = left_child(Z); // Inner child of Z
  // Y is by 1 higher than sibling
  t3 = right_child(Y);
  left_child(Z) = t3;
  if (t3 != NULL)
    parent(t3) = Z;
  right_child(Y) = Z;
  parent(Z) = Y;
  t2 = left_child(Y);
  right_child(X) = t2;
  if (t2 != NULL)
    parent(t2) = X;
  left_child(Y) = X;
  parent(X) = Y;
  // 1st case, BalanceFactor(Y) > 0, happens with insertion or deletion:
  if (BalanceFactor(Y) > 0) { // t3 was higher
    BalanceFactor(X) = -1;  // t1 now higher
    BalanceFactor(Z) = 0;
  }
  else // 2nd case, BalanceFactor(Y) == 0, only happens with deletion, not insertion:
  if (BalanceFactor(Y) == 0) {
    BalanceFactor(X) = 0;
    BalanceFactor(Z) = 0;
  } else { // 3rd case happens with insertion or deletion:
    // t2 was higher
    BalanceFactor(X) = 0;
    BalanceFactor(Z) = 1;  // t4 now higher
  }
  BalanceFactor(Y) = 0;
  return Y; // return new root of rotated subtree
}

m_BTNode*
m_BTNode_rotate_LeftRight(m_BTNode* const X,
        m_BTNode* const Z)
{
  m_BTNode* Y, *t2, *t3;

  M_TRACE("Node -- rotate_LeftRight ("M_PTR_FMT") ("M_PTR_FMT")", X, Z);

  // Z is by 2 higher than its sibling
  Y = right_child(Z); // Inner child of Z
  // Y is by 1 higher than sibling
  t3 = left_child(Y);
  right_child(Z) = t3;
  if (t3 != NULL)
    parent(t3) = Z;
  left_child(Y) = Z;
  parent(Z) = Y;
  t2 = right_child(Y);
  left_child(X) = t2;
  if (t2 != NULL)
    parent(t2) = X;
  right_child(Y) = X;
  parent(X) = Y;
  // 1st case, BalanceFactor(Y) > 0, happens with insertion or deletion:
  if (BalanceFactor(Y) > 0) { // t3 was higher
    BalanceFactor(X) = 0;  // t1 now higher
    BalanceFactor(Z) = -1;
  }
  else // 2nd case, BalanceFactor(Y) == 0, only happens with deletion, not insertion:
  if (BalanceFactor(Y) == 0) {
    BalanceFactor(X) = 0;
    BalanceFactor(Z) = 0;
  } else { // 3rd case happens with insertion or deletion:
    // t2 was higher
    BalanceFactor(X) = 1;
    BalanceFactor(Z) = 0;  // t4 now higher
  }
  BalanceFactor(Y) = 0;
  return Y; // return new root of rotated subtree
}

M_VOID
m_BTNode_inserted(m_BTNode* Z)
{
  m_BTNode* X, *G, *N;

  M_TRACE("Node -- inserted ("M_PTR_FMT")", Z);

  for (X = parent(Z); X != NULL; X = parent(Z))
  { // Loop (possibly up to the root)
    // BalanceFactor(X) has to be updated:
    if (Z == right_child(X)) { // The right subtree increases
      if (BalanceFactor(X) > 0) { // X is right-heavy
        // ===> the temporary BalanceFactor(X) == +2
        // ===> rebalancing is required.
        G = parent(X); // Save parent of X around rotations
        if (BalanceFactor(Z) < 0)      // Right Left Case
          N = m_BTNode_rotate_RightLeft(X, Z); // Double rotation: Right(Z) then Left(X)
        else                           // Right Right Case
          N = m_BTNode_rotate_Left(X, Z);     // Single rotation Left(X)
        // After rotation adapt parent link
      }
      else {
        if (BalanceFactor(X) < 0) {
          BalanceFactor(X) = 0; // Z’s height increase is absorbed at X.
          break; // Leave the loop
        }
        BalanceFactor(X) = +1;
        Z = X; // Height(Z) increases by 1
        continue;
      }
    }
    else { // Z == left_child(X): the left subtree increases
      if (BalanceFactor(X) < 0) { // X is left-heavy
        // ===> the temporary BalanceFactor(X) == –2
        // ===> rebalancing is required.
        G = parent(X); // Save parent of X around rotations
        if (BalanceFactor(Z) > 0)      // Left Right Case
          N = m_BTNode_rotate_LeftRight(X, Z); // Double rotation: Left(Z) then Right(X)
        else                           // Left Left Case
          N = m_BTNode_rotate_Right(X, Z);    // Single rotation Right(X)
        // After rotation adapt parent link
      }
      else {
        if (BalanceFactor(X) > 0) {
          BalanceFactor(X) = 0; // Z’s height increase is absorbed at X.
          break; // Leave the loop
        }
        BalanceFactor(X) = -1;
        Z = X; // Height(Z) increases by 1
        continue;
      }
    }
    // After a rotation adapt parent link:
    // N is the new root of the rotated subtree
    // Height does not change: Height(N) == old Height(X)
    parent(N) = G;
    if (G != NULL) {
      if (X == left_child(G))
        left_child(G) = N;
      else
        right_child(G) = N;
      break;
    }
    else {
      //tree->root = N; // N is the new root of the total tree
      break;
    }
    // There is no fall thru, only break; or continue;
    // Unless loop is left via break, the height of the total tree increases by 1.
  }
}

M_INT8
m_BTNode_insert(m_BTNode** bt,
        const M_ID key,
        const M_PTR val,
        M_PTR (* const mallocdoer)(M_SZ))
{
  m_BTNode* parent = NULL;

  assert(bt);
  M_TRACE("Node -- insert ("M_PTR_FMT") key ("M_ID_FMT") val ("M_PTR_FMT")",
      bt, key, val);
  if (!bt) return -1;

  for (;;)
  {
    if (!*bt)
    {
      if (!m_BTNode_new(bt, key, val, parent, mallocdoer))
        return -1;
      if (parent)
        m_BTNode_inserted(*bt);
      return 1;
    }
    parent = *bt;
    if (key < (*bt)->key)
    {
      bt = &(*bt)->less;
      M_TRACE("Node -- insert ("M_PTR_FMT") searching less", *bt);
      continue;
    }
    else if (key > (*bt)->key)
    {
      bt = &(*bt)->more;
      M_TRACE("Node -- insert ("M_PTR_FMT") searching more", *bt);
      continue;
    }
    else /* duplicate */
    {
      M_TRACE("Node -- insert ("M_PTR_FMT") duplicate key ("M_ID_FMT")", *bt, key);
      return 0;
    }
  }
  /* not reached */
}

M_INT8
m_BTree_insert(m_BTree* const bt,
        const M_ID key,
        const M_PTR val)
{
  M_INT8 i;

  assert(bt);
  M_TRACE("insert ("M_PTR_FMT") key ("M_ID_FMT") val ("M_PTR_FMT")",
      bt, key, val);
  if (!bt) return -1;

  i = m_BTNode_insert(&bt->root, key, val, bt->mallocdoer);
  switch (i)
  {
  case -1: return -1;
  case 1: bt->root = m_BTNode_root(bt->root);
  default: return i;
  }
}

M_VOID
m_BTNode_removed(m_BTNode* N,
    m_BTNode* X,
    const M_INT8 from_child)
{
  m_BTNode* G, *Z;
  M_INT8 b;

  M_TRACE("Node -- removed ("M_PTR_FMT") ("M_PTR_FMT") ("M_INT8_FMT")",
      N, X, from_child);

  if (N)
  {
    assert(from_child == 0);
    X = parent(N);
  }
  else
  {
    assert(from_child != 0);
  }

  for (; X != NULL; X = G) { // Loop (possibly up to the root)
    G = parent(X); // Save parent of X around rotations
    // BalanceFactor(X) has not yet been updated!
    if ((N && N == left_child(X))
        || (!N && from_child < 0))
    { // the left subtree decreases
      if (BalanceFactor(X) > 0) { // X is right-heavy
        // ===> the temporary BalanceFactor(X) == +2
        // ===> rebalancing is required.
        Z = right_child(X); // Sibling of N (higher by 2)
        b = BalanceFactor(Z);
        if (b < 0)                     // Right Left Case     (see figure 5)
          N = m_BTNode_rotate_RightLeft(X, Z); // Double rotation: Right(Z) then Left(X)
        else                           // Right Right Case    (see figure 4)
          N = m_BTNode_rotate_Left(X, Z);     // Single rotation Left(X)
        // After rotation adapt parent link
      } else {
        if (BalanceFactor(X) == 0) {
          BalanceFactor(X) = +1; // N’s height decrease is absorbed at X.
          break; // Leave the loop
        }
        N = X;
        BalanceFactor(N) = 0; // Height(N) decreases by 1
        continue;
      }
    } else { // (N == right_child(X)): The right subtree decreases
      if (BalanceFactor(X) < 0) { // X is left-heavy
        // ===> the temporary BalanceFactor(X) == –2
        // ===> rebalancing is required.
        Z = left_child(X); // Sibling of N (higher by 2)
        b = BalanceFactor(Z);
        if (b > 0)                     // Left Right Case
          N = m_BTNode_rotate_LeftRight(X, Z); // Double rotation: Left(Z) then Right(X)
        else                        // Left Left Case
          N = m_BTNode_rotate_Right(X, Z);    // Single rotation Right(X)
        // After rotation adapt parent link
      } else {
        if (BalanceFactor(X) == 0) {
          BalanceFactor(X) = -1; // N’s height decrease is absorbed at X.
          break; // Leave the loop
        }
        N = X;
        BalanceFactor(N) = 0; // Height(N) decreases by 1
        continue;
      }
    }
    // After a rotation adapt parent link:
    // N is the new root of the rotated subtree
    parent(N) = G;
    if (G != NULL) {
      if (X == left_child(G))
        left_child(G) = N;
      else
        right_child(G) = N;
      if (b == 0)
        break; // Height does not change: Leave the loop
    } else {
      //tree->root = N; // N is the new root of the total tree
    }
    // Height(N) decreases by 1 (== old Height(X)-1)
  }
  // Unless loop is left via break, the height of the total tree decreases by 1.
}

M_VOID
m_BTNode_replace(m_BTNode* const bt,
        m_BTNode* const nd)
{
  assert(bt);
  M_TRACE("Node -- replace ("M_PTR_FMT") nd ("M_PTR_FMT")", bt, nd);

  if (bt->parent)
  {
    if (bt == bt->parent->less)
      bt->parent->less = nd;
    else
      bt->parent->more = nd;
  }
  if (nd)
  {
    if (nd != bt->less && nd != bt->more)
    {
      assert(nd->key != bt->key);
      if (nd->key < bt->key)
      {
        assert(nd->more == NULL);
        if (bt->more)
        {
          bt->more->parent = nd;
          nd->more = bt->more;
        }
        nd->parent->more = nd->less;
        if (nd->less)
        {
          nd->less->parent = nd->parent;
        }
        nd->less = bt->less;
        bt->less->parent = nd;
      }
      else /* nd->key > bt->key */
      {
        assert(nd->less == NULL);
        if (bt->less)
        {
          bt->less->parent = nd;
          nd->less = bt->less;
        }
        nd->parent->less = nd->more;
        if (nd->more)
        {
          nd->more->parent = nd->parent;
        }
        nd->more = bt->more;
        bt->more->parent = nd;
      }
    }
    else if (nd == bt->less)
    {
      nd->more = bt->more;
      if (bt->more)
      {
        bt->more->parent = nd;
      }
    }
    else /* nd == bt->more */
    {
      nd->less = bt->less;
      if (bt->less)
      {
        bt->less->parent = nd;
      }
    }
    nd->parent = bt->parent;
  }
}

m_BTNode*
m_BTNode_remove(m_BTNode* bt,
        M_ID key,
        M_VOID (* const freedoer)(M_PTR),
        M_VOID (* const fn)(M_PTR))
{
  m_BTNode* root, *parent;
  m_BTNode* subtree = NULL;
  m_BTNode* nd = NULL;
  M_INT8 from_child = 0;

  assert(bt);
  assert(!bt->parent);
  M_TRACE("Node -- remove ("M_PTR_FMT") key ("M_ID_FMT")", bt, key);
  if (!bt || bt->parent) return NULL;

  root = bt;
  for (;;) /* find key and remove node */
  {
    parent = bt->parent;
    if (bt->key == key)
    {
      /* node found */
      if (fn)
        (*fn)(bt->val);

      if (bt->less && bt->more)
      {
        if (bt->bfactor <= 0)
        {
          /* get predecessor */
          nd = m_BTNode_prev(bt);
          if (nd->less)
          {
            /* retracing starts at nd->less */
            subtree = nd->less;
            nd->bfactor = bt->bfactor;
          }
          else if (nd->parent != bt)
          {
            /* retracing starts at nd->parent */
            from_child = +1;
            parent = nd->parent;
            nd->bfactor = bt->bfactor;
          }
          else /* nd->parent == bt */
          {
            /* retracing starts at bt */
            from_child = -1;
            parent = nd;
          }
        }
        else /* bt->bfactor > 0 */
        {
          /* get successor */
          nd = m_BTNode_next(bt);
          if (nd->more)
          {
            /* retracing starts at nd->more */
            subtree = nd->more;
            nd->bfactor = bt->bfactor;
          }
          else if (nd->parent != bt)
          {
            /* retracing starts at nd->parent */
            from_child = -1;
            parent = nd->parent;
            nd->bfactor = bt->bfactor;
          }
          else /* nd->parent == bt */
          {
            /* retracing starts at bt */
            from_child = +1;
            parent = nd;
          }
        }
        m_BTNode_replace(bt, nd);
      }
      else if (bt->less)
      {
        subtree = bt->less;
        m_BTNode_replace(bt, bt->less);
      }
      else if (bt->more)
      {
        subtree = bt->more;
        m_BTNode_replace(bt, bt->more);
      }
      else /* removing leaf */
      {
        if (parent)
        {
          from_child = bt == parent->less ? -1 : +1;
        }
        else /* removing last node */
        {
          root = NULL;
        }
        m_BTNode_replace(bt, NULL);
      }
      break;
    }
    else /* bt->key != key */
    {
      if (key < bt->key)
      {
        M_TRACE("Node -- remove ("M_PTR_FMT") searching less", bt);
        bt = bt->less;
      }
      else
      {
        M_TRACE("Node -- remove ("M_PTR_FMT") searching more", bt);
        bt = bt->more;
      }
      if (!bt) /* key not found */
      {
        M_TRACE("Node -- remove key ("M_ID_FMT") not found", key);
        return root;
      }
      continue;
    }
  }
  /* adjust subtree */
  if (root)
  {
    m_BTNode_removed(subtree, parent ? parent : root, from_child);
    root = m_BTNode_root(root != bt ? root : bt->less ? bt->less : bt->more);
  }
  m_BTNode_delete(&bt, freedoer);
  return root;
}

M_VOID
m_BTree_remove(m_BTree* const bt,
        const M_ID key,
        M_VOID (* const fn)(M_PTR))
{
  assert(bt);
  M_TRACE("remove ("M_PTR_FMT") key ("M_ID_FMT")", bt, key);
  if (!bt) return;
  bt->root = m_BTNode_remove(bt->root, key, bt->freedoer, fn);
}

m_BTNode*
m_BTNode_get_node(const m_BTNode* bt,
        const M_ID key)
{
  for (;;)
  {
    if (!bt) return NULL;
    if (key == bt->key) return (m_BTNode*) bt;
    bt = key < bt->key ? bt->less : bt->more;
  }
}

M_PTR
m_BTNode_get(const m_BTNode* bt,
        const M_ID key)
{
  for (;;)
  {
    if (!bt) return NULL;
    if (key == bt->key) return bt->val;
    bt = key < bt->key ? bt->less : bt->more;
  }
}

M_BOOL
m_BTNode_set(m_BTNode* const bt,
        const M_ID key,
        const M_PTR val,
        M_PTR* const prev)
{
  m_BTNode* nd = m_BTNode_get_node(bt, key);
  if (!nd) return M_FALSE;
  if (prev) *prev = nd->val;
  nd->val = (M_PTR) val;
  return M_TRUE;
}

M_UINT64
m_BTNode_count(const m_BTNode* bt)
{
  M_UINT64 i = 0;

  for (bt = m_BTNode_least(bt); bt; bt = m_BTNode_next(bt))
    ++i;
  return i;
}

M_UINT64
m_BTNode_num_levels(const m_BTNode* const bt)
{
  M_UINT64 lcnt, mcnt;

  assert(bt);
  if (!bt) return -1;

  if (!bt->less && !bt->more) return 0;
  lcnt = bt->less ? m_BTNode_num_levels(bt->less) : 0;
  mcnt = bt->more ? m_BTNode_num_levels(bt->more) : 0;
  return 1 + (lcnt > mcnt ? lcnt : mcnt);
}

M_VOID
m_BTNode_traverse(m_BTNode* bt,
        M_VOID (* const func)(M_PTR))
{
  for (bt = m_BTNode_least(bt); bt; bt = m_BTNode_next(bt))
    (*func)(bt->val);
}

M_VOID
m_BTNode_traverse2(m_BTNode* bt,
        M_VOID (* const func)(M_PTR, M_PTR),
        M_PTR userdata)
{
  for (bt = m_BTNode_least(bt); bt; bt = m_BTNode_next(bt))
    (*func)(bt->val, userdata);
}

m_BTNode*
m_BTNode_root(const m_BTNode* bt)
{
  if (!bt) return NULL;

  while (bt->parent) bt = bt->parent;
  return (m_BTNode*) bt;
}

m_BTNode*
m_BTNode_least(const m_BTNode* bt)
{
  if (!bt) return NULL;

  while (bt->less) bt = bt->less;
  return (m_BTNode*) bt;
}

m_BTNode*
m_BTNode_most(const m_BTNode* bt)
{
  if (!bt) return NULL;

  while (bt->more) bt = bt->more;
  return (m_BTNode*) bt;
}

m_BTNode*
m_BTNode_next(const m_BTNode* bt)
{
  assert(bt);

  if (bt->more) return m_BTNode_least(bt->more);
  while (bt->parent && bt->parent->key < bt->key)
    bt = bt->parent;
  return bt->parent;
}

m_BTNode*
m_BTNode_prev(const m_BTNode* bt)
{
  assert(bt);

  if (bt->less) return m_BTNode_most(bt->less);
  while (bt->parent && bt->parent->key > bt->key)
    bt = bt->parent;
  return bt->parent;
}

M_SZ
m_BTNode_linearize(const m_BTNode* const bt,
        M_PTR* const arr)
{
  M_SZ i = 0;
  m_BTNode* it;

  if (!bt) return 0;
  assert(arr);
  if (!arr) return -1;

  it = m_BTNode_least(bt);
  for (; it; it = m_BTNode_next(it))
    arr[i++] = it->val;

  return i;
}

M_BOOL
m_BTNode_unlinearize(m_BTNode** const bt,
        const M_PTR* const arr,
        const M_SZ sz,
        const m_BTNode* const parent,
        M_PTR (* const mallocdoer)(M_SZ))
{
  const M_SZ mid = sz / 2;

  assert(arr);
  assert(sz);
  assert(bt && *bt == NULL);
  if (!arr || !sz || !bt || *bt) return M_FALSE;

  if (!m_BTNode_new(bt, 0, NULL, NULL, mallocdoer)) return M_FALSE;

  (*bt)->key = *((M_ID*) arr[mid]);
  (*bt)->val = (M_PTR) arr[mid];
  (*bt)->parent = (m_BTNode*) parent;

  if (mid)
  {
    m_BTNode* tmp = NULL;
    if (!m_BTNode_unlinearize(&tmp, arr, mid, *bt, mallocdoer))
      return M_FALSE;
    (*bt)->less = tmp;
  }
  if (sz > mid + 1)
  {
    m_BTNode* tmp = NULL;
    if (!m_BTNode_unlinearize(&tmp, &arr[mid+1], sz-(mid+1), *bt, mallocdoer))
      return M_FALSE;
    (*bt)->more = tmp;
  }
  return M_TRUE;
}

#ifndef NDEBUG

M_UINT64
m_BTNode_balance_factor(const m_BTNode* const bt)
{
  const M_UINT64 i = bt->less ? m_BTNode_num_levels(bt->less) + 1 : 0;
  const M_UINT64 j = bt->more ? m_BTNode_num_levels(bt->more) + 1 : 0;
  return i > j ? i - j : j - i;
}

M_BOOL
m_BTNode_is_unbalanced(const m_BTNode* const bt)
{
  M_BOOL b;
  if (!bt) return M_FALSE;
  printf("-- Check wether btree node ("M_PTR_FMT") is balanced... ", bt); fflush(stdout);
  b = m_BTNode_balance_factor(bt) > 1 ? M_TRUE : M_FALSE;
  printf(b ? "Failed\n" : "OK\n"); fflush(stdout);
  return b;
}

M_VOID
m_BTNode_print_debug(const m_BTNode* const bt)
{
  printf("-- BTNode debug ("M_PTR_FMT"):\n"
         "--     Key =    "M_ID_FMT"\n"
         "--     Parent = "M_ID_FMT"\n"
         "--     Less =   "M_ID_FMT"\n"
         "--     More =   "M_ID_FMT"\n"
         "--     BFactor= "M_INT8_FMT"\n"
         "-- end BTNode debug\n",
    bt, bt->key,
    bt->parent ? bt->parent->key : 0,
    bt->less ? bt->less->key : 0,
    bt->more ? bt->more->key : 0,
    bt->bfactor);
  fflush(stdout);
}

M_VOID
m_BTNode_debug(const m_BTNode* const bt)
{
  assert(bt);

  m_BTNode_print_debug(bt);
  if (bt->less) m_BTNode_debug(bt->less);
  if (bt->more) m_BTNode_debug(bt->more);
}

M_VOID
m_BTree_debug(const m_BTNode* bt)
{
  assert(bt);

  for (bt = m_BTNode_least(bt); bt; bt = m_BTNode_next(bt))
      m_BTNode_print_debug(bt);
}

#endif /* !NDEBUG */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
