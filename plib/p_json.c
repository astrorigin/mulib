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

#include "p_json.h"

#include "p_dict.h"
#include "p_utf8.h"

P_BOOL
p_json( P_CHAR* s,
        const P_CHAR* nil,
        p_Shell* null,
        P_CHAR** next )
{
    P_ASSERT( s )
    P_ASSERT( s < nil )
    P_ASSERT( null && null->type == P_SHELLTYPE_NULL )

    switch ( p_json_scan( s, nil, next ))
    {
    case P_JSON_LBRACE:
        if ( !p_shell_set_type( null, P_SHELLTYPE_DICT ))
            return P_FALSE;
        return p_json_scan_object( *next, nil, null, next );
    case P_JSON_LBRACKET:
        if ( !p_shell_set_type( null, P_SHELLTYPE_VECTOR ))
            return P_FALSE;
        return p_json_scan_array( *next, nil, null, next );
    case P_JSON_STRING:
        if ( !p_shell_set_type( null, P_SHELLTYPE_STRING ))
            return P_FALSE;
        return p_json_scan_string( *next, nil, null, next );
    case P_JSON_NUMBER:
        return p_json_scan_number( *next, nil, null, next );
    case P_JSON_TRUE:
        if ( !p_shell_set_type( null, P_SHELLTYPE_BOOL ))
            return P_FALSE;
        *(P_BOOL*)null->p = P_TRUE;
        break;
    case P_JSON_FALSE:
        if ( !p_shell_set_type( null, P_SHELLTYPE_BOOL ))
            return P_FALSE;
        *(P_BOOL*)null->p = P_FALSE;
        break;
    case P_JSON_NULL:
        break;
    default:
        return P_FALSE;
    }
    return P_TRUE;
}

P_INT8
p_json_scan( P_CHAR* s,
        const P_CHAR* nil,
        P_CHAR** next )
{
    P_ASSERT( s )
    P_ASSERT( next )

    if ( s >= nil )
        return P_JSON_ERROR;

    while ( s < nil && *s && isspace( *s ))
        ++s;

    *next = s;
    if ( !( s < nil && *s ))
        return P_JSON_END;

    switch ( *s )
    {
    case '{': ++(*next); return P_JSON_LBRACE;
    case '}': ++(*next); return P_JSON_RBRACE;
    case '[': ++(*next); return P_JSON_LBRACKET;
    case ']': ++(*next); return P_JSON_RBRACKET;
    case ':': ++(*next); return P_JSON_COLON;
    case ',': ++(*next); return P_JSON_COMMA;
    case '"': return P_JSON_STRING;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '-':
        return P_JSON_NUMBER;
    default:
        if ( s + 3 >= nil )
            return P_JSON_ERROR;
        if ( !strncmp( s, "null", 4 ))
        {
            *next += 4;
            return P_JSON_NULL;
        }
        if ( !strncmp( s, "true", 4 ))
        {
            *next += 4;
            return P_JSON_TRUE;
        }
        if ( s + 4 >= nil )
            return P_JSON_ERROR;
        if ( !strncmp( s, "false", 5 ))
        {
            *next += 5;
            return P_JSON_FALSE;
        }
        return P_JSON_ERROR;
    }
}

