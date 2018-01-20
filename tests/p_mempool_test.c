#include <p_mempool.h>

#include <p_memlist.h>

P_INT32
p_mempool_test( P_VOID )
{
    printf( "-- MEMPOOL -- test\n"
            "-- sizeof( chunk ) = "P_SZ_FMT"\n"
            "-- sizeof( bucket ) = "P_SZ_FMT"\n"
            "-- sizeof( mempool ) = "P_SZ_FMT"\n",
        sizeof( p_MemChunk ),
        sizeof( p_MemBucket ),
        sizeof( p_MemPool ) );

    p_memlist_debug();
    /* TODO find something here */
    p_memlist_debug();

    printf( "-- end mempool test\n" );
    return 0;
}

int main( int argc, char* argv[] )
{
    P_UNUSED( argc );
    P_UNUSED( argv );
    return p_mempool_test();
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
