#include <m_memcnt.h>
#include <m_sllist.h>

typedef struct Test
{
  struct Test *next;
  int i;
} Test_t;

int main(int argc, char* argv[])
{
  Test_t* t = NULL;
  Test_t* handle = NULL;

  Test_t* a = _M_MALLOC(sizeof(Test_t));
  Test_t* b = _M_MALLOC(sizeof(Test_t));
  Test_t* c = _M_MALLOC(sizeof(Test_t));
  Test_t* d = _M_MALLOC(sizeof(Test_t));
  Test_t* e = _M_MALLOC(sizeof(Test_t));

  a->i = 1;
  a->next = b;
  b->i = 2;
  b->next = c;
  c->i = 3;
  c->next = d;
  d->i = 4;
  d->next = e;
  e->i = 5;
  e->next = NULL;

  handle = a;

  t = (Test_t*) m_SLList_PULL(&handle);
  m_assert(t == a);
  m_assert(t->i == 1);
  m_assert(handle == b);

  m_SLList_PUSH(&handle, t);
  m_assert(handle == a);

  m_assert(m_SLList_COUNT(handle) == 5);



  _M_FREE(c);
  _M_FREE(b);
  _M_FREE(a);
  _M_FREE(d);
  _M_FREE(e);

  M_MEMCNT_DEBUG();

  M_UNUSED(argc);
  M_UNUSED(argv);

  return 0;
}

/* vim: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
