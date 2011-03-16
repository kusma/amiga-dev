/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: time.h,v 1.2 2005/02/27 13:50:47 phx Exp $
 */

#ifndef _SYS_TIME_H_
#define _SYS_TIME_H_

#include <time.h>

/* workaround to coexist with AmigaOS timeval structure */
#ifndef TIMERNAME /* defined in devices/timer.h */
struct timeval {
        long    tv_sec;         /* seconds */
        long    tv_usec;        /* and microseconds */
};
#else
#define tv_sec tv_secs
#define tv_usec tv_micro
#endif

struct timespec {
        time_t  tv_sec;         /* seconds */
        long    tv_nsec;        /* and nanoseconds */
};

#define TIMEVAL_TO_TIMESPEC(tv, ts) {                                   \
        (ts)->tv_sec = (tv)->tv_sec;                                    \
        (ts)->tv_nsec = (tv)->tv_usec * 1000;                           \
}
#define TIMESPEC_TO_TIMEVAL(tv, ts) {                                   \
        (tv)->tv_sec = (ts)->tv_sec;                                    \
        (tv)->tv_usec = (ts)->tv_nsec / 1000;                           \
}


/* Prototypes */

int futimes(int,const struct timeval *);
int gettimeofday(struct timeval *,void *);
int utimes(const char *,const struct timeval *);
int settimeofday(const struct timeval *,void *);

#endif /* _SYS_TIME_H_ */