P_BOOL
p_json_scan_object( P_CHAR* s,
        const P_CHAR* nil,
        p_Shell* dict,
        P_CHAR** next )
{
    p_Shell str;
    p_Shell* value = NULL;
    P_PTR old;
    P_BOOL firstpass = P_TRUE;

    P_ASSERT( s )
    P_ASSERT( s < nil )
    P_ASSERT( dict && dict->type == P_SHELLTYPE_DICT )

    ((p_Dict*)dict->p)->finalize_fn = (P_VOID(*)(P_PTR))&p_shell_delete_by_val;

    if ( !p_shell_init( &str, P_SHELLTYPE_STRING ))
        return P_FALSE;

    do
    {
        switch ( p_json_scan( s, nil, next ))
        {
        case P_JSON_STRING:
            if ( !firstpass )
                goto failed;
            break;
        case P_JSON_RBRACE:
            goto finish;
        case P_JSON_COMMA:
            if ( firstpass )
                goto failed;
            if ( p_json_scan( *next, nil, next ) != P_JSON_STRING )
                goto failed;
            break;
        default:
            goto failed;
        }
        if ( !p_json_scan_string( *next, nil, &str, next ))
            goto failed;
        if ( p_json_scan( *next, nil, next ) != P_JSON_COLON )
            goto failed;
        if ( !p_shell_new( &value, P_SHELLTYPE_NULL ))
            goto failed;
        if ( !p_json_scan_value( *next, nil, value, next ))
            goto failed;
        if ( !p_dict_set( (p_Dict*)dict->p,
            (P_CHAR*)((p_String*)str.p)->data, value, &old ))
            goto failed;
        if ( old != value )
        {
            value = NULL;
            p_shell_delete( (p_Shell**)&old );
            goto failed;
        }
        value = NULL;
        if ( !p_string_set_len( (p_String*)str.p, NULL, 0 ))
            goto failed;
        s = *next;
        if ( firstpass )
            firstpass = P_FALSE;
    }
    while ( s < nil );

  failed:
    p_shell_fini( &str );
    if ( value )
        p_shell_delete( &value );
    return P_FALSE;

  finish:
    p_shell_fini( &str );
    return P_TRUE;
}

P_BOOL
p_json_scan_array( P_CHAR* s,
        const P_CHAR* nil,
        p_Shell* arr,
        P_CHAR** next )
{
    p_Shell* value;
    P_BOOL firstpass = P_TRUE;

    P_ASSERT( s )
    P_ASSERT( s < nil )
    P_ASSERT( arr && arr->type == P_SHELLTYPE_VECTOR )

    if ( !p_vector_init( (p_Vector*)arr->p, 0, sizeof( p_Shell ), NULL ))
        return P_FALSE;
    ((p_Vector*)arr->p)->finalize_fn = (P_VOID(*)(P_PTR))&p_shell_fini;

    do
    {
        switch ( p_json_scan( s, nil, next ))
        {
        case P_JSON_COMMA:
            if ( firstpass )
                goto failed;
            break;
        case P_JSON_RBRACKET:
            goto finish;
        default:
            if ( firstpass )
                break;
            goto failed;
        }
        if ( !( value = p_vector_get_next( (p_Vector*)arr->p )))
            goto failed;
        if ( !p_shell_init( value, P_SHELLTYPE_NULL ))
            goto failed;
        if ( !p_json_scan_value( *next, nil, value, next ))
        {
            p_shell_fini( value );
            goto failed;
        }
        ((p_Vector*)arr->p)->len += 1;
        s = *next;
        if ( firstpass )
            firstpass = P_FALSE;
    }
    while ( s < nil );

  failed:
    return P_FALSE;

  finish:
    return P_TRUE;
}

