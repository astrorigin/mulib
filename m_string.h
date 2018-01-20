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
 *  \file m_string.h
 *  \brief String container.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <stan@astrorigin.com>
 */

#ifndef M_STRING_H
#define M_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "m_h.h"
#include "m_array.h"

/**
 *  \typedef m_String
 */
typedef m_Array m_String;

#ifndef M_STRING_QUANTA
/**
 *  \brief Allocation strategy for strings.
 */
#define M_STRING_QUANTA  8
#elif M_STRING_QUANTA < 1
#error "Invalid M_STRING_QUANTA"
#endif

/**
 *  \brief Allocate for a new string.
 *  \param s The string (by ref, initialized to NULL).
 *  \param content Null-terminated string to copy (or NULL).
 *  \return Address of the string data, or NULL on error.
 */
M_DLLAPI M_CHAR*
m_String_new(m_String** const s,
        const M_CHAR* const content);

/**
 *  \brief Allocate for a new string with length.
 *  \param s The string (by ref, initialized to NULL).
 *  \param content String to copy (or NULL).
 *  \param len Length of string to copy.
 *  \return Address of the string data, or NULL on error.
 */
M_DLLAPI M_CHAR*
m_String_new_len(m_String** const s,
        const M_CHAR* const content,
        const M_SZ len);

/**
 *  \brief Delete string.
 */
M_DLLAPI M_VOID
m_String_delete(m_String** const s);

/**
 *  \brief Initialize a string.
 *  \param s The string.
 *  \param content Null-terminated string to copy (or NULL).
 *  \return Address of the string data, or NULL on error.
 */
M_DLLAPI M_CHAR*
m_String_init(m_String* const s,
        const M_CHAR* const content);

/**
 *  \brief Initialize a string with length.
 *  \param s The string.
 *  \param content String to copy (or NULL).
 *  \param len Length of string to copy.
 *  \return Address of the string data, or NULL on error.
 */
M_DLLAPI M_CHAR*
m_String_init_len(m_String* const s,
        const M_CHAR* const content,
        const M_SZ len);

/**
 *  \brief Finalize a string.
 */
M_DLLAPI M_VOID
m_String_fini(m_String* const s);

/**
 *  \brief Compare strings.
 */
#define m_String_CMP(titi, toto) \
  (strcmp((const char*)(titi)->data,(const char*)(toto)->data))

/**
 *  \brief Get string data.
 */
#define m_String_DATA(s) ((s)->data)

/**
 *  \brief Get string length.
 */
#define m_String_LEN(s) ((s)->len-1)

/**
 *  \brief Set string content.
 *  \param s The string.
 *  \param content Null-terminated string to copy (or NULL).
 *  \return Address of the string data, or NULL on error.
 */
M_DLLAPI M_CHAR*
m_String_set(m_String* const s,
        const M_CHAR* const content);

/**
 *  \brief Set string content with length.
 *  \param s The string.
 *  \param content String to copy (or NULL).
 *  \param len Length of string to copy.
 *  \return Address of the string data, or NULL on error.
 */
M_DLLAPI M_CHAR*
m_String_set_len(m_String* const s,
        const M_CHAR* const content,
        const M_SZ len);

/**
 *  \brief Copy a string.
 *  \param dest The destination string.
 *  \param src The string to copy.
 *  \return Address of the destination string data, or NULL on error.
 */
M_DLLAPI M_CHAR*
m_String_copy(m_String* const dest,
        const m_String* const src);

/**
 *  \brief Duplicate a string.
 *  \param s String to duplicate.
 *  \return New string, or NULL on error.
 */
M_DLLAPI m_String*
m_String_dup(const m_String* const s);

/**
 *  \brief Append data to a string.
 *  \param s The string.
 *  \param content Null-terminated string to append (or NULL).
 *  \return Address of string data, or NULL on error.
 */
M_DLLAPI M_CHAR*
m_String_cat(m_String* const s,
        const M_CHAR* const content);

/**
 *  \brief Append data to a string (with length).
 *  \param s The string.
 *  \param content String to append (or NULL).
 *  \param len Length of string to append.
 *  \return Address of string data, or NULL on error.
 */
M_DLLAPI M_CHAR*
m_String_cat_len(m_String* const s,
        const M_CHAR* const content,
        const M_SZ len);

/**
 *  \brief Find and replace occurences of a substring.
 *  \param s The string.
 *  \param titi Substring to be replaced (not NULL).
 *  \param toto Substring to insert (not NULL).
 *  \return Address of string data, or NULL on error.
 */
M_DLLAPI M_CHAR*
m_String_replace(m_String* const s,
        const M_CHAR* const titi,
        const M_CHAR* const toto);

#ifndef NDEBUG

/**
 *  \brief Print debug information about a string.
 *  \param s The string.
 */
M_DLLAPI M_VOID
m_String_debug(const m_String* const s);

#endif /* !NDEBUG */

#ifdef __cplusplus
}
#endif
#endif /* !M_STRING_H */
/* vi: set fenc=utf-8 ff=unix ai et sw=2 ts=2 sts=2 : */
