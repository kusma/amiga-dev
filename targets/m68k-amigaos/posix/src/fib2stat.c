/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: fib2stat.c,v 1.2 2005/02/27 13:45:23 phx Exp $
 */

#pragma amiga-align
#include <dos/dosextens.h>
#pragma default-align
#include <errno.h>
#include <sys/stat.h>
#include "prot.h"

#define UNIXOFFSET 252457200  /* seconds from 1.1.70 to 1.1.78 */


int __fib2stat(struct FileInfoBlock *fib,struct stat *sb)
{
  blksize_t bs;

  /* these fields have no equivalent under AmigaOS and are set to zero */
  sb->st_dev = 0;
  sb->st_rdev = 0;

  /* just assume that there are no hard-links to this object */
  sb->st_nlink = 1;

  /* the DiskKey has a similar meaning like a Unix-inode */
  sb->st_ino = (ino_t)fib->fib_DiskKey;

  /* convert protection mode and set file type */
  sb->st_mode = __prot2mode((unsigned long)fib->fib_Protection);

  switch (fib->fib_DirEntryType) {
    case ST_PIPEFILE:
      sb->st_mode |= _S_IFIFO; break;
    case ST_SOFTLINK:
      sb->st_mode |= _S_IFLNK; break;
    case ST_ROOT:
    case ST_USERDIR:
    case ST_LINKDIR:
      sb->st_mode |= _S_IFDIR; break;
    default: /* everything else is a regular file */
      sb->st_mode |= _S_IFREG; break;
  }

  if (fib->fib_Protection & FIBF_ARCHIVE)
    sb->st_mode |= _S_ARCH1;  /* @@@ show archive-state 1 */

  /* uid/gid from FIB may be filled by a multi-user filesystem */
  sb->st_uid = (uid_t)fib->fib_OwnerUID;
  sb->st_gid = (gid_t)fib->fib_OwnerGID;

  /* no difference between modification/change/access time */
  sb->st_atime = sb->st_mtime = sb->st_ctime =
    (time_t)(UNIXOFFSET + (uint32_t)(fib->fib_Date.ds_Days*(60*60*24) +
                                     fib->fib_Date.ds_Minute*60 +
                                     fib->fib_Date.ds_Tick/TICKS_PER_SECOND));

  /* size in bytes and blocks */
  sb->st_size = (off_t)fib->fib_Size;
  sb->st_blocks = (blkcnt_t)fib->fib_NumBlocks;

  /* calculate propable block size from the above values */
  for (bs = 512; bs<0x10000; bs<<=1) {
    if ((blkcnt_t)(((blksize_t)sb->st_size + (bs-1)) / bs) == sb->st_blocks)
      break;
  }
  sb->st_blksize = bs;

  /* only the SF_ARCHIVED flag is supported */
  sb->st_flags = (fib->fib_Protection & FIBF_ARCHIVE) ? SF_ARCHIVED : 0;

  errno = 0;
  return 0;
}
