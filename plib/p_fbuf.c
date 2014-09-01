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

#include "p_fbuf.h"

P_VOID
p_fbuf_init( p_FBuf* fbuf )
{
    P_ASSERT( fbuf )

    memset( fbuf, 0, sizeof( p_FBuf ));
}

P_VOID
p_fbuf_fini( p_FBuf* fbuf )
{
    P_ASSERT( fbuf )

    if ( fbuf->buf )
    {
        free( fbuf->buf );
        fbuf->buf = NULL;
    }
    fbuf->buflen = 0;
    fbuf->tracker = NULL;
    fbuf->tracklen = 0;
}

P_SZ
p_fbuf_incr( p_FBuf* fbuf,
        const P_INT32 step )
{
    P_ASSERT( fbuf )

    if ( step == 0 )
        return fbuf->tracklen;

    if ( step > (P_INT64)fbuf->tracklen
        || step < (P_INT64)( fbuf->tracklen - fbuf->buflen ))
        return (P_SZ) -1;

    fbuf->tracker += step;
    fbuf->tracklen -= step;
    return fbuf->tracklen;
}

P_SZ
p_fbuf_read( p_FBuf* fbuf,
        const P_SZ maxsz )
{
    P_UCHAR* tmp;
    P_SZ tmplen;

    P_ASSERT( fbuf )
    P_ASSERT( fbuf->f )

    if ( maxsz == 0 )
        return 0;
    if ( !( tmp = malloc( maxsz )))
        return (P_SZ) -1;
    if ( ( tmplen = fread( tmp, 1, maxsz, fbuf->f )) == 0 )
    {
        free( tmp );
        return 0;
    }
    if ( !( fbuf->buf = realloc( fbuf->buf, fbuf->buflen + tmplen )))
    {
        free( tmp );
        return (P_SZ) -1;
    }
    memcpy( &(fbuf->buf)[ fbuf->buflen ], tmp, tmplen );
    fbuf->buflen += tmplen;
    fbuf->tracklen += tmplen;
    fbuf->tracker = &(fbuf->buf)[ fbuf->buflen - fbuf->tracklen ];
    free( tmp );
    return tmplen;
}

P_SZ
p_fbuf_reset( p_FBuf* fbuf,
        const P_SZ maxreadsz )
{
    P_ASSERT( fbuf )

    if ( !fbuf->buf || fbuf->tracklen == 0 )
    {
        if ( fbuf->buf )
        {
            free( fbuf->buf );
            fbuf->buf = NULL;
            fbuf->tracker = NULL;
            fbuf->tracklen = 0;
        }
        fbuf->buflen = 0;
        return p_fbuf_read( fbuf, maxreadsz );
    }
    assert( fbuf->buflen );
    assert( fbuf->buflen >= fbuf->tracklen );
    if ( fbuf->buflen > fbuf->tracklen )
    {
        memmove( fbuf->buf,
            &(fbuf->buf)[ fbuf->buflen - fbuf->tracklen ], fbuf->tracklen );
        fbuf->buf = realloc( fbuf->buf, fbuf->tracklen );
        if ( !fbuf->buf )
            return (P_SZ) -1;
        fbuf->buflen = fbuf->tracklen;
        fbuf->tracker = fbuf->buf;
    }
    return fbuf->buflen;
}

P_INT8
p_fbuf_consume( p_FBuf* fbuf,
        const P_UCHAR* upto,
        const P_SZ uptolen,
        FILE* dest,
        const P_SZ maxsz,
        const P_SZ maxreadsz )
{
    P_INT8 ret = 0;
    P_SZ consumed = 0;
    P_SZ uptocnt = 0;
    P_UCHAR* uptofound = NULL;
    P_UCHAR c;
    P_SZ z;
    P_SZ blen;

    P_ASSERT( fbuf )

    if ( uptolen )
    {
        if ( !upto )
            return -1;
        if ( !( uptofound = malloc( uptolen )))
            return -1;
    }

    while ( ( z = p_fbuf_reset( fbuf, maxreadsz )))
    {
        if ( z == (P_SZ) -1 )
        {
            ret = -1;
            goto finish;
        }
        blen = fbuf->buflen;
        for ( z = 0; z < blen; ++z )
        {
            c = *(fbuf->tracker)++;
            --(fbuf->tracklen);
            ++consumed;
            if ( uptolen && c == upto[ uptocnt ] )
            {
                uptofound[ uptocnt++ ] = c;
                if ( uptocnt == uptolen )
                {
                    ret = 1;
                    goto finish;
                }
            }
            else if ( uptocnt )
            {
                if ( dest
                    && fwrite( uptofound, 1, uptocnt, dest ) != uptocnt )
                {
                    ret = -1;
                    goto finish;
                }
                uptocnt = 0;
                if ( dest && fputc( c, dest ) == EOF )
                {
                    ret = -1;
                    goto finish;
                }
            }
            else
            {
                if ( dest && fputc( c, dest ) == EOF )
                {
                    ret = -1;
                    goto finish;
                }
            }
            if ( maxsz && consumed == maxsz )
                goto finish;
        }
    }
    finish:
    if ( uptofound )
        free( uptofound );
    return ret;
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
