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
 *  \file p_dllist.h
 *  \brief Macros for a generic doubly-linked list.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 */

#ifndef P_DLLIST_H
#define P_DLLIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "p_h.h"

/**
 *  \def p_dllist_push( lst, p )
 *  \brief Push an element to the doubly-linked list.
 *  \param lst First element (handle) of the list (can be NULL).
 *  \param p An element to push.
 *  \note This is minimal: p->prev will not be set to NULL.
 *
 *  Example:
 *  \code
 *  ...
 *  p->prev = NULL;
 *  p_dllist_push( lst, p );
 *  ...
 *  \endcode
 */
#define p_dllist_push( lst, p ) \
        do { if ( lst ) lst->prev = p; p->next = lst; lst = p; } while (0)

/**
 *  \def p_dllist_pull( lst, p )
 *  \brief Pull an element from the doubly-linked list.
 *  \param lst First element (handle) of the list (not NULL).
 *  \param p The element pulled.
 *  \note This is minimal: p->next will not be set to NULL.
 *
 *  Example:
 *  \code
 *  ...
 *  p_dllist_pull( lst, p );
 *  p->next = NULL;
 *  ...
 *  \endcode
 */
#define p_dllist_pull( lst, p ) \
        do { p = lst; if ( p->next ) { \
             lst = p->next; lst->prev = NULL; } \
             else lst = NULL; } while (0)

/**
 *  \def p_dllist_take( lst, p )
 *  \brief Take an element from the doubly-linked list.
 *  \param lst First element (handle) of the list (not NULL).
 *  \param p An element to take.
 *  \note This is minimal: p->prev and p->next will not be set to NULL.
 *
 *  Example:
 *  \code
 *  ...
 *  p_dllist_take( lst, p );
 *  p->next = p->prev = NULL;
 *  ...
 *  \endcode
 */
#define p_dllist_take( lst, p ) \
        do { if ( p->prev || p->next ) { \
             if ( p->prev ) p->prev->next = p->next; \
             if ( p->next ) p->next->prev = p->prev; } \
             else lst = NULL; } while (0)

#ifdef __cplusplus
}
#endif

#endif /* P_DLLIST_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
