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
 *  \file p_h.h
 *  \brief Common headers, macros and types.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 */

#ifndef P_H_H
#define P_H_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Standard C includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 *  Support for fcgi
 */
#ifdef P_FCGI_STDIO
#include <fcgi_stdio.h>
#endif

/**
 *  \def P_EXPORT
 *  \brief DLL instructions, for MSVC
 *
 *  P_MAKE_DLL: define to export symbols
 */
#ifdef _MSC_VER
#if defined( P_MAKE_DLL )
#define P_EXPORT    __declspec(dllexport)
#else /* using library */
#define P_EXPORT    __declspec(dllimport)
#endif
#else /* not MSVC */
#define P_EXPORT
#endif

/**
 *  \def P_TLS
 *  \brief Thread level storage
 */
#ifdef _MSC_VER
#define P_TLS   __declspec(thread)
#else
#define P_TLS   __thread
#endif

/*
 *  Basic debugging macros (no semicolon needed)
 */
#ifndef NDEBUG
#include <assert.h>
/**
 *  \def P_DEBUG( code )
 *  \brief Conditional compilation
 */
#define P_DEBUG( code )     code
/**
 *  \def P_ASSERT( expr )
 *  \brief Test assertion
 */
#define P_ASSERT( expr )    assert( expr );
#else /* release mode */
#define P_DEBUG( code )
#define P_ASSERT( expr )
#endif /* NDEBUG */

/**
 *  \def P_TRACER( msg, ... )
 *  \brief Formatted print (trace mode)
 *
 *  P_TRACE_MODE: define to enable debug information
 */
#ifdef P_TRACE_MODE
#define P_TRACER( msg, ... )    printf( msg, __VA_ARGS__ );
#endif

/**
 *  \def P_FATAL( code )
 *  \brief Last instructions before failure exit
 */
#define P_FATAL( code )     do { code exit( EXIT_FAILURE ); } while (0)

/**
 *  \def P_FATAL_ERROR( msg, ... )
 *  \brief Fatal error with a (formatted) message
 */
#define P_FATAL_ERROR( msg, ... ) \
        P_FATAL( printf( "*FATAL* error from %s line %d:\n" msg "\n",\
        __FILE__, __LINE__, __VA_ARGS__ ); )

/**
 *  \def P_LIMIT( x, max )
 *  \brief Limit macro
 */
#define P_LIMIT( x, max )   ( (x) > (max) ? (max) : (x) )

/**
 *  \def P_MIN( a, b )
 *  \brief Minimum macro
 */
#define P_MIN( a, b )       ( (a) <= (b) ? (a) : (b) )

/**
 *  \def P_MAX( a, b )
 *  \brief Maximum macro
 */
#define P_MAX( a, b )       ( (a) >= (b) ? (a) : (b) )

/**
 *  \def P_UNUSED( var )
 *  \brief Unused variable (to extinct compiler warnings)
 */
#define P_UNUSED( var )     ((void)var)

/*
 *  Pseudo Types
 */
#define P_VOID      void
#define P_PTR       void *
#define P_CHAR      char
#define P_UCHAR     unsigned char
#define P_INT8      signed char
#define P_UINT8     unsigned char
#define P_INT16     signed short int
#define P_UINT16    unsigned short int
#define P_INT32     signed int
#define P_UINT32    unsigned int
#ifdef _MSC_VER
#define P_INT64     signed __int64
#define P_UINT64    unsigned __int64
#else
#define P_INT64     signed long long int
#define P_UINT64    unsigned long long int
#endif /* not _MSC_VER */
#define P_DBL       double
#define P_WCHAR     wchar_t
#define P_SZ        size_t
#define P_ID        P_UINT64
#define P_BOOL      P_INT8
#define P_FALSE     (0)
#define P_TRUE      (1)

/*
 *  Printf format strings
 */
#ifdef _MSC_VER
#define P_PTR_FMT       "%p"
#define P_INT16_FMT     "%hd"
#define P_INT32_FMT     "%I32d"
#define P_UINT32_FMT    "%I32u"
#define P_INT64_FMT     "%I64d"
#define P_UINT64_FMT    "%I64u"
#define P_SZ_FMT        "%Iu"
#define P_ID_FMT        P_UINT64_FMT
#else
#define P_PTR_FMT       "%p"
#define P_INT16_FMT     "%hd"
#define P_INT32_FMT     "%ld"
#define P_UINT32_FMT    "%lu"
#define P_INT64_FMT     "%lld"
#define P_UINT64_FMT    "%llu"
#define P_SZ_FMT        "%zu"
#define P_ID_FMT        P_UINT64_FMT
#endif /* not _MSC_VER */

#ifdef __cplusplus
}
#endif

#endif /* P_H_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
