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
 *  \file p_vdict.h
 *  \brief Hash tree container for data.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 */

#ifndef P_VDICT_H
#define P_VDICT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "p_btree.h"
#include "p_dict.h"
#include "p_h.h"
#include "p_mempool.h"
#include "p_vector.h"

/**
 *  \typedef p_VDictNode
 */
typedef struct _p_vdict_node p_VDictNode;

/**
 *  \struct _p_vdict_node
 */
struct _p_vdict_node
{
    p_VDictNode*    next;
    p_Vector        key;
    P_PTR           val;
};

/**
 *  \typedef p_Dict
 */
typedef p_BTree p_VDict;

/**
 *  \brief Allocate for a vdict.
 */
#define p_vdict_new( d ) \
        p_btree_new( (p_BTree**)(d) )

/**
 *  \brief Initialize a vdict.
 */
#define p_vdict_init( d ) \
        p_btree_init( (p_BTree*)(d), P_MALLOC_REF, p_btree_node_free_ref )

/**
 *  \brief Finalize a vdict.
 */
P_EXPORT P_VOID
p_vdict_fini( p_VDict* d );

/**
 *  \brief Delete a vdict.
 */
P_EXPORT P_VOID
p_vdict_delete( p_VDict** d );

/**
 *  \brief Hash function.
 */
#define p_vdict_hash \
        p_dict_hash
/*P_EXPORT P_ID
p_vdict_hash( const P_PTR k,
        const P_SZ len );*/

/**
 *  \brief Get an elem from the dict or NULL.
 *  \param d The vdict.
 *  \param key The key vector.
 *  \return Element found, or NULL if nothing found (or key is invalid). 
 */
P_EXPORT P_PTR
p_vdict_get( const p_VDict* d,
        const p_Vector* key );

/**
 *  \brief Set or insert an element in the vdict.
 *  \param d The vdict.
 *  \param key The key vector.
 *  \param val The pointer value.
 *  \param old The pointer value if a new element was inserted,
 *  or the pointer that was replaced (can be NULL).
 *  \return P_TRUE, or P_FALSE on error.
 */
P_EXPORT P_BOOL
p_vdict_set( p_VDict* d,
        const p_Vector* key,
        const P_PTR val,
        P_PTR* old );

/**
 *  \brief Remove an element from the vdict.
 *  \param d The vdict.
 *  \param key The key vector.
 *  \return Element that was removed, or NULL if not found (or key is invalid).
 */
P_EXPORT P_PTR
p_vdict_unset( p_VDict* d,
        const p_Vector* key );

/**
 *  \brief Apply a function to each value in the vdict.
 *  \param d The vdict.
 *  \param traverse_fn The function to apply.
 */
#define p_vdict_traverse( d, traverse_fn ) \
        p_dict_traverse( (p_Dict*)d, traverse_fn )

/**
 *  \brief Apply a function to each value in the vdict (with userdata).
 *  \param d The vdict.
 *  \param traverse_fn The function to apply.
 *  \param userdata Data passed to traverse_fn.
 */
#define p_vdict_traverse2( d, traverse_fn, userdata ) \
        p_dict_traverse2( (p_Dict*)d, traverse_fn, userdata )

/**
 *  \brief Apply a function to each key and value in the vdict.
 *  \param d The vdict.
 *  \param traverse_fn The function to apply.
 */
P_EXPORT P_VOID
p_vdict_traverse_keyval( p_VDict* d,
        P_VOID (*traverse_fn)( p_Vector*, P_PTR ) );

/**
 *  \brief Apply a function to each key and value in the vdict (with userdata).
 *  \param d The vdict.
 *  \param traverse_fn The function to apply.
 *  \param userdata Data passed to traverse_fn.
 */
P_EXPORT P_VOID
p_vdict_traverse_keyval2( p_VDict* d,
        P_VOID (*traverse_fn)( p_Vector*, P_PTR, P_PTR ),
        P_PTR userdata );

/**
 *  \brief Allocate for a vdict node.
 *  \return P_TRUE, or P_FALSE on error.
 */
P_EXPORT P_BOOL
p_vdict_node_new( p_VDictNode** nd,
        const p_Vector* key,
        const P_PTR val );

/**
 *  \brief Deallocate a vdict node.
 */
P_EXPORT P_VOID
p_vdict_node_delete( p_VDictNode** nd );

/**
 *  \brief Deallocate a list of vdict nodes.
 */
P_EXPORT P_VOID
p_vdict_node_list_delete( p_VDictNode* nd,
        P_PTR userdata );

/**
 *  \brief Initialize a vdict node.
 *  \return P_TRUE, or P_FALSE on error.
 */
P_EXPORT P_BOOL
p_vdict_node_init( p_VDictNode* nd,
        const p_Vector* key,
        const P_PTR val );

/**
 *  \brief Finalize a vdict node.
 */
P_EXPORT P_VOID
p_vdict_node_fini( p_VDictNode* nd );

#if 0
/**
 *  \brief Allocate a vdict node with read-only data.
 */
P_EXPORT P_BOOL
p_vdict_node_ro_new( p_VDictNode** nd,
        const P_PTR data,
        const P_SZ len,
        const P_PTR val );

/**
 *  \brief Deallocate a vdict node with read-only data.
 */
P_EXPORT P_VOID
p_vdict_node_ro_delete( p_VDictNode** nd );

/**
 *  \brief Deallocate a list of vdict nodes with read-only data.
 */
P_EXPORT P_VOID
p_vdict_node_ro_list_delete( p_VDictNode* nd,
        P_PTR userdata );

/**
 *  \brief Initialize a vdict node with read-only data.
 */
P_EXPORT P_BOOL
p_vdict_node_ro_init( p_VDictNode* nd,
        const P_PTR data,
        const P_SZ len,
        const P_PTR val );

/**
 *  \brief Finalize a vdict node with read-only data.
 */
P_EXPORT P_VOID
p_vdict_node_ro_fini( p_VDictNode* nd );

/**
 *  \brief Set an element in a vdict with read-only data.
 *  \param data The key vector (will not be duplicated).
 *  \param len Size of data.
 *  \param val The element value.
 *  \param old The pointer value if a new element was inserted,
 *  or the pointer that was replaced (can be NULL).
 *  \return P_TRUE, or P_FALSE on error.
 */
P_EXPORT P_BOOL
p_vdict_ro_set( p_VDict* d,
        const P_PTR data,
        const P_SZ len,
        const P_PTR val,
        P_PTR* old );
#endif

#ifndef NDEBUG
#if 0
P_EXPORT P_INT32
p_vdict_test( P_VOID );
#endif
#endif /* NDEBUG */

#ifdef __cplusplus
}
#endif

#endif /* P_VDICT_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
