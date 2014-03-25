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

#include <p_chrono.h>

P_VOID
p_tspec_plus_msec( struct timespec *ts,
        P_INT32 msec )
{
    time_t sec;

    sec = msec / 1000;
    msec -= sec * 1000;
    ts->tv_nsec += msec * P_MILLION;
    sec += ts->tv_nsec / P_BILLION;
    ts->tv_nsec %= P_BILLION;
    ts->tv_sec += sec;
}

P_VOID
p_tspec_minus_tspec( struct timespec *ts,
        const struct timespec *ots )
{
    P_ASSERT( p_tspec_ge_than( ts, ots ))

    ts->tv_sec -= ots->tv_sec;
    ts->tv_nsec -= ots->tv_nsec;
    if ( ts->tv_nsec < 0 )
    {
        ts->tv_sec -= 1;
        ts->tv_nsec += P_BILLION;
    }
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 sts=4 : */
