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
 *  \file m_bdict.h
 *  \brief Hash tree container with binary data keys.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <stan@astrorigin.com>
 */

#ifndef M_BDICT_H
#define M_BDICT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "m_array.h"
#include "m_btree.h"
#include "m_dict.h"
#include "m_h.h"
#include "m_mempool.h"

/**
 *  \typedef m_BDictNode
 */
typedef struct _m_BDictNode m_BDictNode;

/**
 *  \struct _m_BDictNode
 */
struct _m_BDictNode
{
  m_BDictNode* next;
  m_Array key;
  M_PTR val;
};

/**
 *  \typedef m_BDict
 */
typedef m_BTree m_BDict;

/**
 *  \brief Allocate for a vdict.
 */
M_DLLAPI M_BOOL
m_BDict_new(m_BDict** const d);

/**
 *  \brief Initialize a vdict.
 */
M_DLLAPI M_BOOL
m_BDict_init(m_BDict* const d);

/**
 *  \brief Finalize a vdict.
 */
M_DLLAPI M_VOID
m_BDict_fini(m_BDict* const d);

/**
 *  \brief Delete a vdict.
 */
M_DLLAPI M_VOID
m_BDict_delete( m_BDict** d );

/**
 *  \brief Hash function.
 */
#define m_BDict_hash m_Dict_hash

/**
 *  \brief Get an element from the bdict.
 *  \param d The bdict.
 *  \param key The key array (not NULL nor empty).
 *  \return Element found, or NULL if nothing found (or key is invalid).
 */
M_DLLAPI M_PTR
m_BDict_get(const m_BDict* const d,
        const m_Array* const key);

/**
 *  \brief Set or insert an element in the bdict.
 *  \param d The vdict.
 *  \param key The key array (not NULL nor empty).
 *  \param val The element.
 *  \param prev The pointer value if a new element was inserted,
 *  or the pointer that was replaced (can be NULL).
 *  \return M_TRUE, or M_FALSE on error (or key is invalid).
 */
M_DLLAPI M_BOOL
m_BDict_set(m_BDict* const d,
        const m_Array* const key,
        const M_PTR const val,
        M_PTR* const prev);

/**
 *  \brief Remove an element from the bdict.
 *  \param d The bdict.
 *  \param key The key array (not NULL nor empty).
 *  \return Element that was removed, or NULL if not found (or key is invalid).
 */
M_DLLAPI M_PTR
m_BDict_unset(m_BDict* const d,
        const m_Array* const key);

/**
 *  \brief Apply a function to each value in the vdict.
 *  \param d The vdict.
 *  \param traverse_fn The function to apply.
 */
M_DLLAPI M_VOID
m_BDict_traverse(m_BDict* const d,
        M_VOID (* const traverse_fn)(M_PTR val));

/**
 *  \brief Apply a function to each value in the vdict (with userdata).
 *  \param d The vdict.
 *  \param traverse_fn The function to apply.
 *  \param userdata Data passed to traverse_fn.
 */
M_DLLAPI M_VOID
m_BDict_traverse2(m_BDict* const d,
        M_VOID (* const traverse_fn)(M_PTR val, M_PTR udata),
        M_PTR const userdata);

/**
 *  \brief Apply a function to each key and value in the bdict.
 *  \param d The bdict.
 *  \param traverse_fn The function to apply.
 */
M_DLLAPI M_VOID
m_BDict_traverse_keyval(m_BDict* const d,
        M_VOID (* const traverse_fn)(m_Array* key, M_PTR val));

/**
 *  \brief Apply a function to each key and value in the vdict (with userdata).
 *  \param d The bdict.
 *  \param traverse_fn The function to apply.
 *  \param userdata Data passed to traverse_fn.
 */
M_DLLAPI M_VOID
m_BDict_traverse_keyval2(m_BDict* const d,
        M_VOID (* const traverse_fn)(m_Array* key, M_PTR val, M_PTR udata),
        M_PTR const userdata);

/**
 *  \brief Allocate for a vdict node.
 *  \return M_TRUE, or M_FALSE on error.
 */
M_DLLAPI M_BOOL
m_BDict_node_new( m_BDictNode** nd,
        const m_Array* key,
        const M_PTR val );

/**
 *  \brief Deallocate a vdict node.
 */
M_DLLAPI M_VOID
m_BDict_node_delete( m_BDictNode** nd );

/**
 *  \brief Deallocate a list of vdict nodes.
 */
M_DLLAPI M_VOID
m_BDict_node_list_delete( m_BDictNode* nd );

/**
 *  \brief Initialize a vdict node.
 *  \return M_TRUE, or M_FALSE on error.
 */
M_DLLAPI M_BOOL
m_BDict_node_init( m_BDictNode* nd,
        const m_Array* key,
        const M_PTR val );

/**
 *  \brief Finalize a vdict node.
 */
M_DLLAPI M_VOID
m_BDict_node_fini( m_BDictNode* nd );

#if 0
/**
 *  \brief Allocate a vdict node with read-only data.
 */
M_DLLAPI M_BOOL
m_BDict_node_ro_new( m_BDictNode** nd,
        const M_PTR data,
        const M_SZ len,
        const M_PTR val );

/**
 *  \brief Deallocate a vdict node with read-only data.
 */
M_DLLAPI M_VOID
m_BDict_node_ro_delete( m_BDictNode** nd );

/**
 *  \brief Deallocate a list of vdict nodes with read-only data.
 */
M_DLLAPI M_VOID
m_BDict_node_ro_list_delete( m_BDictNode* nd,
        M_PTR userdata );

/**
 *  \brief Initialize a vdict node with read-only data.
 */
M_DLLAPI M_BOOL
m_BDict_node_ro_init( m_BDictNode* nd,
        const M_PTR data,
        const M_SZ len,
        const M_PTR val );

/**
 *  \brief Finalize a vdict node with read-only data.
 */
M_DLLAPI M_VOID
m_BDict_node_ro_fini( m_BDictNode* nd );

/**
 *  \brief Set an element in a vdict with read-only data.
 *  \param data The key array (will not be duplicated).
 *  \param len Size of data.
 *  \param val The element value.
 *  \param old The pointer value if a new element was inserted,
 *  or the pointer that was replaced (can be NULL).
 *  \return M_TRUE, or M_FALSE on error.
 */
M_DLLAPI M_BOOL
m_BDict_ro_set( m_BDict* d,
        const M_PTR data,
        const M_SZ len,
        const M_PTR val,
        M_PTR* old );
#endif

#ifndef NDEBUG
#if 0
M_DLLAPI M_INT32
m_BDict_test( M_VOID );
#endif
#endif /* NDEBUG */

#ifdef __cplusplus
}
#endif
#endif /* M_BDICT_H */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
