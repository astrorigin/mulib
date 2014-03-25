/*
    Plib
    Copyright (C) 2014 Stanislas Marquis <stnsls@gmail.com>

    This program is free software: you can redistribute it and/or modify it
    under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "p_btree.h"

#include "p_mempool.h"
#include "p_llabs.h"

#undef P_TRACE
#if defined( P_TRACE_MODE ) && defined( P_TRACE_BTREE )
#define P_TRACE P_TRACER
#else
#define P_TRACE( moo, ... )
#endif

#ifndef P_NO_MEMPOOL
P_VOID
p_btree_node_free( P_PTR p )
{
    P_FREE( p, sizeof( p_BTNode ));
}
#endif

P_VOID
p_btree_new( p_BTree** bt )
{
    P_TRACE( "-- BTREE -- new ("P_PTR_FMT")\n", (P_PTR)*bt )
    *bt = P_MALLOC( sizeof( p_BTree ));
    P_ASSERT( *bt )
    p_btree_init( *bt, P_MALLOC_REF, p_btree_node_free_ref );
}

P_VOID
p_btree_delete( p_BTree** bt )
{
    P_ASSERT( *bt )
    P_TRACE( "-- BTREE -- delete ("P_PTR_FMT")\n", (P_PTR)*bt )
    p_btree_fini( *bt );
    P_FREE( *bt, sizeof( p_BTree ));
    *bt = NULL;
}

P_VOID
p_btree_init( p_BTree* bt,
        P_PTR (*mallocdoer)( P_SZ ),
        P_VOID (*freedoer)( P_PTR ) )
{
    P_TRACE( "-- BTREE -- init ("P_PTR_FMT")\n", (P_PTR)bt )
    bt->root = NULL;
    bt->mallocdoer = mallocdoer ? mallocdoer : &p_btree_mallocdoer;
    bt->freedoer = freedoer ? freedoer : &p_btree_freedoer;
}

P_VOID
p_btree_fini( p_BTree* bt )
{
    P_TRACE( "-- BTREE -- fini ("P_PTR_FMT")\n", (P_PTR)bt )
    p_btree_node_delete( &bt->root, bt->freedoer );
    bt->mallocdoer = NULL;
    bt->freedoer = NULL;
}

P_VOID
p_btree_node_new( p_BTNode** bt,
        const P_ID key,
        const P_PTR val,
        const p_BTNode* parent,
        P_PTR (*mallocdoer)( P_SZ ) )
{
    P_TRACE( "-- BTREE NODE -- new ("P_ID_FMT" with parent: "P_ID_FMT")\n",
             key, parent ? parent->key : 0 )
    *bt = (*mallocdoer)( sizeof( p_BTNode ) );
    P_ASSERT( *bt )

    (*bt)->key = key;
    (*bt)->val = (P_PTR) val;
    (*bt)->less = NULL;
    (*bt)->more = NULL;
    (*bt)->parent = (p_BTNode*) parent;
}

P_VOID
p_btree_node_delete( p_BTNode** bt,
        P_VOID (*freedoer)( P_PTR ) )
{
    if ( !*bt ) return;

    p_btree_node_delete( &(*bt)->less, freedoer );
    p_btree_node_delete( &(*bt)->more, freedoer );

    P_TRACE( "-- BTREE NODE -- delete ("P_PTR_FMT")\n", (P_PTR)*bt )
    (*freedoer)( *bt );
    *bt = NULL;
}

P_BOOL
p_btree_node_insert( p_BTNode** bt,
        const P_ID key,
        const P_PTR val,
        const p_BTNode* parent,
        P_PTR (*mallocdoer)( P_SZ ) )
{
    if ( !*bt )
    {
        p_btree_node_new( bt, key, val, parent, mallocdoer );
        if ( parent )
            p_btree_node_balance( (*bt)->parent, 0, 0 );
        return P_TRUE;
    }

#if 0
    p_BTNode* tmp = *bt;
    BOOL b;

    if ( key < tmp->key )
        b = p_btree_node_insert( &tmp->less, key, val, tmp );

    else if ( key > tmp->key )
        b = p_btree_node_insert( &tmp->more, key, val, tmp );

    else
    {
        P_DEBUG( printf( "Error: btree duplicate key %I64u\n", key ); )
        return P_FALSE;
    }

    return b;
#endif
    return
        key < (*bt)->key ?
            p_btree_node_insert( &(*bt)->less, key, val, (*bt), mallocdoer )
        : key > (*bt)->key ?
            p_btree_node_insert( &(*bt)->more, key, val, (*bt), mallocdoer )
        : P_FALSE;
}

P_BOOL
p_btree_insert( p_BTree* bt,
        const P_ID key,
        const P_PTR val )
{
    P_BOOL b;
    P_ASSERT( bt )
    b = p_btree_node_insert( &bt->root, key, val, NULL, bt->mallocdoer );
    bt->root = p_btree_node_root( bt->root );
    return b;
}

P_BOOL
p_btree_node_remove( p_BTNode* bt,
        const P_ID key,
        P_VOID (*freedoer)( P_PTR ) )
{
    if ( !bt )
        return P_FALSE;

    if ( key == bt->key )
    {
        p_BTNode* root = bt->parent;

        if ( root && !bt->less && !bt->more )
        {
            P_TRACE( "-- Removing leaf k="P_ID_FMT"\n", bt->key )
            if ( root->less && root->less == bt )
                root->less = NULL;
            else
                root->more = NULL;

            p_btree_node_balance( root, 0, 0 );
        }
        else if ( root )
        {
            const P_UINT64 i = bt->less ? p_btree_node_num_levels( bt->less ) + 1 : 0;
            const P_UINT64 j = bt->more ? p_btree_node_num_levels( bt->more ) + 1 : 0;
            p_BTNode* piv = NULL;
            p_BTNode* tmp;
            P_TRACE( "-- Removing node k="P_ID_FMT"\n", bt->key )
            if ( i >= j )
            {
                piv = p_btree_node_get_node( bt->less, p_btree_node_max( bt->less ) );
                P_TRACE( "-- Using less pivot k="P_ID_FMT"\n", piv->key )
            }
            else
            {
                piv = p_btree_node_get_node( bt->more, p_btree_node_min( bt->more ) );
                P_TRACE( "-- Using more pivot k=%"P_ID_FMT"\n", piv->key )
            }
            P_ASSERT( piv )

            if ( piv->parent->less && piv->parent->less == piv )
                piv->parent->less = NULL;
            else
                piv->parent->more = NULL;

            if ( root->less && root->less == bt )
                root->less = piv;
            else
                root->more = piv;

            if ( bt->less )
            {
                piv->less = bt->less;
                bt->less->parent = piv;
            }
            if ( bt->more )
            {
                piv->more = bt->more;
                bt->more->parent = piv;
            }

            tmp = piv->parent;
            piv->parent = root;
            p_btree_node_balance( tmp, 0, 0 );
        }
        else if ( !root && ( bt->less || bt->more ) )
        {
            const P_UINT64 i = bt->less ? p_btree_node_num_levels( bt->less ) + 1 : 0;
            const P_UINT64 j = bt->more ? p_btree_node_num_levels( bt->more ) + 1 : 0;
            p_BTNode* piv = NULL;
            p_BTNode* tmp;
            P_TRACE( "-- Removing root node k="P_ID_FMT"\n", bt->key )
            if ( i >= j )
            {
                piv = p_btree_node_get_node( bt->less, p_btree_node_max( bt->less ) );
                P_TRACE( "-- Using less pivot k="P_ID_FMT"\n", piv->key )
            }
            else
            {
                piv = p_btree_node_get_node( bt->more, p_btree_node_min( bt->more ) );
                P_TRACE( "-- Using more pivot k="P_ID_FMT"\n", piv->key )
            }
            P_ASSERT( piv )

            if ( piv->parent->less && piv->parent->less == piv )
                piv->parent->less = NULL;
            else
                piv->parent->more = NULL;

            if ( bt->less )
            {
                piv->less = bt->less;
                bt->less->parent = piv;
            }
            if ( bt->more )
            {
                piv->more = bt->more;
                bt->more->parent = piv;
            }

            tmp = piv->parent;
            piv->parent = NULL;
            p_btree_node_balance( tmp, 0, 0 );
        }
        else { P_TRACE( "-- Removing last node k="P_ID_FMT"\n", bt->key ) }

        bt->less = bt->more = NULL;
        p_btree_node_delete( &bt, freedoer );
        return P_TRUE;
    }
#if 0
    if ( key < bt->key )
    {
        if ( !bt->less )
            return FALSE;
        return p_btree_node_remove( bt->less, key );
    }

    if ( !bt->more )
        return FALSE;
    return p_btree_node_remove( bt->more, key );
#endif
    return key < bt->key ?
        ( !bt->less ? P_FALSE : p_btree_node_remove( bt->less, key, freedoer ) )
        : ( !bt->more ? P_FALSE : p_btree_node_remove( bt->more, key, freedoer ) );
}

p_BTNode*
p_btree_node_safe_remove( p_BTNode* bt,
        const P_ID key,
        P_VOID (*freedoer)( P_PTR ) )
{
    P_ASSERT( bt )

    if ( !bt->parent && !bt->less && !bt->more )
    {
        if ( bt->key == key )
        {
            p_btree_node_delete( &bt, freedoer );
            return NULL;
        }
        return bt;
    }
    else if ( !bt->parent )
    {
        p_BTNode* tmp = bt->less ? bt->less : bt->more;

        if ( tmp->key == key )
            tmp = bt;
#if 0
        if ( p_btree_node_remove( bt, key ) )
            return p_btree_node_root( tmp );
        return bt;
#endif
        return p_btree_node_remove( bt, key, freedoer ) ?
            p_btree_node_root( tmp ) : bt;
    }
    else
    {
        p_btree_node_remove( bt, key, freedoer );
        return bt;
    }
}

P_VOID
p_btree_remove( p_BTree* bt,
        const P_ID key )
{
    P_ASSERT( bt )
    bt->root = p_btree_node_safe_remove( bt->root, key, bt->freedoer );
}

p_BTNode*
p_btree_node_get_node( const p_BTNode* bt,
        const P_ID key )
{
#if 0
    if ( !bt )
        return NULL;

    if ( key == bt->key )
        return (p_BTNode*) bt;

    if ( key < bt->key )
        return p_btree_node_get_node( bt->less, key );

    return p_btree_node_get_node( bt->more, key );
#endif
    return !bt ? NULL
        : key == bt->key ? (p_BTNode*) bt
        : key < bt->key ? p_btree_node_get_node( bt->less, key )
        : p_btree_node_get_node( bt->more, key );
}

P_PTR
p_btree_node_get( const p_BTNode* bt,
        const P_ID key )
{
#if 0
    if ( !bt )
        return NULL;

    if ( key == bt->key )
        return bt->val;

    if ( key < bt->key )
        return p_btree_node_get( bt->less, key );

    return p_btree_node_get( bt->more, key );
#endif
    return !bt ? NULL
        : key == bt->key ? bt->val
        : key < bt->key ? p_btree_node_get( bt->less, key )
        : p_btree_node_get( bt->more, key );
}

P_BOOL
p_btree_node_set( const p_BTNode* bt,
        const P_ID key,
        const P_PTR val )
{
    p_BTNode* nd = p_btree_node_get_node( bt, key );
    if ( !nd )
        return P_FALSE;
    nd->val = (P_PTR) val;
    return P_TRUE;
}

P_VOID
p_btree_node_num_nodes( const p_BTNode* bt,
        P_UINT64* i )
{
    P_ASSERT( bt )
    *i += 1;
    if ( bt->less )
        p_btree_node_num_nodes( bt->less, i );
    if ( bt->more )
        p_btree_node_num_nodes( bt->more, i );
}

P_UINT64
p_btree_node_count( const p_BTNode* bt )
{
    P_UINT64 i = 0;
    if ( !bt )
        return 0;
    p_btree_node_num_nodes( bt, &i );
    return i;
}

P_UINT64
p_btree_node_num_levels( const p_BTNode* bt )
{
    P_UINT64 lcnt, mcnt;
    P_ASSERT( bt )

    if ( !bt->less && !bt->more )
        return 0;

    lcnt = bt->less ? p_btree_node_num_levels( bt->less ) : 0;
    mcnt = bt->more ? p_btree_node_num_levels( bt->more ) : 0;

    return 1 + ( mcnt >= lcnt ? mcnt : lcnt );
}

P_VOID
p_btree_node_traverse( p_BTNode* bt,
        P_VOID (*func)( P_PTR, P_PTR ),
        P_PTR userdata )
{
    if ( !bt )
        return;

    if ( bt->less )
        p_btree_node_traverse( bt->less, func, userdata );

    if ( bt->more )
        p_btree_node_traverse( bt->more, func, userdata );

    if ( bt->val )
        (*func)( bt->val, userdata );
}

p_BTNode*
p_btree_node_root( p_BTNode* bt )
{
    P_ASSERT( bt )

    while ( bt->parent )
        bt = bt->parent;
    return bt;
}

p_BTNode*
p_btree_node_least( p_BTNode* bt )
{
    if ( !bt )
        return NULL;

    while ( bt->less )
        bt = bt->less;
    return bt;
}

p_BTNode*
p_btree_node_most( p_BTNode* bt )
{
    if ( !bt )
        return NULL;

    while ( bt->more )
        bt = bt->more;
    return bt;
}

p_BTNode*
p_btree_node_next( p_BTNode* bt )
{
    P_ASSERT( bt )

    if ( bt->more )
        return p_btree_node_least( bt->more );

    while ( bt->parent && bt->parent->key < bt->key )
        bt = bt->parent;
    return bt->parent;
}

p_BTNode*
p_btree_node_prev( p_BTNode* bt )
{
    P_ASSERT( bt )

    if ( bt->less )
        return p_btree_node_most( bt->less );

    while ( bt->parent && bt->parent->key > bt->key )
        bt = bt->parent;
    return bt->parent;
}

#if 0
int p_btree_node_min( BTree *bt )
{
    return p_btree_node_least( bt )->key;
#if 0
    P_ASSERT( bt );

    int min = bt->key;
    int tmp;

    if ( bt->less )
    {
        tmp = p_btree_node_min( bt->less );
        if ( tmp < min )
            min = tmp;
    }
    if ( bt->more )
    {
        tmp = p_btree_node_min( bt->more );
        if ( tmp < min )
            min = tmp;
    }

    return min;
#endif
}

int p_btree_node_max( BTree *bt )
{
    return p_btree_node_most( bt )->key;
#if 0
    P_ASSERT( bt );

    int max = bt->key;
    int tmp;

    if ( bt->less )
    {
        tmp = p_btree_node_max( bt->less );
        if ( tmp > max )
            max = tmp;
    }
    if ( bt->more )
    {
        tmp = p_btree_node_max( bt->more );
        if ( tmp > max )
            max = tmp;
    }

    return max;
#endif
}
#endif

P_VOID
p_btree_node_balance( p_BTNode* bt,
        const P_UINT64 levels_less,
        const P_UINT64 levels_more )
{
    P_UINT64 i, j;
    P_ASSERT( bt )
    P_TRACE( "-- BTree -- balance ("P_ID_FMT")\n", bt->key )

    i = levels_less ? levels_less + 1
        : bt->less ? p_btree_node_num_levels( bt->less ) + 1 : 0;
    j = levels_more ? levels_more + 1
        : bt->more ? p_btree_node_num_levels( bt->more ) + 1 : 0;

    if ( ( j >= i ? j - i : i - j ) > 1 )
    {
        p_BTNode* piv = NULL;
        p_BTNode* root = bt->parent;
        P_TRACE( "-- Balancing btree k="P_ID_FMT" f="P_UINT64_FMT"\n", bt->key, j - i )
        if ( j > i )
            piv = bt->more->less ? bt->more->less : bt->more;/*p_btree_node_get_node( bt->more, p_btree_node_min( bt->more ) );*/
        else
            piv = bt->less->more ? bt->less->more : bt->less;/*p_btree_node_get_node( bt->less, p_btree_node_max( bt->less ) );*/
        P_ASSERT( piv )
        P_TRACE( "-- Using pivot k="P_ID_FMT"\n", piv->key )
        if ( piv->parent == bt )
        {
            piv->parent = root;
            if ( root )
            {
                if ( piv->key < root->key )
                    root->less = piv;
                else
                    root->more = piv;
            }
            bt->parent = piv;
            if ( bt->less == piv )
            {
                P_TRACE( "-- LL rotation ("P_ID_FMT")\n", piv->key )
                bt->less = piv->more;
                if ( piv->more )
                    piv->more->parent = bt;
                piv->more = bt;
            }
            else
            {
                P_TRACE( "-- RR rotation ("P_ID_FMT")\n", piv->key )
                bt->more = piv->less;
                if ( piv->less )
                    piv->less->parent = bt;
                piv->less = bt;
            }
        }
        else
        {
            p_BTNode* least = p_btree_node_least( piv );
            p_BTNode* most = p_btree_node_most( piv );
            p_BTNode* tmp = piv->parent;
            piv->parent = root;
            if ( root )
            {
                if ( piv->key < root->key )
                    root->less = piv;
                else
                    root->more = piv;
            }
            if ( tmp->more == piv )
            {
                P_TRACE( "-- LR rotation ("P_ID_FMT")\n", piv->key )
                #if 0
                if ( bt->less == tmp->parent )
                {
                    bt->less->more = NULL;
                    bt->less->parent = tmp;
                    tmp->less = bt->less;
                }
                bt->less = piv->more;
                bt->parent = piv;
                tmp->more = piv->less;
                if ( piv->less )
                    piv->less->parent = tmp;
                tmp->parent = piv;
                piv->less = tmp;
                piv->more = bt;
                piv->parent = (p_BTNode*) root;
                #endif
                bt->less = NULL;
                bt->parent = most;
                most->more = bt;
                tmp->parent = least;
                least->less = tmp;
                tmp->more = NULL;

            }
            else
            {
                P_TRACE( "-- RL rotation ("P_ID_FMT")\n", piv->key )
                #if 0
                /*if ( bt->more == tmp->parent )
                {
                    bt->more->less = NULL;
                    bt->more->parent = tmp;
                    tmp->more = bt->more;
                }*/
                /*bt->more = piv->less;*/
                /*bt->parent = piv;*/
                /*tmp->less = piv->more;*/
                /*if ( piv->more )
                    piv->more->parent = tmp;*/
                /*tmp->parent = piv;*/
                /*piv->more = tmp;*/
                /*piv->less = bt;*/
                piv->parent = (p_BTNode*) root;
                #endif
                bt->more = NULL;
                bt->parent = least;
                least->less = bt;
                tmp->parent = most;
                most->more = tmp;
                tmp->less = NULL;
                piv->parent = root;
            }
        }
    }
    else if ( bt->parent )
    {
        if ( bt->parent->less == bt )
            p_btree_node_balance( bt->parent, i >= j ? i : j, 0 );
        else
            p_btree_node_balance( bt->parent, 0, i >= j ? i : j );
    }
}

