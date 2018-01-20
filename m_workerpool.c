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

#include "m_workerpool.h"

#include "m_sllist.h"

#include <unistd.h> /* for sleep */

#undef M_TRACE
#if defined(M_TRACE_MODE) && defined(M_TRACE_WORKERPOOL)
#define M_TRACE(msg, ...) _M_TRACER("-- WorkerPool -- "msg, __VA_ARGS__)
#else
#define M_TRACE(moo, ...)
#endif

#define _M_WORKERPOOL_INVALID  0
#define _M_WORKERPOOL_CREATED  1
#define _M_WORKERPOOL_READY    2
#define _M_WORKERPOOL_WAITING  3

M_BOOL
m_WorkerPool_init(m_WorkerPool* const wp,
        const M_SZ max,
        const M_SZ maxIdle,
        M_PTR (* const routine)(M_PTR))
{
  assert(wp);
  assert(max == 0 || max >= maxIdle);
  assert(routine);

  wp->maxWorkers = max;
  wp->maxIdleWorkers = maxIdle;
  wp->routine = routine;

  wp->secTimeout = 30;
  wp->timeout_fn = NULL;

  wp->status = _M_WORKERPOOL_INVALID;
  wp->cntWorkers = 0;

  wp->cntIdle = 0;
  wp->listIdle = NULL;
  wp->lastIdle = NULL;
  if (sem_init(&wp->semIdle, 0, 0) != 0) goto fail;
  if (pthread_mutex_init(&wp->mtxIdle, NULL) != 0) goto fail;

  wp->cntTrash = 0;
  wp->listTrash = NULL;
  if (sem_init(&wp->semTrash, 0, 0) != 0) goto fail;
  if (pthread_mutex_init(&wp->mtxTrash, NULL) != 0) goto fail;

  wp->status = _M_WORKERPOOL_CREATED;
  return M_TRUE;

fail:
  sem_destroy(&wp->semIdle);
  pthread_mutex_destroy(&wp->mtxIdle);
  sem_destroy(&wp->semTrash);
  pthread_mutex_destroy(&wp->mtxTrash);

  return M_FALSE;
}

M_BOOL
m_WorkerPool_start(m_WorkerPool* const wp,
        const M_SZ numWorkers)
{
  M_SZ i;

  assert(wp);
  assert(numWorkers <= wp->maxIdleWorkers);

  if (wp->status != _M_WORKERPOOL_CREATED) return M_FALSE;

  /* spawn destroyer thread */
  if (pthread_create(&wp->destroyer, NULL, &m_WorkerPool_destroyer, wp) != 0)
    return M_FALSE;

  wp->status = _M_WORKERPOOL_READY;

  /* spawn initial workers */
  i = M_LIMIT(numWorkers, wp->maxIdleWorkers);
  for (; i != 0; --i)
  {
    if (!m_WorkerPool_create_worker(wp)) return M_FALSE;
  }

  return M_TRUE;
}

M_VOID
m_Worker_put_idle(m_Worker* const w)
{
  assert(w);
  m_WorkerPool* wp = w->pool;
  if (!wp->lastIdle)
  {
    assert(!wp->listIdle);
    wp->listIdle = w;
  }
  else
  {
    assert(wp->listIdle);
    wp->lastIdle->next = w;
  }
  wp->lastIdle = w;
  w->next = NULL;
  wp->cntIdle++;
  m_assert(sem_post(&wp->semIdle) == 0);
}

M_VOID
m_Worker_put_trash(m_Worker* const w)
{
  assert(w);
  m_WorkerPool* wp = w->pool;
  m_SLList_PUSH(&wp->listTrash, w);
  wp->cntTrash++;
  m_assert(sem_post(&wp->semTrash) == 0);
}

M_BOOL
m_WorkerPool_create_worker(m_WorkerPool* const wp)
{
  m_Worker* w = NULL;
  struct sigevent ev;

  assert(wp);

  if (!(w = malloc(sizeof(m_Worker)))) return M_FALSE;
  w->next = NULL;
  w->pool = wp;
  w->data = NULL;
  if (sem_init(&w->semData, 0, 0) != 0) goto fail;

  /* prepare timer */
  ev.sigev_notify = SIGEV_THREAD;
  ev.sigev_notify_function = &m_Worker_timeout;
  ev.sigev_notify_attributes = NULL;
  ev.sigev_value.sival_ptr = w;
  if (timer_create(CLOCK_MONOTONIC, &ev, &w->timer) != 0) goto fail;

  if (pthread_mutex_init(&w->mtx, NULL) != 0) goto fail;

  /* this thread blocks waiting for data */
  if (pthread_create(&w->th, NULL, &m_Worker_routine, w) != 0) goto fail;

  m_Worker_put_idle(w);
  wp->cntWorkers++;

  return M_TRUE;

fail:
  sem_destroy(&w->semData);
  timer_delete(w->timer);
  pthread_mutex_destroy(&w->mtx);
  free(w);

  return M_FALSE;
}

