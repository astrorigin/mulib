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
 *  \file p_dict.h
 *  \brief Hash tree container.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 */

#ifndef P_DICT_H
#define P_DICT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "p_btree.h"
#include "p_h.h"
#include "p_mempool.h"
#include "p_string.h"

/**
 *  \typedef p_DictNode
 */
typedef struct _p_dict_node p_DictNode;

/**
 *  \struct _p_dict_node
 */
struct _p_dict_node
{
    p_DictNode* next;
    p_String    key;
    P_PTR       val;
};

/**
 *  \typedef p_Dict
 */
typedef p_BTree p_Dict;

/**
 *  \brief Allocate for a dict.
 */
#define p_dict_new( d ) \
        p_btree_new( (p_BTree**)(d) )

/**
 *  \brief Initialize a dict.
 */
#define p_dict_init( d ) \
        p_btree_init( (p_BTree*)(d), P_MALLOC_REF, p_btree_node_free_ref )

/**
 *  \brief Finalize a dict.
 */
P_EXPORT P_VOID
p_dict_fini( p_Dict* d );

/**
 *  \brief Delete a dict.
 */
P_EXPORT P_VOID
p_dict_delete( p_Dict** d );

/**
 *  \brief Hash function.
 */
P_EXPORT P_ID
p_dict_hash( const P_PTR k,
        const P_SZ len );

/**
 *  \brief Get an elem from the dict or NULL.
 *  \param d The dict.
 *  \param key The key string.
 *  \return Element found, or NULL if nothing found (or key is invalid). 
 */
P_EXPORT P_PTR
p_dict_get( const p_Dict* d,
        const P_CHAR* key );

/**
 *  \brief Set or insert an element in the dict.
 *  \param d The dict.
 *  \param key The key string.
 *  \param val The pointer value.
 *  \param old The pointer value if a new element was inserted,
 *  or the pointer that was replaced (can be NULL).
 *  \return P_TRUE, or P_FALSE on error.
 */
P_EXPORT P_BOOL
p_dict_set( p_Dict* d,
        const P_CHAR* key,
        const P_PTR val,
        P_PTR* old );

/**
 *  \brief Remove an element from the dict.
 *  \param d The dict.
 *  \param key The key string.
 *  \return Element that was removed, or NULL if not found (or key is invalid).
 */
P_PTR
p_dict_unset( p_Dict* d,
        const P_CHAR* key );

/**
 *  \brief Apply a function to each value in the dict.
 *  \param d The dict.
 *  \param traverse_fn The function to apply.
 */
P_EXPORT P_VOID
p_dict_traverse( p_Dict* d,
        P_VOID (*traverse_fn)( P_PTR ) );

/**
 *  \brief Apply a function to each value in the dict (with userdata).
 *  \param d The dict.
 *  \param traverse_fn The function to apply.
 *  \param userdata Data passed to traverse_fn.
 */
P_EXPORT P_VOID
p_dict_traverse2( p_Dict* d,
        P_VOID (*traverse_fn)( P_PTR, P_PTR ),
        P_PTR userdata );

/**
 *  \brief Apply a function to each key and value in the dict.
 *  \param d The dict.
 *  \param traverse_fn The function to apply.
 */
P_EXPORT P_VOID
p_dict_traverse_keyval( p_Dict* d,
        P_VOID (*traverse_fn)( p_String*, P_PTR ) );

/**
 *  \brief Apply a function to each key and value in the dict (with userdata).
 *  \param d The dict.
 *  \param traverse_fn The function to apply.
 *  \param userdata Data passed to traverse_fn.
 */
P_EXPORT P_VOID
p_dict_traverse_keyval2( p_Dict* d,
        P_VOID (*traverse_fn)( p_String*, P_PTR, P_PTR ),
        P_PTR userdata );

/**
 *  \brief Allocate for a dict node.
 *  \return P_TRUE, or P_FALSE on error.
 */
P_EXPORT P_BOOL
p_dict_node_new( p_DictNode** nd,
        const P_CHAR* key,
        const P_PTR val );

/**
 *  \brief Deallocate a dict node.
 */
P_EXPORT P_VOID
p_dict_node_delete( p_DictNode** nd );

/**
 *  \brief Deallocate a list of dict nodes.
 */
P_EXPORT P_VOID
p_dict_node_list_delete( p_DictNode* nd,
        P_PTR userdata );

/**
 *  \brief Initialize a dict node.
 *  \return P_TRUE, or P_FALSE on error.
 */
P_EXPORT P_BOOL
p_dict_node_init( p_DictNode* nd,
        const P_CHAR* key,
        const P_PTR val );

/**
 *  \brief Finalize a dict node.
 */
P_EXPORT P_VOID
p_dict_node_fini( p_DictNode* nd );

#ifndef NDEBUG

P_EXPORT P_VOID
p_dict_debug( const p_Dict* d );

P_EXPORT P_INT32
p_dict_test( P_VOID );

#endif /* NDEBUG */

#ifdef __cplusplus
}
#endif

#endif /* P_DICT_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
