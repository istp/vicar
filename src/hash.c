#include <sys/types.h>
#include <stdint.h>
/* This is from libc/db/hash/hash_func.c, hash3 is static there */
/*
 * This is INCREDIBLY ugly, but fast.  We break the string up into 8 byte
 * units.  On the first time through the loop we get the "leftover bytes"
 * (strlen % 8).  On every other iteration, we perform 8 HASHC's so we handle
 * all 8 bytes.  Essentially, this saves us 7 cmp & branch instructions.  If
 * this routine is heavily used enough, it's worth the ugly coding.
 *
 * OZ's original sdbm hash
 */
int32_t
__nis_hash (const void *keyarg, register size_t len)
{
  register const unsigned char *key;
  register size_t loop;
  register uint32_t h;

#define HASHC   h = *key++ + 65599 * h

  printf("hash.c : _nis_hash ");

  h = 0;
  key = keyarg;
  if (len > 0)
    {
      loop = (len + 8 - 1) >> 3;
      switch (len & (8 - 1))
        {
        case 0:
          do {
            HASHC;
            /* FALLTHROUGH */
          case 7:
            HASHC;
            /* FALLTHROUGH */
          case 6:
            HASHC;
            /* FALLTHROUGH */
          case 5:
            HASHC;
            /* FALLTHROUGH */
          case 4:
            HASHC;
            /* FALLTHROUGH */
          case 3:
            HASHC;
            /* FALLTHROUGH */
          case 2:
            HASHC;
            /* FALLTHROUGH */
          case 1:
            HASHC;
          } while (--loop);
        }
    }
  return (h);
}