P_BOOL
p_json_scan_value( P_CHAR* s,
        const P_CHAR* nil,
        p_Shell* shell,
        P_CHAR** next )
{
    P_ASSERT( s )
    P_ASSERT( s < nil )
    P_ASSERT( shell && shell->type == P_SHELLTYPE_NULL )

    switch ( p_json_scan( s, nil, next ))
    {
    case P_JSON_STRING:
        if ( !p_shell_set_type( shell, P_SHELLTYPE_STRING ))
            return P_FALSE;
        if ( !p_json_scan_string( *next, nil, shell, next ))
        {
            p_shell_fini( shell );
            return P_FALSE;
        }
        return P_TRUE;
    case P_JSON_NUMBER:
        if ( !p_json_scan_number( *next, nil, shell, next ))
        {
            p_shell_fini( shell );
            return P_FALSE;
        }
        return P_TRUE;
    case P_JSON_LBRACE:
        if ( !p_shell_set_type( shell, P_SHELLTYPE_DICT ))
            return P_FALSE;
        if ( !p_json_scan_object( *next, nil, shell, next ))
        {
            p_shell_fini( shell );
            return P_FALSE;
        }
        return P_TRUE;
    case P_JSON_LBRACKET:
        if ( !p_shell_set_type( shell, P_SHELLTYPE_VECTOR ))
            return P_FALSE;
        if ( !p_json_scan_array( *next, nil, shell, next ))
        {
            p_shell_fini( shell );
            return P_FALSE;
        }
        return P_TRUE;
    case P_JSON_TRUE:
        if ( !p_shell_set_type( shell, P_SHELLTYPE_BOOL ))
            return P_FALSE;
        *(P_BOOL*)shell->p = P_TRUE;
        return P_TRUE;
    case P_JSON_FALSE:
        if ( !p_shell_set_type( shell, P_SHELLTYPE_BOOL ))
            return P_FALSE;
        *(P_BOOL*)shell->p = P_FALSE;
        return P_TRUE;
    case P_JSON_NULL:
        return P_TRUE;
    default:
        return P_FALSE;
    }
}

P_BOOL
p_json_scan_string( P_CHAR* s,
        const P_CHAR* nil,
        p_Shell* str,
        P_CHAR** next )
{
    P_CHAR c;
    P_INT8 i;
    P_CHAR* p;

    P_ASSERT( s && *s == '"' )
    P_ASSERT( s < nil )
    P_ASSERT( !str || ( str && str->type == P_SHELLTYPE_STRING ))

    if ( ++s >= nil || !*s )
        goto failed;
    if ( str && !p_string_set_len( (p_String*)str->p, NULL, 0 ))
        goto failed;

  process:

    while ( s < nil && ( c = *s ))
    {
        if ( iscntrl( c ))
            goto failed;
        if ( c > 0 ) /* utf-8 1 octet */
        {
            switch ( c )
            {
            case '"':
                *next = s + 1;
                return P_TRUE;
            case '\\':
                goto checkbs;
            default:
                if ( str && !p_string_cat_len( (p_String*)str->p, s, 1 ))
                    goto failed;
                ++s;
                /*continue;*/
            }
        }
        else
        if ( c >= -64 && c < -32 ) /* utf-8 2 octets */
        {
            p = s;
            if ( ++s >= nil || !( c = *s ) || !( c < -64 ))
                goto failed;
            if ( str && !p_string_cat_len( (p_String*)str->p, p, 2 ))
                goto failed;
            ++s;
        }
        else
        if ( c >= -32 && c < -16 ) /* utf-8 3 octets */
        {
            p = s;
            for ( i = 0; i < 2; ++i )
            {
                if ( ++s >= nil || !( c = *s ) || !( c < -64 ))
                    goto failed;
            }
            if ( str && !p_string_cat_len( (p_String*)str->p, p, 3 ))
                goto failed;
            ++s;
        }
        else
        if ( c >= -16 && c < -8 ) /* utf-8 4 octets */
        {
            p = s;
            for ( i = 0; i < 3; ++i )
            {
                if ( ++s >= nil || !( c = *s ) || !( c < -64 ))
                    goto failed;
            }
            if ( str && !p_string_cat_len( (p_String*)str->p, p, 4 ))
                goto failed;
            ++s;
        }
        else /* invalid */
        {
            goto failed;
        }
    }

    goto failed;

  checkbs:

    if ( ++s >= nil )
        goto failed;

    switch ( *s )
    {
        case '"':
            if ( str && !p_string_cat_len( (p_String*)str->p, "\"", 1 ))
                goto failed;
            ++s;
            goto process;
        case '\\':
            if ( str && !p_string_cat_len( (p_String*)str->p, "\\", 1 ))
                goto failed;
            ++s;
            goto process;
        case '/':
            if ( str && !p_string_cat_len( (p_String*)str->p, "/", 1 ))
                goto failed;
            ++s;
            goto process;
        case 'b':
            if ( str && !p_string_cat_len( (p_String*)str->p, "\b", 1 ))
                goto failed;
            ++s;
            goto process;
        case 'f':
            if ( str && !p_string_cat_len( (p_String*)str->p, "\f", 1 ))
                goto failed;
            ++s;
            goto process;
        case 'n':
            if ( str && !p_string_cat_len( (p_String*)str->p, "\n", 1 ))
                goto failed;
            ++s;
            goto process;
        case 'r':
            if ( str && !p_string_cat_len( (p_String*)str->p, "\r", 1 ))
                goto failed;
            ++s;
            goto process;
        case 't':
            if ( str && !p_string_cat_len( (p_String*)str->p, "\t", 1 ))
                goto failed;
            ++s;
            goto process;
        case 'u':
        {
            P_CHAR buf[5];
            P_INT32 ucode;
            P_CHAR* end;
            for ( i = 0; i < 4; ++i )
            {
                if ( ++s >= nil || !isxdigit( *s ))
                    goto failed;
                buf[i] = *s;
            }
            buf[4] = '\0';
            errno = 0;
            ucode = strtol( buf, &end, 16 );
            if ( errno || *end || ucode <= 0 )
                goto failed;
            p_utf8_encode( ucode, buf );
            if ( !*buf )
                goto failed;
            if ( str && !p_string_cat( (p_String*)str->p, buf ))
                goto failed;
            ++s;
            goto process;
        }
        default:
            goto failed;
    }

  failed:

    *next = s;
    return P_FALSE;
}

