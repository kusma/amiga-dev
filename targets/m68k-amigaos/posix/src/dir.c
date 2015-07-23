/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: dir.c,v 1.2 2005/02/27 13:45:22 phx Exp $
 */

#pragma amiga-align
#include <exec/lists.h>
#include <dos/dosextens.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <clib/alib_protos.h>
#pragma default-align
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/dirent.h>
#include "conv.h"


typedef struct _dirdesc {
  struct MinNode node;
  struct FileInfoBlock fib;
  struct dirent dent;
  BPTR lock;
} DIR;

static struct MinList dirlist;



void _INIT_5_dirlist(void)
{
  NewList((struct List *)&dirlist);
}


void _EXIT_5_dirlist(void)
{
  DIR *dnode = (DIR *)dirlist.mlh_Head;
  DIR *nextd;

  while (nextd = (DIR *)dnode->node.mln_Succ) {
    UnLock(dnode->lock);
    free(dnode);
    dnode = nextd;
  }
}


DIR *opendir(const char *filename)
{
  DIR *d;

  if (d = malloc(sizeof(DIR))) {
    if (d->lock = Lock((STRPTR)__convert_path(filename),ACCESS_READ)) {
      if (Examine(d->lock,&(d->fib))) {
        if (d->fib.fib_DirEntryType>=0 &&
            d->fib.fib_DirEntryType!=ST_SOFTLINK) {
          AddTail((struct List *)&dirlist,(struct Node *)&d->node);
          errno = 0;
          return d;
        }
        else
          errno = ENOTDIR;
      }
      else
        errno = EIO;
      UnLock(d->lock);
    }
    else
      errno = ENOENT;
    free(d);
  }
  else
    errno = ENOMEM;

  return NULL;
}


int closedir(DIR *d)
{
  if (d) {
    Remove((struct Node *)&d->node);
    UnLock(d->lock);
    free(d);
  }
  errno = 0;
  return 0;
}


void rewinddir(DIR *d)
{
  if (d)
    Examine(d->lock,&(d->fib));
}


struct dirent *readdir(DIR *d)
{
  errno = 0;
  if (ExNext(d->lock,&(d->fib))) {
    int namlen = strlen(d->fib.fib_FileName);
    int type;

    switch (d->fib.fib_DirEntryType) {
      case ST_PIPEFILE:
        type = DT_FIFO; break;
      case ST_SOFTLINK:
        type = DT_LNK; break;
      case ST_ROOT:
      case ST_USERDIR:
      case ST_LINKDIR:
        type = DT_DIR; break;
      default: /* everything else is a regular file */
        type = DT_REG; break;
    }

    d->dent.d_fileno = (u_int32_t)d->fib.fib_DiskKey;  /* @@@ ? */
    d->dent.d_reclen = (u_int16_t)(8 + namlen + 3) & ~3;
    d->dent.d_type = (u_int8_t)type;
    d->dent.d_namlen = (u_int8_t)namlen;
    memset(&d->dent.d_name[namlen],0,256-namlen);
    strcpy(d->dent.d_name,d->fib.fib_FileName);
    return &d->dent;
  }

  if (IoErr() != ERROR_NO_MORE_ENTRIES)
    errno = EIO;  /* @@@ */

  return NULL;
}
