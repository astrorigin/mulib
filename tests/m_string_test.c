#include <m_string.h>

#include <m_mempool.h>

M_INT32
m_String_test(M_VOID)
{
  M_MEMPOOL_INIT();

  m_String s;
  m_String_init( &s , "123" );
  m_assert( s.len == 4 );

  m_String_set( &s, "abcdefgabcdefg" );
  m_assert( !strcmp( s.data, "abcdefgabcdefg" ));

  m_String_replace( &s, "d", "x" );
  m_assert( !strcmp( s.data, "abcxefgabcxefg" ));

  m_String_replace( &s, "fg", "z" );
  m_assert( !strcmp( s.data, "abcxezabcxez" ));

  m_String_replace( &s, "c", "yy" );
  m_assert( !strcmp( s.data, "abyyxezabyyxez" ));

  m_String_cat( &s, "1234" );
  m_assert( !strcmp( s.data, "abyyxezabyyxez1234" ));

  m_String_cat_len( &s, "5678", 3 );
  m_assert( !strcmp( s.data, "abyyxezabyyxez1234567" ));

  m_String_cat_len( &s, "ABCDEFGHIJKL", 12 );
  m_assert( !strcmp( s.data, "abyyxezabyyxez1234567ABCDEFGHIJKL" ));

  m_String_fini( &s );

  M_MEMPOOL_STATUS();
  M_MEMPOOL_FINI();
  M_MEMPOOL_STATUS();
  return 0;
}

int main(int argc, char* argv[])
{
  M_UNUSED(argc);
  M_UNUSED(argv);
  return m_String_test();
}

/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 */
