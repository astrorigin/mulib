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
 *  \file p_vector.h
 *  \brief Vector container.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 */

#ifndef P_VECTOR_H
#define P_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "p_h.h"

/**
 *  \typedef p_Vector
 */
typedef struct _p_vector_t p_Vector;

/**
 *  \struct _p_vector_t
 */
struct _p_vector_t
{
    P_PTR   data;       /* start of vector[0] */
    P_SZ    len;        /* total of elements */
    P_SZ    unit;       /* size of one element */
    P_SZ    capacity;   /* size of data buffer */
    /* return requested size of data buffer (can be NULL) */
    P_SZ    (*calc_space_fn)( p_Vector* self, P_SZ len );
};

/**
 *  \brief Allocate for a vector.
 *  \return P_TRUE, or P_FALSE on error.
 */
P_EXPORT P_BOOL
p_vector_new( p_Vector** vec,
        P_SZ len,
        P_SZ unit,
        P_SZ (*calc_space_fn)( p_Vector*, P_SZ ) );

/**
 *  \brief Initialize a vector
 *  \return P_TRUE, or P_FALSE on error.
 */
P_EXPORT P_BOOL
p_vector_init( p_Vector* vec,
        P_SZ len,
        P_SZ unit,
        P_SZ (*calc_space_fn)( p_Vector*, P_SZ ) );

/**
 *  \brief Finalize a vector
 */
P_EXPORT P_VOID
p_vector_fini( p_Vector* vec );

/**
 *  \brief Delete a vector
 */
P_EXPORT P_VOID
p_vector_delete( p_Vector** vec );

/**
 *  \brief Default memory strategy.
 */
P_EXPORT P_SZ
p_vector_default_space_fn( p_Vector* self,
        P_SZ len );

/**
 *  \brief Fill a vector up.
 *  \param v The vector.
 *  \param p Pointer to data.
 *  \param sz Number of elements to copy.
 *  \note Dont overflow (check capacity).
 */
#define p_vector_fill( v, p, sz ) \
        do { memcpy( (v)->data, (p), (sz) * (v)->unit ); (v)->len = (sz); } while (0)

/**
 *  \brief Get an element in the vector.
 *  \param v The vector.
 *  \param index Index of element.
 *  \return NULL if index is out of range, else address of element.
 */
P_EXPORT P_PTR
p_vector_get( const p_Vector* v,
        const P_SZ index );

/**
 *  \brief Set an element in the vector.
 *  \param v The vector (not null).
 *  \param index Index of element.
 *  \param value Data to copy.
 *  \return NULL if index is out of range or value was NULL; else the indexed value.
 */
P_EXPORT P_PTR
p_vector_set( p_Vector* v,
        const P_ID index,
        const P_PTR value );

/**
 *  \brief Reserve space for elements.
 *  \param v The vector (not null).
 *  \param len Number of elements to accept.
 *  \return Address of new allocated memory, or NULL on error.
 */
P_EXPORT P_PTR
p_vector_reserve( p_Vector* v,
        const P_SZ len );

/**
 *  \brief Append some data to the vector.
 *  \param v The vector.
 *  \param ptr The data.
 *  \param len Number of elements to append.
 *  \return P_TRUE, or P_FALSE on error.
 */
P_EXPORT P_BOOL
p_vector_append( p_Vector* v,
        const P_PTR ptr,
        const P_SZ len );

/**
 *  \brief Sort a vector.
 */
#define p_vector_qsort( v, compar_fn ) \
        qsort( (v)->data, (v)->len, (v)->unit, (compar_fn) )

/**
 *  \brief Apply a function to each member of the vector.
 *  \param v The vector.
 *  \param traverse_fn The function to apply.
 */
P_EXPORT P_VOID
p_vector_traverse( p_Vector* v,
        P_VOID (*traverse_fn)( P_PTR ) );

/**
 *  \brief Apply a function to each member of the vector (2 parameters version).
 *  \param v The vector.
 *  \param traverse_fn The function to apply.
 *  \param userdata Data passed to traverse_fn.
 */
P_EXPORT P_VOID
p_vector_traverse2( p_Vector* v,
        P_VOID (*traverse_fn)( P_PTR, P_PTR ),
        P_PTR userdata );

#ifndef NDEBUG

P_EXPORT P_VOID
p_vector_debug( const p_Vector* v );

P_EXPORT P_INT32
p_vector_test( P_VOID );

#endif /* NDEBUG */

#ifdef __cplusplus
}
#endif

#endif /* P_VECTOR_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
