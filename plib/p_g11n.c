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

#include "p_g11n.h"

#include "p_vector.h"

p_l10n_Translations*
_p_l10n_t = NULL;

P_CHAR*
p_i18n( const P_CHAR* txt )
{
    P_CHAR* tr;

    P_ASSERT( txt && txt[0] )

    if ( !_p_l10n_t )
        return (P_CHAR*) txt;

    if ( !( tr = p_dict_get( _p_l10n_t->l10n_dict, txt )))
        return (P_CHAR*) txt;
    return tr;
}

P_CHAR*
p_i18n_c( const P_CHAR* ctxt,
        const P_CHAR* txt )
{
    P_CHAR* tr;
    P_SZ len, len1, len2;
    p_Vector vec;

    P_ASSERT( ctxt && ctxt[0] )
    P_ASSERT( txt && txt[0] )

    if ( !_p_l10n_t )
        return (P_CHAR*) txt;

    len1 = strlen( ctxt );
    len2 = strlen( txt );
    len = len1 + len2 + 2;
    if ( !p_vector_init( &vec, len, 1, NULL ))
        return (P_CHAR*) txt;
    ((P_CHAR*)vec.data)[0] = '\0';
    strcat( (P_CHAR*)vec.data, ctxt );
    strcpy( &((P_CHAR*)vec.data)[ len1 + 1 ], txt );
    vec.len = len;
    tr = p_vdict_get( _p_l10n_t->l10n_c_vdict, &vec );
    p_vector_fini( &vec );
    return tr ? tr : (P_CHAR*) txt;
}

P_CHAR*
p_i18n_p( const P_CHAR* txt,
        const P_CHAR* plural,
        const P_SZ count )
{
    P_CHAR* tr;

    P_ASSERT( txt && txt[0] )
    P_ASSERT( plural && plural[0] )

    if ( !_p_l10n_t )
        return count < 2 ? (P_CHAR*) txt : (P_CHAR*) plural;

    if ( !( tr = p_dict_get( count < 2 ?
        _p_l10n_t->l10n_p_dict_1 : _p_l10n_t->l10n_p_dict_2,
        count < 2 ? txt : plural )))
        return count < 2 ? (P_CHAR*) txt : (P_CHAR*) plural;
    return tr;
}

P_CHAR*
p_i18n_cp( const P_CHAR* ctxt,
        const P_CHAR* txt,
        const P_CHAR* plural,
        const P_SZ count )
{
    P_CHAR* tr;
    P_SZ len, len1, len2;
    p_Vector vec;

    P_ASSERT( ctxt && ctxt[0] )
    P_ASSERT( txt && txt[0] )
    P_ASSERT( plural && plural[0] )

    if ( !_p_l10n_t )
        return count < 2 ? (P_CHAR*) txt : (P_CHAR*) plural;

    len1 = strlen( ctxt );
    len = len1 + ( count < 2 ?
        ( len2 = strlen( txt )) : ( len2 = strlen( plural ))) + 2;
    if ( !p_vector_init( &vec, len, 1, NULL ))
        return count < 2 ? (P_CHAR*) txt : (P_CHAR*) plural;
    ((P_CHAR*)vec.data)[0] = '\0';
    strcat( (P_CHAR*)vec.data, ctxt );
    strcpy( &((P_CHAR*)vec.data)[ len1 + 1 ], count < 2 ? txt : plural );
    vec.len = len;
    tr = p_vdict_get( count < 2 ?
        _p_l10n_t->l10n_cp_vdict_1 : _p_l10n_t->l10n_cp_vdict_2, &vec );
    p_vector_fini( &vec );
    return tr ? tr : count < 2 ? (P_CHAR*) txt : (P_CHAR*) plural;
}

P_BOOL
p_l10n_init( p_l10n_Translations* t )
{
    P_BOOL flag = P_FALSE;
    P_ASSERT( t )
    if ( !p_dict_new( &t->l10n_dict ))
        goto finish;
    if ( !p_vdict_new( &t->l10n_c_vdict ))
        goto finish;
    if ( !p_dict_new( &t->l10n_p_dict_1 ))
        goto finish;
    if ( !p_dict_new( &t->l10n_p_dict_2 ))
        goto finish;
    if ( !p_vdict_new( &t->l10n_cp_vdict_1 ))
        goto finish;
    if ( !p_vdict_new( &t->l10n_cp_vdict_2 ))
        goto finish;
    flag = P_TRUE;
    finish:
    if ( !flag )
        p_l10n_fini( t );
    return flag;
}

