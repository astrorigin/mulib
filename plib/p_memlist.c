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

#include "p_memlist.h"

#ifndef NDEBUG

#include "p_sllist.h"

#undef P_TRACE
#if defined( P_TRACE_MODE ) && defined( P_TRACE_MEMLIST )
#define P_TRACE P_TRACER
#else
#define P_TRACE( moo, ... )
#endif

P_MUTEX_INIT( _p_memlist_mutex );

p_MemListNode*
_p_memlist_nodes = NULL;

P_PTR
p_memlist_malloc( const P_SZ sz )
{
    p_MemListNode* nd;

    P_TRACE( "-- MEMLIST -- allocating "P_SZ_FMT" bytes:\n", sz )

    if ( sz == 0 )
        return NULL;

    nd = malloc( sizeof( p_MemListNode ) + sz );
    if ( !nd )
        return NULL;

    nd->sz = sz;

    P_MEMLIST_LOCK();
    p_sllist_push( _p_memlist_nodes, nd );
    P_MEMLIST_UNLOCK();

    P_TRACE( "-- MEMLIST -- now tracking ("P_PTR_FMT")\n", nd->data )
    return nd->data;
}

P_VOID
p_memlist_free( P_PTR p )
{
    p_MemListNode* nd;
    p_MemListNode* prev = NULL;

    P_TRACE( "-- MEMLIST -- freeing tracked ("P_PTR_FMT")\n", p )

    if ( !p )
        return;

    P_MEMLIST_LOCK();
    P_ASSERT( _p_memlist_nodes )
    nd = _p_memlist_nodes;

    do
    {
        if ( nd->data == p )
        {
            p_sllist_take( _p_memlist_nodes, nd, prev );
            P_MEMLIST_UNLOCK();
            P_TRACE( "-- MEMLIST -- now freeing "SZ_FMT" bytes\n", nd->sz )
            free( nd );
            return;
        }
        prev = nd;
        nd = nd->next;
    }
    while ( nd );

    P_FATAL_ERROR( "pointer ("P_PTR_FMT") untracked", p );
}

P_PTR
p_memlist_realloc( P_PTR p,
        const P_SZ sz )
{
    p_MemListNode* nd;
    p_MemListNode* prev = NULL;

    if ( !p )
        return p_memlist_malloc( sz );

    if ( sz == 0 )
    {
        p_memlist_free( p );
        return NULL;
    }

    P_ASSERT( _p_memlist_nodes )
    P_MEMLIST_LOCK();
    nd = _p_memlist_nodes;

    do
    {
        if ( nd->data == p )
        {
            p_sllist_take( _p_memlist_nodes, nd, prev );
            P_MEMLIST_UNLOCK();
            P_TRACE( "-- MEMLIST -- reallocating "P_SZ_FMT" to "
                     P_SZ_FMT" bytes\n", nd->sz, sz )
            nd = realloc( nd, sizeof( p_MemListNode ) + sz );
            if ( !nd )
                return NULL;

            nd->sz = sz;

            P_MEMLIST_LOCK();
            p_sllist_push( _p_memlist_nodes, nd );
            P_MEMLIST_UNLOCK();

            return nd->data;
        }
        prev = nd;
        nd = nd->next;
    }
    while ( nd );

    P_FATAL_ERROR( "pointer ("P_PTR_FMT") untracked", p );
}

P_VOID
p_memlist_debug( P_VOID )
{
    P_SZ cnt = 0;
    P_SZ used = 0;
    p_MemListNode* nd;

    P_MEMLIST_LOCK();
    for ( nd = _p_memlist_nodes; nd; nd = nd->next )
    {
        cnt += 1;
        used += nd->sz;
    }
    P_MEMLIST_UNLOCK();

    printf( "-- MEMLIST -- debug:\n"
            "--     Count:  "P_SZ_FMT"\n"
            "--     Using:  "P_SZ_FMT"\n"
            "--     (Self:  "P_SZ_FMT")\n"
            "-- end memlist debug\n",
        cnt, used, cnt * sizeof( p_MemListNode ));
}

#endif /* NDEBUG */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
