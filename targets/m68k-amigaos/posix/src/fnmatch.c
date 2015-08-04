/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: fnmatch.c,v 1.2 2005/02/27 13:45:24 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fnmatch.h>
#include "conv.h"


int fnmatch(const char *pattern,const char *string,int flags)
{
  char *adospat,*tokpat;
  LONG len;
  int rc = FNM_NOMATCH;

  errno = ENOMEM;
  if (adospat = __make_ados_pattern(pattern,(flags & FNM_NOESCAPE)==0)) {
    /* tokenize pattern and match against string */
    len = 2*strlen(adospat)+3;
    if (tokpat = malloc(len)) {
      if (ParsePattern((STRPTR)adospat,(STRPTR)tokpat,len) >= 0) {
        errno = 0;
        if (MatchPattern((STRPTR)tokpat,(STRPTR)__convert_path(string))) {
          rc = 0;
        }
      }
      free(tokpat);
    }
    free(adospat);
  }
  return rc;
}
