#include <m_btree.h>
#include <m_memcnt.h>
#include <m_mempool.h>

m_BTree bt;

void do_count(void* val)
{
  static int i = 0;
  printf("COUNT= %d\n", i++);
  M_UNUSED(val);
}

void do_free(void* val)
{
  M_FREE(val, sizeof(M_ID));
}

void do_ins(M_ID id)
{
  M_ID* ptr = NULL;

  ptr = M_MALLOC(sizeof(M_ID));
  assert(ptr);
  *ptr = id;
  printf("ins "M_ID_FMT"\n", id);fflush(stdout);
  m_assert(m_BTree_insert(&bt, id, ptr) == 1);
  if (m_BTNode_is_unbalanced(bt.root))
  {
    m_BTree_debug(bt.root);
    exit(1);
  }
}

void do_rem(M_ID id)
{
  M_ID* val = NULL;
  printf("rem "M_ID_FMT"\n", id);fflush(stdout);
  //m_BTree_debug(bt.root);
  val = m_BTree_get(&bt, id);
  if (!val)
  {
    printf("CANNOT FIND "M_ID_FMT"\n", id);fflush(stdout);
    m_BTree_debug(bt.root);
    exit(1);
  }
  do_free(val);
  m_BTree_remove(&bt, id, NULL);
  //m_BTree_debug(bt.root);
  if (m_BTNode_is_unbalanced(bt.root))
  {
    m_BTree_debug(bt.root);
    exit(1);
  }
}

void do_del(void)
{
  printf("del\n");fflush(stdout);
  //m_BTree_debug(bt.root);
  m_BTree_fini(&bt);
  m_assert(m_BTree_init(&bt));
  bt.finalize_fn = &do_free;
}

int main(int argc, char* argv[])
{
  char* p;
  char input[12];
  M_ID id;

  M_UNUSED(argc);
  M_UNUSED(argv);

  M_MEMPOOL_INIT();

  m_assert(m_BTree_init(&bt));
  bt.finalize_fn = &do_free;

  while (fgets(input, 12, stdin) != NULL)
  {
    p = strchr(input, ' ');
    if (p)
    {
      *p = '\0';
      id = strtoul(p+1, NULL, 10);
    }
    if (!strcmp(input, "ins"))
    {
      do_ins(id);
    }
    else if (!strcmp(input, "rem"))
    {
      do_rem(id);
    }
    else if (!strncmp(input, "del", 3))
    {
      do_del();
    }
    else
    {
      printf("Unknown instruction %s\n", input);
      exit(1);
    }
  }

  /*m_BTree_debug(bt.root);*/
  /*m_BTNode_traverse(bt.root, &do_count);*/
  /*printf("COUNT = %llu\n", m_BTree_count(&bt));*/

  /*M_MEMPOOL_STATUS();*/
  m_BTree_fini(&bt);
  M_MEMPOOL_STATUS();

  M_MEMCNT_DEBUG();

  return 0;
}

/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
