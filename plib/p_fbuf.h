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
 *  \file p_fbuf.h
 *  \brief File buffer.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 */

#ifndef P_FBUF_H
#define P_FBUF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "p_h.h"

#ifdef P_FCGI_STDIO
#include <fcgi_stdio.h>
#endif

/**
 *  \typedef p_FBuf
 */
typedef struct _p_fbuf p_FBuf;

/**
 *  \struct _p_fbuf
 */
struct _p_fbuf
{
    FILE*       f;          /**< stream */
    P_UCHAR*    buf;        /**< internal buffer */
    P_SZ        buflen;     /**< size of buffer */
    P_UCHAR*    tracker;    /**< tracking pointer */
    P_SZ        tracklen;   /**< remaining size to track */
};

/**
 *  \brief Initialize the file buffer struct.
 *  \param fbuf Uninitialized file buffer.
 *  \note You still have to set fbuf->f yourself.
 */
P_EXPORT P_VOID
p_fbuf_init( p_FBuf* fbuf );

/**
 *  \brief Finalize the file buffer struct.
 *  \param fbuf Initialized file buffer.
 *  \note You still have to take care of fbuf->f yourself.
 */
P_EXPORT P_VOID
p_fbuf_fini( p_FBuf* fbuf );

/**
 *  \brief Increment (or decrement) the tracker.
 *  \param fbuf The file buffer.
 *  \param step Amount of chars to step.
 *  \return The file buffer remaining tracking length, or ((P_SZ)-1) on error.
 */
P_EXPORT P_SZ
p_fbuf_incr( p_FBuf* fbuf,
        const P_INT32 step );

/**
 *  \brief Read stream and store the data in buffer.
 *  \param fbuf The file buffer.
 *  \param maxsz Maximum of data to read.
 *  \return Amount of data read from stream, or ((P_SZ)-1) on error.
 */
P_EXPORT P_SZ
p_fbuf_read( p_FBuf* fbuf,
        const P_SZ maxsz );

/**
 *  \brief Discard data read from stream, up to tracker position.
 *  \param fbuf The file buffer.
 *  \param maxreadsz Maximum amount of data to read if necessary.
 *  \return Size of buffer (or amount of data read), or ((P_SZ)-1) on error.
 *
 *  If either buf is NULL or tracker is at end, a call to p_fbuf_read() is done.
 *  \see p_fbuf_read()
 */
P_EXPORT P_SZ
p_fbuf_reset( p_FBuf* fbuf,
        const P_SZ maxreadsz );

/**
 *  \brief Consume (copy) stream up to a sequence of chars or a number of chars.
 *  \param fbuf The file buffer.
 *  \param upto Sequence of char to find (if NULL, read up to maxsz).
 *  \param uptolen Length of sequence (0 if upto is NULL).
 *  \param dest File to write chars, not including final sequence (can be NULL).
 *  \param maxsz Maximum chars to read, or 0 for no limits.
 *  \param maxreadsz Maximum amount of data to read in one time, if necessary.
 *  \return 0 if maxsz reached or EOF, or 1 if sequence char found, -1 on error.
 */
P_EXPORT P_INT8
p_fbuf_consume( p_FBuf* fbuf,
        const P_UCHAR* upto,
        const P_SZ uptolen,
        FILE* dest,
        const P_SZ maxsz,
        const P_SZ maxreadsz );

#ifdef __cplusplus
}
#endif

#endif /* P_FBUF_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
