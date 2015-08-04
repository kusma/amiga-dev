/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003,2006
 *
 * $Id: prot.c,v 1.4 2006/06/02 22:13:37 phx Exp $
 */

#pragma amiga-align
#include <dos/dos.h>
#include <proto/dos.h>
#pragma default-align
#include <sys/stat.h>

static mode_t __umask = S_IWGRP|S_IWOTH;  /* default umask */


mode_t __prot2mode(unsigned long p)
{
  mode_t m = 0;

  if (!(p & FIBF_READ))  m |= 0400;
  if (!(p & FIBF_WRITE))  m |= 0200;
  if (!(p & FIBF_EXECUTE))  m |= 0100;
  if (p & FIBF_GRP_READ)  m |= 0040;
  if (p & FIBF_GRP_WRITE)  m |= 0020;
  if (p & FIBF_GRP_EXECUTE)  m |= 0010;
  if (p & FIBF_OTR_READ)  m |= 0004;
  if (p & FIBF_OTR_WRITE)  m |= 0002;
  if (p & FIBF_OTR_EXECUTE)  m |= 0001;

  return m;
}


unsigned long __mode2prot(mode_t m)
{
  unsigned long p = FIBF_READ|FIBF_WRITE|FIBF_EXECUTE;

  if (m & 0400)  p &= ~FIBF_READ;
  if (m & 0200)  p &= ~FIBF_WRITE;
  if (m & 0100)  p &= ~FIBF_EXECUTE;
  if (m & 0040)  p |= FIBF_GRP_READ;
  if (m & 0020)  p |= FIBF_GRP_WRITE;
  if (m & 0010)  p |= FIBF_GRP_EXECUTE;
  if (m & 0004)  p |= FIBF_OTR_READ;
  if (m & 0002)  p |= FIBF_OTR_WRITE;
  if (m & 0001)  p |= FIBF_OTR_EXECUTE;

  return p;
}


int __set_prot(char *name,mode_t mode)
{
  return SetProtection((STRPTR)name,__mode2prot(mode)) ? 0 : -1;
}


int __set_masked_prot(char *name,mode_t mode)
{
  return __set_prot(name,mode & ~__umask);
}


mode_t __get_masked_prot(mode_t mode)
{
  return (mode & ~__umask);
}


mode_t umask(mode_t new)
{
  mode_t old = __umask;

  __umask = new;
  return old;
}
