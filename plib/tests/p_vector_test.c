#include <p_vector.h>

#include <p_mempool.h>

struct p_vector_test_t
{
    P_INT32 v1;
    P_INT32 v2;
};

P_SZ
p_vector_test_csfn( p_Vector* v,
        P_SZ sz )
{
    P_UNUSED( v );
    return sz * sizeof( struct p_vector_test_t ) * 2;
}

P_INT32
p_vector_test( P_VOID )
{
#ifndef P_NO_MEMPOOL
    p_MemPool mp;
#endif
    struct p_vector_test_t data[] =
    {
    { 1, 2 },
    { 3, 4 },
    { 5, 6 }
    };
    int i;
    p_Vector vec;
#ifndef P_NO_MEMPOOL
    p_mempool_set( &mp );
    p_mempool_init( &mp, 0 );
#endif
    p_vector_init( &vec, 3, sizeof( struct p_vector_test_t ), &p_vector_test_csfn );
    P_ASSERT( vec.len == 0 )
    P_ASSERT( vec.capacity == 3 * sizeof( struct p_vector_test_t ) * 2 )

    p_vector_fill( &vec, data, 3 );
    P_ASSERT( vec.len == 3 )
    p_vector_debug( &vec );

    for ( i = 0; i < 3; i++ )
    {
        P_PTR p = p_vector_get( &vec, i );
        P_ASSERT( p )
        P_ASSERT( ((struct p_vector_test_t*)p)->v1 == (i+(i+1)) )
    }

    p_vector_fini( &vec );
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
    return p_vector_test();
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
