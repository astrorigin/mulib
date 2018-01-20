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
 *  \file m_array.h
 *  \brief Dynamic array.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <stan@astrorigin.com>
 */

#ifndef M_ARRAY_H
#define M_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "m_h.h"

/**
 *  \typedef m_Array
 */
typedef struct _m_Array m_Array;

/**
 *  \brief Function type for getting the size of an array's data buffer.
 *
 *  This function must return a multiple of arr->unit, and a sufficient
 *  number for num_elem elements.
 */
typedef M_SZ (*m_array_calc_space_fn_t)(const m_Array* self, const M_SZ num_elem);

/**
 *  \brief Function type to finalize an array element.
 */
typedef M_VOID (*m_array_finalize_fn_t)(M_PTR elem);

/**
 *  \brief Function type to handle operations on elements when set or replaced.
 */
typedef M_VOID (*m_array_copy_fn_t)(M_PTR dest_elem, const M_PTR src_elem);

/**
 *  \struct _m_Array
 */
struct _m_Array
{
  M_PTR data; /* start of array data buffer (can be NULL when len=0) */
  M_SZ len; /* total of elements in array */
  M_SZ unit; /* size of one element (not 0) */
  M_SZ capacity; /* size of data buffer (if 0, data is NULL) */
  /* function to get size of data buffer (can be NULL) */
  m_array_calc_space_fn_t calc_space_fn;
};

/**
 *  \brief Allocate for an array.
 *  \param arr The array (by ref, initialized to NULL).
 *  \param len Prepare space for a number of elements.
 *  \param unit Size of one element (not 0).
 *  \param calc_space_fn Allocation strategy function (or NULL).
 *  \return M_TRUE, or M_FALSE on input or memory error.
 */
M_DLLAPI M_BOOL
m_Array_new(m_Array** const arr,
        const M_SZ len,
        const M_SZ unit,
        const m_array_calc_space_fn_t calc_space_fn);

/**
 *  \brief Delete an array (and its content).
 *  \param arr The array (by ref, not NULL).
 *  \param finalize_fn Finalization function for elements, or NULL.
 */
M_DLLAPI M_VOID
m_Array_delete(m_Array** const arr,
        const m_array_finalize_fn_t finalize_fn);

/**
 *  \brief Initialize an array.
 *  \param arr The array (not NULL).
 *  \param len Prepare space for a number of elements.
 *  \param unit Size of one element (not 0).
 *  \param calc_space_fn Allocation strategy function (or NULL).
 *  \return M_TRUE, or M_FALSE on input or memory error.
 */
M_DLLAPI M_BOOL
m_Array_init(m_Array* const arr,
        const M_SZ len,
        const M_SZ unit,
        const m_array_calc_space_fn_t calc_space_fn);

/**
 *  \brief Finalize an array.
 *  \param arr The array (not NULL).
 *  \param finalize_fn Finalization function for elements, or NULL.
 */
M_DLLAPI M_VOID
m_Array_fini(m_Array* const arr,
        const m_array_finalize_fn_t finalize_fn);

/**
 *  \brief Reserve space for a certain number of elements.
 *  \param arr The array (not NULL).
 *  \param len Number of elements to expect (not less than array current length).
 *  \return M_TRUE, or M_FALSE on input or memory error.
 *
 *  The intended use of this function is to prepare space for more
 *  elements than the array actually has. (And after that, dabble with
 *  that space and set arr->len accordingly, at your own risks.)
 *
 *  This function does nothing but returns M_FALSE if the len argument
 *  is lesser than the array current length.
 */
M_DLLAPI M_BOOL
m_Array_reserve(m_Array* const arr,
        const M_SZ len);

/**
 *  \brief Adapt array capacity for a certain number of elements.
 *  \param arr The array (not NULL).
 *  \param len Number of elements to expect.
 *  \return M_TRUE, or M_FALSE on input or memory error.
 *
 *  This is the same as m_Array_reserve, just you are now able to screw
 *  yourself by dropping some data with a len argument that is lesser
 *  than the array current length.
 */
M_DLLAPI M_BOOL
m_Array_adapt(m_Array* const arr,
        const M_SZ len);

/**
 *  \brief Reserve some space for one more element and get its address.
 *  \param arr The array (not NULL).
 *  \return NULL on error, else address of space for one more element.
 *  \note You must increment the length of the array after use of space allocated.
 */
M_DLLAPI M_PTR
m_Array_reserve_one(m_Array* const arr);

/**
 *  \brief Remove all elements in the array.
 *  \param arr The array (not NULL).
 *  \param finalize_fn Finalization function for elements, or NULL.
 *  \return M_TRUE, or M_FALSE on input or memory error.
 */
