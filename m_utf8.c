/*
    mulib
    Copyright 2014-2018 Stanislas Marquis <stan@astrorigin.com>

MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

 */

#include "m_utf8.h"

#if 0
M_INT8
m_utf8_octetnum(const M_INT32 c)
{
  return
    /* prohibited range */
    c >= 0xD800 && c <= 0xDFFF ? 0 :
    /* allowed ranges */
    c >= 0x0 && c <= 0x7F ? 1 :
    c >= 0x80 && c <= 0x7FF ? 2 :
    c >= 0x800 && c <= 0xFFFF ? 3 :
    c >= 0x10000 && c <= 0x10FFFF ? 4 :
    /* out of range */
    0;
}
#endif

M_VOID
m_utf8_octetprepare(M_CHAR* const octets,
        const M_INT8 num)
{
  assert(octets);
  assert(num >= 1 && num <= 4);

  switch (num)
  {
  case 1:
    octets[0] = 0;
    return;
  case 2:
    octets[0] = -64;
    octets[1] = -128;
    return;
  case 3:
    octets[0] = -32;
    octets[1] = -128;
    octets[2] = -128;
    return;
  case 4:
    octets[0] = -16;
    octets[1] = -128;
    octets[2] = -128;
    octets[3] = -128;
    return;
  default:
    return;
  }
}

M_CHAR*
m_utf8_encode(const M_INT32 c,
        M_CHAR* const buf)
{
  M_INT8 num;

  assert(buf);

  memset(buf, 0, 5);
  if ((num = m_utf8_octetnum(c)) == 0)
    return buf;
  m_utf8_octetprepare(buf, num);
  switch (num)
  {
  case 1:
    buf[0] |= c;
    break;
  case 2:
    buf[0] |= (char) ( c >> 6 );
    buf[1] |= ((char)c) & ~-64;
    break;
  case 3:
    buf[0] |= (char) ( c >> 12 );
    buf[1] |= ((char)( c >> 6 )) & ~-64;
    buf[2] |= ((char)c) & ~-64;
    break;
  case 4:
    buf[0] |= (char) ( c >> 19 );
    buf[1] |= ((char)( c >> 12 )) & ~-64;
    buf[2] |= ((char)( c >> 6 )) & ~-64;
    buf[3] |= ((char)c) & ~-64;
    break;
  }
  return buf;
}

M_SZ
m_utf8_strlen(const M_CHAR* s)
{
  M_CHAR c;
  M_CHAR* p;
  M_SZ cnt = 0;

  assert(s);

  p = (M_CHAR*) s;
  while ((c = *p++))
  {
    if ( c >= 0 )
    {
      ++cnt;
    }
    else
    if ( c >= -64 && c < -32 )
    {
      if ( c < -62 ) /* overlong */
        return -1;
      else
      {
        if ( !( c = *p++ ) || !( c < -64 ))
          return -1;
      }
      ++cnt;
    }
    else
    if ( c >= -32 && c < -16 )
    {
      if ( c == -32 ) /* check overlong */
      {
        if ( !( c = *p++ ) || !( c >= -96 ) || !( c < -64 ))
          return -1;
        if ( !( c = *p++ ) || !( c < -64 ))
          return -1;
      }
      else
      {
        M_SZ i;
        for ( i = 0; i < 2; ++i )
        {
          if ( !( c = *p++ ) || !( c < -64 ))
            return -1;
        }
      }
      ++cnt;
    }
    else
    if ( c >= -16 && c < -8 )
    {
      M_SZ i;
      if ( c == -16 ) /* check overlong */
      {
        if ( !( c = *p++ ) || !( c >= -112 ) || !( c < -64 ))
          return -1;
        for ( i = 0; i < 2; ++i )
        {
          if ( !( c = *p++ ) || !( c < -64 ))
            return -1;
        }
      }
      else
      if ( c >= -13 ) /* out of unicode range */
      {
        return -1;
      }
      else
      if ( c == -14 ) /* check out of unicode range */
      {
        if ( !( c = *p++ ) || !( c <= -113 )) /*|| !( c < -64 ))*/
          return -1;
        for ( i = 0; i < 2; ++i )
        {
          if ( !( c = *p++ ) || !( c < -64 ))
            return -1;
        }
      }
      else
      {
        for ( i = 0; i < 3; ++i )
        {
          if ( !( c = *p++ ) || !( c < -64 ))
            return -1;
        }
      }
      ++cnt;
    }
    else
      return -1;
  }
  return cnt;
}

M_BOOL
m_utf8_get_char(const M_CHAR* s,
        M_CHAR* const buf)
{
  M_CHAR c;

  assert(s);
  assert(buf);

  memset(buf, 0, 5);

  if ( !( c = *s ))
    return M_FALSE;
  if ( c >= 0 )
    buf[0] = c;
  else
  if ( c >= -64 && c < -32 )
    memcpy(buf, s, 2);
  else
  if ( c >= -32 && c < -16 )
    memcpy(buf, s, 3);
  else
  if ( c >= -16 && c < -8 )
    memcpy(buf, s, 4);
  else
    return M_FALSE;
  return M_TRUE;
}

M_CHAR*
m_utf8_next_char(const M_CHAR* s)
{
  M_CHAR c;

  assert(s);

  if ( !( c = *s ))
    return NULL;
  if ( c >= 0 )
  {
    if ( *( s + 1 ))
      return (M_CHAR*) s + 1;
  }
  else
  if ( c >= -64 && c < -32 )
  {
    if ( *( s + 2 ))
      return (M_CHAR*) s + 2;
  }
  else
  if ( c >= -32 && c < -16 )
  {
    if ( *( s + 3 ))
      return (M_CHAR*) s + 3;
  }
  else
  if ( c >= -16 && c < -8 )
  {
    if ( *( s + 4 ))
      return (M_CHAR*) s + 4;
  }
  return NULL;
}

M_BOOL
m_utf8_decode(const M_CHAR* s,
        M_INT32* i)
{
  M_CHAR c;

  assert(s);

  *i = 0;
  if ( !( c = *s++ ))
    return M_TRUE;
  else
  if ( c >= 0 )
  {
    *i |= c;
  }
  else
  if ( c >= -64 && c < -32 )
  {
    *i |= ((int)( c ^ -64 )) << 6;
    if ( !( c = *s ) || !( c < -64 ))
      return M_FALSE;
    *i |= c ^ -128;
  }
  else
  if ( c >= -32 && c < -16 )
  {
    *i |= ((int)( c ^ -32 )) << 12;
    if ( !( c = *s++ ) || !( c < -64 ))
      return M_FALSE;
    *i |= ((int)( c ^ -128 )) << 6;
    if ( !( c = *s ) || !( c < -64 ))
      return M_FALSE;
    *i |= c ^ -128;
  }
  else
  if ( c >= -16 && c < -8 )
  {
    *i |= ((int)( c ^ -16 )) << 18;
    if ( !( c = *s++ ) || !( c < -64 ))
      return M_FALSE;
    *i |= ((int)( c ^ -128 )) << 12;
    if ( !( c = *s++ ) || !( c < -64 ))
      return M_FALSE;
    *i |= ((int)( c ^ -128 )) << 6;
    if ( !( c = *s ) || !( c < -64 ))
      return M_FALSE;
    *i |= c ^ -128;
  }
  else
    return M_FALSE;
  return M_TRUE;
}

/* vi: set fenc=utf-8 ff=unix et sw=2 ts=2 sts=2 : */
