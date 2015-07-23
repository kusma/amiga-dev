/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: bcmp.c,v 1.2 2005/02/27 13:45:20 phx Exp $
 */

#include <stddef.h>


int bcmp(const void *b1,const void *b2,size_t len)
{
  const char *p1=b1, *p2=b2;

  if (!len)
    return 0;

  do {
    if (*p1++ != *p2++)
      break;
  }
  while(--len);

  return len;
}
