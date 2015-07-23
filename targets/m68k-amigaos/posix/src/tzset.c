/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: tzset.c,v 1.1 2005/03/31 18:01:32 phx Exp $
 */

#include <time.h>
#include <stdlib.h>

extern long __gmtoffset;
extern int __dstflag;


static int readtzfile(char *name)
{
  return 0;   /* to be implemented */
}


static char *skiptzname(char *p)
{
  char c;

  do {
    c = *p++;
  }
  while (!((c>='0' && c<='9') || c==',' || c=='+' || c=='-' || c==0));

  return (--p);
}


static char *getoffset(long *optr,char *p)
{
  *optr = atol(p);
  if (*p=='+' || *p=='-') p++;
  while (*p>='0' && *p<='9') p++;

  /* @@@ ignore minutes and seconds for now */
  if (*p == ':') {
    p++;
    while (*p>='0' && *p<='9') p++;
    if (*p == ':') {
      p++;
      while (*p>='0' && *p<='9') p++;
    }
  }
  return p;
}


void tzset(void)
{
  char *tz;
  long stdoffs=0,dstoffs=0;
  int dstm=-1,dstd,dsth=0,stdm=-1,stdd,stdh=0;
  time_t t;
  struct tm *tm;

  __gmtoffset = 0;
  if (tz = getenv("TZ")) {
    if (*tz == ':') {
      readtzfile(tz+1);
      return;
    }
    if (readtzfile(tz))
      return;

    tz = skiptzname(tz);
    if (*tz) {
      if (*tz == ',')
        return;
      tz = getoffset(&stdoffs,tz);
      if (*tz) {
        if (*tz != ',') {
          tz = skiptzname(tz);
          if (*tz)
            tz = getoffset(&dstoffs,tz);
        }
        if (*tz == ',') {
          /* @@@ rule is ignored at the moment */
        }
      }
    }

    if (dstm < 0) {
      /* @@@ dst defaults to 1st of April */
      dstm = 3;
      dstd = 1;
    }
    if (stdm < 0) {
      /* @@@ std time defaults to 1st of November */
      stdm = 10;
      stdd = 1;
    }

    time(&t);
    tm = gmtime(&t);
    if (!dstoffs) {
      /*dstoffs = stdoffs - 1;*/
      dstoffs = stdoffs;  /*@@@ dst time is manually adjusted? */
    }
    if (stdm > dstm)
      __gmtoffset = (tm->tm_mon>dstm&&tm->tm_mon<=stdm) ? dstoffs : stdoffs;
    else
      __gmtoffset = (tm->tm_mon>stdm&&tm->tm_mon<=dstm) ? stdoffs : dstoffs;

    tm = localtime(&t);
    if (__gmtoffset == dstoffs) {
      if (tm->tm_mon == stdm) {
        if (tm->tm_mday>stdd ||
            (tm->tm_mday==stdd && tm->tm_hour>=stdh))
          __gmtoffset = stdoffs;
      }
    }
    else {
      if (tm->tm_mon == dstm) {
        if (tm->tm_mday>dstd ||
            (tm->tm_mday==dstd && tm->tm_hour>=dsth))
          __gmtoffset = dstoffs;
      }
    }

    __dstflag = __gmtoffset == dstoffs;
  }
}
