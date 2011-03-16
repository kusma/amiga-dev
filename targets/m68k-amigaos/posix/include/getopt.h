/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: getopt.h,v 1.3 2006/03/13 18:41:06 phx Exp $
 */

#ifndef _GETOPT_H_
#define _GETOPT_H_

#include <unistd.h>

#define no_argument        0
#define required_argument  1
#define optional_argument  2

struct option {
  const char *name;
  int has_arg;
  int *flag;
  int val;
};


/* Prototypes */

int getopt_long(int,char * const *,const char *,const struct option *,int *);

#endif /* _GETOPT_H_ */