M_DLLAPI M_BOOL
m_Array_empty(m_Array* const arr,
        const m_array_finalize_fn_t finalize_fn);

/**
 *  \brief Get an element address in the array.
 *  \param arr The array (not NULL).
 *  \param index Index of element.
 *  \return NULL if index is out of range, else address of element found.
 */
M_DLLAPI M_PTR
m_Array_get(const m_Array* const arr,
        const M_ID index);

/**
 *  \brief Get an element address in the array, possibly out of existing range.
 *  \param arr The array (not NULL).
 *  \param index Index of element.
 *  \return NULL if index is out of array capacity, else address of element found.
 */
M_DLLAPI M_PTR
m_Array_get_reserved(const m_Array* const arr,
        const M_ID index);

/**
 *  \brief Set an element in the array.
 *  \param arr The array (not NULL).
 *  \param index Index of element.
 *  \param ptr Data to copy (not NULL).
 *  \param cpyfunc Custom copy function, or NULL for shallow copy (using memcpy).
 *  \return NULL if index is out of range, else the address of element set.
 *
 *  If you need some operations executed on the element replaced,
 *  that can also be done by the cpyfunc argument.
 */
M_DLLAPI M_PTR
m_Array_set(m_Array* const arr,
        const M_ID index,
        const M_PTR const ptr,
        const m_array_copy_fn_t cpyfunc);

/**
 *  \brief Unset (remove) an element in the array.
 *  \param arr The array (not NULL).
 *  \param index Index of element to remove.
 *  \param finalize_fn Finalization function for elements, or NULL.
 *  \return M_TRUE, or M_FALSE on memory error or if index is out of range.
 */
M_DLLAPI M_BOOL
m_Array_unset(m_Array* const arr,
        const M_ID index,
        const m_array_finalize_fn_t finalize_fn);

/**
 *  \brief Append some data to the array.
 *  \param arr The array (not NULL).
 *  \param ptr The data (not NULL).
 *  \param len Number of elements to append.
 *  \param cpyfunc Custom copy function, or NULL for shallow copy (using memcpy).
 *  \return M_TRUE, or M_FALSE on error.
 */
M_DLLAPI M_BOOL
m_Array_append(m_Array* const arr,
        const M_PTR const ptr,
        const M_SZ len,
        const m_array_copy_fn_t cpyfunc);

/**
 *  \brief Prepend some data to the array.
 *  \param arr The array (not NULL).
 *  \param ptr The data (not NULL).
 *  \param len Number of elements to prepend.
 *  \param cpyfunc Custom copy function, or NULL for shallow copy (using memcpy).
 *  \return M_TRUE, or M_FALSE on error.
 */
M_DLLAPI M_BOOL
m_Array_prepend(m_Array* const arr,
        const M_PTR const ptr,
        const M_SZ len,
        const m_array_copy_fn_t cpyfunc);

/**
 *  \brief Insert some data into the array.
 *  \param arr The array (not NULL).
 *  \param ptr The data (not NULL).
 *  \param len Number of elements to prepend.
 *  \param index Where to insert the data.
 *  \param cpyfunc Custom copy function, or NULL for shallow copy (using memcpy).
 *  \return M_TRUE, or M_FALSE on input or memory error.
 */
M_DLLAPI M_BOOL
m_Array_insert(m_Array* const arr,
        const M_PTR const ptr,
        const M_SZ len,
        const M_ID index,
        const m_array_copy_fn_t cpyfunc);

/**
 *  \brief Sort a array.
 */
#define m_Array_QSORT(v, compar_fn) \
  qsort((v)->data,(v)->len,(v)->unit,(compar_fn))

/**
 *  \brief Apply a function to each member of the array.
 *  \param arr The array (not NULL).
 *  \param traverse_fn The function to apply.
 */
M_DLLAPI M_VOID
m_Array_traverse(m_Array* const arr,
        M_VOID (* const traverse_fn)(M_PTR elem));

/**
 *  \brief Apply a function to each member of the array (with userdata).
 *  \param arr The array (not NULL).
 *  \param traverse_fn The function to apply.
 *  \param userdata Data passed to traverse_fn.
 */
M_DLLAPI M_VOID
m_Array_traverse2(m_Array* const arr,
        M_VOID (* const traverse_fn)(M_PTR elem, M_PTR udata),
        M_PTR userdata);

#ifndef NDEBUG

M_DLLAPI M_VOID
m_Array_debug(const m_Array* const arr);

#endif /* !NDEBUG */

#ifdef __cplusplus
}
#endif
#endif /* !M_ARRAY_H */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
