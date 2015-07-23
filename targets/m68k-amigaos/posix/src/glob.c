/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2005
 *
 * $Id: glob.c,v 1.2 2005/02/27 13:21:26 phx Exp $
 */

#pragma amiga-align
#include <exec/lists.h>
#ifdef __amigaos4__
#include <dos/anchorpath.h>
#else
#include <dos/dosasl.h>
#endif
#include <dos/dosextens.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <clib/alib_protos.h>
#pragma default-align
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glob.h>
#include <sys/param.h>
#include "conv.h"

struct matchnode {
  struct MinNode n;
  char *name;
};



static void freeminlist(struct MinList *ml)
{
  struct matchnode *mn = (struct matchnode *)ml->mlh_Head;
  struct matchnode *nextmn;

  while (nextmn = (struct matchnode *)mn->n.mln_Succ) {
    if (mn->name)
      free(mn->name);
    free(mn);
    mn = nextmn;
  }
}


static int extendpathv(glob_t *pglob,int new)
{
  char **pv,**oldpv;

  if (new) {
    if (pv = calloc(pglob->gl_offs+pglob->gl_pathc+new+1,sizeof(char *))) {
      if (oldpv = pglob->gl_pathv) {
        int i;
        int last = pglob->gl_offs + pglob->gl_pathc;

        for (i=pglob->gl_offs; i<last; i++) {
          pv[i] = oldpv[i];
        }
      }
      pglob->gl_pathv = pv;
    }
    else
      return 0;
  }
  return 1;
}


