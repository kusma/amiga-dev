/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2006
 *
 * $Id: scandir.c,v 1.1 2006/03/12 15:12:21 phx Exp $
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#define DIRENTTHRESH 64

#undef DIRSIZ
#define DIRSIZ(dp) \
        ((sizeof(struct dirent) - sizeof(dp)->d_name) + \
        (((dp)->d_namlen + 1 + 3) &~ 3))


int scandir(const char *dirname,struct dirent ***namelist,
            int (*select)(const struct dirent *),
            int (*compar)(const void *,const void *))
{
  DIR *dir;
  struct dirent **direntarray = NULL;
  struct dirent *dent,*new,**newarray;
  int arraysize = 0;
  int namecnt = 0;

  if (dirname==NULL || namelist==NULL) {
    errno = EFAULT;
    return -1;
  }

  if (!(dir = opendir(dirname)))
    return -1;

  while (dent = readdir(dir)) {
    if (select) {
      if (!(*select)(dent))
        continue;  /* filtered out */
    }

    if (namecnt >= arraysize) {  /* realloc when names-array gets too small */
      arraysize += DIRENTTHRESH;
      newarray = realloc(direntarray,arraysize*sizeof(struct dirent *));
      if (newarray == NULL)
        goto outofmem;
      direntarray = newarray;
    }

    /* new entry */
    if (new = (struct dirent *)malloc(DIRSIZ(dent))) {
      new->d_fileno = dent->d_fileno;
      new->d_reclen = dent->d_reclen;
      new->d_type = dent->d_type;
      new->d_namlen = dent->d_namlen;
      memmove(new->d_name,dent->d_name,dent->d_namlen+1);
      direntarray[namecnt++] = new;
    }
    else
      goto outofmem;
  }
  closedir(dir);

  newarray = realloc(direntarray,namecnt*sizeof(struct dirent *));
  if (newarray == NULL) {
outofmem:
    if (direntarray) {
      while (namecnt-- > 0)
        free(direntarray[namecnt]);
      free(direntarray);
    }
    closedir(dir);
    errno = ENOMEM;
    return -1;
  }

  if (namecnt>0 && compar!=NULL)
    qsort(newarray,namecnt,sizeof(struct dirent *),compar);
  *namelist = newarray;
  return namecnt;
}


int alphasort(const void *d1,const void *d2)
{
  return strcmp((*(struct dirent **)d1)->d_name,
                (*(struct dirent **)d2)->d_name);
}
