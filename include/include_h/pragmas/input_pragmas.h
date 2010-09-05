#ifndef PRAGMAS_INPUT_PRAGMAS_H
#define PRAGMAS_INPUT_PRAGMAS_H

/*
**	$VER: input_pragmas.h 40.1 (17.5.1996)
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

#ifndef CLIB_INPUT_PROTOS_H
#include <clib/input_protos.h>
#endif /* CLIB_INPUT_PROTOS_H */

/*--- functions in V36 or higher (Release 2.0) ---*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall InputBase PeekQualifier 2a 00
#endif /* __CLIB_PRAGMA_LIBCALL */

#endif /* PRAGMAS_INPUT_PRAGMAS_H */
