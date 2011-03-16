#ifndef FCNTL_H
#define FCNTL_H \
       "$Id: fcntl.h,v 4.3 1996/03/26 22:58:42 too Exp $"
/*
 *      fcntl.h using AmigaOS 2.04 dos.library 
 *
 *      Copyright © 1994 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

#ifndef USE_DOSIO
#include <include:fcntl.h>
#else

#if defined(__SASC) || defined(__GNUC__)
#ifndef  PROTO_DOS_H
#include <proto/dos.h>
#endif
#else
#ifndef  CLIB_DOS_PROTOS_H
#include <clib/dos_protos.h>
#endif
#endif

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif

extern BPTR __dosio_files[3];	/* defined in dosio_init.c, autoinitialized */

/*
 * level 1 IO is supported for stdio/stdout/stderr only
 */

#ifndef _COMMIFMT_H
#include <sys/commifmt.h>
#endif

#if 0 /* not supported */
extern int  open   (const char *, int, ...);
extern int  creat  (const char *, int);
#endif

#define read(fd, buf, len) \
  (((unsigned)(fd) < 3) ? Read(__dosio_files[(fd)], (buf), (len)) : -1)

#define write(fd, buf, len) \
  (((unsigned)(fd) < 3) ? Write(__dosio_files[(fd)], (buf), (len)) : -1)

#define lseek(fd, pos, mode) \
  (((unsigned)(fd) < 3) ? Seek(__dosio_files[(fd)], (pos), (mode)-1) : -1)


#define tell(x)    lseek(x, 0L, 1)

#if 0 /* stdin, stdout, stderr should not be closed */
extern int  close  (int);
#endif

#define unlink(name) !DeleteFile((STRPTR)(name))

#if 0 /* SAS/C iomode() not supported */
extern int  iomode (int, int);
#endif

#define isatty(fd) \
  (((unsigned)(fd) < 3) ? IsInteractive(__dosio_files[(fd)]) & 0x1 : 0)

#ifndef _COMMNULL_H
#include <sys/commnull.h>
#endif

#endif /* USE_DOSIO */

#endif /* FCNTL_H */
