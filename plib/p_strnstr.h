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
 *  \file p_strnstr.h
 *  \brief function strnstr().
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 */

#ifndef P_STRNSTR_H
#define P_STRNSTR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "p_h.h"

#define strnstr     p_strnstr

P_EXPORT char*
p_strnstr( const char* haystack,
        const char* needle,
        size_t n );

#ifdef __cplusplus
}
#endif

#endif /* P_STRNSTR_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
