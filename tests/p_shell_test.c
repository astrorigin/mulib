#include <p_shell.h>

#include <p_mempool.h>
#include <p_string.h>

P_INT32
p_shell_test( P_VOID )
{
    p_Shell sh;

#ifndef P_NO_MEMPOOL
    p_MemPool mp;
    p_mempool_init( &mp, 0 );
    p_mempool_set( &mp );
#endif

    /* test double */
    P_ASSERT( p_shell_init( &sh, P_SHELLTYPE_DBL ))
    *(P_DBL*)sh.p = 0.2;
    P_ASSERT( *(P_DBL*)sh.p == 0.2 )
    p_shell_fini( &sh );
    P_ASSERT( sh.p == NULL )

    /* test string */
    P_ASSERT( p_shell_init( &sh, P_SHELLTYPE_STRING ))
    P_ASSERT( ((P_CHAR*)((p_String*)sh.p)->data)[0] == '\0' )
    P_ASSERT( ((p_String*)sh.p)->len == 1 )
    p_shell_fini( &sh );
    P_ASSERT( sh.p == NULL )

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
    return p_shell_test();
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
