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

#include "m_array.h"

#include "m_mempool.h"

#undef M_TRACE
#if defined(M_TRACE_MODE) && defined(M_TRACE_ARRAY)
#define M_TRACE(msg, ...) _M_TRACER("-- Array -- "msg, __VA_ARGS__)
#else
#define M_TRACE(moo, ...)
#endif

M_BOOL
m_Array_new(m_Array** const arr,
    const M_SZ len,
    const M_SZ unit,
    const m_array_calc_space_fn_t csfn)
{
  assert(arr && !*arr);
  assert(unit);
  M_TRACE("new ("M_PTR_FMT") len ("M_SZ_FMT") unit ("M_SZ_FMT")",
      arr, len, unit);
  if (!arr || *arr || !unit) return M_FALSE;

  *arr = M_MALLOC(sizeof(m_Array));
  assert(*arr);
  if (!*arr) return M_FALSE;
  return m_Array_init(*arr, len, unit, csfn);
}

M_VOID
m_Array_delete(m_Array** const arr,
        const m_array_finalize_fn_t fn)
{
  assert(arr && *arr);
  M_TRACE("delete ("M_PTR_FMT") finalize_fn ("M_PTR_FMT")", *arr, fn);
  if (!arr || !*arr) return;

  m_Array_fini(*arr, fn);
  M_FREE(*arr, sizeof(m_Array));
  *arr = NULL;
}

M_BOOL
m_Array_init(m_Array* const arr,
        const M_SZ len,
        const M_SZ unit,
        const m_array_calc_space_fn_t csfn)
{
  M_SZ space_req;

  assert(arr);
  assert(unit);
  M_TRACE("init ("M_PTR_FMT") len ("M_SZ_FMT") unit ("M_SZ_FMT")",
      arr, len, unit);
  if (!arr || !unit) return M_FALSE;

  arr->data = NULL;
  arr->len = 0;
  arr->unit = unit;
  arr->capacity = 0;
  arr->calc_space_fn = csfn;

  space_req = csfn ? (*csfn)(arr, len) : len * unit;
  assert(space_req >= len * unit);
  if (space_req)
  {
    arr->data = M_MALLOC(space_req);
    assert(arr->data);
    if (!arr->data) return M_FALSE;
    arr->capacity = space_req;
  }
  return M_TRUE;
}

M_VOID
m_Array_fini(m_Array* const arr,
        const m_array_finalize_fn_t fn)
{
  assert(arr);
  M_TRACE("fini ("M_PTR_FMT") finalize_fn ("M_PTR_FMT")", arr, fn);
  if (!arr) return;

  if (arr->data)
  {
    assert(arr->capacity);
    if (fn) m_Array_traverse(arr, fn);
    M_FREE(arr->data, arr->capacity);
    arr->data = NULL;
  }
  arr->len = 0;
  arr->unit = 0;
  arr->capacity = 0;
  arr->calc_space_fn = NULL;
}

M_BOOL
_m_Array_set_capacity(m_Array* const arr,
        const M_SZ len)
{
  const M_SZ space_req = arr->calc_space_fn ?
      (*arr->calc_space_fn)(arr, len) : len * arr->unit;
  assert(space_req >= len * arr->unit);
  if (arr->capacity != space_req)
  {
    if (space_req)
    {
      arr->data = arr->data ? M_REALLOC(arr->data, space_req, arr->capacity)
          : M_MALLOC(space_req);
      assert(arr->data);
      if (!arr->data) return M_FALSE;
    }
    else
    {
      assert(arr->data);
      M_FREE(arr->data, arr->capacity);
      arr->data = NULL;
    }
    arr->capacity = space_req;
  }
  return M_TRUE;
}

M_BOOL
m_Array_reserve(m_Array* const arr,
        const M_SZ len)
{
  assert(arr);
  assert(len >= arr->len);
  M_TRACE("reserve ("M_PTR_FMT") len ("M_SZ_FMT")", arr, len);
  if (!arr || len < arr->len) return M_FALSE;

  return _m_Array_set_capacity(arr, len);
}

M_BOOL
m_Array_adapt(m_Array* const arr,
        const M_SZ len)
{
  assert(arr);
  M_TRACE("adapt ("M_PTR_FMT") len ("M_SZ_FMT")", arr, len);
  if (!arr) return M_FALSE;

  return _m_Array_set_capacity(arr, len);
}

