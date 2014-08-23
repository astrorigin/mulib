/*
    Plib
    Copyright (C) 2014 Stanislas Marquis <smarquis@astrorigin.ch>

    This program is free software: you can redistribute it and/or modify it
    under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  \file p_btree.h
 *  \brief A self-balanced binary-tree.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 *
 *  This tree is used by the global memory pool internally.
 *  Therefore, this module cannot rely on the memory pool itself for
 *  allocation and deallocation of its components.
 *
 *  But it can optionaly rely on another memory pool, by changing the
 *  malloc'doer and free'doer accordingly.
 */

#ifndef P_BTREE_H
#define P_BTREE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "p_h.h"

/**
 *  \typedef p_BTNode
 */
typedef struct _p_btree_node_t p_BTNode;

/**
 *  \struct _p_btree_node_t
 *  \brief Binary tree node.
 */
struct _p_btree_node_t
{
    P_ID        key;
    P_PTR       val;
    p_BTNode*   less;
    p_BTNode*   more;
    p_BTNode*   parent;
};

/**
 *  \typedef p_BTree
 */
typedef struct _p_btree_t p_BTree;

/**
 *  \struct _p_btree_t
 *  \brief Binary tree.
 */
struct _p_btree_t
{
    p_BTNode*   root;
    P_PTR       (*mallocdoer)( P_SZ );
    P_VOID      (*freedoer)( P_PTR );
};

/**
 *  \brief Dummy allocator function for btree nodes (not used).
 *  \see p_memlist.h
 */
#define p_btree_mallocdoer  malloc

/**
 *  \brief Dummy deallocator function for btree nodes (not used).
 *  \see p_memlist.h
 */
#define p_btree_freedoer    free

/**
 *  \brief Allocate for a tree.
 *  \param bt The tree.
 *  \return P_TRUE, or P_FALSE on error.
 */
P_EXPORT P_BOOL
p_btree_new( p_BTree** bt );

/**
 *  \brief Deallocate a tree.
 *  \param bt The tree.
 */
P_EXPORT P_VOID
p_btree_delete( p_BTree** bt );

/**
 *  \brief Initialize a btree.
 *  \param btree The binary tree.
 *  \param mallocdoer Allocation function for nodes, or NULL to use malloc.
 *  \param freedoer Deallocation function for nodes, or NULL to use free.
 *  \return P_TRUE, or P_FALSE on error.
 */
P_EXPORT P_BOOL
p_btree_init( p_BTree* btree,
        P_PTR (*mallocdoer)( P_SZ ),
        P_VOID (*freedoer)( P_PTR ) );

/**
 *  \brief Free the memory used by btree nodes.
 *  \param btree The binary tree.
 */
P_EXPORT P_VOID
p_btree_fini( p_BTree* btree );

/**
 *  \brief Allocate for a btree node.
 *  \param node The node.
 *  \param key The key.
 *  \param val The value.
 *  \param parent The parent (or NULL).
 *  \param mallocdoer The allocating function.
 *  \return P_TRUE, or P_FALSE on error.
 */
P_EXPORT P_BOOL
p_btree_node_new( p_BTNode** node,
        const P_ID key,
        const P_PTR val,
        const p_BTNode* parent,
        P_PTR (*mallocdoer)( P_SZ ) );

/**
 *  \brief Deallocate a btree node.
 *  \param node The node.
 *  \param freedoer The deallocating function.
 */
P_EXPORT P_VOID
p_btree_node_delete( p_BTNode** node,
        P_VOID (*freedoer)( P_PTR ) );

#ifndef P_NO_MEMPOOL
/**
 *
 */
P_EXPORT P_VOID
p_btree_node_free( P_PTR p );
#define p_btree_node_free_ref \
        &p_btree_node_free
#else
#define p_btree_node_free( p ) \
        _P_FREE( p )
#define p_btree_node_free_ref \
        _P_FREE_REF
#endif /* P_NO_MEMPOOL */

/**
 *  \brief Insert a new node starting from the given node.
 *  \param node The starting (root) node.
 *  \param key The key.
 *  \param val The value.
 *  \param parent The parent node (usually NULL).
 *  \param mallocdoer The allocating function.
 *  \return 1 on success, 0 if the key is duplicate, -1 on error.
 */
P_EXPORT P_INT8
p_btree_node_insert( p_BTNode** node,
        const P_ID key,
        const P_PTR val,
        const p_BTNode* parent,
        P_PTR (*mallocdoer)( P_SZ ) );

/**
 *  \brief Insert a new node in the given btree.
 *  \param btree The binary tree
 *  \param key The key.
 *  \param val The value.
 *  \return 1 on success, 0 if the key is duplicate, -1 on error.
 */
P_EXPORT P_INT8
p_btree_insert( p_BTree* btree,
        const P_ID key,
        const P_PTR val );

