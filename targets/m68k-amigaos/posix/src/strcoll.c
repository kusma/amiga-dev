/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2006
 *
 * $Id: strcoll.c,v 1.1 2006/08/08 16:59:22 phx Exp $
 */

#include <string.h>


int strcoll(const char *s1,const char *s2)
{
  /* LC_COLLATE is unimplemented, hence always "C" */
  return strcmp(s1,s2);
}
