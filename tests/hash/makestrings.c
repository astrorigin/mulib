/*
 *  Produce a file of random strings, one per line.
 *
 *  Get rid of duplicates with uniq:
 *  ./makestrings | uniq > outfile
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* how many strings to create */
#define NUMSTRINGS 100000
/* strings maximum length */
#define MAXLEN 14
/* allowed chars */
#define CHARS " abcdefghijklmnopqrstuvwxyz0123456789"
/* count of chars in CHARS */
#define NUMCHARS 37


int main(int argc, char *argv[])
{
  size_t i, j;
  char buf[MAXLEN+1];

  srand(time(NULL));

  for (i = 0; i < NUMSTRINGS; ++i) {
    /* make a random string */
    const size_t slen = (rand() % MAXLEN) + 1;
    for (j = 0; j < slen; ++j) {
      buf[j] = CHARS[rand() % NUMCHARS];
    }
    buf[j] = '\0';
    fputs(buf, stdout);
    if (i != NUMSTRINGS-1) {
      putc('\n', stdout);
    }
  }

  return 0;
}