/**
 *  \brief Remove a node starting from the given node.
 *  \param node The root node.
 *  \param key The key.
 *  \param freedoer The deallocating function.
 *  \return P_TRUE if the node was found and deallocated.
 */
P_EXPORT P_BOOL
p_btree_node_remove( p_BTNode* node,
        const P_ID key,
        P_VOID (*freedoer)( P_PTR ) );

/**
 *  \brief Remove a node starting from the given node; keep track of root node.
 *  \param node The root node.
 *  \param key The key.
 *  \param freedoer The deallocating function.
 *  \return The resulting root node.
 *  \note This function uses \a p_btree_node_remove internally.
 */
P_EXPORT p_BTNode*
p_btree_node_safe_remove( p_BTNode* node,
        const P_ID key,
        P_VOID (*freedoer)( P_PTR ) );

/**
 *  \brief Remove a node from the btree.
 *  \param btree The binary tree.
 *  \param key The key.
 *  \note This function uses \a p_btree_node_safe_remove internally.
 */
P_EXPORT P_VOID
p_btree_remove( p_BTree* btree,
        const P_ID key );

/**
 *  \brief Get a node starting from the given node.
 *  \param node The starting (root) node.
 *  \param key The key.
 *  \return The node found or NULL.
 */
P_EXPORT p_BTNode*
p_btree_node_get_node( const p_BTNode* node,
        const P_ID key );

/**
 *  \brief Get a node from the btree.
 *  \param bt The binary tree.
 *  \param idx The key.
 *  \return The node found or NULL.
 *  \see p_btree_node_get_node
 */
#define p_btree_node( bt, idx ) \
        p_btree_node_get_node( (bt)->root, idx )

/**
 *  \brief Get a value starting from the given node.
 *  \param node The starting (root) node.
 *  \param key The key.
 *  \return The value or NULL.
 */
P_EXPORT P_PTR
p_btree_node_get( const p_BTNode* node,
        const P_ID key );

/**
 *  \brief Get a value from the btree.
 *  \param bt The binary tree.
 *  \param idx The key.
 *  \return The value or NULL.
 *  \see p_btree_node_get
 */
#define p_btree_get( bt, idx ) \
        p_btree_node_get( (bt)->root, idx )

/**
 *  \brief Set the value for a given key starting from the node.
 *  \param node The starting (root) node.
 *  \param key The key.
 *  \param val The value.
 *  \return P_TRUE if the key was found and value changed.
 */
P_EXPORT P_BOOL
p_btree_node_set( const p_BTNode* node,
        const P_ID key,
        const P_PTR val );

/**
 *  \brief Set the value for a given key in the btree.
 *  \param bt The binary tree.
 *  \param idx The key.
 *  \param val The value.
 *  \return P_TRUE if the key was found and value changed.
 *  \see p_btree_node_set
 */
#define p_btree_set( bt, idx, val ) \
        p_btree_node_set( (bt)->root, idx, val )

/**
 *  \brief Enumerate the nodes contained in the given node.
 *  \param node The node.
 *  \param i The counter.
 */
P_EXPORT P_VOID
p_btree_node_num_nodes( const p_BTNode* node,
        P_UINT64* i );

/**
 *  \brief Count the nodes including the given node.
 *  \param node The node.
 *  \return The count.
 *  \see p_btree_node_num_nodes
 */
P_EXPORT P_UINT64
p_btree_node_count( const p_BTNode* );

/**
 *  \brief Count the nodes inside a btree.
 *  \param bt The binary tree.
 *  \return The count.
 *  \see p_btree_node_count
 */
#define p_btree_count( bt ) \
        p_btree_node_count( (bt)->root )

/**
 *  \brief Count the levels starting from given node.
 *  \param node the node.
 *  \return The count.
 */
P_EXPORT P_UINT64
p_btree_node_num_levels( const p_BTNode* node );

/**
 *  \brief Traverse the nodes and apply function to the values.
 *  \param node The starting (root) node.
 *  \param func The function to apply.
 */
P_EXPORT P_VOID
p_btree_node_traverse( p_BTNode* node,
        P_VOID (*func)( P_PTR, P_PTR ),
        P_PTR userdata );

/**
 *  \brief Traverse a btree and apply function to the values.
 *  \param bt The binary tree.
 *  \param func The function to apply.
 *  \see p_btree_node_traverse
 */
#define p_btree_traverse( bt, func, udata ) \
        p_btree_node_traverse( (bt)->root, (func), (udata) )

/**
 *  \brief Get root node.
 *  \param node A node.
 *  \return The root node.
 */
P_EXPORT p_BTNode*
p_btree_node_root( p_BTNode* node );

/**
 *  \brief Get the node with lowest key starting from given node.
 *  \param node The node.
 *  \return The lowest node.
 */
P_EXPORT p_BTNode*
p_btree_node_least( p_BTNode* node );

/**
 *  \brief Get the node with lowest key from btree.
 *  \param bt The binary tree.
 *  \return The lowest node.
 *  \see p_btree_node_least
 */
