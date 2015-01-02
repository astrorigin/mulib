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

#ifdef P_POSTGRES

#include "p_postgres.h"
#include <stdarg.h>

char*
_p_pg_conninfo = NULL;

PGconn*
_p_pg_cnx = NULL;

char*
_p_pg_err = NULL;

/* Not in C89 */
static char*
strdup( const char* s )
{
    char* dup;
    size_t sz;
    assert( s );
    sz = strlen( s ) + 1;
    if ( !( dup = malloc( sz )))
        return NULL;
    memcpy( dup, s, sz );
    return dup;
}

static void
_p_pgsql_error( const char* msg )
{
    char* p;
    if ( _p_pg_err )
    {
        free( _p_pg_err );
        _p_pg_err = NULL;
    }
    if ( !( _p_pg_err = strdup( msg )))
        return;
    if ( ( p = strrchr( _p_pg_err, '\n' )))
        *p = '\0';
}

char*
p_pgsql_last_error( void )
{
    return _p_pg_err;
}

static PGconn*
_p_pgsql_connect( const char* conninfo )
{
    PGconn* ret = NULL;
    assert( conninfo );
    if ( _p_pg_cnx == NULL ) /* connect */
    {
        if ( !( ret = PQconnectdb( conninfo )))
            return NULL;
    }
    else /* reconnect */
    {
        ret = _p_pg_cnx;
        PQreset( ret );
    }
    /* check status */
    if ( PQstatus( ret ) != CONNECTION_OK )
    {
        _p_pgsql_error( PQerrorMessage( ret ));
        return NULL;
    }
    return ret;
}

PGconn*
p_pgsql_init( const char* conninfo )
{
    assert( conninfo );
    /* return singleton if connection has same conninfo */
    if ( _p_pg_cnx )
    {
        assert( _p_pg_conninfo );
        if ( !strcmp( _p_pg_conninfo, conninfo ))
        {
            /* check connection status */
            if ( PQstatus( _p_pg_cnx ) != CONNECTION_OK )
                _p_pg_cnx = _p_pgsql_connect( conninfo );
            return _p_pg_cnx;
        }
        p_pgsql_fini();
    }
    /* new connection */
    if ( !( _p_pg_conninfo = strdup( conninfo )))
        return NULL;
    return ( _p_pg_cnx = _p_pgsql_connect( conninfo ));
}

void
p_pgsql_fini( void )
{
    if ( _p_pg_cnx )
    {
        PQfinish( _p_pg_cnx );
        _p_pg_cnx = NULL;
    }
    if ( _p_pg_conninfo )
    {
        free( _p_pg_conninfo );
        _p_pg_conninfo = NULL;
    }
    if ( _p_pg_err )
    {
        free( _p_pg_err );
        _p_pg_err = NULL;
    }
}

PGresult*
p_pgsql_exec( const char* sql )
{
    PGresult* res;
    assert( _p_pg_cnx );
    assert( sql && sql[0] );
    if ( !( res = PQexec( _p_pg_cnx, sql )))
    {
        _p_pgsql_error( PQerrorMessage( _p_pg_cnx ));
        return NULL;
    }
    switch ( PQresultStatus( res ))
    {
    case PGRES_COMMAND_OK:
    case PGRES_TUPLES_OK:
        break;
    default:
        _p_pgsql_error( PQresultErrorMessage( res ));
        PQclear( res );
        return NULL;
    }
    return res;
}

