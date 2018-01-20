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
 *  \file m_mutex.h
 *  \brief Simple mutex macros.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <stan@astrorigin.com>
 */

#ifndef M_MUTEX_H
#define M_MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "m_h.h"

#ifdef _MSC_VER
#include <windows.h>

#define M_MUTEX HANDLE

#define M_MUTEX_INITIALIZER NULL

#define m_Mutex_init(mtx) \
        do{if(mtx==NULL)mtx=CreateMutex(NULL,FALSE,NULL);}while(0)

#define m_Mutex_lock(mtx) \
        WaitForSingleObject(mtx,INFINITE)

#define m_Mutex_unlock(mtx) \
        ReleaseMutex(mtx)

#define m_Mutex_fini(mtx) \
        do{CloseHandle(mtx);mtx=NULL;}while(0)

#else /* Posix */
#include <pthread.h>

#define M_MUTEX pthread_mutex_t

#define M_MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER

#define m_Mutex_init(mtx) \
        pthread_mutex_init(&mtx, NULL)

#define m_Mutex_lock(mtx) \
        pthread_mutex_lock(&mtx)

#define m_Mutex_unlock(mtx) \
        pthread_mutex_unlock(&mtx)

#define m_Mutex_fini(mtx) \
        pthread_mutex_destroy(&mtx)

#endif /* !_MSC_VER */

#ifdef __cplusplus
}
#endif
#endif /* !M_MUTEX_H */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
