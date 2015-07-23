/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: strncasecmp.c,v 1.2 2005/02/27 13:45:27 phx Exp $
 */

#include <ctype.h>
#include <string.h>


int strncasecmp(const char *str1, const char *str2, size_t n)
{
  if (str1==NULL || str2==NULL)
    return 0;

  if (n) {
    const unsigned char *us1 = (const unsigned char *)str1,
                        *us2 = (const unsigned char *)str2;

    do {
      if (tolower(*us1) != tolower(*us2++))
        return tolower(*us1) - tolower(*--us2);
      if (*us1++ == '\0')
        break;
    }
    while (--n != 0);
  }
  return 0;
}
