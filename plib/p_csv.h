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
 *  \file csv.h
 *  \brief "Comma separated" values parser.
 *  \copyright GNU Lesser General Public License
 *  \author Stanislas Marquis <smarquis@astrorigin.ch> 
 */

#ifndef P_CSV_H
#define P_CSV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "p_h.h"

#define P_CSV_EOL       0
#define P_CSV_EOF       1
#define P_CSV_WORD      2

typedef struct _p_csv_ctxt_t p_CsvContext;

/**
 *  \struct p_csv_ctxt_t
 */
struct _p_csv_ctxt_t
{
    P_CHAR* data;       /* csv file contents */
    P_CHAR* sepchars;   /* chars separating words */
    P_CHAR* comchars;   /* chars for comment until end of line */
    P_CHAR* p;          /* private */
};

/**
 *  \brief Initialize the csv context.
 *  \param csv The csv context.
 *  \param data The file contents.
 *  \param sepchars The chars separating words, or null for default " \t\r"
 *  \param comchars the chars marking comments, or null for default "#"
 */
P_EXPORT P_VOID
p_csv_init( p_CsvContext* csv,
        const P_CHAR* data,
        const P_CHAR* sepchars,
        const P_CHAR* comchars );

/**
 *  \brief Parse file and get next event.
 *  \param csv The csv context.
 *  \return An event (eof, eol, or word).
 */
P_EXPORT P_INT32
p_csv_parse( p_CsvContext* csv );

/**
 *  \brief Get the current word boundaries.
 *  \param csv The csv context.
 *  \param start The start of the word.
 *  \param end The end of the word.
 */
P_EXPORT P_VOID
p_csv_word( p_CsvContext* csv,
        P_CHAR** start,
        P_CHAR** end );

/**
 *  \brief Copy the current word to a buffer.
 *  \param csv The csv context.
 *  \param buf The buffer.
 *  \param limit Size of buffer, not zero.
 */
P_EXPORT P_VOID
p_csv_word_copy( p_CsvContext* csv,
        P_CHAR* buf,
        const P_SZ limit );

/**
 *  \brief Copy from current word until end of line.
 *  \param csv The csv context.
 *  \param buf The buffer.
 *  \param limit Size of buffer, not zero.
 */
P_EXPORT P_VOID
p_csv_line_copy( p_CsvContext* csv,
        P_CHAR* buf,
        const P_SZ limit );

/**
 *  \brief Parse step and retrieve a word.
 *  \param csv The Csv object.
 *  \param buf The buffer.
 *  \param limit Size of buffer, not zero.
 *  \param i Event parsed after the wordi, if P_TRUE is returned.
 *  \return P_TRUE on success.
 */
P_EXPORT P_BOOL
p_csv_get_word( p_CsvContext* csv,
        P_CHAR* buf,
        const P_SZ limit,
        P_INT32* i );

/**
 *  \brief Parse step and retrieve the rest of the line.
 *  \param csv The Csv object.
 *  \param buf The buffer.
 *  \param limit Size of buffer, not zero.
 *  \param i Event parsed after the line, if P_TRUE is returned.
 *  \return P_TRUE on success.
 */
P_EXPORT P_BOOL
p_csv_get_line( p_CsvContext* csv,
        P_CHAR* buf,
        const P_SZ limit,
        P_INT32* i );

#ifdef __cplusplus
}
#endif

#endif /* P_CSV_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
