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

#include "p_strnstr.h"

char*
p_strnstr( const char* haystack,
        const char* needle,
        size_t n )
{
    char c;
    char* p = (char*) haystack;
    size_t nlen;

    if ( !( c = needle[0] )
        || n == 0 )
        return NULL;

    for ( nlen = strlen( needle ); *p && n >= nlen; ++p, --n )
    {
        if ( *p == c
            && !strncmp( p, needle, nlen ))
            return p;
    }
    return NULL;
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
