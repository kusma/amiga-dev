/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: dirent.h,v 1.3 2006/03/12 15:12:22 phx Exp $
 */

#ifndef _DIRENT_H_
#define _DIRENT_H_

#include <sys/types.h>
#include <sys/dirent.h>

typedef void *DIR;  /* DIR is private! */


/* Prototypes */

int closedir(DIR *);
DIR *opendir(const char *);
struct dirent *readdir(DIR *);
void rewinddir(DIR *);

#if !defined(_POSIX_C_SOURCE)
int scandir(const char *, struct dirent ***, int (*)(const struct dirent *),
            int (*)(const void *, const void *));
int alphasort(const void *, const void *);
#endif

#endif /* _DIRENT_H_ */
