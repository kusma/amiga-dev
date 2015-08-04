/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2005
 *
 * $Id: utime.c,v 1.3 2005/10/17 20:14:53 phx Exp $
 * Time zone is ignored.
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <errno.h>
#include <utime.h>
#include <sys/time.h>
#include <sys/syslimits.h>
#include "conv.h"
#include "fdesc.h"

#define UNIXOFFSET 252457200  /* seconds from 1.1.70 to 1.1.78 */
#define SECPERDAY (60*60*24)
#define SECPERMIN 60


static int setfiledate(char *fname,const struct timeval *tv)
{
  struct DateStamp ds;

  if (tv) {
    time_t sec = tv[1].tv_sec - UNIXOFFSET;

    ds.ds_Days = (LONG)(sec / SECPERDAY);
    sec -= (time_t)ds.ds_Days * SECPERDAY;
    ds.ds_Minute = (LONG)(sec / SECPERMIN);
    sec -= (time_t)ds.ds_Minute * SECPERMIN;
    ds.ds_Tick = (LONG)(sec * TICKS_PER_SECOND +
                        (tv[1].tv_usec * TICKS_PER_SECOND) / 1000000);
  }
  else
    DateStamp(&ds);

  if (!SetFileDate(fname,&ds)) {
    switch (IoErr()) {
      case ERROR_OBJECT_NOT_FOUND:
        errno = ENOENT; break;
      case ERROR_DISK_WRITE_PROTECTED:
        errno = EROFS; break;
      default:
        errno = EIO; break;
    }
    return -1;
  }

  errno = 0;
  return 0;
}


int utimes(const char *path,const struct timeval *times)
{
  return setfiledate(__convert_path(path),times);
}


int futimes(int fd,const struct timeval *times)
{
  struct __fd_s *fp;
  char name[PATH_MAX];
  LONG rc;

  if (!(fp = __chk_fd(fd)))
    return -1;

  if (fp->flags & FDFL_SOCKET) {
    errno = EFTYPE;
    return -1;
  }

  if (fp->flags & FDFL_DIRLOCK)
    rc = NameFromLock(fp->file,name,PATH_MAX);
  else
    rc = NameFromFH(fp->file,name,PATH_MAX);

  if (rc == DOSFALSE) {
    errno = ENAMETOOLONG;
    return -1;
  }

  return setfiledate(name,times);
}


int utime(const char *file,const struct utimbuf *timep)
{
  struct timeval tv[2];

  if (timep) {
    tv[0].tv_sec = (long)timep->actime;
    tv[1].tv_sec = (long)timep->modtime;
    tv[0].tv_usec = tv[1].tv_usec = 0;
  }

  return setfiledate(__convert_path(file),timep ? tv : NULL);
}
