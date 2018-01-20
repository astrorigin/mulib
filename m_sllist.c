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

#include "m_sllist.h"

#undef M_TRACE
#if defined(M_TRACE_MODE) && defined(M_TRACE_SLLIST)
#define M_TRACE(msg, ...) _M_TRACER("-- SLList -- "msg, __VA_ARGS__)
#else
#define M_TRACE(moo, ...)
#endif

m_SLList*
m_SLList_pull(m_SLList** const lst)
{
  m_SLList* el;

  assert(lst && *lst);
  if (!lst || !*lst) return NULL;

  el = *lst;
  *lst = (*lst)->next ? (*lst)->next : NULL;
  el->next = NULL;
  return el;
}

M_BOOL
m_SLList_push(m_SLList** const lst,
        m_SLList* const el)
{
  assert(lst);
  assert(el);
  if (!lst || !el) return M_FALSE;

  el->next = *lst;
  *lst = el;
  return M_TRUE;
}

M_BOOL
m_SLList_take(m_SLList** const lst,
        m_SLList* const el,
        m_SLList* const prev)
{
  assert(lst && *lst);
  assert(el);
  assert(!prev && (el == *lst));
  if (!lst || !*lst || !el) return M_FALSE;

  if (prev) prev->next = el->next;
  else *lst = el->next;
  el->next = NULL;
  return M_TRUE;
}

M_SZ
m_SLList_count(const m_SLList* lst)
{
  M_SZ i = 0;

  assert(lst);
  if (!lst) return 0;

  for (; lst; lst = lst->next)
    if (++i >= (M_SZ)-1) return -1;

  return i;
}

M_SZ
m_SLList_append(m_SLList** const lst,
        const m_SLList* const el)
{
  M_SZ i = 0;
  m_SLList* p;

  assert(lst);
  assert(el);
  if (!lst || !el) return -1;

  if (!*lst)
  {
    *lst = (m_SLList*) el;
    return 0;
  }
  p = *lst;
  for (; p->next; p = p->next)
    if (++i >= (M_SZ)-1) return -1;

  p->next = (m_SLList*) el;
  return ++i;
}

m_SLList*
m_SLList_at(const m_SLList* lst,
        const M_SZ index)
{
  M_SZ cnt = 0;

  assert(lst);

  for (; lst; lst = lst->next)
    if (cnt++ == index) return (m_SLList*) lst;

  return NULL;
}

/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