#define p_btree_least( bt ) \
        p_btree_node_least( (bt)->root )

/**
 *  \brief Get the node with highest key starting from given node.
 *  \param node The node.
 *  \return The highest node.
 */
P_EXPORT p_BTNode*
p_btree_node_most( p_BTNode* node );

/**
 *  \brief Get the node with highest key from btree.
 *  \param bt The binary tree.
 *  \see p_btree_node_most
 */
#define p_btree_most( bt ) \
        p_btree_node_most( (bt)->root )

/**
 *  \brief Get the next node from a given node.
 *  \param node The node.
 *  \return The next node.
 */
P_EXPORT p_BTNode*
p_btree_node_next( p_BTNode* node );

/**
 *  \brief Get the next node from a given node (shorter version).
 *  \param node The node
 *  \return The next node.
 *  \see p_btree_node_next
 */
#define p_btree_next( node ) \
        p_btree_node_next( node )

/**
 *  \brief Get the previous node from a given node.
 *  \param node The node.
 *  \return The previous node.
 */
P_EXPORT p_BTNode*
p_btree_node_prev( p_BTNode* node );

/**
 *  \brief Get the previous node from a given node (shorter version).
 *  \param node The node.
 *  \return The previous node.
 *  \see p_btree_node_prev
 */
#define p_btree_prev( node ) \
        p_btree_node_prev( node )

/**
 *  \brief Get the lowest key from the given node.
 *  \param node The starting (root) node.
 *  \return The lowest key.
 *  \see p_btree_node_least
 */
#define p_btree_node_min( node ) \
        (p_btree_node_least( node )->key)

/**
 *  \brief Get the lowest key in a binary tree.
 *  \param bt The binary tree.
 *  \return The lowest key.
 *  \see p_btree_node_min
 */
#define p_btree_min( bt ) \
        p_btree_node_min( (bt)->root )

/**
 *  \brief Get the highest key from the given node.
 *  \param node The starting (root) node.
 *  \return The highest key.
 *  \see p_btree_node_most
 */
#define p_btree_node_max( node ) \
        (p_btree_node_most( node )->key)

/**
 *  \brief Get the highest key from a binary tree.
 *  \param bt The binary tree.
 *  \return The highest key.
 *  \see p_btree_node_max
 */
#define p_btree_max( bt ) \
        p_btree_node_max( (bt)->root )

/**
 *  \brief Balance starting from given node.
 *  \param node The starting (root) node.
 *  \param levels_less The count of levels of the less branch (if known, or 0).
 *  \param levels_more The count of levels of the more branch (if known, or 0).
 */
P_EXPORT P_VOID
p_btree_node_balance( p_BTNode* node,
        const P_UINT64 levels_less,
        const P_UINT64 levels_more );

/**
 *  \brief Copy the values onto a vector.
 *  \param node The root node.
 *  \param vec The memory for all the values.
 *  \return The number of values.
 */
P_EXPORT P_SZ
p_btree_node_vectorize( const p_BTNode* node,
        P_PTR* vec );

/**
 *  \brief Reconstruct a nodes structure with sorted values in a vector.
 *  \param node A starting node, must be NULL.
 *  \param vec The vector containing the values.
 *  \param num The size of vector (may not be 0).
 *  \param parent The parent node (may be NULL).
 *  \param mallocdoer The allocating function.
 *  \return P_TRUE, or P_FALSE on error.
 */
P_EXPORT P_BOOL
p_btree_node_unvectorize( p_BTNode** node,
        P_PTR* vec,
        const P_SZ num,
        const p_BTNode* parent,
        P_PTR (*mallocdoer)( P_SZ ) );

#ifndef NDEBUG

/**
 *  \brief Get the balance factor.
 *  \param node The starting (root) node.
 *  \return The factor.
 */
P_EXPORT P_INT64
p_btree_node_balance_factor( const p_BTNode* node );

/**
 *  \brief Check the balance factor.
 *  \param node The starting (root) node.
 *  \return TRUE if the structure is unbalanced.
 */
P_EXPORT P_BOOL
p_btree_node_is_unbalanced( const p_BTNode* node );

/**
 *  \brief Print info about a node.
 *  \param node A node.
 */
P_EXPORT P_VOID
p_btree_node_print_debug( const p_BTNode* node );
/**
 *  \brief Print debug to stdout, from root node and so on.
 *  \param node A node to debug.
 */
P_EXPORT P_VOID
p_btree_node_debug( const p_BTNode* node );

/**
 *  \brief Print debug to stdout, from least node and so on.
 *  \param node A node to debug.
 */
P_EXPORT P_VOID
p_btree_debug( const p_BTNode* node );

/**
 *  \brief Unittest for binary-tree module.
 *  \return 0 if all checks were ok.
 */
P_EXPORT P_INT32
p_btree_test( P_VOID );

#endif /* NDEBUG */

#ifdef __cplusplus
}
#endif

#endif /* P_BTREE_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
