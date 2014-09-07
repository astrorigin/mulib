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

#include "p_shell.h"

#include "p_btree.h"
#include "p_dict.h"
#include "p_dllist.h"
#include "p_mempool.h"
#include "p_sllist.h"
#include "p_string.h"
#include "p_vdict.h"
#include "p_vector.h"

P_BOOL
p_shell_new( p_Shell** sh,
        const P_SHELLTYPE tp )
{
    P_ASSERT( sh )

    if ( !( *sh = P_MALLOC( sizeof( p_Shell ))))
        return P_FALSE;
    return p_shell_init( *sh, tp );
}

P_VOID
p_shell_delete( p_Shell** sh )
{
    P_ASSERT( sh )

    p_shell_fini( *sh );
    P_FREE( *sh, sizeof( p_Shell ));
    *sh = NULL;
}

P_VOID
p_shell_delete_by_val( p_Shell* sh )
{
    P_ASSERT( sh )
    p_shell_delete( &sh );
}

P_BOOL
p_shell_init( p_Shell* sh,
        const P_SHELLTYPE tp )
{
    P_ASSERT( sh )

    memset( sh, 0, sizeof( p_Shell ));

    if ( tp != P_SHELLTYPE_NULL )
    {
        if ( !p_shell_set_type( sh, tp ))
            return P_FALSE;
    }
    return P_TRUE;
}

P_VOID
p_shell_fini( p_Shell* sh )
{
    P_ASSERT( sh )

    if ( sh->type != P_SHELLTYPE_NULL && sh->p )
    {
        switch ( sh->type )
        {
        /* standard types */
        case P_SHELLTYPE_CHAR:
        case P_SHELLTYPE_UCHAR:
        case P_SHELLTYPE_INT8:
        case P_SHELLTYPE_UINT8:
        case P_SHELLTYPE_BOOL:
            P_FREE( sh->p, sizeof( P_CHAR ));
            break;
        case P_SHELLTYPE_INT16:
        case P_SHELLTYPE_UINT16:
            P_FREE( sh->p, sizeof( P_INT16 ));
            break;
        case P_SHELLTYPE_INT32:
        case P_SHELLTYPE_UINT32:
            P_FREE( sh->p, sizeof( P_INT32 ));
            break;
        case P_SHELLTYPE_INT64:
        case P_SHELLTYPE_UINT64:
        case P_SHELLTYPE_ID:
            P_FREE( sh->p, sizeof( P_INT64 ));
            break;
        case P_SHELLTYPE_DBL:
            P_FREE( sh->p, sizeof( P_DBL ));
            break;
        case P_SHELLTYPE_WCHAR:
            P_FREE( sh->p, sizeof( P_WCHAR ));
            break;
        case P_SHELLTYPE_SZ:
            P_FREE( sh->p, sizeof( P_SZ ));
            break;
        case P_SHELLTYPE_CSTR:
        case P_SHELLTYPE_PTR:
            if ( sh->freedoer )
                (*sh->freedoer)( sh->p );
            break;
        /* P types */
        case P_SHELLTYPE_BTREE:
            p_btree_delete( (p_BTree**)&sh->p );
            break;
        case P_SHELLTYPE_DICT:
            p_dict_delete( (p_Dict**)&sh->p );
            break;
        case P_SHELLTYPE_DLLIST:
        case P_SHELLTYPE_SLLIST:
            if ( sh->freedoer )
            {
                p_SlList* p = sh->p, *pp;
                while ( p )
                {
                    pp = p->next;
                    (*sh->freedoer)( p );
                    p = pp;
                }
            }
            break;
        case P_SHELLTYPE_STRING:
            p_string_delete( (p_String**)&sh->p );
            break;
        case P_SHELLTYPE_VDICT:
            p_vdict_delete( (p_VDict**)&sh->p );
            break;
        case P_SHELLTYPE_VECTOR: /* uninitialized? */
            if ( ((p_Vector*)sh->p)->data )
                p_vector_fini( sh->p );
            P_FREE( sh->p, sizeof( p_Vector ));
            break;
        default: /* dummy warning */
            break;
        }
    }
    memset( sh, 0, sizeof( p_Shell ));
}

