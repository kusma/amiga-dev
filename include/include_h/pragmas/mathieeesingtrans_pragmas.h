#ifndef PRAGMAS_MATHIEEESINGTRANS_PRAGMAS_H
#define PRAGMAS_MATHIEEESINGTRANS_PRAGMAS_H

/*
**	$VER: mathieeesingtrans_pragmas.h 40.1 (17.5.1996)
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

#ifndef CLIB_MATHIEEESINGTRANS_PROTOS_H
#include <clib/mathieeesingtrans_protos.h>
#endif /* CLIB_MATHIEEESINGTRANS_PROTOS_H */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingTransBase IEEESPAtan 1e 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingTransBase IEEESPSin 24 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingTransBase IEEESPCos 2a 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingTransBase IEEESPTan 30 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingTransBase IEEESPSincos 36 0802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingTransBase IEEESPSinh 3c 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingTransBase IEEESPCosh 42 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingTransBase IEEESPTanh 48 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingTransBase IEEESPExp 4e 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingTransBase IEEESPLog 54 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingTransBase IEEESPPow 5a 0102
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingTransBase IEEESPSqrt 60 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingTransBase IEEESPTieee 66 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingTransBase IEEESPFieee 6c 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingTransBase IEEESPAsin 72 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingTransBase IEEESPAcos 78 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall MathIeeeSingTransBase IEEESPLog10 7e 001
#endif /* __CLIB_PRAGMA_LIBCALL */

#endif /* PRAGMAS_MATHIEEESINGTRANS_PRAGMAS_H */
