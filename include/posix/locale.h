/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2006
 *
 * $Id: locale.h,v 1.1 2006/01/17 20:44:56 phx Exp $
 */

#ifndef _LOCALE_H_
#define _LOCALE_H_

#include_next <locale.h>

#define LC_MESSAGES     6

#ifdef _LC_LAST
#undef _LC_LAST
#endif
#define _LC_LAST        7

#endif /* _LOCALE_ */
