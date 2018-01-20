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
 *  \file m_btree.h
 *  \brief AVL binary-tree, self-balanced.
 *  \author Stanislas Marquis <stan@astrorigin.com>
 */

/*
 *  This tree is used internally by the global memory pool (until hashmap is done).
 *  Therefore in that case, we cannot rely on the memory pool itself for
 *  allocation and deallocation of its own components.
 *
 *  We can optionaly rely on another memory pool, by changing the
 *  malloc'doer and free'doer accordingly.
 */

#ifndef M_BTREE_H
#define M_BTREE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "m_h.h"

/**
 *  \typedef m_BTNode
 */
typedef struct _m_BTNode m_BTNode;

/**
 *  \struct _m_BTNode
 *  \brief Binary tree node.
 */
struct _m_BTNode
{
  M_ID key;
  M_PTR val;
  m_BTNode* parent;
  m_BTNode* less;
  m_BTNode* more;
  M_INT8 bfactor : 2;
};

/**
 *  \typedef m_BTree
 */
typedef struct _m_BTree m_BTree;

/**
 *  \struct _m_BTree
 *  \brief Binary tree.
 */
struct _m_BTree
{
  m_BTNode* root;
  M_PTR (*mallocdoer)(M_SZ);
  M_VOID (*freedoer)(M_PTR);
  M_VOID (*finalize_fn)(M_PTR);
};

/* Private functions */
#include "m_btree_priv.h"

/**
 *  \brief Allocate for a new btree.
 *  \param bt The tree.
 *  \return M_TRUE, or M_FALSE on error.
 */
M_DLLAPI M_BOOL
m_BTree_new(m_BTree** const bt);

/**
 *  \brief Initialize a btree (extended version).
 *  \param bt The binary tree.
 *  \param mallocdoer Allocation function for nodes, or NULL to use malloc.
 *  \param freedoer Deallocation function for nodes, or NULL to use free.
 *  \return M_TRUE, or M_FALSE on error.
 */
M_DLLAPI M_BOOL
m_BTree_init2(m_BTree* const bt,
        M_PTR (*const mallocdoer)(M_SZ),
        M_VOID (* const freedoer)(M_PTR));

/**
 *  \brief Initialize a btree.
 *  \param bt The binary tree.
 *  \return M_TRUE, or M_FALSE on error.
 */
M_DLLAPI M_BOOL
m_BTree_init(m_BTree* const bt);

/**
 *  \brief Finalize a btree.
 *  \param bt The binary tree.
 *
 *  If not NULL, bt->finalize_fn is executed on the nodes values.
 */
M_DLLAPI M_VOID
m_BTree_fini(m_BTree* const bt);

/**
 *  \brief Deallocate a tree.
 *  \param bt The tree.
 *
 *  If not NULL, bt->finalize_fn is executed on the nodes values.
 */
M_DLLAPI M_VOID
m_BTree_delete(m_BTree** const bt);

/**
 *  \brief Insert a new node in the given btree.
 *  \param bt The binary tree.
 *  \param key The key.
 *  \param val The value.
 *  \return 1 on success, 0 if the key is duplicate, -1 on error.
 */
M_DLLAPI M_INT8
m_BTree_insert(m_BTree* const bt,
        const M_ID key,
        const M_PTR val);

/**
 *  \brief Remove a node from the btree.
 *  \param bt The binary tree.
 *  \param key The key.
 *  \param fn If not null, execute function on value of found node.
 */
M_DLLAPI M_VOID
m_BTree_remove(m_BTree* const bt,
        const M_ID key,
        M_VOID (* const fn)(M_PTR));

/**
 *  \brief Get a node from the btree.
 *  \param bt The binary tree.
 *  \param idx The key.
 *  \return The node found or NULL.
 *  \see m_BTNode_get_node
 */
#define m_BTree_node( bt, idx ) \
        m_BTNode_get_node( (bt)->root, idx )

/**
 *  \brief Get a value from the btree.
 *  \param bt The binary tree.
 *  \param idx The key.
 *  \return The value or NULL.
 *  \see m_BTNode_get
 */
