/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: info2statfs.c,v 1.2 2005/02/27 13:45:25 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#pragma default-align
#include <errno.h>
#include <sys/mount.h>


int __info2statfs(struct InfoData *info,struct statfs *buf)
{
  /* always assume fs type 1 */
  buf->f_type = 1;

  /* @@@ unknown fs id type */
  buf->f_fsid.val[0] = -1;
  buf->f_fsid.val[1] = -1;

  /* owner is always root (=0) */
  buf->f_owner = 0;

  /* mount flags - setting SYNCHRONOUS might make sense here? @@@ */
  buf->f_flags = MNT_SYNCHRONOUS;
  if (info->id_DiskState == ID_WRITE_PROTECTED)
    buf->f_flags |= MNT_RDONLY;
  buf->f_oflags = (uint16_t)buf->f_flags & 0xffff;

  /* get number of block, free block and block size */
  buf->f_bsize = info->id_BytesPerBlock;
  buf->f_blocks = info->id_NumBlocks;
  buf->f_bfree = info->id_NumBlocks - info->id_NumBlocksUsed;
  buf->f_bavail = buf->f_bfree;

  /* can't count number of file nodes, copy number of alloc/free blocks */
  buf->f_files = buf->f_blocks;
  buf->f_ffree = buf->f_bfree;

  errno = 0;
  return 0;
}
