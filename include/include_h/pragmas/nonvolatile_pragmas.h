#ifndef PRAGMAS_NONVOLATILE_PRAGMAS_H
#define PRAGMAS_NONVOLATILE_PRAGMAS_H

/*
**	$VER: nonvolatile_pragmas.h 40.1 (17.5.1996)
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

#ifndef CLIB_NONVOLATILE_PROTOS_H
#include <clib/nonvolatile_protos.h>
#endif /* CLIB_NONVOLATILE_PROTOS_H */

/*--- functions in V40 or higher (Release 3.1) ---*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall NVBase GetCopyNV 1e 19803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall NVBase FreeNVData 24 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall NVBase StoreNV 2a 10A9805
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall NVBase DeleteNV 30 19803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall NVBase GetNVInfo 36 101
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall NVBase GetNVList 3c 1802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall NVBase SetNVProtection 42 129804
#endif /* __CLIB_PRAGMA_LIBCALL */

#endif /* PRAGMAS_NONVOLATILE_PRAGMAS_H */
