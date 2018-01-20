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
 *  \file m_h.h
 *  \brief Common headers, macros and types.
 *  \author Stanislas Marquis <stan@astrorigin.com>
 */

#ifndef M_H_H
#define M_H_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Standard C includes
 */

#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 *  \def M_DLLAPI
 *  \brief Instructions for MSVC.
 *
 *  M_MAKE_DLL: define to export symbols.
 */
#ifdef _MSC_VER
#if defined(M_MAKE_DLL)
#define M_DLLAPI __declspec(dllexport)
#else /* using library */
#define M_DLLAPI __declspec(dllimport)
#endif
#else
#define M_DLLAPI
#endif /* !_MSC_VER */

/**
 *  \def M_TLS
 *  \brief Thread level storage.
 */
#ifdef _MSC_VER
#define M_TLS __declspec(thread)
#else
#define M_TLS __thread
#endif /* !_MSC_VER */

/*
 *  Basic debugging macros
 */
#ifndef NDEBUG
#include <assert.h>
/**
 *  \def M_DEBUG( code )
 *  \brief Conditional compilation.
 *
 *  Does about the same as:
 *  \code
 *  #ifndef NDEBUG
 *  ...
 *  code
 *  ...
 *  #endif
 *  \endcode
 */
#define M_DEBUG(code) code
#else /* release mode, no-ops */
#define M_DEBUG(code)
#endif /* NDEBUG */

/**
 *  \def _M_TRACER( msg, ... )
 *  \brief Formatted print (if M_TRACE_MODE is defined).
 *  \note Use M_TRACE(msg, ...) with other M_TRACE_MODULENAME options.
 *
 *  M_TRACE_MODE: define to enable trace information
 */
#ifdef M_TRACE_MODE
#define _M_TRACER(msg, ...) printf("mutrace: "msg"\n", __VA_ARGS__)
#else /* no-op */
#define _M_TRACER
#endif

/**
 *  \def M_FATAL( code )
 *  \brief Last instructions before failure exit.
 */
#define M_FATAL(code) do{code exit(EXIT_FAILURE);}while(0)

/**
 *  \def M_FATAL_ERRMSG( msg, ... )
 *  \brief Fatal error (with formated message).
 */
#define M_FATAL_ERRMSG(msg, ...) \
  M_FATAL(printf("*FATAL* error from %s line %d:\n"msg"\n", \
  __FILE__,__LINE__,__VA_ARGS__);)

/**
 *  \brief Hard assert.
 */
#define m_assert(expr) \
  do{if(!(expr)){ \
    M_FATAL_ERRMSG("m_assert failed: (%s)",#expr); \
  }}while(0)

/**
 *  \def M_LIMIT( x, max )
 *  \brief Limit macro.
 */
#define M_LIMIT(x, max) ((x)>(max)?(max):(x))

/**
 *  \def M_MIN( a, b )
 *  \brief Minimum macro.
 */
#define M_MIN(a, b) ((a)<=(b)?(a):(b))

/**
 *  \def M_MAX( a, b )
 *  \brief Maximum macro.
 */
#define M_MAX(a, b) ((a)>=(b)?(a):(b))

/**
 *  \def M_UNUSED( var )
 *  \brief Unused variable (to extinct compiler warnings).
 */
#define M_UNUSED(var) ((void)var)

/*
 *  Types macros
 */
#define M_VOID      void
#define M_PTR       void*
#define M_CHAR      char
#define M_UCHAR     unsigned char
#define M_INT8      signed char
#define M_UINT8     unsigned char
#define M_INT16     signed short int
#define M_UINT16    unsigned short int
#ifdef _MSC_VER
#define M_INT32     signed __int32
#define M_UINT32    unsigned __int32
#define M_INT64     signed __int64
#define M_UINT64    unsigned __int64
#else
#define M_INT32     signed int
#define M_UINT32    unsigned int
#define M_INT64     signed long long int
#define M_UINT64    unsigned long long int
#endif /* !_MSC_VER */
#define M_FLOAT     float
#define M_DOUBLE    double
#define M_WCHAR     wchar_t
#define M_SZ        size_t
#define M_ID        size_t
#define M_BOOL      M_INT32
#define M_FALSE     (0)
#define M_TRUE      (1)

/*
 *  Printf format strings
 */
#define M_PTR_FMT     "%p"
#define M_INT8_FMT    "%hhd"
#define M_UINT8_FMT   "%hhu"
#define M_INT16_FMT   "%hd"
#define M_UINT16_FMT  "%hu"
#define M_STR_FMT     "%s"
#ifdef _MSC_VER
#define M_INT32_FMT   "%I32d"
#define M_UINT32_FMT  "%I32u"
#define M_INT64_FMT   "%I64d"
#define M_UINT64_FMT  "%I64u"
#define M_SZ_FMT      "%Iu"
#define M_ID_FMT      "%Iu"
#else
#define M_INT32_FMT   "%ld"
#define M_UINT32_FMT  "%lu"
#define M_INT64_FMT   "%lld"
#define M_UINT64_FMT  "%llu"
#define M_SZ_FMT      "%zu"
#define M_ID_FMT      "%zu"
#endif /* !_MSC_VER */

#ifdef __cplusplus
}
#endif

#endif /* !M_H_H */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
