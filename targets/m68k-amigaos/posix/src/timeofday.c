/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: timeofday.c,v 1.2 2005/02/27 13:45:27 phx Exp $
 * Time zone is ignored.
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <errno.h>
#include <sys/time.h>

#define UNIXOFFSET 252460800  /* seconds from 1.1.70 to 1.1.78 */

#ifndef EPERM
#define EPERM 1 /* vbcc's errno.h doesn't define EPERM */
#endif


int gettimeofday(struct timeval *tv,void *tz)
{
  struct DateStamp ds;

  DateStamp(&ds);
  tv->tv_sec = (long)(UNIXOFFSET + ds.ds_Days*(60*60*24) +
                      ds.ds_Minute*60 + ds.ds_Tick/TICKS_PER_SECOND);
  tv->tv_usec = (long)(ds.ds_Tick%TICKS_PER_SECOND)*(1000000/TICKS_PER_SECOND);
  return 0;
}


int settimeofday(const struct timeval *tv,void *tz)
{
  errno = EPERM;
  return -1;
}
