/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2006
 *
 * $Id: limits.h,v 1.1 2006/01/17 19:42:37 phx Exp $
 */

#ifndef _MACHINE_LIMITS_H_
#define _MACHINE_LIMITS_H_


#define CHAR_BIT        8
#define MB_LEN_MAX      32

#define SCHAR_MAX       0x7f
#define SCHAR_MIN       (-0x7f-1)

#define UCHAR_MAX       0xffU
#define CHAR_MAX        0x7f
#define CHAR_MIN        (-0x7f-1)

#define USHRT_MAX       0xffffU
#define SHRT_MAX        0x7fff
#define SHRT_MIN        (-0x7fff-1)

#define UINT_MAX        0xffffffffU
#define INT_MAX         0x7fffffff
#define INT_MIN         (-0x7fffffff-1)

#define ULONG_MAX       0xffffffffUL
#define LONG_MAX        0x7fffffffL
#define LONG_MIN        (-0x7fffffffL-1)

#if __STDC_VERSION__ == 199901
#define ULLONG_MAX      0xffffffffffffffffULL
#define LLONG_MAX       0x7fffffffffffffffLL
#define LLONG_MIN       (-0x7fffffffffffffffLL-1)
#endif


#endif /* _MACHINE_LIMITS_H_ */