M_PTR
m_Array_reserve_one(m_Array* const arr)
{
  assert(arr);
  M_TRACE("reserve_one ("M_PTR_FMT")", arr);
  if (!arr) return NULL;

  if (!m_Array_reserve(arr, arr->len + 1)) return NULL;
  assert(arr->len * arr->unit < arr->capacity);
  return (char*)arr->data + (arr->len * arr->unit);
}

M_BOOL
m_Array_empty(m_Array* const arr,
        const m_array_finalize_fn_t fn)
{
  assert(arr);
  M_TRACE("empty ("M_PTR_FMT") finalize_fn ("M_PTR_FMT")", arr, fn);
  if (!arr) return M_FALSE;

  if (arr->len && fn) m_Array_traverse(arr, fn);
  arr->len = 0;
  if (!m_Array_adapt(arr, 0)) return M_FALSE;
  return M_TRUE;
}

M_PTR
m_Array_get(const m_Array* const arr,
        const M_ID index)
{
  assert(arr);
  M_TRACE("get ("M_PTR_FMT") index ("M_ID_FMT")", arr, index);
  if (!arr) return NULL;

  return index >= arr->len ? NULL : (char*)arr->data + (index * arr->unit);
}

M_PTR
m_Array_get_reserved(const m_Array* const arr,
        const M_ID index)
{
  M_SZ max;

  assert(arr);
  M_TRACE("get_reserved ("M_PTR_FMT") index ("M_ID_FMT")", arr, index);
  if (!arr) return NULL;

  max = arr->capacity / arr->unit;
  return index >= max ? NULL : (char*)arr->data + (index * arr->unit);
}

M_PTR
m_Array_set(m_Array* const arr,
        const M_ID index,
        const M_PTR const ptr,
        const m_array_copy_fn_t cpyfunc)
{
  M_PTR dest;

  assert(arr);
  assert(ptr);
  M_TRACE("set ("M_PTR_FMT") index ("M_ID_FMT") ptr ("M_PTR_FMT")"
      " cpyfunc ("M_PTR_FMT")", arr, index, ptr, cpyfunc);
  if (!arr || !ptr) return NULL;

  if (index >= arr->len) return NULL;
  dest = (char*)arr->data + (index * arr->unit);
  if (dest == ptr) return dest;
  if (cpyfunc)
    (*cpyfunc)(dest, ptr);
  else /* shallow copy */
    memcpy(dest, ptr, arr->unit);
  return dest;
}

M_BOOL
m_Array_unset(m_Array* const arr,
        const M_ID index,
        const m_array_finalize_fn_t fn)
{
  M_PTR dest;

  assert(arr);
  M_TRACE("unset ("M_PTR_FMT") index ("M_ID_FMT") finalize_fn ("M_PTR_FMT")",
      arr, index, fn);
  if (!arr) return M_FALSE;

  if (index >= arr->len) return M_FALSE;
  dest = (char*)arr->data + (index * arr->unit);
  if (fn) (*fn)(dest);
  if (index != arr->len - 1)
    memmove(dest, (char*)dest + arr->unit, (arr->len - index - 1) * arr->unit);
  arr->len -= 1;
  if (!m_Array_reserve(arr, arr->len)) return M_FALSE;
  return M_TRUE;
}

M_BOOL
m_Array_append(m_Array* const arr,
        const M_PTR const ptr,
        const M_SZ len,
        const m_array_copy_fn_t cpyfunc)
{
  assert(arr);
  assert(ptr);
  M_TRACE("append ("M_PTR_FMT") ptr ("M_PTR_FMT") len ("M_SZ_FMT")"
      " cpyfunc ("M_PTR_FMT")", arr, ptr, len, cpyfunc);
  if (!arr || !ptr) return M_FALSE;

  if (len == 0) return M_TRUE;
  if (!m_Array_reserve(arr, arr->len + len)) return M_FALSE;

  if (cpyfunc)
  {
    M_SZ ii, i = 0;
    const M_PTR dest = (char*)arr->data + (arr->len * arr->unit);
    for (; i < len; ++i)
    {
      ii = i * arr->unit;
      (*cpyfunc)((char*)dest + ii, (char*)ptr + ii);
    }
  }
  else /* shallow copy */
    memcpy((char*)arr->data + (arr->len * arr->unit), ptr, len * arr->unit);

  arr->len += len;
  assert(arr->len * arr->unit <= arr->capacity);
  return M_TRUE;
}

