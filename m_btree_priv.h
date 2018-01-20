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
 *  \file m_btree_priv.h
 *  \brief AVL binary-tree, self-balanced.
 *  \author Stanislas Marquis <stan@astrorigin.com>
 */

#ifndef M_BTREE_PRIV_H
#define M_BTREE_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \brief Allocate for a btree node.
 *  \param node The node.
 *  \param key The key.
 *  \param val The value.
 *  \param parent The parent (or NULL).
 *  \param mallocdoer The allocating function.
 *  \return M_TRUE, or M_FALSE on error.
 */
M_DLLAPI M_BOOL
m_BTNode_new(m_BTNode** const node,
        const M_ID key,
        const M_PTR val,
        const m_BTNode* const parent,
        M_PTR (* const mallocdoer)(M_SZ));

/**
 *  \brief Deallocate all nodes in the subtree.
 *  \param node The node.
 *  \param freedoer The deallocating function.
 */
M_DLLAPI M_VOID
m_BTNode_delete_all(m_BTNode** const node,
        M_VOID (* const freedoer)(M_PTR));

/**
 *  \brief Deallocate one node.
 */
M_DLLAPI M_VOID
m_BTNode_delete(m_BTNode** const bt,
        M_VOID (* const freedoer)(M_PTR));

#ifndef M_NO_MEMPOOL
/*
 *  This function because our mempool'freedoer needs size of
 *  allocated memory.
 */
M_DLLAPI M_VOID
_m_BTNode_free(M_PTR p);

#define _m_BTNode_free_ref &_m_BTNode_free
#else
/*
 *  We are relying on MemCnt if MemPool is disabled.
 */
#define _m_BTNode_free(p) _M_FREE(p)
#define _m_BTNode_free_ref _M_FREE_REF
#endif /* !M_NO_MEMPOOL */

m_BTNode*
m_BTNode_rotate_Left(m_BTNode* const X,
        m_BTNode* const Z);

m_BTNode*
m_BTNode_rotate_Right(m_BTNode* const X,
        m_BTNode* const Z);

m_BTNode*
m_BTNode_rotate_RightLeft(m_BTNode* const X,
        m_BTNode* const Z);

m_BTNode*
m_BTNode_rotate_LeftRight(m_BTNode* const X,
        m_BTNode* const Z);

M_VOID
m_BTNode_inserted(m_BTNode* Z);

/**
 *  \brief Insert a new node starting from the given node.
 *  \param node The starting (root) node.
 *  \param key The key.
 *  \param val The value.
 *  \param mallocdoer The allocating function.
 *  \return 1 on success, 0 if the key is duplicate, -1 on error.
 */
M_DLLAPI M_INT8
m_BTNode_insert(m_BTNode** node,
        const M_ID key,
        const M_PTR val,
        M_PTR (* const mallocdoer)(M_SZ));

M_VOID
m_BTNode_removed(m_BTNode* N,
    m_BTNode* X,
    const M_INT8 from_child);

/**
 *  \brief Remove a node starting from the given node.
 *  \param node The root node.
 *  \param key The key.
 *  \param freedoer The deallocating function for node.
 *  \param fn If not null, execute function on value of found node.
 *  \return The resulting root node.
 */
M_DLLAPI m_BTNode*
m_BTNode_remove(m_BTNode* node,
        M_ID key,
        M_VOID (* const freedoer)(M_PTR),
        M_VOID (* const fn)(M_PTR));

/**
 *  \brief Get a node starting from the given node.
 *  \param node The starting (root) node.
 *  \param key The key.
 *  \return The node found or NULL.
 */
M_DLLAPI m_BTNode*
m_BTNode_get_node(const m_BTNode* node,
        const M_ID key);

/**
 *  \brief Get a value starting from the given node.
 *  \param node The starting (root) node.
 *  \param key The key.
 *  \return The value or NULL.
 */
M_DLLAPI M_PTR
m_BTNode_get(const m_BTNode* node,
        const M_ID key);

/**
 *  \brief Set the value for a given key starting from the node.
 *  \param node The starting (root) node.
 *  \param key The key.
 *  \param val The value.
 *  \param prev If not NULL, return value replaced.
 *  \return M_TRUE if the key was found and value changed.
 */