M_PTR
m_WorkerPool_destroyer(M_PTR arg)
{
  int i;
  m_WorkerPool* wp = (m_WorkerPool*) arg;

  while ((i = sem_wait(&wp->semTrash)) == 0)
  {
    m_Worker* w = NULL;

    m_assert(pthread_mutex_lock(&wp->mtxTrash) == 0);

    if (!wp->listTrash)
    {
      /* terminate the thread */
      assert(wp->cntTrash == 0);
      assert(wp->cntWorkers == 0);
      m_assert(pthread_mutex_unlock(&wp->mtxTrash) == 0);
      return NULL;
    }
    /* take a worker */
    w = (m_Worker*) m_SLList_PULL(&wp->listTrash);
    wp->cntTrash--;

    m_assert(pthread_mutex_unlock(&wp->mtxTrash) == 0);

    assert(w);
    /* dispose of worker */
    m_assert(sem_destroy(&w->semData) == 0);
    if ((i = pthread_join(w->th, NULL)) != 0)
    {
      m_assert(i == EINVAL); /* thread had timed out */
    }
    m_assert(timer_delete(w->timer) == 0);
    m_assert(pthread_mutex_destroy(&w->mtx) == 0);

    free(w);
  }
  m_assert(i == EINTR);
  return NULL;
}

M_VOID
m_Worker_timeout(union sigval val)
{
  int i;
  m_Worker* w = NULL;
  m_WorkerPool* wp;

  w = (m_Worker*) val.sival_ptr;
  assert(w);
  wp = w->pool;

  if ((i = pthread_mutex_trylock(&w->mtx)) != 0)
  {
    m_assert(i == EAGAIN);
    /* in the mean time, worker is done, so abort */
    return;
  }
  /* stop that worker asap */
  m_assert(pthread_cancel(w->th) == 0);
  m_assert(pthread_join(w->th, NULL) == 0);
  /* execute a time out function? */
  if (wp->timeout_fn) (*wp->timeout_fn)(w->data);
  /* Get it recycled */
  m_assert(pthread_mutex_lock(&wp->mtxTrash) == 0);
  m_assert(pthread_mutex_lock(&wp->mtxIdle) == 0);
  m_Worker_put_trash(w);
  wp->cntWorkers--;
  m_assert(pthread_mutex_unlock(&wp->mtxIdle) == 0);
  m_assert(pthread_mutex_unlock(&wp->mtxTrash) == 0);
}

M_PTR
m_Worker_routine(M_PTR arg)
{
  int i;
  M_PTR ret = NULL;
  m_Worker* w = (m_Worker*) arg;
  m_WorkerPool* wp = w->pool;
  struct itimerspec tspec;

  tspec.it_interval.tv_sec = 0;
  tspec.it_interval.tv_nsec = 0;
  tspec.it_value.tv_sec = 0;
  tspec.it_value.tv_nsec = 0;

  /* cannot be timed out while this is locked */
  m_assert(pthread_mutex_lock(&w->mtx) == 0);

  for (;;)
  {
    m_assert(sem_wait(&w->semData) == 0);
    /* exit if user input is null */
    if (w->data == NULL)
    {
      m_assert(pthread_mutex_lock(&wp->mtxIdle) == 0);
      break;
    }
    /* arm the timer */
    tspec.it_value.tv_sec = wp->secTimeout;
    m_assert(timer_settime(w->timer, 0, &tspec, NULL) == 0);
    /* execute our routine */
    m_assert(pthread_mutex_unlock(&w->mtx) == 0);
    ret = (*wp->routine)(w->data);
    /* disarm timer */
    tspec.it_value.tv_sec = 0;
    if ((i = pthread_mutex_trylock(&w->mtx)) != 0)
    {
      m_assert(i == EAGAIN);
      /* we timed out, thread is being canceled */
      return NULL;
    }
    m_assert(timer_settime(w->timer, 0, &tspec, NULL) == 0);
    /* this must be locked in all cases now */
    m_assert(pthread_mutex_lock(&wp->mtxIdle) == 0);
    /* exit if routine has returned non null */
    if (ret != NULL) break;
    /* if wp is stuck waiting, go idle */
    if (wp->status == _M_WORKERPOOL_WAITING)
    {
      assert(wp->cntIdle == 0);
      wp->status = _M_WORKERPOOL_READY;
    }
    else
    if (wp->cntIdle >= wp->maxIdleWorkers)
    {
      break;
    }
    /* go idle and continue */
    m_Worker_put_idle(w);
    m_assert(pthread_mutex_unlock(&wp->mtxIdle) == 0);
  }

  /* get recycled */
  m_assert(pthread_mutex_lock(&wp->mtxTrash) == 0);
  m_Worker_put_trash(w);
  wp->cntWorkers--;
  m_assert(pthread_mutex_unlock(&wp->mtxIdle) == 0);
  m_assert(pthread_mutex_unlock(&wp->mtxTrash) == 0);

  return ret;
}