P_VOID
p_l10n_fini( p_l10n_Translations* t )
{
    P_ASSERT( t )
    if ( t->l10n_dict )
        p_dict_delete( &t->l10n_dict );
    if ( t->l10n_c_vdict )
        p_vdict_delete( &t->l10n_c_vdict );
    if ( t->l10n_p_dict_1 )
        p_dict_delete( &t->l10n_p_dict_1 );
    if ( t->l10n_p_dict_2 )
        p_dict_delete( &t->l10n_p_dict_2 );
    if ( t->l10n_cp_vdict_1 )
        p_vdict_delete( &t->l10n_cp_vdict_1 );
    if ( t->l10n_cp_vdict_2 )
        p_vdict_delete( &t->l10n_cp_vdict_2 );
}

P_BOOL
p_l10n_load( p_l10n_Translations* t,
        const P_CHAR* strings,
        const P_SZ len )
{
    P_CHAR* p, *q;
    P_CHAR* ctxt;
    P_CHAR* txt;
    P_CHAR* plural;
    P_CHAR* tr1;
    P_CHAR* tr2;

    P_ASSERT( strings )
    P_ASSERT( len )

    p = (P_CHAR*) strings;
    while ( 1 )
    {
        if ( ( q = strchr( p, '\0' )) == p )
            ctxt = NULL;
        else
            ctxt = p;
        P_ASSERT( q + 1 < strings + len )
        P_ASSERT( *( q + 1 ))
        p = strchr( ( txt = ++q ), '\0' );
        P_ASSERT( p + 1 < strings + len )
        ++p;
        if ( ( q = strchr( p, '\0' )) == p )
            plural = NULL;
        else
            plural = p;
        P_ASSERT( q + 1 < strings + len )
        P_ASSERT( *( q + 1 ))
        p = strchr( ( tr1 = ++q ), '\0' );
        P_ASSERT( p + 1 <= strings + len )
        ++p;
        if ( ( q = strchr( p, '\0' )) == p )
            tr2 = NULL;
        else
            tr2 = p;
        P_ASSERT( q <= strings + len )

        P_ASSERT( txt && tr1 );
        P_ASSERT( ( plural && tr2 ) || !tr2 )

        if ( !ctxt )
        {
            if ( !plural )
            {
                if ( !p_dict_set( t->l10n_dict, txt, tr1, NULL ))
                    return P_FALSE;
            }
            else
            {
                if ( !p_dict_set( t->l10n_p_dict_1, txt, tr1, NULL ))
                    return P_FALSE;
                if ( !p_dict_set( t->l10n_p_dict_2, plural, tr2, NULL ))
                    return P_FALSE;
            }
        }
        else /* with context */
        {
            if ( !plural )
            {
                p_Vector vec;
                P_SZ z = strlen( ctxt );
                P_SZ len = z + strlen( txt ) + 2;
                if ( !p_vector_init( &vec, len, 1, NULL ))
                    return P_FALSE;
                ((P_CHAR*)vec.data)[0] = '\0';
                strcat( (P_CHAR*)vec.data, ctxt );
                strcpy( &((P_CHAR*)vec.data)[ z + 1 ], txt );
                vec.len = len;
                if ( !p_vdict_set( t->l10n_c_vdict, &vec, tr1, NULL ))
                {
                    p_vector_fini( &vec );
                    return P_FALSE;
                }
                p_vector_fini( &vec );
            }
            else
            {
                p_Vector vec;
                P_SZ z = strlen( ctxt );
                P_SZ len1 = z + strlen( txt ) + 2;
                P_SZ len2 = z + strlen( plural ) + 2;
                if ( !p_vector_init( &vec, P_MAX( len1, len2 ), 1, NULL ))
                    return P_FALSE;
                ((P_CHAR*)vec.data)[0] = '\0';
                strcat( (P_CHAR*)vec.data, ctxt );
                strcpy( &((P_CHAR*)vec.data)[ z + 1 ], txt );
                vec.len = len1;
                if ( !p_vdict_set( t->l10n_cp_vdict_1, &vec, tr1, NULL ))
                {
                    p_vector_fini( &vec );
                    return P_FALSE;
                }
                strcpy( &((P_CHAR*)vec.data)[ z + 1 ], plural );
                vec.len = len2;
                if ( !p_vdict_set( t->l10n_cp_vdict_2, &vec, tr2, NULL ))
                {
                    p_vector_fini( &vec );
                    return P_FALSE;
                }
                p_vector_fini( &vec );
            }
        }
        if ( ( p = ++q ) == strings + len )
            break;
    }
    return P_TRUE;
}

