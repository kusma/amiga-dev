/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: convpatt.c,v 1.3 2005/07/17 15:13:37 phx Exp $
 */

#include <stdlib.h>
#include <string.h>


char *__make_ados_pattern(const char *upat,int esc)
{
  char *apat,*p,c;

  if (upat == NULL)
    return NULL;

  if (p = apat = malloc(2*strlen(upat)+1)) {
    if (*upat == '.') {
      if (*(upat+1) == '/')
        upat += 2;
      else if (*(upat+1) == '\0')
        upat++;
    }

    while (c = *upat++) {
      if (c=='.' && *upat=='.' && *(upat+1)=='/') {
        upat++;
        continue;
      }
      else if (c=='/' && *upat=='\0') {
        continue;
      }
      else if (c==0x5c && esc) {
        if (*p++ = *upat++)
          continue;
        else
          break;
      }
      else if (c == '*') {
        *p++ = '#';
        *p++ = '?';
        continue;
      }

      *p++ = c;
    }
    *p++ = '\0';
  }

  return apat;
}
