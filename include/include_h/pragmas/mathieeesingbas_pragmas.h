#ifndef PRAGMAS_MATHIEEESINGBAS_PRAGMAS_H
#define PRAGMAS_MATHIEEESINGBAS_PRAGMAS_H

/*
**	$VER: mathieeesingbas_pragmas.h 40.1 (17.5.1996)
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

#ifndef CLIB_MATHIEEESINGBAS_PROTOS_H
#include <clib/mathieeesingbas_protos.h>
#endif /* CLIB_MATHIEEESINGBAS_PROTOS_H */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingBasBase IEEESPFix 1e 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingBasBase IEEESPFlt 24 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingBasBase IEEESPCmp 2a 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingBasBase IEEESPTst 30 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingBasBase IEEESPAbs 36 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingBasBase IEEESPNeg 3c 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingBasBase IEEESPAdd 42 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingBasBase IEEESPSub 48 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingBasBase IEEESPMul 4e 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingBasBase IEEESPDiv 54 1002
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingBasBase IEEESPFloor 5a 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingBasBase IEEESPCeil 60 001
#endif /* __CLIB_PRAGMA_LIBCALL */

#endif /* PRAGMAS_MATHIEEESINGBAS_PRAGMAS_H */
