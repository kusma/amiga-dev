/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2005
 *
 * $Id: unistd.h,v 1.9 2005/10/16 18:57:07 phx Exp $
 */

#ifndef _UNISTD_H_
#define _UNISTD_H_

#include <stddef.h>
#include <machine/int_types.h>
#include <sys/types.h>
#include <sys/unistd.h>


#define STDIN_FILENO    0       /* standard input file descriptor */
#define STDOUT_FILENO   1       /* standard output file descriptor */
#define STDERR_FILENO   2       /* standard error file descriptor */


/* Prototypes */
int access(const char *,int);
int chdir(const char *);
int chown(const char *,uid_t,gid_t);
int close(int);
int dup(int);
int dup2(int,int);
int fchdir(int);
int fchown(int,uid_t,gid_t);
char *getcwd(char *,size_t);
int getdtablesize(void);
gid_t getegid(void);
uid_t geteuid(void);
gid_t getgid(void);
char *getlogin(void);
pid_t getpid(void);
pid_t getppid(void);
uid_t getuid(void);
int isatty(int);
int link(const char *,const char *);
off_t lseek(int,off_t,int);
int read(int,void *,size_t);
int rmdir(const char *);
int setegid(gid_t);
int seteuid(uid_t);
int setgid(gid_t);
int setlogin(const char *);
int setuid(uid_t);
unsigned int sleep(unsigned int);
int symlink(const char *,const char *);
int unlink(const char *);
int write(int,const void *,size_t);


#if !defined(_POSIX_C_SOURCE)

int fsync(int);

int getopt(int,char * const [],const char *);
/* getopt() external variables */
extern char *optarg;
extern int opterr;
extern int optind;
extern int optopt;

char *getpass(const char *);

long __Pgethostid(void);
int __Pgethostname(char *, size_t);

#ifndef gethostid
#define gethostid() __Pgethostid()
#endif
#ifndef gethostname
#define gethostname(a,b) __Pgethostname(a,b)
#endif

#if __STDC__
struct timeval;
#endif
int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
int usleep(useconds_t);

#endif /* _POSIX_C_SOURCE */


#endif /* _UNISTD_H_ */
