#include <p_btree.h>

/* Also serves as a memlist test */
#include <p_memlist.h>

P_INT32
p_btree_test( P_VOID )
{
    P_BOOL b = P_FALSE;
    p_BTree bt;
    P_INT32 i, max;

    struct _test_t
    {
        P_ID    idx;
        P_PTR   val;
    };

    printf( "-- BTREE -- test\n"
            "-- sizeof( btree ) = "P_SZ_FMT"\n"
            "-- sizeof( node )  = "P_SZ_FMT"\n"
            "--\n",
        sizeof( p_BTree ),
        sizeof( p_BTNode ) );

    p_btree_init( &bt, _P_MALLOC_REF, _P_FREE_REF );

    struct _test_t elems[] =
    {

    #if 0 /* max=17 */
    { 16, (P_PTR)16 },
    { 4, (P_PTR)4 },
    { 104, (P_PTR)104 },
    { 12, (P_PTR)12 },
    { 8, (P_PTR)8 },
    { 20, (P_PTR)20 },
    { 44, (P_PTR)44 },
    { 1068, (P_PTR)1068 },
    { 40, (P_PTR)40 },
    { 24, (P_PTR)24 },
    { 32, (P_PTR)32 },
    { 48, (P_PTR)48 },
    { 512, (P_PTR)512 },
    { 1, (P_PTR)1 },
    { 617, (P_PTR)617 },
    { 38, (P_PTR)38 },
    { 213, (P_PTR)213 }
    #endif

    #if 0/* max=22 */
    { 16, (P_PTR)16 },
    { 4, (P_PTR)4 },
    { 104, (P_PTR)104 },
    { 12, (P_PTR)12 },
    { 8, (P_PTR)8 },
    { 20, (P_PTR)20 },
    { 44, (P_PTR)44 },
    { 1068, (P_PTR)1068 },
    { 40, (P_PTR)40 },
    { 24, (P_PTR)24 },
    { 32, (P_PTR)32 },
    { 48, (P_PTR)48 },
    { 64, (P_PTR)64 },
    { 80, (P_PTR)80 },
    { 96, (P_PTR)96 },
    { 68, (P_PTR)68 },
    { 184, (P_PTR)184 },
    { 52, (P_PTR)52 },
    { 36, (P_PTR)36 },
    { 220, (P_PTR)220 },
    { 276, (P_PTR)276 },
    { 56, (P_PTR)56 }
    #endif

    /* max=10 */
    { 1, (P_PTR)1 },
    { 2, (P_PTR)2 },
    { 3, (P_PTR)3 },
    { 4, (P_PTR)4 },
    { 5, (P_PTR)5 },
    { 6, (P_PTR)6 },
    { 7, (P_PTR)7 },
    { 8, (P_PTR)8 },
    { 9, (P_PTR)9 },
    { 10, (P_PTR)10 }
    };

    max = 10;
    for ( i = 0; i < max; ++i )
    {
        P_INT32 j;
        P_PTR v;

        /*printf( "i="P_INT32_FMT"\n", i );*/
        /*printf( P_ID_FMT" "P_PTR_FMT"\n", elems[i].idx, elems[i].val );*/
        b = p_btree_insert( &bt, elems[i].idx, elems[i].val );
        P_ASSERT( b )
        p_btree_debug( bt.root );
        for ( j=0; j <= i; ++j )
        {
            /*printf( "j="P_INT32_FMT"\n", j );*/
            v = p_btree_get( &bt, elems[j].idx );
            P_ASSERT( v == elems[j].val )
        }
    }

    p_memlist_debug();
    p_btree_fini( &bt );
    p_memlist_debug();

    printf( "-- end btree test\n" );
    return 0;
}

int main( int argc, char* argv[] )
{
    P_UNUSED( argc );
    P_UNUSED( argv );
    return p_btree_test();
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
