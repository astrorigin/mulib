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

#include "m_string.h"

#include "m_mempool.h"

#undef M_TRACE
#if defined(M_TRACE_MODE) && defined(M_TRACE_STRING)
#define M_TRACE(msg, ...) _M_TRACER("-- String -- "msg, __VA_ARGS__)
#else
#define M_TRACE(moo, ...)
#endif

/**
 *  Function to get the size of a string buffer in relation to
 *  its expected length (not including final null).
 *
 *  \see M_STRING_QUANTA
 */
M_SZ
_m_String_calc_space_fn(const m_String* const s,
        const M_SZ len)
{
  M_UNUSED(s);
  return M_STRING_QUANTA * ((len / M_STRING_QUANTA) + 1);
}

/**
 *  Function to copy a string into internal buffer.
 */
M_CHAR*
_m_String_set_data(m_String* const s,
        const M_CHAR* const content,
        const M_SZ len)
{
  memset(s->data, 0, s->capacity);
  if (len == 0 || !content || content[0] == '\0')
  {
    s->len = 1;
  }
  else
  {
    strncpy(s->data, content, len);
    s->len = len + 1;
  }
  return s->data;
}

M_CHAR*
m_String_new(m_String** const s,
        const M_CHAR* const content)
{
  return m_String_new_len(s, content, content ? strlen(content) : 0);
}

M_CHAR*
m_String_new_len(m_String** const s,
        const M_CHAR* const content,
        const M_SZ len)
{
  assert(s);
  if (!s) return NULL;

  if (!m_Array_new((m_Array**)s, len + 1, sizeof(M_CHAR),
      (m_array_calc_space_fn_t) &_m_String_calc_space_fn))
  {
    return NULL;
  }
  return _m_String_set_data(*s, content, len);
}

M_VOID
m_String_delete(m_String** const s)
{
  m_Array_delete((m_Array**)s, NULL);
}

M_CHAR*
m_String_init(m_String* const s,
        const M_CHAR* const content)
{
  return m_String_init_len(s, content, content ? strlen(content) : 0);
}

M_CHAR*
m_String_init_len(m_String* const s,
        const M_CHAR* const content,
        const M_SZ len)
{
  assert(s);
  if (!s) return NULL;

  if (!m_Array_init((m_Array*)s, len + 1, sizeof(M_CHAR),
      (m_array_calc_space_fn_t) &_m_String_calc_space_fn))
  {
    return NULL;
  }
  return _m_String_set_data(s, content, len);
}

M_VOID
m_String_fini(m_String* const s)
{
  m_Array_fini((m_Array*)s, NULL);
}

M_CHAR*
m_String_set(m_String* const s,
        const M_CHAR* const content)
{
  return m_String_set_len(s, content, content ? strlen(content) : 0);
}

M_CHAR*
m_String_set_len(m_String* const s,
        const M_CHAR* const content,
        const M_SZ len)
{
  assert(s);
  if (!s) return NULL;

  if (!m_Array_adapt((m_Array*)s, len + 1)) return NULL;
  return _m_String_set_data(s, content, len);
}

M_CHAR*
m_String_copy(m_String* const s,
        const m_String* const s2)
{
  return m_String_set_len(s, (M_CHAR*)s2->data, s2->len - 1);
}

m_String*
m_String_dup(const m_String* const s)
{
  m_String* dup;

  assert(s);
  if (!s) return NULL;

  return m_String_new_len(&dup, s->data, s->len - 1) ? dup : NULL;
}

M_CHAR*
m_String_cat(m_String* const s,
        const M_CHAR* const content)
{
  return m_String_cat_len(s, content, content ? strlen(content) : 0);
}

M_CHAR*
m_String_cat_len(m_String* const s,
        const M_CHAR* const content,
        const M_SZ len)
{
  M_SZ sz;

  assert(s);
  assert(s->len > 0);
  if (!s) return NULL;

  if (len == 0 || !content || content[0] == '\0') return s->data;

  sz = s->capacity;
  if (!m_Array_reserve((m_Array*)s, s->len + len)) return NULL;
  if (sz != s->capacity)
    memset((char*)s->data + s->len, 0, s->capacity - s->len);
  strncpy((char*)s->data + s->len - 1, content, len);
  s->len += len;

  return s->data;
}

M_CHAR*
m_String_replace(m_String* const s,
        const M_CHAR* const titi,
        const M_CHAR* const toto)
{
  M_CHAR* p, *b, *d;
  M_SZ sz, cnt = 0;
  M_SZ len1, len2;

  assert(s);
  assert(titi);
  assert(toto);

  len1 = strlen(titi);
  if (m_String_LEN(s) == 0 || len1 == 0 || m_String_LEN(s) < len1)
    return s->data;

  p = strstr(s->data, titi);
  if (!p) return s->data;
  len2 = strlen(toto);

  if (len1 == len2)
  {
    do
    {
      memcpy(p, toto, len1);
      p += len1;
      p = strstr(p, titi);
    }
    while (p);
  }
  else
  if (len1 > len2)
  {
    b = p;
    do
    {
      memcpy(b, toto, len2);
      p += len1;
      b += len2;
      d = p;
      cnt++;
      if ((p = strstr(p, titi)))
      {
        memcpy(b, d, p - d);
        b += p - d;
      }
    }
    while (p);
    strcpy(b, d);
    sz = cnt * (len1 - len2);
    memset(strchr(b, '\0') + 1, 0, sz);
    s->len -= sz;
    if (!m_Array_reserve((m_Array*)s, s->len)) return NULL;
  }
  else
  {
    M_CHAR* buf;
    M_SZ req;
    /* count occurrences */
    do
    {
      cnt++;
      p += len1;
      p = strstr(p, titi);
    }
    while (p);
    /* prepare buffer */
    sz = s->len + ((len2 - len1) * cnt);
    req = s->calc_space_fn ? (*s->calc_space_fn)(s, sz) : sz;
    buf = b = M_MALLOC(req);
    if (!buf) return NULL;
    memset(buf, 0, req);
    /* read and rewrite */
    d = p = s->data;
    while ((p = strstr(p, titi)))
    {
      memcpy(b, d, p - d);
      b += p - d;
      memcpy(b, toto, len2);
      b += len2;
      p += len1;
      d = p;
    }
    strcpy(b, d);
    /* switch data */
    p = s->data;
    s->data = buf;
    M_FREE(p, s->capacity);
    s->len += cnt * (len2 - len1);
    s->capacity = req;
  }
  return s->data;
}

#ifndef NDEBUG

M_VOID
m_String_debug(const m_String* const s)
{
  assert(s);
  printf("-- String -- debug ("M_PTR_FMT"):\n"
         "--     data = ("M_STR_FMT")\n"
         "--     len = "M_SZ_FMT"\n"
         "--     unit = "M_SZ_FMT"\n"
         "--     capacity = "M_SZ_FMT"\n"
         "--     calc_space_fn = ("M_PTR_FMT")\n"
         "-- end string debug\n",
      s, (char*)s->data, s->len, s->unit, s->capacity, s->calc_space_fn);
}

#endif /* !NDEBUG */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
