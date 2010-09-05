/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: utime.h,v 1.2 2005/02/27 13:50:45 phx Exp $
 */

#ifndef _UTIME_H_
#define _UTIME_H_

#include <time.h>


struct utimbuf {
        time_t actime;          /* Access time */
        time_t modtime;         /* Modification time */
};


/* Prototypes */

int utime(const char *,const struct utimbuf *);

#endif /* _UTIME_H_ */
