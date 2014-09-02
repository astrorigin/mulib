/*
 *  Memory usage comparison
 *  See p_mempool_test_01.c
 */

#include <p_chrono.h>
#include <p_h.h>

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
                pointers[l++] = malloc( i*j );
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
                free( pointers[l++] );
            }
        }
    }
}

int main( int argc, char* argv [] )
{
    int i;
    P_UNUSED( argc );
    P_UNUSED( argv );

    P_PRINT_ELAPSED_TIME(
    for ( i=0; i < 10000; ++i )
        alloc1000();
        alloc1000();
        alloc1000();
    )
    return 0;
}

