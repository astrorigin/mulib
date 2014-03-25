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
 *  \file p_memlist.h
 *  \brief Basic memory checks.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 *
 *  This module provides substitutes for malloc, realloc and free,
 *  and keeps track of the amount of allocated memory by maintaining
 *  a singly-linked list of information (thread-safely).
 *
 *  That is completely disabled when compiled with NDEBUG.
 */

#ifndef P_MEMLIST_H
#define P_MEMLIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "p_h.h"

#ifndef NDEBUG

#include "p_mutex.h"

#define _P_MALLOC   p_memlist_malloc
#define _P_REALLOC  p_memlist_realloc
#define _P_FREE     p_memlist_free

#define _P_MALLOC_REF   &p_memlist_malloc
#define _P_REALLOC_REF  &p_memlist_realloc
#define _P_FREE_REF     &p_memlist_free

/**
 *  \brief Acquire the global lock.
 */
#define P_MEMLIST_LOCK() \
        P_MUTEX_LOCK( _p_memlist_mutex )

/**
 *  \brief Release the global lock.
 */
#define P_MEMLIST_UNLOCK() \
        P_MUTEX_UNLOCK( _p_memlist_mutex )

/**
 *  \brief A global mutex (private).
 */
extern P_MUTEX
_p_memlist_mutex;

/**
 *  \typedef p_MemListNode
 */
typedef struct _p_memlist_node_t p_MemListNode;

/**
 *  \struct _p_memlist_node_t
 *  \brief A node with information about allocated memory.
 */
struct _p_memlist_node_t
{
    p_MemListNode*  next;
    P_SZ            sz;
    P_PTR           data[];
};

/**
 *  \brief The list of nodes (private).
 */
extern p_MemListNode*
_p_memlist_nodes;

/**
 *  \brief Malloc replacement.
 */
P_EXPORT P_PTR
p_memlist_malloc( const P_SZ );

/**
 *  \brief Free replacement.
 */
P_EXPORT P_VOID
p_memlist_free( P_PTR );

/**
 *  \brief Realloc replacement.
 */
P_EXPORT P_PTR
p_memlist_realloc( P_PTR,
        const P_SZ );

/**
 *  \brief Print some stats to stdout.
 */
P_EXPORT P_VOID
p_memlist_debug( P_VOID );

#else /* Release mode */

#define _P_MALLOC   malloc
#define _P_REALLOC  realloc
#define _P_FREE     free

#define _P_MALLOC_REF   &malloc
#define _P_REALLOC_REF  &realloc
#define _P_FREE_REF     &free

#endif /* !NDEBUG */

#ifdef __cplusplus
}
#endif

#endif /* P_MEMLIST_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
