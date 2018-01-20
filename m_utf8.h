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
 *  \file m_utf8.h
 *  \brief UTF-8 string tools.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <stan@astrorigin.com>
 *  \see https://tools.ietf.org/html/rfc3629
 */

#ifndef M_UTF8_H
#define M_UTF8_H

#ifdef __cplusplus
extern "C" {
#endif

#include "m_h.h"

/**
 *  \brief Get number of octets needed by a Unicode character.
 *  \param c Unicode character number.
 *  \return 1 to 4, or 0 on error (invalid character number).
 */
#define m_utf8_octetnum(c) \
  ( c >= 0xD800 && c <= 0xDFFF ? 0 : \
  c >= 0x0 && c <= 0x7F ? 1 : \
  c >= 0x80 && c <= 0x7FF ? 2 : \
  c >= 0x800 && c <= 0xFFFF ? 3 : \
  c >= 0x10000 && c <= 0x10FFFF ? 4 : \
  0 )

/**
 *  \brief Prepare string for an Unicode character.
 */
M_DLLAPI M_VOID
m_utf8_octetprepare(M_CHAR* const octets,
        const M_INT8 num);

/**
 *  \brief Encode an Unicode character in UTF-8.
 *  \param c Unicode character number.
 *  \param buf Buffer for 5 chars.
 *  \return Null-terminated, UTF-8 encoded representation of the Unicode character.
 */
M_DLLAPI M_CHAR*
m_utf8_encode(const M_INT32 c,
        M_CHAR* const buf);

/**
 *  \brief Count characters in UTF-8 string.
 *  \param s The string.
 *  \return Number of UTF-8 chars, or -1 on error (invalid sequence).
 *  \note This function can be used as validator.
 */
M_DLLAPI M_SZ
m_utf8_strlen(const M_CHAR* s);

/**
 *  \brief Extract non-null utf-8 character sequence.
 *  \param s Start of character.
 *  \param buf Buffer for 5 chars.
 *  \return M_TRUE on success.
 *
 *  No in-depth utf-8 validity check is made.
 */
M_DLLAPI M_BOOL
m_utf8_get_char(const M_CHAR* s,
        M_CHAR* const buf);

/**
 *  \brief Go to the next non-null utf-8 character sequence.
 *  \param s Start of an utf-8 character sequence.
 *  \return Next first char of an utf-8 character sequence, or NULL.
 *
 *  No in-depth utf-8 validity check is made.
 */
M_DLLAPI M_CHAR*
m_utf8_next_char(const M_CHAR* s);

/**
 *  \brief Decode an utf-8 character sequence.
 *  \param s Start of an utf-8 character sequence.
 *  \param i Unicode character number.
 *  \return M_TRUE on success.
 *
 *  No in-depth utf-8 validity check is made.
 */
M_DLLAPI M_BOOL
m_utf8_decode(const M_CHAR* s,
        M_INT32* i);

#ifdef __cplusplus
}
#endif
#endif /* !M_UTF8_H */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
