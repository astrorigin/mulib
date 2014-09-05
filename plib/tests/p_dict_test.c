#include <p_dict.h>

P_INT32
p_dict_test( P_VOID )
{
#ifndef P_NO_MEMPOOL
    p_MemPool mp;
    p_mempool_init( &mp, 0 );
    p_mempool_set( &mp );
#endif
    p_Dict d;
    P_PTR old;

    p_dict_init( &d );

    p_dict_set( &d, "test", (P_PTR)0xdeadbeef, &old );
    P_ASSERT( old == (P_PTR)0xdeadbeef );

    p_dict_set( &d, "moo", (P_PTR)0x12345678, &old );
    P_ASSERT( old == (P_PTR)0x12345678 );

    P_ASSERT( p_dict_get( &d, "test" ) == (P_PTR)0xdeadbeef )

    p_dict_set( &d, "moo", (P_PTR)0x87654321, &old );
    P_ASSERT( old == (P_PTR)0x12345678 );

    P_ASSERT( p_dict_get( &d, "moo" ) == (P_PTR)0x87654321 );

    P_ASSERT( p_dict_unset( &d, "moo" ) == (P_PTR)0x87654321 );
    P_ASSERT( p_dict_get( &d, "moo" ) == NULL );

    p_dict_debug( &d );

    p_dict_fini( &d );

#ifndef P_NO_MEMPOOL
    p_mempool_debug( &mp );
    p_mempool_fini( &mp );
#endif
    return 0;
}

int main( int argc, char* argv[] )
{
    P_UNUSED( argc );
    P_UNUSED( argv );
    return p_dict_test();
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
