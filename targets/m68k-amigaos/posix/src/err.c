/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: err.c,v 1.1 2005/04/11 19:25:24 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define NO_EXIT (-256)


static void showerr(int status,int xflag,const char *fmt,va_list vl)
{
  int sverrno = errno;

  fprintf(stderr,"%s: ",FilePart((char *)getprogname()));

  if (fmt) {
    vfprintf(stderr,fmt,vl);
    if (!xflag)
      fprintf(stderr,": ");
  }

  if (xflag)
    fprintf(stderr,"\n");
  else
    fprintf(stderr,"%s\n",strerror(sverrno));

  if (status != NO_EXIT)
    exit(status);
}


void err(int status,const char *fmt,...)
{
  va_list vl;

  va_start(vl,fmt);
  showerr(status,0,fmt,vl);
  va_end(vl);
}


void verr(int status,const char *fmt,va_list vl)
{
  showerr(status,0,fmt,vl);
}


void errx(int status,const char *fmt,...)
{
  va_list vl;

  va_start(vl,fmt);
  showerr(status,1,fmt,vl);
  va_end(vl);
}


void verrx(int status,const char *fmt,va_list vl)
{
  showerr(status,1,fmt,vl);
}


void warn(const char *fmt,...)
{
  va_list vl;

  va_start(vl,fmt);
  showerr(NO_EXIT,0,fmt,vl);
  va_end(vl);
}


void vwarn(const char *fmt,va_list vl)
{
  showerr(NO_EXIT,0,fmt,vl);
}


void warnx(const char *fmt,...)
{
  va_list vl;

  va_start(vl,fmt);
  showerr(NO_EXIT,1,fmt,vl);
  va_end(vl);
}


void vwarnx(const char *fmt,va_list vl)
{
  showerr(NO_EXIT,1,fmt,vl);
}
