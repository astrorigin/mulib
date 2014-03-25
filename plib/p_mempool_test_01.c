/*
 *  Memory usage comparison
 *  See p_mempool_test_02.c
 */

#include <p_mempool.h>

void alloc1000()
{
    void* pointers[1000];
    int i;
    int j;
    int k;
    int l = 0;
    
    for ( i=1; i < 11; ++i )
    {
        for ( j=1; j < 11; ++j )
        {
            for ( k=1; k < 11; ++k )
            {
                pointers[l++] = P_MALLOC( i*j );
            }
        }
    }
    l = 0;
    for ( i=1; i < 11; ++i )
    {
        for ( j=1; j < 11; ++j )
        {
            for ( k=1; k < 11; ++k )
            {
                P_FREE( pointers[l++], i*j );
            }
        }
    }
}

int main( int argc, char* argv [] )
{
    int i;
#ifndef P_NO_MEMPOOL
    p_MemPool mp;
#endif
    P_UNUSED( argc );
    P_UNUSED( argv );

#ifndef P_NO_MEMPOOL
    p_mempool_init( &mp, 0 );
    p_mempool_set( &mp );
#endif
    P_PRINT_ELAPSED_TIME(
    for ( i=0; i < 10000; ++i )
        alloc1000();
        alloc1000();
        alloc1000();
    )
    return 0;
}

