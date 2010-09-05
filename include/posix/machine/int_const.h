/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: int_const.h,v 1.1 2005/03/25 10:38:39 phx Exp $
 */

#ifndef _MACHINE_INT_CONST_H_
#define _MACHINE_INT_CONST_H_

/*
 * 7.18.4 Macros for integer constants
 */

/* 7.18.4.1 Macros for minimum-width integer constants */

#define INT8_C(c)       c
#define INT16_C(c)      c
#define INT32_C(c)      c
#define INT64_C(c)      c ## LL

#define UINT8_C(c)      c ## U
#define UINT16_C(c)     c ## U
#define UINT32_C(c)     c ## U
#define UINT64_C(c)     c ## ULL

/* 7.18.4.2 Macros for greatest-width integer constants */

#define INTMAX_C(c)     c ## LL
#define UINTMAX_C(c)    c ## ULL

#endif /* _MACHINE_INT_CONST_H_ */
