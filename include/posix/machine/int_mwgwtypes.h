/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003-2005
 *
 * $Id: int_mwgwtypes.h,v 1.1 2005/03/25 10:38:39 phx Exp $
 */

#ifndef _MACHINE_INT_MWGWTYPES_H_
#define _MACHINE_INT_MWGWTYPES_H_

/*
 * 7.18.1 Integer types
 */

/* 7.18.1.2 Minimum-width integer types */

typedef signed char               int_least8_t;
typedef unsigned char            uint_least8_t;
typedef short int                int_least16_t;
typedef unsigned short int      uint_least16_t;
typedef int                      int_least32_t;
typedef unsigned int            uint_least32_t;
typedef long long                int_least64_t;
typedef unsigned long long      uint_least64_t;

/* 7.18.1.3 Fastest minimum-width integer types */

typedef int                        int_fast8_t;
typedef unsigned int              uint_fast8_t;
typedef int                       int_fast16_t;
typedef unsigned int             uint_fast16_t;
typedef int                       int_fast32_t;
typedef unsigned int             uint_fast32_t;
typedef long long                 int_fast64_t;
typedef unsigned long long       uint_fast64_t;

/* 7.18.1.5 Greatest-width integer types */

typedef long long                     intmax_t;
typedef unsigned long long           uintmax_t;

#endif /* _MACHINE_INT_MWGWTYPES_H_ */
