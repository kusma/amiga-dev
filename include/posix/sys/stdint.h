/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: stdint.h,v 1.4 2006/01/17 20:49:56 phx Exp $
 */

#ifndef _SYS_STDINT_H_
#define _SYS_STDINT_H_

#include <machine/int_types.h>

#ifndef int8_t
typedef __int8_t        int8_t;
#define int8_t          __int8_t
#endif

#ifndef uint8_t
typedef __uint8_t       uint8_t;
#define uint8_t         __uint8_t
#endif

#ifndef int16_t
typedef __int16_t       int16_t;
#define int16_t         __int16_t
#endif

#ifndef uint16_t
typedef __uint16_t      uint16_t;
#define uint16_t        __uint16_t
#endif

#ifndef int32_t
typedef __int32_t       int32_t;
#define int32_t         __int32_t
#endif

#ifndef uint32_t
typedef __uint32_t      uint32_t;
#define uint32_t        __uint32_t
#endif

#if (__STDC__ == 1L) && (__STDC_VERSION__ >= 199901L)
#ifndef int64_t
typedef __int64_t       int64_t;
#define int64_t         __int64_t
#endif

#ifndef uint64_t
typedef __uint64_t      uint64_t;
#define uint64_t        __uint64_t
#endif
#endif

#ifndef intptr_t
typedef __intptr_t      intptr_t;
#define intptr_t        __intptr_t
#endif

#ifndef uintptr_t
typedef __uintptr_t     uintptr_t;
#define uintptr_t       __uintptr_t
#endif

#if (__STDC__ == 1L) && (__STDC_VERSION__ >= 199901L)
#include <machine/int_mwgwtypes.h>
#endif

#if !defined(__cplusplus) || defined(__STDC_LIMIT_MACROS)
#include <machine/int_limits.h>
#endif

#if !defined(__cplusplus) || defined(__STDC_CONSTANT_MACROS)
#include <machine/int_const.h>
#endif

#endif  /* _SYS_STDINT_H_ */
