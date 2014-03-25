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
 *  \file p_llabs.h
 *  \brief llabs
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 *  \see http://www.opensource.apple.com/source/Libc/Libc-825.40.1/stdlib/FreeBSD/llabs.c
 */

#ifndef P_LLABS_H
#define P_LLABS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#if !(_XOPEN_SOURCE >= 600 || _ISOC99_SOURCE || _POSIX_C_SOURCE >= 200112L)
#define llabs   p_llabs

/**
 *  \brief llabs for dummies.
 */
long long
p_llabs( long long j );

#else
#define p_llabs llabs
#endif

#ifdef __cplusplus
}
#endif

#endif /* P_LLABS_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
