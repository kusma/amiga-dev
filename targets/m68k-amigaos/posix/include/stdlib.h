/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005-2006
 *
 * $Id: stdlib.h,v 1.6 2006/08/09 11:05:34 phx Exp $
 */

#ifndef _STDLIB_H_
#define _STDLIB_H_

#include_next <stdlib.h>

#undef RAND_MAX
#define RAND_MAX 0x7fffffff

/* Prototypes */
int mkstemp(char *);
char *mktemp(char *);

#if !defined(_POSIX_C_SOURCE) && !defined(_ANSI_SOURCE)
const char *getprogname(void);
long random(void);
void setprogname(const char *);
void srandom(unsigned long);
int setenv(const char *, const char *, int);
void unsetenv(const char *);

/*
 * Implementation-defined extensions
 */
char *cgetcap(char *, const char *, int);
int cgetclose(void);
int cgetent(char **, char **, const char *);
int cgetfirst(char **, char **);
int cgetmatch(const char *, const char *);
int cgetnext(char **, char **);
int cgetnum(char *, const char *, long *);
int cgetset(const char *);
int cgetstr(char *, const char *, char **);
int cgetustr(char *, const char *, char **);
#endif

#endif /* _STDIO_H_ */
