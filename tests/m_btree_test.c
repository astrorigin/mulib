#include <m_btree.h>
#include <m_memcnt.h>

typedef struct Test
{
  M_ID idx;
  M_PTR val;
} Test_t;

M_INT32
m_btree_test(M_VOID)
{
  M_BOOL b = M_FALSE;
  m_BTree bt;
  M_INT32 i, max;

  printf("-- BTREE -- test\n"
         "-- sizeof( btree ) = "M_SZ_FMT"\n"
         "-- sizeof( node )  = "M_SZ_FMT"\n"
         "--\n",
      sizeof(m_BTree),
      sizeof(m_BTNode));

  m_BTree_init2(&bt, _M_MALLOC_REF, (void(*)(void*))_M_FREE_REF);

  Test_t elems[] =
  {

  #if 0 /* max=17 */
  { 16, (M_PTR)16 },
  { 4, (M_PTR)4 },
  { 104, (M_PTR)104 },
  { 12, (M_PTR)12 },
  { 8, (M_PTR)8 },
  { 20, (M_PTR)20 },
  { 44, (M_PTR)44 },
  { 1068, (M_PTR)1068 },
  { 40, (M_PTR)40 },
  { 24, (M_PTR)24 },
  { 32, (M_PTR)32 },
  { 48, (M_PTR)48 },
  { 512, (M_PTR)512 },
  { 1, (M_PTR)1 },
  { 617, (M_PTR)617 },
  { 38, (M_PTR)38 },
  { 213, (M_PTR)213 }
  #endif

  #if 0/* max=22 */
  { 16, (M_PTR)16 },
  { 4, (M_PTR)4 },
  { 104, (M_PTR)104 },
  { 12, (M_PTR)12 },
  { 8, (M_PTR)8 },
  { 20, (M_PTR)20 },
  { 44, (M_PTR)44 },
  { 1068, (M_PTR)1068 },
  { 40, (M_PTR)40 },
  { 24, (M_PTR)24 },
  { 32, (M_PTR)32 },
  { 48, (M_PTR)48 },
  { 64, (M_PTR)64 },
  { 80, (M_PTR)80 },
  { 96, (M_PTR)96 },
  { 68, (M_PTR)68 },
  { 184, (M_PTR)184 },
  { 52, (M_PTR)52 },
  { 36, (M_PTR)36 },
  { 220, (M_PTR)220 },
  { 276, (M_PTR)276 },
  { 56, (M_PTR)56 }
  #endif

  /* max=10 */
  { 1, (M_PTR)1 },
  { 2, (M_PTR)2 },
  { 3, (M_PTR)3 },
  { 4, (M_PTR)4 },
  { 5, (M_PTR)5 },
  { 6, (M_PTR)6 },
  { 7, (M_PTR)7 },
  { 8, (M_PTR)8 },
  { 9, (M_PTR)9 },
  { 10, (M_PTR)10 }
  };

  max = 10;
  for (i = 0; i < max; ++i)
  {
    M_INT32 j;
    M_PTR v;

    /*printf( "i="M_INT32_FMT"\n", i );*/
    /*printf( M_ID_FMT" "M_PTR_FMT"\n", elems[i].idx, elems[i].val );*/
    b = m_BTree_insert(&bt, elems[i].idx, elems[i].val);
    m_assert(b);
    m_BTree_debug(bt.root);
    for (j=0; j <= i; ++j)
    {
      /*printf( "j="M_INT32_FMT"\n", j );*/
      v = m_BTree_get(&bt, elems[j].idx);
      m_assert(v == elems[j].val);
    }
  }

  M_MEMCNT_DEBUG();
  m_BTree_fini(&bt);
  M_MEMCNT_DEBUG();

  printf("-- end btree test\n");
  return 0;
}

int main(int argc, char* argv[])
{
  M_UNUSED(argc);
  M_UNUSED(argv);
  return m_btree_test();
}

/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