M_DLLAPI M_BOOL
m_BTNode_set(m_BTNode* const node,
        const M_ID key,
        const M_PTR val,
        M_PTR* const prev);

/**
 *  \brief Count the nodes including the given node.
 *  \param node The node.
 *  \return The count.
 *  \see m_BTNode_num_nodes
 */
M_DLLAPI M_UINT64
m_BTNode_count(const m_BTNode* node);

/**
 *  \brief Count the levels starting from given node.
 *  \param node the node.
 *  \return The count.
 */
M_DLLAPI M_UINT64
m_BTNode_num_levels(const m_BTNode* const node);

/**
 *  \brief Traverse the nodes and apply function to the values.
 *  \param node The starting (root) node.
 *  \param func The function to apply.
 */
M_DLLAPI M_VOID
m_BTNode_traverse(m_BTNode* node,
        M_VOID (* const func)(M_PTR));

/**
 *  \brief Traverse the nodes and apply function to the values (userdata version).
 *  \param node The starting (root) node.
 *  \param func The function to apply.
 *  \param userdata Pointer passed to function.
 */
M_DLLAPI M_VOID
m_BTNode_traverse2(m_BTNode* node,
        M_VOID (* const func)(M_PTR, M_PTR),
        M_PTR userdata);

/**
 *  \brief Get root node.
 *  \param node A node.
 *  \return The root node.
 */
M_DLLAPI m_BTNode*
m_BTNode_root(const m_BTNode* node);

/**
 *  \brief Get the node with lowest key starting from given node.
 *  \param node The node.
 *  \return The lowest node.
 */
M_DLLAPI m_BTNode*
m_BTNode_least(const m_BTNode* node);

/**
 *  \brief Get the node with highest key starting from given node.
 *  \param node The node.
 *  \return The highest node.
 */
M_DLLAPI m_BTNode*
m_BTNode_most(const m_BTNode* node);

/**
 *  \brief Get the next node from a given node.
 *  \param node The node.
 *  \return The next node.
 */
M_DLLAPI m_BTNode*
m_BTNode_next(const m_BTNode* node);

/**
 *  \brief Get the previous node from a given node.
 *  \param node The node.
 *  \return The previous node.
 */
M_DLLAPI m_BTNode*
m_BTNode_prev(const m_BTNode* node);

/**
 *  \brief Get the lowest key from the given node.
 *  \param node The starting (root) node.
 *  \return The lowest key.
 *  \see m_BTNode_least
 */
#define m_BTNode_min( node ) \
        (m_BTNode_least( node )->key)

/**
 *  \brief Get the highest key from the given node.
 *  \param node The starting (root) node.
 *  \return The highest key.
 *  \see m_BTNode_most
 */
#define m_BTNode_max( node ) \
        (m_BTNode_most( node )->key)

#ifndef NDEBUG

/**
 *  \brief Get the balance factor.
 *  \param node The starting (root) node.
 *  \return The factor.
 */
M_DLLAPI M_UINT64
m_BTNode_balance_factor(const m_BTNode* const node);

/**
 *  \brief Check the balance factor.
 *  \param node The starting (root) node.
 *  \return M_TRUE if the structure is unbalanced.
 */
M_DLLAPI M_BOOL
m_BTNode_is_unbalanced(const m_BTNode* const node);

/**
 *  \brief Print info about a node.
 *  \param node A node.
 */
M_DLLAPI M_VOID
m_BTNode_print_debug(const m_BTNode* const node);

/**
 *  \brief Print debug to stdout, from root node and so on.
 *  \param node A node to debug.
 */
M_DLLAPI M_VOID
m_BTNode_debug(const m_BTNode* const node);

/**
 *  \brief Print debug to stdout, from least node and so on.
 *  \param node A node to debug.
 */
M_DLLAPI M_VOID
m_BTree_debug(const m_BTNode* node);

#endif /* !NDEBUG */

#ifdef __cplusplus
}
#endif
#endif /* !M_BTREE_PRIV_H */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