#ifndef NDEBUG

#include <p_mempool.h>

P_INT32
p_l10n_test( P_VOID )
{
#ifndef P_NO_MEMPOOL
    p_MemPool mp;
#endif

    const char strings[] = {
    0x0,0x48,0x65,0x6C,0x6C,0x6F,0x20,0x54,0x72,0x61,0x6E,0x73,0x6C,0x61,0x74,0x69,
    0x6F,0x6E,0x0,0x0,0x47,0x75,0x74,0x65,0x6E,0x20,0x54,0x61,0x67,0x0,0x0,0x43,0x6F,
    0x6E,0x74,0x65,0x78,0x74,0x75,0x61,0x6C,0x20,0x69,0x6E,0x66,0x6F,0x72,0x6D,0x61,
    0x74,0x69,0x6F,0x6E,0x0,0x43,0x6C,0x69,0x63,0x6B,0x20,0x68,0x65,0x72,0x65,0x0,
    0x0,0x43,0x6C,0x69,0x63,0x6B,0x20,0x64,0x61,0x0,0x0,0x0,0x49,0x20,0x61,0x6D,0x0,
    0x57,0x65,0x20,0x61,0x72,0x65,0x0,0x49,0x63,0x68,0x20,0x62,0x69,0x6E,0x0,0x57,
    0x69,0x72,0x20,0x73,0x69,0x6E,0x64,0x0,0x43,0x6F,0x6D,0x70,0x6C,0x69,0x63,0x61,
    0x74,0x65,0x64,0x0,0x79,0x6F,0x75,0x20,0x67,0x6F,0x74,0x20,0x69,0x74,0x0,0x79,
    0x6F,0x75,0x20,0x67,0x6F,0x74,0x20,0x74,0x68,0x65,0x6D,0x0,0x74,0x75,0x20,0x61,
    0x73,0x20,0x63,0x6F,0x6D,0x70,0x72,0x69,0x73,0x0,0x74,0x75,0x20,0x6C,0x65,0x73,
    0x20,0x61,0x73,0x20,0x65,0x75,0x0
    };

    const size_t sizeof_strings = 169;

    p_l10n_Translations t;

#ifndef P_NO_MEMPOOL
    p_mempool_set( &mp );
    p_mempool_init( &mp, 0 );
#endif

    P_ASSERT( p_l10n_init( &t ) )
    P_ASSERT( p_l10n_load( &t, strings, sizeof_strings ) )

    _p_l10n_t = &t;

    P_ASSERT( !strcmp( p_i18n( "Hello Translation" ), "Guten Tag" ))
    P_ASSERT( !strcmp( p_i18n_c( "Contextual information", "Click here" ), "Click da" ))
    P_ASSERT( !strcmp( p_i18n_p( "I am", "We are", 1 ), "Ich bin" ))
    P_ASSERT( !strcmp( p_i18n_p( "I am", "We are", 2 ), "Wir sind" ))
    P_ASSERT( !strcmp( p_i18n_cp( "Complicated", "you got it", "you got them", 1 ), "tu as compris" ))
    P_ASSERT( !strcmp( p_i18n_cp( "Complicated", "you got it", "you got them", 2 ), "tu les as eu" ))

    p_l10n_fini( &t );

#ifndef P_NO_MEMPOOL
    p_mempool_debug( &mp );
    p_mempool_fini( &mp );
    p_mempool_debug( &mp );
#endif
    return 0;
}
#endif /* NDEBUG */

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
