#ifndef SYS_STAT_H
#define SYS_STAT_H \
       "$Id: stat.h,v 4.1 1994/10/05 23:16:17 ppessi Exp $"
/*
 *	Unix-compatible stat() definitions for SAS/C
 *
 *      Copyright © 1994 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

#ifndef SYS_TYPES_H
#include <sys/types.h>
#endif

struct	__stat
{
	dev_t	st_dev;		/* unique device id */
	ino_t	st_ino;		/* inode of file (key block) */
	mode_t  st_mode;	/* Unix style mode */
	u_short	st_nlink;	/* number of links (unimplemented) */
	uid_t	st_uid;		/* owner's user ID */
	gid_t	st_gid;		/* owner's group ID */
	dev_t	st_rdev;	/* special file ID (unimplemented) */
	off_t	st_size;	/* file size */
	time_t	st_atime;	/* Time of last access */
	time_t	st_mtime;	/* Last modification time */
	time_t	st_ctime;       /* Last file status change time */
	long	st_blksize;	/* Size of disk block */
	long	st_blocks;	/* Size in blocks */
	long    st_dosmode;	/* DOS protection bits */
        short   st_type;	/* DOS file type */
        char   *st_comment;     /* DOS file comment */
};

#if defined(__SASC) && defined(S_IFMT)
#undef S_IFMT
#undef S_IFDIR
#undef S_IFREG
#undef S_ISCRIPT
#undef S_IPURE
#undef S_IARCHIVE
#undef S_IREAD
#undef S_IWRITE
#undef S_IEXECUTE
#undef S_IDELETE
#endif

#define	S_ISUID	0004000		/* set user id on execution */
#define	S_ISGID	0002000		/* set group id on execution */
#define	S_ISVTX	0001000		/* save swapped text even after use */

#define	S_IRWXU	0000700		/* RWX mask for owner */
#define	S_IRUSR	0000400		/* R for owner */
#define	S_IWUSR	0000200		/* W for owner */
#define	S_IXUSR	0000100		/* X for owner */

#define	S_IREAD		S_IRUSR
#define	S_IWRITE	S_IWUSR
#define	S_IEXEC		S_IXUSR

#define	S_IRWXG	0000070		/* RWX mask for group */
#define	S_IRGRP	0000040		/* R for group */
#define	S_IWGRP	0000020		/* W for group */
#define	S_IXGRP	0000010		/* X for group */

#define	S_IRWXO	0000007		/* RWX mask for other */
#define	S_IROTH	0000004		/* R for other */
#define	S_IWOTH	0000002		/* W for other */
#define	S_IXOTH	0000001		/* X for other */

#define	S_IFMT	 0170000	/* type of file */
#define	S_IFCHR	 0020000	/* character special */
#define	S_IFDIR	 0040000	/* directory */
#define	S_IFBLK	 0060000	/* block special */
#define	S_IFREG	 0100000	/* regular */
#define	S_IFLNK	 0120000	/* symbolic link */
#define	S_IFSOCK 0140000	/* socket */
#define	S_IFIFO	 0010000	/* named pipe (fifo) */

#define S_BLKSIZE	512	/* block size used in the stat struct */

#define	S_ISDIR(m)	((m & 0170000) == 0040000)	/* directory */
#define	S_ISCHR(m)	((m & 0170000) == 0020000)	/* char special */
#define	S_ISBLK(m)	((m & 0170000) == 0060000)	/* block special */
#define	S_ISREG(m)	((m & 0170000) == 0100000)	/* regular file */
#define	S_ISLNK(m)	((m & 0170000) == 0120000)	/* symbolic link */
#define	S_ISFIFO(m)	((m & 0170000) == 0010000)	/* fifo */
#define	S_ISSOCK(m)	((m & 0170000) == 0140000)	/* socket */

mode_t umask(mode_t);
int __chmod(const char *, int);
int __fstat(int, struct __stat *);
int __stat(const char *, struct __stat *);
int __lstat(const char *, struct __stat *);

#define chmod __chmod
#define fstat __fstat
#define stat  __stat
#define lstat __lstat

#endif /* SYS_STAT_H */
