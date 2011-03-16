#ifndef STDIO_H
#define STDIO_H \
       "$Id: stdio.h,v 4.5 1996/03/26 23:00:27 too Exp $"
/*
 *      Standard ANSI C IO library subset for AmigaOS 2.04 dos.library
 *
 *      Copyright © 1994 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

#ifndef USE_DOSIO
#include <include:stdio.h>
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

#ifndef DOS_STDIO_H
#include <dos/stdio.h>
#endif

#ifndef _COMMSIZE_H
#include <sys/commsize.h>
#endif

#ifndef _COMMLIST_H
#include <sys/commlist.h>
#endif

typedef unsigned long fpos_t;

#ifndef _COMMNULL_H
#include <sys/commnull.h>
#endif

#ifndef DOS_RDARGS_H
#include <dos/rdargs.h>
#endif

ULONG VCSPrintf(struct CSource *buf, const char *fmt, va_list ap);
ULONG CSPrintf(struct CSource *buf, const char *fmt, ...);
ULONG VSPrintf(STRPTR, const char *, va_list);
ULONG SPrintf(STRPTR, const char *, ...);

#define _IOFBF   BUF_FULL
#define _IONBF   BUF_NONE
#define _IOLBF   BUF_LINE

#define BUFSIZ		512
#define EOF		ENDSTREAMCH
#define FOPEN_MAX	20
#define FILENAME_MAX	64
#define L_tmpnam	64

#define SEEK_SET OFFSET_BEGINNING	/* Seek from beginning of file */
#define SEEK_CUR OFFSET_CURRENT		/* Seek from current file position */
#define SEEK_END OFFSET_END		/* Seek from end of file */

#define TMP_MAX 999             /* Guaranteed unique temp names */

typedef long FILE;		/* have to define something */

extern BPTR __dosio_files[3];	/* defined in dosio_init.c, autoinitialized */

#define stdin  __dosio_files[0]	/* standard input file pointer */
#define stdout __dosio_files[1]	/* standard output file pointer */
#define stderr __dosio_files[2]	/* standard error file pointer */

/*
 * Prototypes for ANSI standard functions.
 */

#define remove(name) !DeleteFile((STRPTR)(name))

#define rename(oldname, newname) !Rename((STRPTR)(oldname), (STRPTR)(newname))

#if 0
extern FILE *tmpfile(void);
extern char *tmpnam(char *s);
#endif

#define fclose(f) !Close((f))

#define fflush(f) ((Flush((f))) ? 0 : EOF)

#if 0
extern FILE *fopen(const char *, const char *);
extern FILE *freopen(const char *, const char *, FILE *);

extern void setbuf(FILE *, char *);
#endif

#define setvbuf(fh, buff, type, size) \
  SetVBuf((fh), (STRPTR)(buff), (type), (size))

#define fprintf FPrintf

#if 0
extern int fscanf(FILE *, const char *, ...);
#endif

#define printf Printf

#if 0
extern int scanf(const char *, ...);
#endif

/* this is from net.lib, not from dos.library */
#define sprintf SPrintf

#if 0
extern int sscanf(const char *, const char *, ...);
#endif

#define vfprintf VFPrintf
#define vprintf VPrintf

/* this is from net.lib, not from dos.library */
#define vsprintf VSPrintf

#define fgetc FGetC

#define fgets(buf, len, fh) FGets((fh), (buf), (len))

#define fputc(c, fh) FPutC((fh), (c))

#define fputs(str, fh) FPuts((fh), (STRPTR)(str))

#define getc fgetc

#define getchar() getc(stdin)

#define gets(buf) fgets((buf), 1024, stdin) /* length is a kludge */

#define putc fputc

#define putchar(c) putc((c),stdout)

#define puts(str) fputs((str), stdout)

#define ungetc(c, fh) UnGetC((fh), (c))

#define fread(buf, blocklen, blocks, fh) \
  FRead((fh), (APTR)(buf), (blocklen), (blocks))

#define fwrite(buf, blocklen, blocks, fh) \
  FWrite((fh), (APTR)(buf), (blocklen), (blocks))

#define fgetpos(fh, fposp) (*(fposp) = Seek((fh), 0L, OFFSET_CURRENT), \
			    (*(fposp) == (-1)))

#define fseek Seek

#define fsetpos(fh, fposp) \
  ((Seek((fh), *(fposp), OFFSET_BEGINNING) == -1) ? EOF : 0)

#define ftell(fh) Seek((fh), 0L, OFFSET_CURRENT)

#define rewind(fh) (void)Seek((fh), 0L, OFFSET_BEGINNING)

#if 0
#define clearerr(fh) SetIoErr(0L) /* not exact */
#endif

#if 0
extern int feof(FILE *);
#define feof(p) (((p)->_flag & _IOEOF) != 0)
#endif

#if 0
#define ferror(fh) (IoErr() != 0) /* not exact */
#endif

/* this is in net.lib */
extern void perror(const char *);

#ifndef _STRICT_ANSI

/* defines for mode of access() */
#define R_OK 4
#define W_OK 2
#define X_OK 1
#define F_OK 0

/*
 * Prototypes for Non-ANSI functions.
 */

extern int __io2errno(int);

#define fgetchar() fgetc(stdin)

#define fputchar(c) fputc((c), stdout)

#define setnbf(fh) (void)SetVBuf((fh), NULL, BUF_NONE, -1)

extern int __access(const char *, int);
extern int access(const char *, int);
extern int chdir(const char *);
extern int chmod(const char *, int);
extern char *getcwd(char *, int);

/* extern int unlink(const char *); in fcntl.h*/

#define  clrerr  clearerr
#define  access  __access

#endif /* _STRICT_ANSI */

#if 0
extern unsigned long __fmask;
extern int __fmode;
#endif

#endif /* USE_DOSIO */
#endif /* STDIO_H */