P_BOOL
p_json_scan_number( P_CHAR* s,
        const P_CHAR* nil,
        p_Shell* shell,
        P_CHAR** next )
{
    p_String str;
    P_BOOL isdouble = P_FALSE;

    P_ASSERT( s && *s )
    P_ASSERT( s < nil )

    if ( shell && !p_string_init( &str, NULL ))
        goto failed;

    if ( *s == '-' )
    {
        if ( shell && !p_string_cat_len( &str, s, 1 ))
            goto failed;
        if ( ++s >= nil || !*s )
            goto failed;
    }
    else
    if ( !isdigit( *s ))
        goto failed;

    if ( *s == '0' )
    {
        if ( shell && !p_string_cat_len( &str, s, 1 ))
            goto failed;
        ++s;
    }
    else
    if ( isdigit( *s ))
    {
        while ( s < nil && isdigit( *s ))
        {
            if ( shell && !p_string_cat_len( &str, s, 1 ))
                goto failed;
            ++s;
        }
    }
    else
        goto failed;
    if ( s >= nil || !*s )
        goto failed;

    if ( *s == '.' )
    {
        if ( shell && !p_string_cat_len( &str, s, 1 ))
            goto failed;
        if ( ++s >= nil || !isdigit( *s ))
            goto failed;
        while ( s < nil && isdigit( *s ))
        {
            if ( shell && !p_string_cat_len( &str, s, 1 ))
                goto failed;
            ++s;
        }
        if ( s >= nil || !*s )
            goto failed;
        isdouble = P_TRUE;
    }

    if ( tolower( *s ) == 'e' )
    {
        if ( shell && !p_string_cat_len( &str, s, 1 ))
            goto failed;
        if ( ++s < nil && ( *s == '+' || *s == '-' ))
        {
            if ( shell && !p_string_cat_len( &str, s, 1 ))
                goto failed;
            ++s;
        }
        if ( s >= nil || !isdigit( *s ))
            goto failed;
        while ( s < nil && isdigit( *s ))
        {
            if ( shell && !p_string_cat_len( &str, s, 1 ))
                goto failed;
            ++s;
        }
        if ( s >= nil || !*s )
            goto failed;
    }

    if ( isspace( *s ))
        goto finish;
    else
    {
        switch ( *s )
        {
        case ',':
        case ']':
        case '}':
            goto finish;
        }
    }

  failed:

    if ( shell )
        p_string_fini( &str );
    *next = s;
    return P_FALSE;

  finish:

    if ( shell )
    {
        P_CHAR* end;
        if ( isdouble )
        {
            P_DBL d;
            errno = 0;
            d = strtod( str.data, &end );
            if ( errno || *end )
                goto failed;
            if ( !p_shell_set_type( shell, P_SHELLTYPE_DBL ))
                goto failed;
            *(P_DBL*)shell->p = d;
        }
        else
        {
            P_INT32 i;
            errno = 0;
            i = strtol( str.data, &end, 10 );
            if ( errno || *end )
                goto failed;
            if ( !p_shell_set_type( shell, P_SHELLTYPE_INT32 ))
                goto failed;
            *(P_INT32*)shell->p = i;
        }
        p_string_fini( &str );
    }
    *next = s;
    return P_TRUE;
}

