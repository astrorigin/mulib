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

#ifndef M_DICT_PRIV_H
#define M_DICT_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \brief Allocate for a dict node.
 *  \return M_TRUE, or M_FALSE on error.
 */
M_DLLAPI M_BOOL
m_DictNode_new(m_DictNode** const nd,
        const M_CHAR* const key,
        const M_PTR const val);

/**
 *  \brief Deallocate a dict node.
 */
M_DLLAPI M_VOID
m_DictNode_delete(m_DictNode** const nd);

/**
 *  \brief Deallocate a list of dict nodes.
 */
M_DLLAPI M_VOID
m_DictNode_list_delete(m_DictNode* nd);

/**
 *  \brief Initialize a dict node.
 *  \return M_TRUE, or M_FALSE on error.
 */
M_DLLAPI M_BOOL
m_DictNode_init(m_DictNode* const nd,
        const M_CHAR* const key,
        const M_PTR const val);

/**
 *  \brief Finalize a dict node.
 */
M_DLLAPI M_VOID
m_DictNode_fini(m_DictNode* const nd);

#ifndef NDEBUG

M_DLLAPI M_VOID
m_Dict_debug(const m_Dict* const d);

#endif /* NDEBUG */

#ifdef __cplusplus
}
#endif
#endif /* !M_DICT_PRIV_H */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
