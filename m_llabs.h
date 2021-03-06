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
 *  \file m_llabs.h
 *  \brief llabs
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <stan@astrorigin.com>
 */

#ifndef M_LLABS_H
#define M_LLABS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#define m_llabs llabs
#else

#include <stdlib.h>
#if !(_XOPEN_SOURCE >= 600 || _ISOC99_SOURCE || _POSIX_C_SOURCE >= 200112L)
/* We do not have llabs */
#define llabs m_llabs

/**
 *  \brief llabs for dummies.
 */
long long
m_llabs(const long long j);

#else
/* We have llabs */
#define m_llabs llabs
#endif
#endif /* !_MSC_VER */

#ifdef __cplusplus
}
#endif
#endif /* !M_LLABS_H */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
