#ifndef CLIB_NETLIB_PROTOS_H
#define CLIB_NETLIB_PROTOS_H \
       "$Id: netlib_protos.h,v 4.4 1996/02/16 16:19:31 too Exp $"
/*
 *      Prototypes for netlib utility functions
 *
 *      Copyright © 1994 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef SYS_TYPES_H
#include <sys/types.h>
#endif
#ifndef SYS_CDEFS_H
#include <sys/cdefs.h>
#endif
#ifndef EXEC_LIBRARIES_H
struct Library;
#endif
#ifndef SYS_TIME_H
#include <sys/time.h>
#endif
#ifndef SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifndef NETDB_H
struct hostent;
struct netent;
struct servent;
struct protoent;
#endif
#ifndef NETINET_IN_H
#include <netinet/in.h>
#endif
#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif
#ifndef DOS_RDARGS_H
#include <dos/rdargs.h>
#endif
#ifndef SYS_STAT_H
#include <sys/stat.h>
#endif

#ifndef _STDARG_H
#include <stdarg.h>
#endif

#if !__SASC
#define __stdargs
#endif

/* access.c */
int __access(const char *name, int mode);

/* autoinit.c */
extern int h_errno;
extern struct Library *SocketBase;
LONG __stdargs _STI_200_openSockets(void);
void __stdargs _STD_200_closeSockets(void);

/* chmod.c */
int chmod(const char *path, int mode);

/* chown.c */
int chown(const char *name, uid_t uid, gid_t gid);

/* dosio_init.c */
extern BPTR __dosio_files[3];
long __stdargs _STI_500_dosio_init(void);

/* dosio_sprintf.c */
ULONG VCSPrintf(struct CSource *, const char *, va_list);
ULONG CSPrintf(struct CSource *, const char *, ...);
ULONG VSPrintf(STRPTR, const char *, va_list);
ULONG SPrintf(STRPTR, const char *, ...);

/* dummy.c */
struct hostent  *gethostent(void);
struct netent  *getnetent(void);
struct servent  *getservent(void);
struct protoent *getprotoent(void);

/* fhopen.c */
int fhopen(long file, int mode);

/* getopt.c */
extern int opterr;
extern int optind;
extern int optopt;
extern char *optarg;
int   getopt(int argc, char * const argv[], char const *opts);

/* getpid.c */
pid_t getpid(void);

/* gettimeofday.c */
int   gettimeofday(struct timeval *tp, struct timezone *tzp);

/* herror.c */
void  herror(const char *banner);

/* init_inet_daemon.c */
int init_inet_daemon(void);

/* init_usergroup.c */
extern struct Library *UserGroupBase;
LONG __stdargs _STI_200_openUserGroup(void);
void __stdargs _STD_200_closeUserGroup(void);

/* ioctl.c */
int ioctl(int fd, unsigned int request, char *argp);

/* iomode.c */
int iomode(int fd, int mode);

/* isatty.c */
int isatty(int fd);

/* lineread.c */
int   lineRead(struct LineRead * rl);

/* perror.c */
void  perror(const char *banner);

/* popen.c */
#ifndef _STDIO_H
struct __iobuf; /* typedef struct __iobuf FILE */
#endif
struct __iobuf *popen(const char *cmd, const char *mode);
struct __iobuf *popenl(const char *arg0, ...);
int pclose(struct __iobuf *fptr);
void __stdargs _STD_4000_popen(void);
char *mktemp(char * template);

/* printfault.c */
void  PrintNetFault(LONG code, const UBYTE *banner);

/* printuserfault.c */
void  PrintUserFault(LONG code, const UBYTE *banner);

/* rcmd.c */
int   rcmd(char **, int, const char *, const char *, const char *, int *);
int   rresvport(int *alport);

/* serveraccept.c */
long serveraccept(char *pname, struct sockaddr_in *ha);

/* set_socket_stdio.c */
int set_socket_stdio(int sock);

/* setegid.c */
int setegid(gid_t g);

/* seteuid.c */
int seteuid(uid_t u);

/* sleep.c */
void sleep(unsigned int secs);

/* stat.c */
int stat(const char *name, struct stat *st);

/* strerror.c */
char *strerror(int code);

/* stubs.c */
#if !defined(__SASC) || !defined(_OPTINLINE) /* these are inlined for SAS/C */
char * inet_ntoa(struct in_addr addr);
struct in_addr inet_makeaddr(int net, int host);
unsigned long inet_lnaof(struct in_addr addr);
unsigned long inet_netof(struct in_addr addr);
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exeptfds,
	   struct timeval *timeout);
#endif

/* syslog.c */
void  openlog(const char *ident, int logstat, int logfac);
void  closelog(void);
int   setlogmask(int pmask);

/* timerinit.c */
extern long __local_to_GMT;
extern struct Library *TimerBase; /* actually struct Device * */
LONG __stdargs _STI_200_openTimer(void);
void __stdargs _STD_200_closeTimer(void);

/* usleep.c */
void usleep(unsigned int usecs);

/* utime.c */
#ifndef UTIME_H
struct utimbuf;
#endif
int utime(const char *name, const struct utimbuf *times);

#ifndef _IOS1_H
struct UFB;
#endif

/* _allocufb.c */
struct UFB * __allocufb(int *fdp);

/* _chkufb.c */
long __stdargs _STI_1000_install_AmiTCP_callback(void);
long ASM SAVEDS fdCallback(REG(d0) int fd, REG(d1) int action);
struct UFB * __chkufb(int fd);

/* _close.c */
int __close(int fd);

/* _dup.c */
/* _dup2.c */

/* _fstat.c */
int fstat(int fd, struct stat *st);

/* _lseek.c */
long __lseek(int fd, long rpos, int mode);

/* _open.c */
__stdargs int __open(const char *name, int mode, ...);

/* _read.c */
int __read(int fd, void *buffer, unsigned int length);

/* _write.c */
int __write(int fd, const void *buffer, unsigned int length);

#endif /* !CLIB_NETLIB_PROTOS_H */
