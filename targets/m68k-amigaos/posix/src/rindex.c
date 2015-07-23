/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: rindex.c,v 1.2 2005/02/27 13:45:26 phx Exp $
 */

#include <string.h>


char *rindex(const char *s,int c)
{
  return strrchr(s,c);
}
