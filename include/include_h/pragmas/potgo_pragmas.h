#ifndef PRAGMAS_POTGO_PRAGMAS_H
#define PRAGMAS_POTGO_PRAGMAS_H

/*
**	$VER: potgo_pragmas.h 40.1 (17.5.1996)
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

#ifndef CLIB_POTGO_PROTOS_H
#include <clib/potgo_protos.h>
#endif /* CLIB_POTGO_PROTOS_H */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall PotgoBase AllocPotBits 6 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall PotgoBase FreePotBits c 001
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall PotgoBase WritePotgo 12 1002
#endif /* __CLIB_PRAGMA_LIBCALL */

#endif /* PRAGMAS_POTGO_PRAGMAS_H */