#define m_BTree_get( bt, idx ) \
        m_BTNode_get( (bt)->root, idx )

/**
 *  \brief Set the value for a given key in the btree.
 *  \param bt The binary tree.
 *  \param idx The key.
 *  \param val The value.
 *  \param prev If not NULL, return value replaced.
 *  \return M_TRUE if the key was found and value changed.
 *  \see m_BTNode_set
 */
#define m_BTree_set( bt, idx, val, prev ) \
        m_BTNode_set( (bt)->root, idx, val, prev )

/**
 *  \brief Count the nodes inside a btree.
 *  \param bt The binary tree.
 *  \return The count.
 *  \see m_BTNode_count
 */
#define m_BTree_count( bt ) \
        m_BTNode_count( (bt)->root )

/**
 *  \brief Traverse a btree and apply function to the values.
 *  \param bt The binary tree.
 *  \param func The function to apply.
 *  \see m_BTNode_traverse
 */
#define m_BTree_traverse( bt, func ) \
        m_BTNode_traverse( (bt)->root, (func) )

/**
 *  \brief Traverse a btree and apply function to the values (userdata version).
 *  \param bt The binary tree.
 *  \param func The function to apply.
 *  \param userdata Pointer passed to function.
 *  \see m_BTNode_traverse2
 */
#define m_BTree_traverse2( bt, func, udata ) \
        m_BTNode_traverse2( (bt)->root, (func), (udata) )

/**
 *  \brief Get the node with lowest key from btree.
 *  \param bt The binary tree.
 *  \return The lowest node.
 *  \see m_BTNode_least
 */
#define m_BTree_least( bt ) \
        m_BTNode_least( (bt)->root )

/**
 *  \brief Get the node with highest key from btree.
 *  \param bt The binary tree.
 *  \see m_BTNode_most
 */
#define m_BTree_most( bt ) \
        m_BTNode_most( (bt)->root )

/**
 *  \brief Get the next node from a given node (shorter version).
 *  \param node The node.
 *  \return The next node.
 *  \see m_BTNode_next
 */
#define m_BTree_next( node ) \
        m_BTNode_next( node )

/**
 *  \brief Get the previous node from a given node (shorter version).
 *  \param node The node.
 *  \return The previous node.
 *  \see m_BTNode_prev
 */
#define m_BTree_prev( node ) \
        m_BTNode_prev( node )

/**
 *  \brief Get the lowest key in a binary tree.
 *  \param bt The binary tree.
 *  \return The lowest key.
 *  \see m_BTNode_min
 */
#define m_BTree_min( bt ) \
        m_BTNode_min( (bt)->root )

/**
 *  \brief Get the highest key from a binary tree.
 *  \param bt The binary tree.
 *  \return The highest key.
 *  \see m_BTNode_max
 */
#define m_BTree_max( bt ) \
        m_BTNode_max( (bt)->root )

/**
 *  \brief Copy the tree values into a linear array.
 *  \param node The root node.
 *  \param arr Memory address of the array elements.
 *  \return The number of elements linearized, or (-1) which probably means error.
 */
M_DLLAPI M_SZ
m_BTNode_linearize(const m_BTNode* const node,
        M_PTR* const arr);

/**
 *  \brief Reconstruct a tree structure with sorted values in an array.
 *  \param node A starting node, must be initialized to NULL.
 *  \param arr The array containing the values.
 *  \param num The size of array (may not be 0).
 *  \param parent The parent node (may be NULL).
 *  \param mallocdoer The allocating function.
 *  \return M_TRUE, or M_FALSE on error.
 */
M_DLLAPI M_BOOL
m_BTNode_unlinearize(m_BTNode** const node,
        const M_PTR* const arr,
        const M_SZ num,
        const m_BTNode* const parent,
        M_PTR (* const mallocdoer)(M_SZ));

#ifdef __cplusplus
}
#endif
#endif /* !M_BTREE_H */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