M_BOOL
m_Array_prepend(m_Array* const arr,
        const M_PTR const ptr,
        const M_SZ len,
        const m_array_copy_fn_t cpyfunc)
{
  assert(arr);
  assert(ptr);
  M_TRACE("prepend ("M_PTR_FMT") ptr ("M_PTR_FMT") len ("M_SZ_FMT")"
      " cpyfunc ("M_PTR_FMT")", arr, ptr, len, cpyfunc);
  if (!arr || !ptr) return M_FALSE;

  if (len == 0) return M_TRUE;
  if (!m_Array_reserve(arr, arr->len + len)) return M_FALSE;

  memmove((char*)arr->data + (len * arr->unit), arr->data, arr->len * arr->unit);

  if (cpyfunc)
  {
    M_SZ ii, i = 0;
    const M_PTR dest = (char*)arr->data;
    for (; i < len; ++i)
    {
      ii = i * arr->unit;
      (*cpyfunc)((char*)dest + ii, (char*)ptr + ii);
    }
  }
  else /* shallow copy */
    memcpy(arr->data, ptr, len * arr->unit);

  arr->len += len;
  assert(arr->len * arr->unit <= arr->capacity);
  return M_TRUE;
}

M_BOOL
m_Array_insert(m_Array* const arr,
        const M_PTR const ptr,
        const M_SZ len,
        const M_ID index,
        const m_array_copy_fn_t cpyfunc)
{
  M_PTR p;

  assert(arr);
  assert(ptr);
  M_TRACE("insert ("M_PTR_FMT") ptr ("M_PTR_FMT") len ("M_SZ_FMT")"
      " index ("M_ID_FMT") cpyfunc ("M_PTR_FMT")", arr, ptr, len, index, cpyfunc);
  if (!arr || !ptr) return M_FALSE;

  if (index >= arr->len) return M_FALSE;
  if (len == 0) return M_TRUE;
  if (!m_Array_reserve(arr, arr->len + len)) return M_FALSE;

  p = (char*)arr->data + (index * arr->unit);
  memmove((char*)arr->data + ((arr->len - len) * arr->unit), p,
      (arr->len - index) * arr->unit);

  if (cpyfunc)
  {
    M_SZ ii, i = 0;
    for (; i < len; ++i)
    {
      ii = i * arr->unit;
      (*cpyfunc)((char*)p + ii, (char*)ptr + ii);
    }
  }
  else /* shallow copy */
    memcpy(p, ptr, len * arr->unit);

  arr->len += len;
  assert(arr->len * arr->unit <= arr->capacity);
  return M_TRUE;
}

M_VOID
m_Array_traverse(m_Array* const arr,
        M_VOID (* const traverse_fn)(M_PTR))
{
  M_SZ i, vlen;

  assert(arr);
  assert(traverse_fn);
  M_TRACE("traverse ("M_PTR_FMT") func ("M_PTR_FMT")", arr, traverse_fn);
  if (!arr || !traverse_fn) return;

  vlen = arr->len;
  for (i = 0; i < vlen; ++i)
    (*traverse_fn)((char*)arr->data + (i * arr->unit));
}

M_VOID
m_Array_traverse2(m_Array* const arr,
        M_VOID (* const traverse_fn)(M_PTR, M_PTR),
        M_PTR userdata)
{
  M_SZ i, vlen;

  assert(arr);
  assert(traverse_fn);
  M_TRACE("traverse2 ("M_PTR_FMT") func ("M_PTR_FMT") udata ("M_PTR_FMT")",
      arr, traverse_fn, userdata);
  if (!arr || !traverse_fn) return;

  vlen = arr->len;
  for (i = 0; i < vlen; ++i)
    (*traverse_fn)((char*)arr->data + (i * arr->unit), userdata);
}

#ifndef NDEBUG

M_VOID
m_Array_debug(const m_Array* const arr)
{
  assert(arr);
  printf("-- Array -- debug ("M_PTR_FMT"):\n"
         "--     data = ("M_PTR_FMT")\n"
         "--     len = "M_SZ_FMT"\n"
         "--     unit = "M_SZ_FMT"\n"
         "--     capacity = "M_SZ_FMT"\n"
         "--     calc_space_fn = ("M_PTR_FMT")\n"
         "-- end array debug\n",
      arr, arr->data, arr->len, arr->unit, arr->capacity, arr->calc_space_fn);
}

#endif /* !NDEBUG */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
