/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: strcasecmp.c,v 1.2 2005/02/27 13:45:27 phx Exp $
 */

#include <ctype.h>
#include <string.h>


int strcasecmp(const char *str1, const char *str2)
{
  const unsigned char *us1 = (const unsigned char *)str1,
                      *us2 = (const unsigned char *)str2;

  while (tolower(*us1) == tolower(*us2++))
    if (*us1++ == '\0')
      return 0;
  return (tolower(*us1) - tolower(*--us2));
}
