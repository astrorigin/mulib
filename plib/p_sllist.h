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
 *  \file p_sllist.h
 *  \brief Macros for a generic singly-linked list.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch> 
 *  
 *  Each node must be a struct with a member called 'next'.
 */

#ifndef P_SLLIST_H
#define P_SLLIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "p_h.h"

/**
 *  \def p_sllist_pull( lst, p )
 *  \brief Pull an element from a singly-linked list.
 *  \param lst The first element (handle) of the list (not NULL).
 *  \param p A pointer for the element pulled.
 *  \note This is minimal: p->next will not be set to NULL.
 *
 *  Example:
 *  \code
 *  ...
 *  p_sllist_pull( lst, p );
 *  p->next = NULL;
 *  ...
 *  \endcode
 */
#define p_sllist_pull( lst, p ) \
        do { p = lst; lst = lst->next; } while (0)

/**
 *  \def p_sllist_push( lst, p )
 *  \brief Push an element into the singly-linked list.
 *  \param lst The first element (handle) of the list (can be NULL).
 *  \param p An element to push.
 */
#define p_sllist_push( lst, p ) \
        do { p->next = lst; lst = p; } while (0)

/**
 *  \def p_sllist_take( lst, p, prev )
 *  \brief Take an element of the singly-linked list.
 *  \param lst The first element (handle) of the list (not NULL).
 *  \param p The element to take.
 *  \param prev The previous element (or NULL).
 *  \note This is minimal: p->next will not be set to NULL.
 *
 *  Example:
 *  \code
 *  ...
 *  p_sllist_take( lst, p, prev );
 *  p->next = NULL;
 *  ...
 *  \endcode
 */
#define p_sllist_take( lst, p, prev ) \
        do { if ( prev ) prev->next = p->next; else lst = p->next; } while (0)

/**
 *  \typedef p_SlList
 */
typedef struct _p_sllist_t p_SlList;

/**
 *  \struct _p_sllist_t
 *  \brief Dummy prototype for singly-linked list elements.
 */
struct _p_sllist_t
{
    p_SlList* next;
};

/**
 *  \brief Count elements in any list complying with a p_SlList type.
 *  \param el The list.
 *  \return The count.
 */
P_EXPORT P_SZ
p_sllist_count( p_SlList* el );

/**
 *  \brief Append an element to the list.
 *  \param lst The list first element.
 *  \param el The element to append.
 *  \return Index of element inside the list.
 */
P_EXPORT P_SZ
p_sllist_append( p_SlList** lst,
        const p_SlList* el );

/**
 *  \brief Get the nth element of the list.
 *  \param lst The list first element.
 *  \param index Index searched.
 *  \return Element found or NULL.
 */
P_EXPORT p_SlList*
p_sllist_at( const p_SlList* lst,
        const P_SZ index );

#ifdef __cplusplus
}
#endif

#endif /* P_SLLIST_H */
/* vim: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
