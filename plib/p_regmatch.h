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
 *  \file p_regmatch.h
 *  \brief function regmatch().
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 */

#ifndef P_REGMATCH_H
#define P_REGMATCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "p_h.h"
#include <regex.h>

#define regmatch    p_regmatch

/**
 *  \brief Test a string against a POSIX regex pattern.
 *  \param str The string.
 *  \param pattern The pattern.
 *  \param flags Regex compilation flags (REG_NOSUB will be included).
 *  \param err Pointer for error message (to free).
 *  \return 1 if string matches pattern, 0 if not, or -1 on error.
 */
P_EXPORT int
p_regmatch( const char* str,
        const char* pattern,
        int flags,
        char** err );

#ifdef __cplusplus
}
#endif

#endif /* P_REGMATCH_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
