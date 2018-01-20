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
 *  \file m_sllist.h
 *  \brief Functions and macros for a singly-linked list.
 *  \author Stanislas Marquis <stan@astrorigin.com>
 *
 *  Each list node must be a struct with a first member called 'next'.
 */

#ifndef M_SLLIST_H
#define M_SLLIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "m_h.h"

/**
 *  \typedef m_SLList
 */
typedef struct _m_SLList m_SLList;

/**
 *  \struct _m_SLList
 *  \brief Dummy prototype for singly-linked list elements.
 */
struct _m_SLList
{
  m_SLList* next;
};

/**
 *  \brief Pull an element from a singly-linked list.
 *  \param lst The first element (handle) of the list (can be NULL).
 *  \return Element pulled, or NULL.
 *
 *  If in a hurry you could as well:
 *  \code
 *  do {el=lst; lst=lst->next;} while(0);
 *  \endcode
 */
M_DLLAPI m_SLList*
m_SLList_pull(m_SLList** const lst);

#define m_SLList_PULL(lst) m_SLList_pull((m_SLList**)lst)

/**
 *  \brief Push an element into the singly-linked list.
 *  \param lst The first element (handle) of the list (can be NULL).
 *  \param el An element to push.
 *  \return M_FALSE if arguments are invalid.
 *
 *  In a hurry you could as well:
 *  \code
 *  do {el->next=lst; lst=el;} while(0);
 *  \endcode
 */
M_DLLAPI M_BOOL
m_SLList_push(m_SLList** const lst,
        m_SLList* const el);

#define m_SLList_PUSH(lst, el) m_SLList_push((m_SLList**)lst, (m_SLList*)el)

/**
 *  \brief Take an element of the singly-linked list.
 *  \param lst The first element (handle) of the list (not NULL).
 *  \param el The element to take.
 *  \param prev The previous element (or NULL).
 *  \return M_FALSE if arguments are invalid.
 *
 *  In a hurry you could as well:
 *  \code
 *  do {if (prev) prev->next=el->next; else lst=el->next;} while(0)
 *  \endcode
 */
M_DLLAPI M_BOOL
m_SLList_take(m_SLList** const lst,
        m_SLList* const el,
        m_SLList* const prev);

#define m_SLList_TAKE(lst, el, prev) \
        m_SLList_take((m_SLList**)lst, (m_SLList*)el, (m_SLList*)prev)

/**
 *  \brief Count elements in any list complying with a m_SLList type.
 *  \param lst The list.
 *  \return The count. Or (-1) which probably means error.
 */
M_DLLAPI M_SZ
m_SLList_count(const m_SLList* lst);

#define m_SLList_COUNT(lst) m_SLList_count((m_SLList*)lst)

/**
 *  \brief Append an element to the list.
 *  \param lst The list first element (handle).
 *  \param el The element to append.
 *  \return Index of element inside the list. Or (-1) which probably means error.
 */
M_DLLAPI M_SZ
m_SLList_append(m_SLList** const lst,
        const m_SLList* const el);

#define m_SLList_APPEND(lst, el) m_SLList_append((m_SLList**)lst, (m_SLList*)el)

/**
 *  \brief Get the nth element of the list.
 *  \param lst The list first element.
 *  \param index Index searched.
 *  \return Element found or NULL.
 */
M_DLLAPI m_SLList*
m_SLList_at(const m_SLList* lst,
        const M_SZ index);

#define m_SLList_AT(lst, idx) m_SLList_at((m_SLList*)lst, idx)

#ifdef __cplusplus
}
#endif
#endif /* !M_SLLIST_H */
/* vim: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
