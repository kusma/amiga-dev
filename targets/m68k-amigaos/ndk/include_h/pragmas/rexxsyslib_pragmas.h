#ifndef PRAGMAS_REXXSYSLIB_PRAGMAS_H
#define PRAGMAS_REXXSYSLIB_PRAGMAS_H

/*
**	$VER: rexxsyslib_pragmas.h 40.1 (17.5.1996)
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

#ifndef CLIB_REXXSYSLIB_PROTOS_H
#include <clib/rexxsyslib_protos.h>
#endif /* CLIB_REXXSYSLIB_PROTOS_H */

/*--- functions in V33 or higher (Release 1.2) ---*/
/*--- (16 function slots reserved here) ---*/

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall RexxSysBase CreateArgstring 7e 0802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall RexxSysBase DeleteArgstring 84 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall RexxSysBase LengthArgstring 8a 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall RexxSysBase CreateRexxMsg 90 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall RexxSysBase DeleteRexxMsg 96 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall RexxSysBase ClearRexxMsg 9c 0802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall RexxSysBase FillRexxMsg a2 10803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall RexxSysBase IsRexxMsg a8 801
#endif /* __CLIB_PRAGMA_LIBCALL */

/*--- (46 function slots reserved here) ---*/

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall RexxSysBase LockRexxBase 1c2 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall RexxSysBase UnlockRexxBase 1c8 001
#endif /* __CLIB_PRAGMA_LIBCALL */


#endif /* PRAGMAS_REXXSYSLIB_PRAGMAS_H */
