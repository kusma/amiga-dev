/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2004
 *
 * $Id: strings.h,v 1.2 2004/11/28 17:37:15 phx Exp $
 */

#ifndef _STRINGS_H_
#define _STRINGS_H_

#include <stddef.h>


/* Prototypes */
int bcmp(const void *,const void *,size_t);
void bcopy(const void *,void *,size_t);
void bzero(void *,size_t);
int ffs(int);
char *index(const char *,int);
char *rindex(const char *,int);
int strcasecmp(const char *,const char *);
int strncasecmp(const char *,const char *,size_t);


#ifndef _STRING_H_
#include <string.h>
#endif

#endif /* _STRINGS_H_ */
