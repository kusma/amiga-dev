/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: index.c,v 1.2 2005/02/27 13:45:25 phx Exp $
 */

#include <string.h>


char *index(const char *s,int c)
{
  return strchr(s,c);
}
