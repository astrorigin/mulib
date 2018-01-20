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
 *  \file m_dict.h
 *  \brief Hash tree container for strings.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <stan@astrorigin.com>
 */

#ifndef M_DICT_H
#define M_DICT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "m_btree.h"
#include "m_h.h"
#include "m_mempool.h"
#include "m_string.h"

/**
 *  \typedef m_DictNode
 */
typedef struct _m_DictNode m_DictNode;

/**
 *  \struct _m_DictNode
 */
struct _m_DictNode
{
  m_DictNode* next;
  m_String key;
  M_PTR val;
};

/**
 *  \typedef m_Dict
 */
typedef m_BTree m_Dict;

#include "m_dict_priv.h"

/**
 *  \brief Allocate for a new dict.
 *  \param d The dict.
 *  \return M_TRUE, or M_FALSE on error.
 */
M_DLLAPI M_BOOL
m_Dict_new(m_Dict** const d);

/**
 *  \brief Initialize a dict.
 */
M_DLLAPI M_BOOL
m_Dict_init(m_Dict* const d);

/**
 *  \brief Finalize a dict.
 */
M_DLLAPI M_VOID
m_Dict_fini(m_Dict* const d);

/**
 *  \brief Delete a dict.
 */
M_DLLAPI M_VOID
m_Dict_delete(m_Dict** const d);

/**
 *  \brief Our hash function.
 */
M_DLLAPI M_ID
m_Dict_hash(const M_PTR k,
        const M_SZ len);

/**
 *  \brief Get an elem from the dict or NULL.
 *  \param d The dict (not NULL).
 *  \param key The key string (not NULL nor empty).
 *  \return Element found, or NULL if nothing found (or key is invalid).
 */
M_DLLAPI M_PTR
m_Dict_get(const m_Dict* const d,
        const M_CHAR* const key);

/**
 *  \brief Set or insert an element in the dict.
 *  \param d The dict (not NULL).
 *  \param key The key string (not NULL nor empty).
 *  \param val The pointer value.
 *  \param prev If not NULL, return value replaced, or value set if there was none.
 *  \return M_TRUE, or M_FALSE on error.
 */
M_DLLAPI M_BOOL
m_Dict_set(m_Dict* const d,
        const M_CHAR* const key,
        const M_PTR const val,
        M_PTR* const prev);

/**
 *  \brief Remove an element from the dict.
 *  \param d The dict.
 *  \param key The key string.
 *  \return Element that was removed, or NULL if not found (or key is invalid).
 */
M_DLLAPI M_PTR
m_Dict_unset(m_Dict* const d,
        const M_CHAR* const key);

/**
 *  \brief Apply a function to each value in the dict.
 *  \param d The dict.
 *  \param traverse_fn The function to apply.
 */
M_DLLAPI M_VOID
m_Dict_traverse(m_Dict* const d,
        M_VOID (* const traverse_fn)(M_PTR val));

/**
 *  \brief Apply a function to each value in the dict (with userdata).
 *  \param d The dict.
 *  \param traverse_fn The function to apply.
 *  \param userdata Data passed to traverse_fn.
 */
M_DLLAPI M_VOID
m_Dict_traverse2(m_Dict* const d,
        M_VOID (* const traverse_fn)(M_PTR val, M_PTR udata),
        M_PTR const userdata);

/**
 *  \brief Apply a function to each key and value in the dict.
 *  \param d The dict.
 *  \param traverse_fn The function to apply.
 */
M_DLLAPI M_VOID
m_Dict_traverse_keyval(m_Dict* const d,
        M_VOID (* const traverse_fn)(m_String* key, M_PTR val));

/**
 *  \brief Apply a function to each key and value in the dict (with userdata).
 *  \param d The dict.
 *  \param traverse_fn The function to apply.
 *  \param userdata Data passed to traverse_fn.
 */
M_DLLAPI M_VOID
m_Dict_traverse_keyval2(m_Dict* const d,
        M_VOID (* const traverse_fn)(m_String* key, M_PTR val, M_PTR udata),
        M_PTR const userdata);

#ifdef __cplusplus
}
#endif
#endif /* !M_DICT_H */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
