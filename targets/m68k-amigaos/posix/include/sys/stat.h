/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2005,2010
 *
 * $Id: stat.h,v 1.5 2010/01/10 17:24:19 phx Exp $
 */

#ifndef _SYS_STAT_H_
#define _SYS_STAT_H_

#include <sys/types.h>


struct stat {
        dev_t     st_dev;               /* inode's device */
        ino_t     st_ino;               /* inode's number */
        mode_t    st_mode;              /* inode protection mode */
        nlink_t   st_nlink;             /* number of hard links */
        uid_t     st_uid;               /* user ID of the file's owner */
        gid_t     st_gid;               /* group ID of the file's group */
        dev_t     st_rdev;              /* device type */
        time_t    st_atime;             /* time of last access */
        time_t    st_mtime;             /* time of last data modification */
        time_t    st_ctime;             /* time of last file status change */
        off_t     st_size;              /* file size, in bytes */
        blkcnt_t  st_blocks;            /* blocks allocated for file */
        blksize_t st_blksize;           /* optimal blocksize for I/O */
        u_int32_t st_flags;             /* user defined flags for file */
};


/* st_mode */
#define S_ISUID 0004000                 /* set user id on execution */
#define S_ISGID 0002000                 /* set group id on execution */

#define S_IRWXU 0000700                 /* RWX mask for owner */
#define S_IRUSR 0000400                 /* R for owner */
#define S_IWUSR 0000200                 /* W for owner */
#define S_IXUSR 0000100                 /* X for owner */

#define S_IRWXG 0000070                 /* RWX mask for group */
#define S_IRGRP 0000040                 /* R for group */
#define S_IWGRP 0000020                 /* W for group */
#define S_IXGRP 0000010                 /* X for group */

#define S_IRWXO 0000007                 /* RWX mask for other */
#define S_IROTH 0000004                 /* R for other */
#define S_IWOTH 0000002                 /* W for other */
#define S_IXOTH 0000001                 /* X for other */

#define _S_ISVTX  0001000               /* save swapped text even after use */
#define _S_IFMT   0170000               /* type of file mask */
#define _S_IFIFO  0010000               /* named pipe (fifo) */
#define _S_IFCHR  0020000               /* character special */
#define _S_IFDIR  0040000               /* directory */
#define _S_IFBLK  0060000               /* block special */
#define _S_IFREG  0100000               /* regular */
#define _S_IFLNK  0120000               /* symbolic link */
#define _S_IFSOCK 0140000               /* socket */
#define _S_IFWHT  0160000               /* whiteout */
#define _S_ARCH1  0200000               /* Archive state 1, ls -l shows 'a' */
#define _S_ARCH2  0400000               /* Archive state 2, ls -l shows 'A' */

#define S_IFMT   _S_IFMT
#define S_IFIFO  _S_IFIFO
#define S_IFCHR  _S_IFCHR
#define S_IFDIR  _S_IFDIR
#define S_IFBLK  _S_IFBLK
#define S_IFREG  _S_IFREG
#define S_IFLNK  _S_IFLNK
#define S_ISVTX  _S_ISVTX
#define S_IFSOCK _S_IFSOCK

#define S_ISDIR(m)      ((m & _S_IFMT) == _S_IFDIR)     /* directory */
#define S_ISCHR(m)      ((m & _S_IFMT) == _S_IFCHR)     /* char special */
#define S_ISBLK(m)      ((m & _S_IFMT) == _S_IFBLK)     /* block special */
#define S_ISREG(m)      ((m & _S_IFMT) == _S_IFREG)     /* regular file */
#define S_ISFIFO(m)     ((m & _S_IFMT) == _S_IFIFO)     /* fifo */
#define S_ISLNK(m)      ((m & _S_IFMT) == _S_IFLNK)     /* symbolic link */
#define S_ISSOCK(m)     ((m & _S_IFMT) == _S_IFSOCK)    /* socket */

/*
 * Definitions of flags stored in file flags word.
 *
 * Super-user and owner changeable flags.
 */
#define UF_SETTABLE     0x0000ffff      /* mask of owner changeable flags */
#define UF_NODUMP       0x00000001      /* do not dump file */
#define UF_IMMUTABLE    0x00000002      /* file may not be changed */
#define UF_APPEND       0x00000004      /* writes to file may only append */
#define UF_OPAQUE       0x00000008      /* directory is opaque wrt. union */
/*
 * Super-user changeable flags.
 */
#define SF_SETTABLE     0xffff0000      /* mask of superuser changeable flags */
#define SF_ARCHIVED     0x00010000      /* file is archived */
#define SF_IMMUTABLE    0x00020000      /* file may not be changed */
#define SF_APPEND       0x00040000      /* writes to file may only append */


/* Prototypes */
int chmod(const char *,mode_t);
int fchmod(int,mode_t);
int fstat(int,struct stat *);
int mkdir(const char *,mode_t);
int stat(const char *,struct stat *);
mode_t umask(mode_t);


#endif /* _SYS_STAT_H_ */
