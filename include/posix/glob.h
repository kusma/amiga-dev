/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: glob.h,v 1.2 2005/02/27 13:50:45 phx Exp $
 */

#ifndef _GLOB_H_
#define _GLOB_H_

#include <sys/types.h>


typedef struct {
        int gl_pathc;           /* Count of total paths so far. */
        int gl_matchc;          /* Count of paths matching pattern. */
        int gl_offs;            /* Reserved at beginning of gl_pathv. */
        int gl_flags;           /* Copy of flags parameter to glob. */
        char **gl_pathv;        /* List of paths matching pattern. */
                                /* Copy of errfunc parameter to glob. */
} glob_t;

#define GLOB_APPEND     0x0001  /* Append to output from previous call. */
#define GLOB_DOOFFS     0x0002  /* Use gl_offs. */
#define GLOB_ERR        0x0004  /* Return on error. */
#define GLOB_MARK       0x0008  /* Append / to matching directories. */
#define GLOB_NOCHECK    0x0010  /* Return pattern itself if nothing matches. */
#define GLOB_NOSORT     0x0020  /* Don't sort. */
#define GLOB_MAGCHAR    0x0100  /* Pattern had globbing characters. */
#define GLOB_NOESCAPE   0x1000  /* Disable backslash escaping. */

#define GLOB_NOSPACE    (-1)    /* Malloc call failed. */
#define GLOB_ABORTED    (-2)    /* Unignored error. */
#define GLOB_NOMATCH    (-3)    /* No match, and GLOB_NOCHECK was not set. */
#define GLOB_NOSYS      (-4)    /* Implementation does not support function. */


/* Prototypes */

int glob(const char *,int,int (*)(const char *,int),glob_t *);
void globfree(glob_t *);

#endif /* _GLOB_H_ */