static p_String*
_p_pgsql_bind( p_String* stmt,
        p_pgtype tid,
        void* value,
        size_t len,
        size_t* skip )
{
    char* p;
    size_t sz;
    p_String tmp;
    assert( _p_pg_cnx );
    assert( stmt && stmt->data );
    assert( skip );
    /* find mark */
    if ( !( p = strchr( stmt->data + *skip, '?' )))
    {
        _p_pgsql_error( "missing '?'" );
        return NULL;
    }
    /* copy beginning of string */
    if ( ( sz = p - (char*) stmt->data ))
    {
        if ( !p_string_init_len( &tmp, stmt->data, sz ))
            return NULL;
    }
    else
    {
        if ( !p_string_init( &tmp, NULL ))
            return NULL;
    }
    /* copy value */
    if ( value == NULL )
    {
        if ( !p_string_cat( &tmp, "NULL" ))
            goto error;
    }
    else
    {
        switch ( tid )
        {
        /* numeric types */
        case P_PGTYPE_SMALLINT:
        {
            char buf[12] = {0};
            short int i = *(short int*) value;
            sprintf( buf, "%hd", i );
            if ( !p_string_cat( &tmp, buf ))
                goto error;
            break;
        }
        case P_PGTYPE_INTEGER:
        case P_PGTYPE_SERIAL:
        {
            char buf[24] = {0};
            int i = *(int*) value;
            sprintf( buf, "%d", i );
            if ( !p_string_cat( &tmp, buf ))
                goto error;
            break;
        }
        case P_PGTYPE_BIGINT:
        case P_PGTYPE_BIGSERIAL:
        {
            char buf[32] = {0};
            long long int i = *(long long int*) value;
            sprintf( buf, "%lld", i );
            if ( !p_string_cat( &tmp, buf ))
                goto error;
            break;
        }
        case P_PGTYPE_DECIMAL:
        case P_PGTYPE_NUMERIC:
        case P_PGTYPE_DOUBLE:
        {
            char buf[32] = {0};
            double d = *(double*) value;
            sprintf( buf, "%.15f", d );
            if ( !p_string_cat( &tmp, buf ))
                goto error;
            break;
        }
        case P_PGTYPE_REAL:
        {
            char buf[24] = {0};
            float d = *(float*) value;
            sprintf( buf, "%.6f", d );
            if ( !p_string_cat( &tmp, buf ))
                goto error;
            break;
        }
        /* character types */
        case P_PGTYPE_CHAR:
        case P_PGTYPE_TEXT:
        case P_PGTYPE_VARCHAR:
        {
            char* s;
            if ( !( s = PQescapeLiteral( _p_pg_cnx,
                value, len ? len : strlen( value ))))
            {
                _p_pgsql_error( PQerrorMessage( _p_pg_cnx ));
                goto error;
            }
            if ( !p_string_cat( &tmp, s ))
            {
                PQfreemem( s );
                goto error;
            }
            PQfreemem( s );
            break;
        }
        /* binary */
        case P_PGTYPE_BYTEA:
        {
            unsigned char* u;
            size_t usz;
            if ( !( u = PQescapeByteaConn( _p_pg_cnx, value, len, &usz )))
                goto error;
            if ( !p_string_cat( &tmp, "'" )
                || !p_string_cat_len( &tmp, (char*) u, usz )
                || !p_string_cat( &tmp, "'" ))
            {
                PQfreemem( u );
                goto error;
            }
            PQfreemem( u );
            break;
        }
        /* boolean */
        case P_PGTYPE_BOOLEAN:
        {
            char c = *(char*) value;
            if ( !p_string_cat( &tmp, c ? "TRUE" : "FALSE" ))
                goto error;
            break;
        }
        default:
            return NULL; /* not reached */
        }
    }
    /* count chars written so far (to skip) */
    *skip = tmp.len - 1;
    /* complete string */
    if ( *++p )
    {
        if ( !p_string_cat( &tmp, p ))
            goto error;
    }
    if ( !p_string_set( stmt, tmp.data ))
        goto error;
    p_string_fini( &tmp );
    return stmt;
  error:
    p_string_fini( &tmp );
    return NULL;
}

p_String*
p_pgsql_bind( p_String* stmt,
        ... )
{
    va_list ap;
    p_pgtype tp;
    void* value;
    size_t len;
    size_t skip = 0;
    assert( stmt );
    va_start( ap, stmt );
    while ( ( tp = va_arg( ap, p_pgtype )) != P_PGTYPE_UNDEFINED )
    {
        value = va_arg( ap, void* );
        len = va_arg( ap, size_t );
        if ( !_p_pgsql_bind( stmt, tp, value, len, &skip ))
        {
            va_end( ap );
            return NULL;
        }
    }
    va_end( ap );
    return stmt;
}

PGresult*
p_pgsql_exec2( const char* sql,
        ... )
{
    PGresult* res;
    p_String stmt;
    va_list ap;
    p_pgtype tp;
    void* value;
    size_t len;
    size_t skip = 0;
    assert( sql && sql[0] );
    if ( !p_string_init( &stmt, sql ))
        return NULL;
    va_start( ap, sql );
    while ( ( tp = va_arg( ap, p_pgtype )) != P_PGTYPE_UNDEFINED )
    {
        value = va_arg( ap, void* );
        len = va_arg( ap, size_t );
        if ( !_p_pgsql_bind( &stmt, tp, value, len, &skip ))
        {
            p_string_fini( &stmt );
            va_end( ap );
            return NULL;
        }
    }
    va_end( ap );
    res = p_pgsql_exec( (char*) stmt.data );
    p_string_fini( &stmt );
    return res;
}

#endif /* P_POSTGRES */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