P_BOOL
p_json_make_string( P_CHAR* s,
        p_String* jstring,
        const P_BOOL ascii_only )
{
    P_CHAR buf[5];
    P_SZ sz;

    P_ASSERT( s )
    P_ASSERT( jstring )

    if ( !p_string_set_len( jstring, "\"", 1 ))
        return P_FALSE;

    while ( s && p_utf8_get_char( s, buf ))
    {
        switch ( ( sz = strlen( buf )))
        {
        case 0: /* not reached */
            return P_FALSE;
        case 1: /* ascii */
        {
            if ( iscntrl( *buf ))
            {
                P_INT32 i;
                P_CHAR u[5];
                if ( !p_string_cat_len( jstring, "\\u", 2 )
                    || !p_utf8_decode( buf, &i )
                    || sprintf( u, "%4.4X", i ) != 4
                    || !p_string_cat_len( jstring, u, 4 ))
                    return P_FALSE;
                break;
            }
            switch ( *buf )
            {
            case '"':
                if ( !p_string_cat_len( jstring, "\\\"", 2 ))
                    return P_FALSE;
                break;
            case '\\':
                if ( !p_string_cat_len( jstring, "\\\\", 2 ))
                    return P_FALSE;
                break;
            case '/':
                if ( !p_string_cat_len( jstring, "\\/", 2 ))
                    return P_FALSE;
                break;
            case '\b':
                if ( !p_string_cat_len( jstring, "\\b", 2 ))
                    return P_FALSE;
                break;
            case '\f':
                if ( !p_string_cat_len( jstring, "\\f", 2 ))
                    return P_FALSE;
                break;
            case '\n':
                if ( !p_string_cat_len( jstring, "\\n", 2 ))
                    return P_FALSE;
                break;
            case '\r':
                if ( !p_string_cat_len( jstring, "\\r", 2 ))
                    return P_FALSE;
                break;
            case '\t':
                if ( !p_string_cat_len( jstring, "\\t", 2 ))
                    return P_FALSE;
                break;
            default:
                if ( !p_string_cat_len( jstring, buf, 1 ))
                    return P_FALSE;
            }
            break;
        }
        default: /* utf-8 sequence */
            if ( ascii_only )
            {
                P_INT32 i;
                P_CHAR u[5];
                if ( !p_string_cat_len( jstring, "\\u", 2 )
                    || !p_utf8_decode( buf, &i )
                    || sprintf( u, "%4.4X", i ) != 4
                    || !p_string_cat_len( jstring, u, 4 ))
                    return P_FALSE;
            }
            else
            {
                if ( !p_string_cat_len( jstring, buf, sz ))
                    return P_FALSE;
            }
            break;
        }
        s = p_utf8_next_char( s );
    }
    if ( !p_string_cat_len( jstring, "\"", 1 ))
        return P_FALSE;
    return P_TRUE;
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
