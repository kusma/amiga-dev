/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: fnmatch.h,v 1.2 2005/02/27 13:50:44 phx Exp $
 */

#ifndef _FNMATCH_H_
#define _FNMATCH_H_

#define FNM_NOMATCH     1       /* Match failed. */
#define FNM_NOSYS       2       /* Function not implemented. */

#define FNM_NOESCAPE    0x01    /* Disable backslash escaping. */
#define FNM_PATHNAME    0x02    /* Slash must be matched by slash. */
#define FNM_PERIOD      0x04    /* Period must be matched by period. */


/* Prototypes */

int fnmatch(const char *,const char *,int);

#endif /* _FNMATCH_H_ */
