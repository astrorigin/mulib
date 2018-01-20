#include <m_dict.h>

M_INT32
m_Dict_test(M_VOID)
{
  m_Dict d;
  M_PTR old;

  M_MEMPOOL_INIT();

  m_Dict_init(&d);

  m_Dict_set(&d, "test", (M_PTR)0xdeadbeef, &old);
  m_assert(old == (M_PTR)0xdeadbeef);

  m_Dict_set(&d, "moo", (M_PTR)0x12345678, &old);
  m_assert(old == (M_PTR)0x12345678);

  m_assert(m_Dict_get(&d, "test") == (M_PTR)0xdeadbeef);

  m_Dict_set(&d, "moo", (M_PTR)0x87654321, &old);
  m_assert(old == (M_PTR)0x12345678);

  m_assert(m_Dict_get(&d, "moo") == (M_PTR)0x87654321);

  m_assert(m_Dict_unset(&d, "moo") == (M_PTR)0x87654321);
  m_assert(m_Dict_get(&d, "moo") == NULL);

  m_Dict_debug(&d);

  m_Dict_fini(&d);

  M_MEMPOOL_STATUS();
  M_MEMPOOL_FINI();
  return 0;
}

int main(int argc, char* argv[])
{
  M_UNUSED(argc);
  M_UNUSED(argv);
  return m_Dict_test();
}

/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
