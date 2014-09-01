/*
    Plib
    Copyright (C) 2014 Stanislas Marquis <smarquis@astrorigin.ch>

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

#include "p_dict.h"

#include "p_sllist.h"

#undef P_TRACE
#if defined( P_TRACE_MODE ) && defined( P_TRACE_DICT )
#define P_TRACE P_TRACER
#else
#define P_TRACE( moo, ... )
#endif

P_VOID
p_dict_fini( p_Dict* d )
{
    P_ASSERT( d )
    P_TRACE( "-- DICT -- fini ("P_PTR_FMT")\n", (P_PTR)d )
    p_btree_traverse( (p_BTree*)d,
        (P_VOID(*)(P_PTR,P_PTR))&p_dict_node_list_delete, NULL );
    p_btree_fini( (p_BTree*)d );
}

P_VOID
p_dict_delete( p_Dict** d )
{
    P_ASSERT( *d )
    P_TRACE( "-- DICT -- delete ("P_PTR_FMT")\n", (P_PTR)*d )
    p_dict_fini( *d );
    P_FREE( *d, sizeof( p_Dict ));
    *d = NULL;
}

P_ID
p_dict_hash( const P_PTR k,
        const P_SZ len )
{ /* this function is public domain */
    P_UCHAR *p = (P_UCHAR*) k;
    P_ID h = 0;
    P_SZ i;

    for ( i = 0; i < len; i++ )
    {
        h += p[i];
        h += ( h << 10 );
        h ^= ( h >> 6 );
    }
    h += ( h << 3 );
    h ^= ( h >> 11 );
    h += ( h << 15 );
    return h;
}

P_PTR
p_dict_get( const p_Dict* d,
        const P_CHAR* key )
{
    p_DictNode* nd;
    P_ID k;
    P_ASSERT( d )

    if ( key == NULL || key[0] == '\0' )
        return NULL;
    k = p_dict_hash( key, strlen( key ));
    nd = p_btree_get( (p_BTree*)d, k );
    for ( ; nd; nd = nd->next )
    {
        if ( !strcmp( nd->key.data, key ))
            return nd->val;
    }
    return NULL;
}

P_BOOL
p_dict_set( p_Dict* d,
        const P_CHAR* key,
        const P_PTR val,
        P_PTR* old )
{
    P_ID k;
    p_DictNode* nd;
    P_ASSERT( d )

    if ( key == NULL || key[0] == '\0' )
        return P_TRUE;
    k = p_dict_hash( key, strlen( key ));
    nd = p_btree_get( (p_BTree*)d, k );
    if ( nd == NULL )
    {
        if ( !p_dict_node_new( &nd, key, val ))
            return P_FALSE;
        if ( p_btree_insert( (p_BTree*)d, k, nd ) != 1 )
            return P_FALSE;
        if ( old )
            *old = (P_PTR) val;
    }
    else
    {
        p_DictNode* last = NULL;
        for ( ; nd; nd = nd->next )
        {
            if ( !strcmp( nd->key.data, key ))
                break;
            last = nd;
        }
        if ( nd == NULL )
        {
            P_ASSERT( last )
            if ( !p_dict_node_new( &nd, key, val ))
                return P_FALSE;
            last->next = nd;
            if ( old )
                *old = (P_PTR) val;
        }
        else
        {
            if ( old )
                *old = nd->val;
            nd->val = (P_PTR) val;
        }
    }
    return P_TRUE;
}

P_PTR
p_dict_unset( p_Dict* d,
        const P_CHAR* key )
{
    p_DictNode* nd, *first, *prev = NULL;
    P_ID k;
    P_ASSERT( d )

    if ( key == NULL || key[0] == '\0' )
        return NULL;
    k = p_dict_hash( key, strlen( key ));
    nd = p_btree_get( (p_BTree*)d, k );
    if ( !nd )
        return NULL;
    if ( !nd->next )
    {
        if ( !strcmp( nd->key.data, key ))
        {
            P_PTR val = nd->val;
            p_btree_remove( (p_BTree*)d, k );
            p_dict_node_delete( &nd );
            return val;
        }
        return NULL;
    }
    first = nd;
    for ( ; nd; nd = nd->next )
    {
        if ( !strcmp( nd->key.data, key ))
        {
            P_PTR val = nd->val;
            if ( nd == first )
                p_btree_set( (p_BTree*)d, k, nd->next );
            p_sllist_take( first, nd, prev );
            p_dict_node_delete( &nd );
            return val;
        }
        prev = nd;
    }
    return NULL;
}

