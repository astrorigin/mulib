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
 *  \file p_chrono.h
 *  \brief Time related functions.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 */

#ifndef P_CHRONO_H
#define P_CHRONO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "p_h.h"

#include <sys/time.h>
#include <time.h>

#define P_MILLION       1000000L
#define P_BILLION       1000000000L

/**
 *  \brief Return true if timespec greater or equal than another.
 */
#define p_tspec_ge_than( ts, ots ) \
        ( (ts)->tv_sec > (ots)->tv_sec \
        || ( (ts)->tv_sec == (ots)->tv_sec && (ts)->tv_nsec >= (ots)->tv_nsec ) ? 1 : 0)

/**
 *  \brief Helper typedef.
 */
typedef struct timespec p_TimeSpec;

/**
 *  \brief Add milliseconds to a timespec.
 *  \param ts The timespec.
 *  \param msec Milliseconds to add.
 */
P_EXPORT P_VOID
p_tspec_plus_msec( struct timespec *ts,
        P_INT32 msec );

/**
 *  \brief Substract timespecs.
 *  \param ts The timespec.
 *  \param ots Timespec to substract, must be less or equal to ts.
 */
P_EXPORT P_VOID
p_tspec_minus_tspec( struct timespec *ts,
        const struct timespec *ots );


#ifndef NDEBUG
/**
 *  \def P_PRINT_ELAPSED_TIME
 *  \brief Print timing of a piece of code.
 */
#define P_PRINT_ELAPSED_TIME( code ) \
        do { \
        struct timeval _p_tv_1, _p_tv_2; \
        int _p_i; \
        _p_i = gettimeofday( &_p_tv_1, NULL ); \
        if ( _p_i != 0 ) printf( "-- gettimeofday failed\n" ); \
        code \
        _p_i = gettimeofday( &_p_tv_2, NULL ); \
        if ( _p_i != 0 ) printf( "-- gettimeofday failed\n" ); \
        printf( "-- Elapsed Time: "P_INT32_FMT" s. "P_INT32_FMT" us.\n", \
                _p_tv_2.tv_sec - _p_tv_1.tv_sec, \
                labs( _p_tv_1.tv_usec - _p_tv_2.tv_usec )); \
        } while (0);

#endif /* NDEBUG */

#ifdef __cplusplus
}
#endif

#endif /* P_CHRONO_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
