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

#include <p_vdict.h>

#include "p_sllist.h"

#undef P_TRACE
#if defined( P_TRACE_MODE ) && defined( P_TRACE_VDICT )
#define P_TRACE P_TRACER
#else
#define P_TRACE( moo, ... )
#endif

P_VOID
p_vdict_fini( p_VDict* d )
{
    P_ASSERT( d )
    P_TRACE( "-- VDICT -- fini ("P_PTR_FMT")\n", (P_PTR)d )
    p_btree_traverse( (p_BTree*)d,
        (P_VOID(*)(P_PTR,P_PTR))&p_vdict_node_list_delete, NULL );
    p_btree_fini( (p_BTree*)d );
}

P_VOID
p_vdict_delete( p_VDict** d )
{
    P_ASSERT( *d )
    P_TRACE( "-- VDICT -- delete ("P_PTR_FMT")\n", (P_PTR)*d )
    p_vdict_fini( *d );
    P_FREE( *d, sizeof( p_VDict ));
    *d = NULL;
}

P_PTR
p_vdict_get( const p_VDict* d,
        const p_Vector* key )
{
    p_VDictNode* nd;
    P_ID k;
    P_ASSERT( d )

    if ( key == NULL || key->data == NULL )
        return NULL;
    k = p_vdict_hash( key->data, key->len );
    nd = p_btree_get( (p_BTree*)d, k );
    for ( ; nd; nd = nd->next )
    {
        if ( nd->key.len == key->len
            && !memcmp( nd->key.data, key->data, key->len ))
            return nd->val;
    }
    return NULL;
}

