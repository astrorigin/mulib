#include <m_array.h>

#include <m_mempool.h>

typedef struct
{
  M_INT32 v1;
  M_INT32 v2;
} Test_t;

M_SZ
m_array_test_csfn(const m_Array* arr, M_SZ sz)
{
  M_UNUSED(arr);
  return sz * sizeof(Test_t) * 2;
}

M_INT32
m_array_test(M_VOID)
{
  Test_t data[] =
  {
  { 1, 2 },
  { 3, 4 },
  { 5, 6 }
  };
  int i;
  m_Array arr;

  M_MEMPOOL_INIT();

  m_Array_init(&arr, 3, sizeof(Test_t), &m_array_test_csfn);
  m_assert(arr.len == 0);
  m_assert(arr.capacity == 3 * sizeof(Test_t) * 2);

  m_Array_append(&arr, data, 3, NULL);
  m_assert(arr.len == 3);
  /*m_Array_debug(&arr);*/

  for (i = 0; i < 3; i++)
  {
      M_PTR p = m_Array_get(&arr, i);
      m_assert(p);
      m_assert(((Test_t*)p)->v1 == (i+(i+1)));
  }

  m_Array_fini(&arr, NULL);

  M_MEMPOOL_STATUS();
  M_MEMPOOL_FINI();
  M_MEMPOOL_STATUS();

  return 0;
}

int main(int argc, char* argv[])
{
  M_UNUSED(argc);
  M_UNUSED(argv);
  return m_array_test();
}

/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
