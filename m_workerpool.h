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
 *  \file m_workerpool.h
 *  \brief Worker pool.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <stan@astrorigin.com>
 */

#ifndef M_WORKERPOOL_H
#define M_WORKERPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _MSC_VER
#define _POSIX_C_SOURCE 199309L /* for timer_t etc */
#endif

#include "m_h.h"

#include <pthread.h>
#include <semaphore.h>

typedef struct _m_WorkerPool m_WorkerPool;

typedef struct _m_Worker m_Worker;

struct _m_WorkerPool
{
  M_SZ maxWorkers; /* max workers (idle or busy), 0 for no limit */
  M_SZ maxIdleWorkers; /* max workers allowed in idle list */
  M_PTR (*routine)(M_PTR data); /* that routine is passed a worker's data as argument */

  time_t secTimeout; /* max execution time in seconds, default 30 */
  M_VOID (*timeout_fn)(M_PTR data); /* run that after worker's thread is canceled */

  M_INT32 status; /* internal business */
  M_SZ cntWorkers; /* current count of workers (idle or busy) */

  M_SZ cntIdle; /* current count of idle workers */
  m_Worker* listIdle; /* list of idle workers */
  m_Worker* lastIdle; /* the latest idle worker */
  sem_t semIdle; /* signal idle workers */
  pthread_mutex_t mtxIdle; /* protect operations on idles, status and cntWorkers */

  M_SZ cntTrash; /* current count of trashed workers */
  m_Worker* listTrash; /* list of trashed workers */
  sem_t semTrash; /* signal end of duty from worker */
  pthread_mutex_t mtxTrash; /* protect operations on trash */
  pthread_t destroyer;
};

struct _m_Worker
{
  m_Worker* next; /* linked-list */
  m_WorkerPool* pool;
  M_PTR data; /* user provided data */
  sem_t semData; /* signal to proceed with new data (or exit) */
  pthread_t th;
  timer_t timer;
  pthread_mutex_t mtx;
};

/**
 *  \brief Initialize a worker pool.
 *  \param wpool
 *  \param start Number of workers to prepare initialy.
 *  \param max Maximum number of workers (both idle and active). 0 for no limits.
 *  \param maxIdle Maximum number of idle workers.
 *  \param routine Job routine (takes worker's data as argument). May not be NULL.
 *  \note No threads are spawn here.
 *  \see m_WorkerPool_start()
 */
M_DLLAPI M_BOOL
m_WorkerPool_init(m_WorkerPool* const wpool,
        const M_SZ max,
        const M_SZ maxIdle,
        M_PTR (* const routine)(M_PTR worker));

/**
 *
 */
M_DLLAPI M_BOOL
m_WorkerPool_start(m_WorkerPool* const wpool,
        const M_SZ numWorkers);

/**
 *  \brief Finalize a worker pool.
 */
M_DLLAPI M_BOOL
m_WorkerPool_fini(m_WorkerPool* const wpool);

/**
 *  \brief Get a worker ready to start.
 *  \note You may set data before calling start function.
 */
M_DLLAPI m_Worker*
m_Worker_get(m_WorkerPool* const wpool);

/**
 *
 */
M_DLLAPI M_VOID
m_Worker_start(m_Worker* const worker);




M_DLLAPI M_VOID
m_Worker_put_idle(m_Worker* const worker);

M_DLLAPI M_VOID
m_Worker_put_trash(m_Worker* const worker);

M_DLLAPI M_BOOL
m_WorkerPool_create_worker(m_WorkerPool* const wpool);

M_DLLAPI M_PTR
m_WorkerPool_destroyer(M_PTR arg);

M_DLLAPI M_PTR
m_Worker_routine(M_PTR arg);

M_DLLAPI M_VOID
m_Worker_timeout(union sigval val);

#ifdef __cplusplus
}
#endif
#endif /* !M_WORKERPOOL_H */
/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
