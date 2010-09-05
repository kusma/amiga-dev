#ifndef PRAGMAS_SPEEDBAR_PRAGMAS_H
#define PRAGMAS_SPEEDBAR_PRAGMAS_H

/*
**	$VER: speedbar_pragmas.h 1.1 (6.10.1999)
**
**	Direct ROM interface (pragma) definitions.
**
**	Copyright © 2001 Amiga, Inc.
**	    All Rights Reserved
*/

#if defined(LATTICE) || defined(__SASC) || defined(_DCC)
#ifndef __CLIB_PRAGMA_LIBCALL
#define __CLIB_PRAGMA_LIBCALL
#endif /* __CLIB_PRAGMA_LIBCALL */
#else /* __MAXON__, __STORM__ or AZTEC_C */
#ifndef __CLIB_PRAGMA_AMICALL
#define __CLIB_PRAGMA_AMICALL
#endif /* __CLIB_PRAGMA_AMICALL */
#endif /* */

#if defined(__SASC) || defined(__STORM__)
#ifndef __CLIB_PRAGMA_TAGCALL
#define __CLIB_PRAGMA_TAGCALL
#endif /* __CLIB_PRAGMA_TAGCALL */
#endif /* __MAXON__, __STORM__ or AZTEC_C */

#ifndef CLIB_SPEEDBAR_PROTOS_H
#include <clib/speedbar_protos.h>
#endif /* CLIB_SPEEDBAR_PROTOS_H */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SpeedBarBase SPEEDBAR_GetClass 1e 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SpeedBarBase AllocSpeedButtonNodeA 24 8002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall SpeedBarBase AllocSpeedButtonNode 24 8002
 #endif /* __CLIB_PRAGMA_LIBCALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SpeedBarBase FreeSpeedButtonNode 2a 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SpeedBarBase SetSpeedButtonNodeAttrsA 30 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall SpeedBarBase SetSpeedButtonNodeAttrs 30 9802
 #endif /* __CLIB_PRAGMA_LIBCALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall SpeedBarBase GetSpeedButtonNodeAttrsA 36 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall SpeedBarBase GetSpeedButtonNodeAttrs 36 9802
 #endif /* __CLIB_PRAGMA_LIBCALL */
#endif /* __CLIB_PRAGMA_TAGCALL */

#endif /* PRAGMAS_SPEEDBAR_PRAGMAS_H */
