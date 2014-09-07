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
 *  \file p_json.h
 *  \brief JSON my son.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 */

#ifndef P_JSON_H
#define P_JSON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "p_h.h"
#include "p_shell.h"
#include "p_string.h"

/* Scanner events */
#define P_JSON_ERROR        -1
#define P_JSON_END          0
#define P_JSON_LBRACE       1
#define P_JSON_RBRACE       2
#define P_JSON_LBRACKET     3
#define P_JSON_RBRACKET     4
#define P_JSON_STRING       5
#define P_JSON_NUMBER       6
#define P_JSON_COLON        7
#define P_JSON_COMMA        8
#define P_JSON_TRUE         9
#define P_JSON_FALSE        10
#define P_JSON_NULL         11

/**
 *  \brief Scan json string and get whatever comes.
 *  \param s String to scan.
 *  \param nil End of string.
 *  \param null Initialized shell (type null) to store what is found.
 *  \param next Where scanner has stopped.
 */
P_EXPORT P_BOOL
p_json( P_CHAR* s,
        const P_CHAR* nil,
        p_Shell* null,
        P_CHAR** next );

/**
 *  \brief Scan string for a json event.
 *  \param s String to scan.
 *  \param nil End of string.
 *  \param next Where scanner has stopped.
 *  \return a json event.
 */
P_EXPORT P_INT8
p_json_scan( P_CHAR* s,
        const P_CHAR* nil,
        P_CHAR** next );

/**
 *
 */
P_EXPORT P_BOOL
p_json_scan_object( P_CHAR* s,
        const P_CHAR* nil,
        p_Shell* dict,
        P_CHAR** next );

/**
 *
 */
P_EXPORT P_BOOL
p_json_scan_array( P_CHAR* s,
        const P_CHAR* nil,
        p_Shell* arr,
        P_CHAR** next );

/**
 *
 */
P_EXPORT P_BOOL
p_json_scan_value( P_CHAR* s,
        const P_CHAR* nil,
        p_Shell* shell,
        P_CHAR** next );

/**
 *  \brief Scan json string.
 *  \param s String to scan.
 *  \param nil End of string.
 *  \param str Initialized shell (type string) to store the decoded json string (or NULL).
 *  \param next Where scanner has stopped.
 *  \return P_TRUE on success.
 */
P_EXPORT P_BOOL
p_json_scan_string( P_CHAR* s,
        const P_CHAR* nil,
        p_Shell* str,
        P_CHAR** next );

/**
 *  \brief Scan json number.
 *  \param s String to scan.
 *  \param nil End of string.
 *  \param shell Initialized shell (type null) to store the number (or NULL).
 *  \param next Where scanner stopped.
 *  \return P_TRUE on success.
 */
P_EXPORT P_BOOL
p_json_scan_number( P_CHAR* s,
        const P_CHAR* nil,
        p_Shell* shell,
        P_CHAR** next );

#ifdef __cplusplus
}
#endif

#endif /* P_JSON_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