m_Worker*
m_Worker_get(m_WorkerPool* const wp)
{
  int i;
  m_Worker* w = NULL;

  assert(wp);

  m_assert(pthread_mutex_lock(&wp->mtxIdle) == 0);

  /* see if there is an idle worker ready */
  if ((i = sem_trywait(&wp->semIdle)) != 0)
  {
    m_assert(i == EAGAIN);
    assert(wp->cntIdle == 0);
    /* no idle worker - do we make one? */
    if (wp->maxWorkers == 0
        || wp->cntWorkers == 0
        || wp->cntWorkers < wp->maxWorkers)
    {
      if (!m_WorkerPool_create_worker(wp))
      {
        m_assert(pthread_mutex_unlock(&wp->mtxIdle) == 0);
        return NULL;
      }
      m_assert(sem_trywait(&wp->semIdle) == 0);
    }
    else
    { /* max workers count reached, wait indefinitely */
      wp->status = _M_WORKERPOOL_WAITING;
      m_assert(pthread_mutex_unlock(&wp->mtxIdle) == 0);
      m_assert(sem_wait(&wp->semIdle) == 0);
      m_assert(pthread_mutex_lock(&wp->mtxIdle) == 0);
      /* status is restored from worker's routine */
      assert(wp->status == _M_WORKERPOOL_READY);
    }
  }
  /* there is at least one spare worker */
  assert(wp->cntIdle > 0);
  assert(wp->listIdle);
  w = (m_Worker*) m_SLList_PULL(&wp->listIdle);
  if (w == wp->lastIdle) wp->lastIdle = NULL;
  wp->cntIdle--;

  m_assert(pthread_mutex_unlock(&wp->mtxIdle) == 0);

  return w;
}

M_VOID
m_Worker_start(m_Worker* const w)
{
  assert(w);
  m_assert(sem_post(&w->semData) == 0);
}

M_BOOL
m_WorkerPool_fini(m_WorkerPool* const wp)
{
  assert(wp);

  if (wp->status == _M_WORKERPOOL_INVALID) return M_TRUE;
  if (wp->status == _M_WORKERPOOL_READY)
  {
    M_BOOL b;
    M_SZ i;

    /* let workers finish jobs */
    for (b = M_FALSE;;)
    {
      m_assert(pthread_mutex_lock(&wp->mtxIdle) == 0);
      if (wp->cntIdle == wp->cntWorkers) b = M_TRUE;
      m_assert(pthread_mutex_unlock(&wp->mtxIdle) == 0);
      if (b) break;
      sleep(1);
    }
    /* trigger workers termination */
    for (i = wp->cntWorkers; i != 0; --i)
    {
      m_Worker* w = m_Worker_get(wp);
      assert(w);
      w->data = NULL;
      m_Worker_start(w);
    }
    /* let all workers move to trash */
    for (b = M_FALSE;;)
    {
      m_assert(pthread_mutex_lock(&wp->mtxIdle) == 0);
      if (wp->cntWorkers == 0) b = M_TRUE;
      m_assert(pthread_mutex_unlock(&wp->mtxIdle) == 0);
      if (b) break;
      sleep(1);
    }
    /* let destroyer finish workers */
    for (b = M_FALSE;;)
    {
      m_assert(pthread_mutex_lock(&wp->mtxTrash) == 0);
      if (wp->cntTrash == 0) b = M_TRUE;
      m_assert(pthread_mutex_unlock(&wp->mtxTrash) == 0);
      if (b) break;
      sleep(1);
    }
    /* trigger destroyer termination */
    m_assert(sem_post(&wp->semTrash) == 0);
    m_assert(pthread_join(wp->destroyer, NULL) == 0);
  }

  sem_destroy(&wp->semIdle);
  pthread_mutex_destroy(&wp->mtxIdle);
  sem_destroy(&wp->semTrash);
  pthread_mutex_destroy(&wp->mtxTrash);

  wp->status = _M_WORKERPOOL_INVALID;
  return M_TRUE;
}

/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
