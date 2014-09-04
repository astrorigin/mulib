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
 *  \file p_string.h
 *  \brief String container.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 */

#ifndef P_STRING_H
#define P_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "p_h.h"
#include "p_vector.h"

/**
 *  \typedef p_String
 */
typedef p_Vector p_String;

/**
 *  \brief Allocation strategy for strings.
 */
#ifndef P_STRING_QUANTA
#define P_STRING_QUANTA  12
#endif

/**
 *  \brief Allocate for a new string.
 *  \param s The string.
 *  \param content String to copy (or NULL).
 *  \return P_TRUE, or P_FALSE on error.
 */
P_EXPORT P_BOOL
p_string_new( p_String** s,
        const P_CHAR* content );

/**
 *  \brief Allocate for a new string with length.
 *  \param s The string.
 *  \param content String to copy (or NULL).
 *  \param len Length of string.
 *  \return P_TRUE, or P_FALSE on error.
 */
P_EXPORT P_BOOL
p_string_new_len( p_String** s,
        const P_CHAR* content,
        const P_SZ len );

/**
 *  \brief Delete string.
 */
#define p_string_delete \
        p_vector_delete

/**
 *  \brief Initialize a string.
 *  \param s The string.
 *  \param content String to copy (or NULL).
 *  \return P_TRUE, or P_FALSE on error.
 */
#define p_string_init( s, content ) \
        p_string_init_len( s, content, content ? strlen( (const char*)content ) : 0 )

/**
 *  \brief Initialize a string with length.
 *  \param s The string.
 *  \param content String to copy (or NULL).
 *  \param len Length of string.
 *  \return P_TRUE, or P_FALSE on error.
 */
P_EXPORT P_BOOL
p_string_init_len( p_String* s,
        const P_CHAR* content,
        const P_SZ len );

/**
 *  \brief Finalize a string.
 */
#define p_string_fini \
        p_vector_fini

/**
 *  \brief Reserve function for strings.
 */
P_EXPORT P_SZ
p_string_calc_space_fn( p_String*,
        P_SZ );

/**
 *  \brief Compare strings.
 */
#define p_string_cmp( titi, toto ) \
        (strcmp( (const char*)(titi)->data, (const char*)(toto)->data ))

/**
 *  \brief Get string length.
 */
#define p_string_len( s ) \
        ((s)->len -1)

/**
 *  \brief Set string content.
 *  \param s The string.
 *  \param content String to copy (or NULL).
 *  \return Start of the string, or NULL on error.
 */
#define p_string_set( s, content ) \
        p_string_set_len( (s), (content), (content) ? strlen( (const char*)(content) ) : 0 )

/**
 *  \brief Set string content with length.
 *  \param s The string.
 *  \param content String to copy (or NULL).
 *  \param len Length of string to copy.
 *  \return Start of the string, or NULL on error.
 */
P_EXPORT P_CHAR*
p_string_set_len( p_String* s,
        const P_CHAR* content,
        const P_SZ len );

/**
 *  \brief Copy a string.
 */
#define p_string_copy( s1, s2 ) \
        p_string_set_len( (s1), (const P_CHAR*)(s2)->data, (s2)->len -1 )

/**
 *  \brief Append data to a string.
 *  \param s The string.
 *  \param content String to append (or NULL).
 *  \return Start of string, or NULL on error.
 */
P_EXPORT P_CHAR*
p_string_cat( p_String* s,
        const P_CHAR* content );

/**
 *  \brief Append data to a string (with length).
 *  \param s The string.
 *  \param content String to append (or NULL).
 *  \param len Length of string to append.
 *  \return Start of string, or NULL on error.
 */
P_EXPORT P_CHAR*
p_string_cat_len( p_String* s,
        const P_CHAR* content,
        const P_SZ len );

/**
 *  \brief Find and replace occurences of a substring.
 *  \param s The string.
 *  \param s1 Substring to be replaced (not NULL).
 *  \param s2 Substring to insert (not NULL).
 *  \return P_TRUE, or P_FALSE on error.
 */
P_EXPORT P_BOOL
p_string_replace( p_String* s,
        const P_CHAR* s1,
        const P_CHAR* s2 );

#ifndef NDEBUG

/**
 *  \brief Print debug information about a string.
 *  \param s The string.
 */
#define p_string_debug( s ) \
        p_vector_debug( (p_Vector*)(s) )

P_EXPORT P_INT32
p_string_test( P_VOID );

#endif

#ifdef __cplusplus
}
#endif

#endif /* P_STRING_H */
/* vi: set fenc=utf-8 ff=unix ai et sw=4 ts=4 sts=4 : */