P_SZ
p_btree_node_vectorize( const p_BTNode* bt,
        P_PTR* vec )
{
    P_SZ i = 0;
    p_BTNode* it;
    P_ASSERT( vec )

    if ( !bt )
        return 0;

    it = p_btree_node_least( (p_BTNode*) bt );

    for ( ; it; it = p_btree_node_next( it ) )
        vec[ i++ ] = it->val;

    return i;
}

P_VOID
p_btree_node_unvectorize( p_BTNode** bt,
        P_PTR* vec,
        const P_SZ sz,
        const p_BTNode* parent,
        P_PTR (*mallocdoer)( P_SZ ) )
{
    const P_SZ mid = sz / 2;
    P_ASSERT( vec )
    P_ASSERT( sz )
    P_ASSERT( *bt == NULL )

    p_btree_node_new( bt, 0, NULL, NULL, mallocdoer );

    (*bt)->key = *((P_ID*) vec[ mid ]);
    (*bt)->val = (P_PTR) vec[ mid ];
    (*bt)->parent = (p_BTNode*) parent;

    if ( mid )
    {
        p_BTNode* tmp = NULL;
        p_btree_node_unvectorize( &tmp, vec, mid,
                *bt, mallocdoer );
        (*bt)->less = tmp;
    }
    if ( sz > mid + 1 )
    {
        p_BTNode* tmp = NULL;
        p_btree_node_unvectorize( &tmp, &vec[ mid + 1 ], sz - ( mid + 1 ),
                *bt, mallocdoer );
        (*bt)->more = tmp;
    }
}

