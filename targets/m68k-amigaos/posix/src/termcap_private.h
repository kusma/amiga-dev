/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2006
 *
 * $Id: termcap_private.h,v 1.1 2006/08/09 11:05:33 phx Exp $
 */

struct tinfo
{
  char *info;
  char *up;
  char *bc;
  struct tbuf {
    struct tbuf *next;
    char *data;
    char *ptr;
    char *eptr;
  } *tbuf;
};
