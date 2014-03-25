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
 *  \file p_mutex.h
 *  \brief Simple mutex macros.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 */

#ifndef P_MUTEX_H
#define P_MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "p_h.h"

#ifdef _MSC_VER
#include <windows.h>

#define P_MUTEX     HANDLE

#define P_MUTEX_INIT( mtx ) \
        P_MUTEX mtx = NULL

#define _P_MUTEX_FIRST_TIME( mtx ) \
        do { if ( mtx == NULL ) { \
            mtx = CreateMutex( NULL, FALSE, NULL ); \
            if ( mtx == NULL ) \
                P_FATAL_ERROR( "CreateMutex error (%d)\n", GetLastError() ); \
        }} while (0)

#define P_MUTEX_LOCK( mtx ) \
        do { _P_MUTEX_FIRST_TIME( mtx ); \
             WaitForSingleObject( mtx, INFINITE ); \
        } while (0)

#define P_MUTEX_UNLOCK( mtx ) \
        ReleaseMutex( mtx )

#define P_MUTEX_FINI( mtx ) \
        Closehandle( mtx )

#else /* Posix */
#include <pthread.h>

#define P_MUTEX     pthread_mutex_t

#define P_MUTEX_INIT( mtx ) \
        P_MUTEX mtx = PTHREAD_MUTEX_INITIALIZER

#define P_MUTEX_LOCK( mtx ) \
        pthread_mutex_lock( &mtx )

#define P_MUTEX_UNLOCK( mtx ) \
        pthread_mutex_unlock( &mtx )

#define P_MUTEX_FINI( mtx ) \
        pthread_mutex_destroy( &mtx )

#endif /* _MSC_VER */

#ifdef __cplusplus
}
#endif

#endif /* P_MUTEX_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