P_BOOL
p_shell_set_type( p_Shell* sh,
        const P_SHELLTYPE tp )
{
    P_ASSERT( sh )

    switch ( tp )
    {
    /* standard types */
    case P_SHELLTYPE_CHAR:
    case P_SHELLTYPE_UCHAR:
    case P_SHELLTYPE_INT8:
    case P_SHELLTYPE_UINT8:
    case P_SHELLTYPE_BOOL:
        if ( !( sh->p = P_MALLOC( sizeof( P_CHAR ))))
            return P_FALSE;
        break;
    case P_SHELLTYPE_INT16:
    case P_SHELLTYPE_UINT16:
        if ( !( sh->p = P_MALLOC( sizeof( P_INT16 ))))
            return P_FALSE;
        break;
    case P_SHELLTYPE_INT32:
    case P_SHELLTYPE_UINT32:
        if ( !( sh->p = P_MALLOC( sizeof( P_INT32 ))))
            return P_FALSE;
        break;
    case P_SHELLTYPE_INT64:
    case P_SHELLTYPE_UINT64:
    case P_SHELLTYPE_ID:
        if ( !( sh->p = P_MALLOC( sizeof( P_INT64 ))))
            return P_FALSE;
        break;
    case P_SHELLTYPE_DBL:
        if ( !( sh->p = P_MALLOC( sizeof( P_DBL ))))
            return P_FALSE;
        break;
    case P_SHELLTYPE_WCHAR:
        if ( !( sh->p = P_MALLOC( sizeof( P_WCHAR ))))
            return P_FALSE;
        break;
    case P_SHELLTYPE_SZ:
        if ( !( sh->p = P_MALLOC( sizeof( P_SZ ))))
            return P_FALSE;
        break;
    case P_SHELLTYPE_CSTR:
    case P_SHELLTYPE_PTR: /* null pointer */
        break;
    /* P types */
    case P_SHELLTYPE_BTREE:
        if ( !p_btree_new( (p_BTree**)&sh->p ))
            return P_FALSE;
        break;
    case P_SHELLTYPE_DICT:
        if ( !p_dict_new( (p_Dict**)&sh->p ))
            return P_FALSE;
        break;
    case P_SHELLTYPE_DLLIST:
    case P_SHELLTYPE_SLLIST: /* null pointer */
        break;
    case P_SHELLTYPE_STRING:
        if ( !p_string_new( (p_String**)&sh->p, NULL ))
            return P_FALSE;
        break;
    case P_SHELLTYPE_VDICT:
        if ( !p_vdict_new( (p_VDict**)&sh->p ))
            return P_FALSE;
        break;
    case P_SHELLTYPE_VECTOR: /* uninitialized */
        if ( !( sh->p = P_MALLOC( sizeof( p_Vector ))))
            return P_FALSE;
        memset( sh->p, 0, sizeof( p_Vector ));
        break;
    default: /* dummy warning */
        break;
    }
    sh->type = tp;
    return P_TRUE;
}

P_BOOL
p_shellelem_new( p_ShellElem** sh,
        const P_SHELLTYPE tp )
{
    P_ASSERT( sh )
    if ( !( *sh = P_MALLOC( sizeof( p_ShellElem ))))
        return P_FALSE;
    return p_shellelem_init( *sh, tp );
}

P_VOID
p_shellelem_delete( p_ShellElem** sh )
{
    P_ASSERT( sh )
    p_shellelem_fini( *sh );
    P_FREE( *sh, sizeof( p_ShellElem ));
    *sh = NULL;
}

P_BOOL
p_shellelem_init( p_ShellElem* sh,
        const P_SHELLTYPE tp )
{
    P_ASSERT( sh )
    sh->next = NULL;
    return p_shell_init( &sh->shell, tp );
}

P_VOID
p_shellelem_fini( p_ShellElem* sh )
{
    P_ASSERT( sh )
    if ( sh->next )
        sh->next = NULL;
    p_shell_fini( &sh->shell );
}

P_VOID
p_shelllist_delete( p_ShellElem* lst )
{
    p_ShellElem* el;
    while ( lst )
    {
        el = lst->next;
        p_shellelem_fini( lst );
        lst = el;
    }
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
