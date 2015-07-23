/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2006
 *
 * $Id: open.c,v 1.8 2006/06/02 22:13:37 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "fdesc.h"
#include "conv.h"
#include "prot.h"


int open(const char *name, int flags, ...)
{
  struct FileInfoBlock fib;  /* longword-aligned! */
  int fd;
  struct __fd_s *fp;
  mode_t mode;
  va_list vl;
  BPTR fh=0,lock;

  errno = 0;
  va_start(vl,flags);
  mode = va_arg(vl,mode_t);
  va_end(vl);

  if (name == NULL) {
    errno = EFAULT;
    return -1;
  }
  if ((flags & O_ACCMODE) == O_ACCMODE) {
    /* only O_RDONLY, O_WRONLY or O_RDWR allowed! */
    errno = EINVAL;
    return -1;
  }
  if ((fd = __next_free_fd(3)) < 0)
    return -1;
  if ((fp = __new_fdesc()) == NULL)
    return -1;
  flags &= O_ACCMODE|O_NONBLOCK|O_APPEND|O_CREAT|O_TRUNC|O_EXCL;
  fp->open_flags = flags;

  if (lock = Lock(__convert_path(name),SHARED_LOCK)) {
    if (Examine(lock,&fib)) {
      if (fib.fib_DirEntryType >= 0) {
        /* it's a directory! special handling! */
        if (flags & (O_CREAT|O_TRUNC|O_APPEND|O_RDWR|O_WRONLY)) {
          errno = EISDIR;
          free(fp);
          UnLock(lock);
          return -1;
        }
        fp->file = lock;
        fp->flags |= FDFL_DIRLOCK;
        __fdesc[fd] = fp;
        return fd;
      }
    }
    if (!(fh = OpenFromLock(lock)))
      UnLock(lock);
  }
  else if (IoErr() == ERROR_ACTION_NOT_KNOWN) {
    /* handler doesn't support Lock(), so it's not a directory - try Open() */
    fh = Open(__convert_path(name),MODE_OLDFILE);
  }

  if (flags & O_CREAT) {
    if ((flags& O_EXCL) && fh!=0) {   /* file already exists? */
      errno = EEXIST;
      free(fp);
      Close(fh);
      return -1;
    }
    if (fh == 0) {
      /* we have to create the file */
      if (fh = Open(__convert_path(name),MODE_NEWFILE)) {
        /* @@@ use ChangeMode() ? */
        Close(fh);
        fh = Open(__convert_path(name),MODE_OLDFILE);
        /* The protection mode has to be set when the file is closed,
           otherwise we might not be able to write to it any longer. */
        fp->flags |= FDFL_MODESET;
        fp->amode = __mode2prot(__get_masked_prot(mode));
      }
      if (fh == 0) {
        if (IoErr() == ERROR_OBJECT_NOT_FOUND)
          errno = ENOENT;  /* a path component is missing? */
        else
          errno = EACCES;
        free(fp);
        return -1;
      }
      flags &= ~(O_TRUNC|O_APPEND);  /* don't need those on a fresh file */
    }
  }
  else if (fh == 0) {
    errno = ENOENT;
    free(fp);
    return -1;
  }

  if (flags & O_TRUNC) {
    LONG rc = SetFileSize(fh,0,OFFSET_BEGINNING);

    if (IoErr() == ERROR_ACTION_NOT_KNOWN) {
      /* fallback to NEWFILE-method, which might destroy protection flags */
      Close(fh);
      if (fh = Open(__convert_path(name),MODE_NEWFILE)) {
        Close(fh);
        fh = Open(__convert_path(name),MODE_OLDFILE);
      }
      rc = fh ? 0 : -1;
    }
    if (rc == -1) {
      errno = EIO; /* @@@ truncating failed */
      free(fp);
      return -1;
    }
    flags &= ~O_APPEND;  /* doesn't make sense on truncated files */
  }

  if (flags & O_APPEND) {
    if (Seek(fh,0,OFFSET_END) == -1) {
      errno = EIO; /* @@@ seek failed */
      Close(fh);
      free(fp);
      return -1;
    }
  }

  if (IsInteractive(fh))
    fp->flags |= FDFL_INTERACTIVE;
  fp->file = fh;
  __fdesc[fd] = fp;

  return fd;
}
