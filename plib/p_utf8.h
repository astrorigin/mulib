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
 *  \file p_utf8.h
 *  \brief UTF-8 tools.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 */

#ifndef P_UTF8_H
#define P_UTF8_H

#ifdef __cplusplus
extern "C" {
#endif

#include "p_h.h"

/**
 *  \brief Get number of octets needed by a Unicode character.
 *  \param c Unicode character number.
 *  \return 1 to 4, or 0 on error (invalid character number).
 */
P_EXPORT P_INT8
p_utf8_octetnum( const P_INT32 c );

/**
 *  \brief Prepare string for an Unicode character.
 */
P_EXPORT P_VOID
p_utf8_octetprepare( P_CHAR* octets,
        const P_INT8 num );

/**
 *  \brief Encode an Unicode character in UTF-8.
 *  \param c Unicode character number.
 *  \param buf Buffer for 5 chars.
 *  \return Null-terminated, UTF-8 encoded representation of the Unicode character.
 */
P_EXPORT P_CHAR*
p_utf8_encode( const P_INT32 c,
        P_CHAR* buf );

/**
 *  \brief Count characters in UTF-8 string.
 *  \param s The string.
 *  \return Number of UTF-8 chars, or ((P_SZ)-1) on error (invalid sequence).
 */
P_EXPORT P_SZ
p_utf8_strlen( const P_CHAR* s );

#ifdef __cplusplus
}
#endif

#endif /* P_UTF8_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
