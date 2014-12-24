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

#include "p_regmatch.h"

int
regmatch( const char* str,
        const char* pattern,
        int flags,
        char** err )
{
    int i, flg = REG_NOSUB;
    regex_t rgx;
    assert( str );
    assert( pattern );
    if ( flags )
        flg |= flags;
    if ( ( i = regcomp( &rgx, pattern, flg )) != 0 )
    {
        if ( err )
        {
            size_t sz = regerror( i, &rgx, NULL, 0 );
            if ( !( *err = malloc( sz )))
            {
                regfree( &rgx );
                return -1;
            }
            regerror( i, &rgx, *err, sz );
        }
        regfree( &rgx );
        return -1;
    }
    i = regexec( &rgx, str, 0, NULL, 0 ) == 0 ? 1 : 0;
    regfree( &rgx );
    return i;
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