int glob(const char *pattern,int flags,
         int (*errfunc)(const char *,int),glob_t *pglob)
{
  char *adospat;
  struct MinList ml;
  struct AnchorPath *ap;
  struct matchnode *newmn;
  ULONG ioerr;
  int err=0;

  NewList((struct List *)&ml);
  errno = 0;

  /* initialize glob-structure */
  pglob->gl_matchc = 0;
  pglob->gl_flags = flags & ~GLOB_MAGCHAR;
  if (!(flags & GLOB_APPEND)) {
    pglob->gl_pathc = 0;
    pglob->gl_pathv = NULL;
    if (!(flags & GLOB_DOOFFS)) {
      pglob->gl_offs = 0;
    }
  }

  /* convert Unix pattern into AmigaDos pattern */
  if (!(adospat = __make_ados_pattern(pattern,(flags & GLOB_NOESCAPE)==0))) {
    errno = ENOMEM;
    return GLOB_NOSPACE;
  }

  /* get an AnchorPath structure for the maximum path length */
#ifdef __amigaos4__
  if (!(ap = (struct AnchorPath *)AllocDosObjectTags(DOS_ANCHORPATH,
                                                     ADO_Strlen,MAXPATHLEN,
                                                     TAG_DONE))) {
#else
  if (!(ap = calloc(sizeof(struct AnchorPath)+MAXPATHLEN,1))) {
#endif
    free(adospat);
    errno = ENOMEM;
    return GLOB_NOSPACE;
  }
#ifndef __amigaos4__
  ap->ap_Strlen = MAXPATHLEN;
#endif

  /* fill matched paths into the temporary match-list */
  if (!MatchFirst((STRPTR)adospat,ap)) {
    if (ap->ap_Flags & APF_ITSWILD)
      pglob->gl_flags |= GLOB_MAGCHAR;

    do {
      if (newmn = malloc(sizeof(struct matchnode))) {
#ifdef __amigaos4__
        if (newmn->name = malloc(strlen(ap->ap_Buffer)+2)) {
          strcpy(newmn->name,ap->ap_Buffer);
#else
        if (newmn->name = malloc(strlen(ap->ap_Buf)+2)) {
          strcpy(newmn->name,ap->ap_Buf);
#endif
          if (ap->ap_Info.fib_DirEntryType>=0 &&
              ap->ap_Info.fib_DirEntryType!=ST_SOFTLINK &&
              (flags & GLOB_MARK))
            strcat(newmn->name,"/");  /* append / to matching directories */

          if (flags & GLOB_NOSORT) {
            /* just append new matchnode at the end of the list */
            AddTail((struct List *)&ml,(struct Node *)newmn);
          }
          else {
            /* enqueue new matchnode in ascending ASCII order */
            struct matchnode *mn;

            for (mn = (struct matchnode *)ml.mlh_Head;
                 mn->n.mln_Succ != NULL;
                 mn = (struct matchnode *)mn->n.mln_Succ) {
              if (strcmp(newmn->name,mn->name) < 0) {
                struct MinNode *pred = (struct MinNode *)mn->n.mln_Pred;

                if (pred->mln_Pred)
                  Insert((struct List *)&ml,(struct Node *)newmn,
                         (struct Node *)pred);
                else
                  AddHead((struct List *)&ml,(struct Node *)newmn);

                mn = NULL;  /* node inserted */
                break;
              }
            }
            if (mn)  /* insert as last element into the list */
              AddTail((struct List *)&ml,(struct Node *)newmn);
          }

          pglob->gl_matchc++;
        }
        else {
          free(newmn);
          newmn = NULL;
        }
      }

      if (!newmn) {
        MatchEnd(ap);
        freeminlist(&ml);
#ifdef __amigaos4__
        FreeDosObject(DOS_ANCHORPATH,ap);
#else
        free(ap);
#endif
        free(adospat);
        errno = ENOMEM;
        return GLOB_NOSPACE;
      }
    }
    while (!MatchNext(ap));
    ioerr = IoErr();

    MatchEnd(ap);
  }
  else {
    ioerr = IoErr();
  }

#ifdef __amigaos4__
  FreeDosObject(DOS_ANCHORPATH,ap);
#else
  free(ap);
#endif
  free(adospat);

  /* not a single object found? */
  if (pglob->gl_matchc == 0) {
    if (flags & GLOB_NOCHECK) {
      /* add only the pattern to the list */
      if (extendpathv(pglob,1)) {
        char *patcpy;

        if (patcpy = malloc(strlen(pattern))) {
          strcpy(patcpy,pattern);
          pglob->gl_pathv[pglob->gl_offs + pglob->gl_pathc++] = patcpy;
        }
        else {
          errno = ENOMEM;
          err = GLOB_NOSPACE;
        }
      }
      else {
        errno = ENOMEM;
        err = GLOB_NOSPACE;
      }
    }
    else {
      errno = ENOENT;
      err = GLOB_NOMATCH;
    }
  }

  else {
    /* at least one new object found */
    if (ioerr == ERROR_OBJECT_NOT_FOUND) {
      errno = ENOENT;
      err = GLOB_ABORTED;
    }
    else if (ioerr != ERROR_NO_MORE_ENTRIES) {
      errno = EIO;  /* @@@ unknown error */
      err = GLOB_ABORTED;
    }

    /* append new object names to list */
    if (extendpathv(pglob,pglob->gl_matchc)) {
      struct matchnode *mn;
      int i = pglob->gl_offs + pglob->gl_pathc;

      for (mn = (struct matchnode *)ml.mlh_Head;
           mn->n.mln_Succ != NULL;
           mn = (struct matchnode *)mn->n.mln_Succ) {
        pglob->gl_pathv[i++] = mn->name;
        mn->name = NULL;
      }
      pglob->gl_pathc += pglob->gl_matchc;
    }
    else {
      errno = ENOMEM;
      err = GLOB_NOSPACE;
    }
  }

  freeminlist(&ml);
  return err;
}


void globfree(glob_t *pglob)
{
  char **pv;
  int i;

  if (pv = pglob->gl_pathv) {
    for (i=pglob->gl_offs; i<pglob->gl_offs+pglob->gl_pathc; i++) {
      if (pv[i])
        free(pv[i]);
    }
    free(pv);
  }
}