P_BOOL
p_vdict_set( p_VDict* d,
        const p_Vector* key,
        const P_PTR val,
        P_PTR* old )
{
    P_ID k;
    p_VDictNode* nd;
    P_ASSERT( d )

    if ( key == NULL || key->data == NULL )
        return P_TRUE;
    k = p_vdict_hash( key->data, key->len );
    nd = p_btree_get( (p_BTree*)d, k );
    if ( nd == NULL )
    {
        if ( !p_vdict_node_new( &nd, key, val ))
            return P_FALSE;
        if ( p_btree_insert( (p_BTree*)d, k, nd ) != 1 )
            return P_FALSE;
        if ( old )
            *old = (P_PTR) val;
    }
    else
    {
        p_VDictNode* last = NULL;
        for ( ; nd; nd = nd->next )
        {
            if ( nd->key.len == key->len
                && !memcmp( nd->key.data, key->data, key->len ))
                break;
            last = nd;
        }
        if ( nd == NULL )
        {
            P_ASSERT( last )
            if ( !p_vdict_node_new( &nd, key, val ))
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
p_vdict_unset( p_VDict* d,
        const p_Vector* key )
{
    p_VDictNode* nd, *first, *prev = NULL;
    P_ID k;
    P_ASSERT( d )

    if ( key == NULL || key->data == NULL )
        return NULL;
    k = p_vdict_hash( key->data, key->len );
    nd = p_btree_get( (p_BTree*)d, k );
    if ( !nd )
        return NULL;
    if ( !nd->next )
    {
        if ( nd->key.len == key->len
            && !memcmp( nd->key.data, key->data, key->len ))
        {
            P_PTR val = nd->val;
            p_btree_remove( (p_BTree*)d, k );
            p_vdict_node_delete( &nd );
            return val;
        }
        return NULL;
    }
    first = nd;
    for ( ; nd; nd = nd->next )
    {
        if ( nd->key.len == key->len
            && !memcmp( nd->key.data, key->data, key->len ))
        {
            P_PTR val = nd->val;
            if ( nd == first )
                p_btree_set( (p_BTree*)d, k, nd->next );
            p_sllist_take( first, nd, prev );
            p_vdict_node_delete( &nd );
            return val;
        }
        prev = nd;
    }
    return NULL;
}

P_VOID
p_vdict_traverse_keyval( p_VDict* d,
        P_VOID (*traverse_fn)( p_Vector*, P_PTR ) )
{
    p_BTNode* nd;
    p_VDictNode* n;
    P_ASSERT( d )

    nd = p_btree_least( (p_BTree*)d );
    for ( ; nd; nd = p_btree_next( nd ))
    {
        n = (p_VDictNode*) nd->val;
        for ( ; n; n = n->next )
        {
            (*traverse_fn)( &n->key, n->val );
        }
    }
}

P_VOID
p_vdict_traverse_keyval2( p_VDict* d,
        P_VOID (*traverse_fn)( p_Vector*, P_PTR, P_PTR ),
        P_PTR userdata )
{
    p_BTNode* nd;
    p_VDictNode* n;
    P_ASSERT( d )

    nd = p_btree_least( (p_BTree*)d );
    for ( ; nd; nd = p_btree_next( nd ))
    {
        n = (p_VDictNode*) nd->val;
        for ( ; n; n = n->next )
        {
            (*traverse_fn)( &n->key, n->val, userdata );
        }
    }
}

P_BOOL
p_vdict_node_new( p_VDictNode** nd,
        const p_Vector* key,
        const P_PTR val )
{
    P_ASSERT( nd )
    *nd = P_MALLOC( sizeof( p_VDictNode ));
    P_ASSERT( *nd )
#ifdef NDEBUG
    if ( !*nd )
        return P_FALSE;
#endif
    return p_vdict_node_init( *nd, key, val );
}

P_VOID
p_vdict_node_delete( p_VDictNode** nd )
{
    P_ASSERT( *nd )
    p_vdict_node_fini( *nd );
    P_FREE( *nd, sizeof( p_VDictNode ));
    *nd = NULL;
}

P_VOID
p_vdict_node_list_delete( p_VDictNode* nd,
        P_PTR userdata )
{
    p_VDictNode* next;
    P_ASSERT( nd )
    while ( nd )
    {
        next = nd->next;
        p_vdict_node_delete( &nd );
        nd = next;
    }
    P_UNUSED( userdata );
}

P_BOOL
p_vdict_node_init( p_VDictNode* nd,
        const p_Vector* key,
        const P_PTR val )
{
    P_ASSERT( nd )
    nd->next = NULL;
    if ( !p_vector_init( &nd->key, key->len, key->unit, key->calc_space_fn ))
        return P_FALSE;
    if ( key->len )
        memcpy( &nd->key.data, key->data, key->len );
    nd->val = (P_PTR) val;
    return P_TRUE;
}

P_VOID
p_vdict_node_fini( p_VDictNode* nd )
{
    P_ASSERT( nd )
    nd->next = NULL;
    p_vector_fini( &nd->key );
    nd->val = NULL;
}

P_BOOL
p_vdict_node_ro_new( p_VDictNode** nd,
        const P_PTR data,
        const P_SZ len,
        const P_PTR val )
{
    P_ASSERT( nd )
    *nd = P_MALLOC( sizeof( p_VDictNode ));
    P_ASSERT( *nd )
#ifdef NDEBUG
    if ( !*nd )
        return P_FALSE;
#endif
    return p_vdict_node_ro_init( *nd, data, len, val );
}

P_VOID
p_vdict_node_ro_delete( p_VDictNode** nd )
{
    P_ASSERT( *nd )
    p_vdict_node_ro_fini( *nd );
    P_FREE( *nd, sizeof( p_VDictNode ));
    *nd = NULL;
}

P_VOID
p_vdict_node_ro_list_delete( p_VDictNode* nd,
        P_PTR userdata )
{
    p_VDictNode* next;
    P_ASSERT( nd )
    while ( nd )
    {
        next = nd->next;
        p_vdict_node_ro_delete( &nd );
        nd = next;
    }
    P_UNUSED( userdata );
}

P_BOOL
p_vdict_node_ro_init( p_VDictNode* nd,
        const P_PTR data,
        const P_SZ len,
        const P_PTR val )
{
    P_ASSERT( nd )
    P_ASSERT( data )
    nd->next = NULL;
    nd->key.data = (P_PTR) data;
    nd->key.len = (P_SZ) len;
    nd->key.unit = 1;
    nd->key.capacity = 0;
    nd->key.calc_space_fn = NULL;
    nd->val = (P_PTR) val;
    return P_TRUE;
}

P_BOOL
p_vdict_ro_set( p_VDict* d,
        const P_PTR data,
        const P_SZ len,
        const P_PTR val,
        P_PTR* old )
{
    P_ID k;
    p_VDictNode* nd;
    P_ASSERT( d )

    if ( data == NULL || len == 0 )
        return P_TRUE;
    k = p_vdict_hash( data, len );
    nd = p_btree_get( (p_BTree*)d, k );
    if ( nd == NULL )
    {
        if ( !p_vdict_node_ro_new( &nd, data, len, val ))
            return P_FALSE;
        if ( p_btree_insert( (p_BTree*)d, k, nd ) != 1 )
            return P_FALSE;
        if ( old )
            *old = (P_PTR) val;
    }
    else
    {
        p_VDictNode* last = NULL;
        for ( ; nd; nd = nd->next )
        {
            if ( nd->key.len == len
                && !memcmp( nd->key.data, data, len ))
                break;
            last = nd;
        }
        if ( nd == NULL )
        {
            P_ASSERT( last )
            if ( !p_vdict_node_ro_new( &nd, data, len, val ))
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

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
