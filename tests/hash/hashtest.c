/*
 *  Test different hashing functions.
 *
 */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define INPUTSIZE 128 /* stdin line size */
#define ARRPOWER 18 /* array size = 2 pow ARRPOWER */

#if 1 /* FNV1a */
size_t
hash_func(const void* k,
        const size_t len)
{
  unsigned char* p = (unsigned char*) k;
  size_t i, hash = 0x811c9dc5;

  for (i = 0; i < len; ++i) {
    hash ^= p[i];
    hash *= 16777619;
  }

  // modulo
  hash &= 0xffffffff >> (32 - ARRPOWER);

  return hash;
}
#endif /* end FNV1a */


#if 0 /* unknown */
size_t
hash_func(const void* k,
        const size_t len)
{
  unsigned char* p = (unsigned char*) k;
  size_t i, h = 0;

  for (i = 0; i < len; ++i)
  {
    h += p[i];
    h += ( h << 10 );
    h ^= ( h >> 6 );
  }
  h += ( h << 3 );
  h ^= ( h >> 11 );
  h += ( h << 15 );

  // modulo
  h &= 0xffffffff >> (32 - ARRPOWER);

  return h;
}
#endif /* end personal */

int main(int argc, char *argv[])
{
  const size_t sz = pow(2, ARRPOWER); /* num buckets */
  size_t i, h;
  size_t wasted = 0, max = 0, maxindex, overten = 0, overtennum = 0;
  char *p = NULL;
  char input[INPUTSIZE+1];
  size_t arr[sz]; /* the array of buckets */

  memset(arr, 0, sizeof(size_t) * sz);

  while (fgets(input, INPUTSIZE+1, stdin) != NULL) {
    /* have read a line */
    if ((p = strrchr(input, '\n'))) {
      /* remove trailing newline */
      *p = '\0';
    }
    /* compute hash */
    h = hash_func(input, strlen(input));
    /* insert in bucket */
    arr[h]++;
    if (p == NULL) {
      /* last line? */
      break;
    }
  }

  /* make some stats */
  for (i = 0; i < sz; ++i) {
      printf("h[%lu] = %lu\n", i, arr[i]);
      if (arr[i] == 0) {
          wasted++;
      }
      if (arr[i] > max) {
        max = arr[i];
        maxindex = i;
      }
      if (arr[i] > 10) {
        overten++;
        overtennum += arr[i];
      }
  }
  printf("Wasted buckets = %lu\n", wasted);
  printf("Maximum = %lu at %lu\n", max, maxindex);
  printf("Over 10 = %lu, total = %lu\n", overten, overtennum);

  return 0;
}

//end.