#include <p_string.h>

#include <p_mempool.h>

P_INT32
p_string_test( P_VOID )
{
#ifndef P_NO_MEMPOOL
    p_MemPool mp;
    p_mempool_init( &mp, 0 );
    p_mempool_set( &mp );
#endif

    p_String s;
    p_string_init( &s , "123" );
    P_ASSERT( s.len == 4 );

    p_string_set( &s, "abcdefgabcdefg" );
    P_ASSERT( !strcmp( s.data, "abcdefgabcdefg" ))

    p_string_replace( &s, "d", "x" );
    P_ASSERT( !strcmp( s.data, "abcxefgabcxefg" ))

    p_string_replace( &s, "fg", "z" );
    P_ASSERT( !strcmp( s.data, "abcxezabcxez" ));

    p_string_replace( &s, "c", "yy" );
    P_ASSERT( !strcmp( s.data, "abyyxezabyyxez" ));

    p_string_cat( &s, "1234" );
    P_ASSERT( !strcmp( s.data, "abyyxezabyyxez1234" ));

    p_string_cat_len( &s, "5678", 3 );
    P_ASSERT( !strcmp( s.data, "abyyxezabyyxez1234567" ));

    p_string_fini( &s );

#ifndef P_NO_MEMPOOL
    p_mempool_debug( &mp );
    p_mempool_fini( &mp );
    p_mempool_debug( &mp );
#endif
    return 0;
}

int main( int argc, char* argv[] )
{
    P_UNUSED( argc );
    P_UNUSED( argv );
    return p_string_test();
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 */
