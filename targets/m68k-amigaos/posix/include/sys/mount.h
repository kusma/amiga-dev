/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: mount.h,v 1.2 2005/02/27 13:50:46 phx Exp $
 */

#ifndef _SYS_MOUNT_H_
#define _SYS_MOUNT_H_

#include <sys/types.h>


typedef struct { int32_t val[2]; } fsid_t;      /* file system id type */

/*
 * file system statistics
 */
struct statfs {
        short   f_type;                 /* type of file system */
        uint16_t f_oflags;              /* deprecated copy of mount flags */
        long    f_bsize;                /* fundamental file system block size */
        long    f_blocks;               /* total data blocks in file system */
        long    f_bfree;                /* free blocks in fs */
        long    f_bavail;               /* free blocks avail to non-superuser */
        long    f_files;                /* total file nodes in file system */
        long    f_ffree;                /* free file nodes in fs */
        fsid_t  f_fsid;                 /* file system id */
        uid_t   f_owner;                /* user that mounted the file system */
        long    f_flags;                /* copy of mount flags */
};

/* mount flags: */
#define MNT_RDONLY      0x00000001      /* read only filesystem */
#define MNT_SYNCHRONOUS 0x00000002      /* file system written synchronously */
#define MNT_NOEXEC      0x00000004      /* can't exec from filesystem */
#define MNT_NOSUID      0x00000008      /* don't honor setuid bits on fs */
#define MNT_NODEV       0x00000010      /* don't interpret special files */
#define MNT_UNION       0x00000020      /* union with underlying filesystem */
#define MNT_ASYNC       0x00000040      /* file system written asynchronously */
#define MNT_NOCOREDUMP  0x00008000      /* don't write core dumps to this FS */
#define MNT_IGNORE      0x00100000      /* don't show entry in df */
#define MNT_NOATIME     0x04000000      /* Never update access times in fs */
#define MNT_SYMPERM     0x20000000      /* recognize symlink permission */
#define MNT_NODEVMTIME  0x40000000      /* Never update mod times for devs */
#define MNT_SOFTDEP     0x80000000      /* Use soft dependencies */


/* Prototypes */
int fstatfs(int,struct statfs *);
int statfs(const char *,struct statfs *);

#endif /* _SYS_MOUNT_H_ */