#ifndef NDEBUG

P_INT64
p_btree_node_balance_factor( const p_BTNode* bt )
{
    const P_UINT64 i = bt->less ? p_btree_node_num_levels( bt->less ) + 1 : 0;
    const P_UINT64 j = bt->more ? p_btree_node_num_levels( bt->more ) + 1 : 0;
    return j >= i ? j - i : i - j;
}

P_BOOL
p_btree_node_is_unbalanced( const p_BTNode* bt )
{
    P_BOOL b;
    printf( "-- Check wether btree node ("P_PTR_FMT") is balanced... ", (P_PTR)bt );
    b = p_btree_node_balance_factor( bt ) > 1 ? P_TRUE : P_FALSE;
    if ( b )
        printf( "Failed\n" );
    else
        printf( "OK\n" );
    return b;
}

P_VOID
p_btree_node_print_debug( const p_BTNode* bt )
{
    printf( "-- BTNode debug ("P_PTR_FMT"):\n"
            "--     Key =       "P_ID_FMT"\n"
            "--     Parent =    "P_ID_FMT"\n"
            "--     Less =      "P_ID_FMT"\n"
            "--     More =      "P_ID_FMT"\n"
            "-- end BTNode debug\n",
        (P_PTR)bt, bt->key,
        bt->parent ? bt->parent->key : 0,
        bt->less ? bt->less->key : 0,
        bt->more ? bt->more->key : 0 );
}

P_VOID
p_btree_node_debug( const p_BTNode* bt )
{
    p_btree_node_print_debug( bt );

    if ( bt->less )
        p_btree_node_debug( bt->less );
    if ( bt->more )
        p_btree_node_debug( bt->more );
}

P_VOID
p_btree_debug( const p_BTNode* bt )
{
    p_BTNode* it;

    it = p_btree_node_least( (p_BTNode*) bt );
    for ( ; it; it = p_btree_node_next( it ) )
        p_btree_node_print_debug( it );
}

/* Also serves as a memlist test */
#include "p_memlist.h"

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

#endif /* NDEBUG */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
