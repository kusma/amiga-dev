/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: int_types.h,v 1.2 2005/02/27 13:50:45 phx Exp $
 */

#ifndef _MACHINE_INTTYPES_H_
#define _MACHINE_INTTYPES_H_

typedef signed char              __int8_t;
typedef unsigned char           __uint8_t;
typedef short int               __int16_t;
typedef unsigned short int     __uint16_t;
typedef int                     __int32_t;
typedef unsigned int           __uint32_t;
#if (__STDC__ == 1L) && (__STDC_VERSION__ >= 199901L)
typedef long long int           __int64_t;
typedef unsigned long long int __uint64_t;
#endif

#define __BIT_TYPES_DEFINED__

/* 7.18.1.4 Integer types capable of holding object pointers */
typedef int                    __intptr_t;
typedef unsigned int          __uintptr_t;

#endif  /* _MACHINE_INTTYPES_H_ */
