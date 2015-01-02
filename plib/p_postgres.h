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

/**
 *  \file p_postgres.h
 *  \brief PostgreSQL wrapper.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch>
 */

#ifndef P_POSTGRES_H
#define P_POSTGRES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "p_string.h"
#include <postgresql/libpq-fe.h>

/**
 *  \brief Postgres types (for bindings).
 */
typedef enum
{
    P_PGTYPE_UNDEFINED = 0,
    /* numeric types */
    P_PGTYPE_SMALLINT,      /**< short int expected */
    P_PGTYPE_INTEGER,       /**< int expected */
    P_PGTYPE_SERIAL,        /**< int expected */
    P_PGTYPE_BIGINT,        /**< long long int expected */
    P_PGTYPE_BIGSERIAL,     /**< long long int expected */
    P_PGTYPE_DECIMAL,       /**< double expected */
    P_PGTYPE_NUMERIC,       /**< double expected */
    P_PGTYPE_DOUBLE,        /**< double expected */
    P_PGTYPE_REAL,          /**< float expected */
    /* character types */
    P_PGTYPE_CHAR,          /**< string expected (length optional) */
    P_PGTYPE_TEXT,          /**< string expected (length optional) */
    P_PGTYPE_VARCHAR,       /**< string expected (length optional) */
    /* binary */
    P_PGTYPE_BYTEA,         /**< unsigned char* expected, with length */
    /* boolean */
    P_PGTYPE_BOOLEAN        /**< char expected */

} p_pgtype;

/**
 *  \brief Get last error message (or NULL).
 */
P_EXPORT char*
p_pgsql_last_error( void );

/**
 *  \brief Initialize connection.
 *  \param conninfo Connection parameters.
 *  \return Connection handler, or NULL on error.
 */
P_EXPORT PGconn*
p_pgsql_init( const char* conninfo );

/**
 *  \brief Finalize connection.
 */
P_EXPORT void
p_pgsql_fini( void );

/**
 *  \brief Execute an sql statement.
 *  \param sql The sql statement.
 *  \return Results, or NULL on error.
 */
P_EXPORT PGresult*
p_pgsql_exec( const char* sql );

/**
 *  \brief Bind several variables parameters in one go.
 *  \param stmt Sql statement, with question marks (?) as variables placeholders.
 *  \param ... Triplets of type (p_pgtype), value (void*), len (size_t), and a final P_PGTYPE_UNDEFINED.
 *  \return The updated statement string, or NULL on error.
 *
 *  \code
 *  if ( !p_pgsql_binds( stmt,
 *      P_PGTYPE_INTEGER, &i, 0,
 *      P_PGTYPE_TEXT, str, 0,
 *      P_PGTYPE_UNDEFINED ))
 *  {
 *      alert( "cant bind vars" );
 *  }
 *  \endcode
 */
P_EXPORT p_String*
p_pgsql_bind( p_String* stmt,
        ... );

/**
 *  \brief Bind parameters and execute a statement in one go.
 *  \param sql Sql statement, with question marks (?) as variables placeholders.
 *  \param ... Triplets of type (p_pgtype), value (void*), len (size_t), and a final P_PGTYPE_UNDEFINED.
 *  \return Results, or NULL on error.
 */
P_EXPORT PGresult*
p_pgsql_exec2( const char* sql,
        ... );

#ifdef __cplusplus
}
#endif

#endif /* P_POSTGRES_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