P_VOID
p_dict_traverse( p_Dict* d,
        P_VOID (*func)( P_PTR ) )
{
    p_BTNode* nd;
    p_DictNode* n;
    P_ASSERT( d )

    nd = p_btree_least( (p_BTree*)d );
    for ( ; nd; nd = p_btree_next( nd ))
    {
        n = (p_DictNode*) nd->val;
        for ( ; n; n = n->next )
        {
            (*func)( n->val );
        }
    }
}

P_VOID
p_dict_traverse2( p_Dict* d,
        P_VOID (*func)( P_PTR, P_PTR ),
        P_PTR userdata )
{
    p_BTNode* nd;
    p_DictNode* n;
    P_ASSERT( d )

    nd = p_btree_least( (p_BTree*)d );
    for ( ; nd; nd = p_btree_next( nd ))
    {
        n = (p_DictNode*) nd->val;
        for ( ; n; n = n->next )
        {
            (*func)( n->val, userdata );
        }
    }
}

P_VOID
p_dict_traverse_keyval( p_Dict* d,
        P_VOID (*func)( p_String*, P_PTR ) )
{
    p_BTNode* nd;
    p_DictNode* n;
    P_ASSERT( d )

    nd = p_btree_least( (p_BTree*)d );
    for ( ; nd; nd = p_btree_next( nd ))
    {
        n = (p_DictNode*) nd->val;
        for ( ; n; n = n->next )
        {
            (*func)( &n->key, n->val );
        }
    }
}

P_VOID
p_dict_traverse_keyval2( p_Dict* d,
        P_VOID (*func)( p_String*, P_PTR, P_PTR ),
        P_PTR userdata )
{
    p_BTNode* nd;
    p_DictNode* n;
    P_ASSERT( d )

    nd = p_btree_least( (p_BTree*)d );
    for ( ; nd; nd = p_btree_next( nd ))
    {
        n = (p_DictNode*) nd->val;
        for ( ; n; n = n->next )
        {
            (*func)( &n->key, n->val, userdata );
        }
    }
}

P_BOOL
p_dict_node_new( p_DictNode** nd,
        const P_CHAR* key,
        const P_PTR val )
{
    P_ASSERT( nd )
    *nd = P_MALLOC( sizeof( p_DictNode ));
    P_ASSERT( *nd )
#ifdef NDEBUG
    if ( !*nd )
        return P_FALSE;
#endif
    return p_dict_node_init( *nd, key, val );
}

P_VOID
p_dict_node_delete( p_DictNode** nd )
{
    P_ASSERT( *nd )
    p_dict_node_fini( *nd );
    P_FREE( *nd, sizeof( p_DictNode ));
    *nd = NULL;
}

P_VOID
p_dict_node_list_delete( p_DictNode* nd,
        P_PTR userdata )
{
    p_DictNode* next;
    P_ASSERT( nd )
    while ( nd )
    {
        next = nd->next;
        p_dict_node_delete( &nd );
        nd = next;
    }
    P_UNUSED( userdata );
}

P_BOOL
p_dict_node_init( p_DictNode* nd,
        const P_CHAR* key,
        const P_PTR val )
{
    P_ASSERT( nd )
    nd->next = NULL;
    if ( !p_string_init( &nd->key, key ))
        return P_FALSE;
    nd->val = (P_PTR) val;
    return P_TRUE;
}

P_VOID
p_dict_node_fini( p_DictNode* nd )
{
    P_ASSERT( nd )
    nd->next = NULL;
    p_string_fini( &nd->key );
    nd->val = NULL;
}

#ifndef NDEBUG

P_VOID
p_dict_debug( const p_Dict* d )
{
    p_BTNode* btn;
    p_DictNode* dn;

    P_ASSERT( d );

    printf( "-- DICT -- debug ("P_PTR_FMT"):\n", d );

    btn = p_btree_least( (p_BTree*)d );
    for ( ; btn; btn = p_btree_next( btn ))
    {
        dn = btn->val;
        for ( ; dn; dn = dn->next )
        {
            printf( "--     \"%s\": ("P_PTR_FMT")\n",
                (P_CHAR*)dn->key.data, dn->val );
        }
    }

    printf( "-- end dict debug\n" );
}

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
#endif /* NDEBUG */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
