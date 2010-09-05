/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: err.h,v 1.1 2005/04/11 19:25:24 phx Exp $
 */

#ifndef _ERR_H_
#define _ERR_H_

#include <stdarg.h>

/* Prototypes */
void err(int,const char *,...);
void verr(int,const char *,va_list);
void errx(int,const char *,...);
void verrx(const char *,va_list);
void warn(const char *,...);
void vwarn(const char *,va_list);
void warnx(const char *,...);
void vwarnx(const char *,va_list);

#endif /* _ERR_H_ */
