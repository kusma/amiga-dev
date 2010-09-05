#ifndef PRAGMAS_KEYMAP_PRAGMAS_H
#define PRAGMAS_KEYMAP_PRAGMAS_H

/*
**	$VER: keymap_pragmas.h 40.1 (17.5.1996)
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

#ifndef CLIB_KEYMAP_PROTOS_H
#include <clib/keymap_protos.h>
#endif /* CLIB_KEYMAP_PROTOS_H */

/*--- functions in V36 or higher (Release 2.0) ---*/
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall KeymapBase SetKeyMapDefault 1e 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall KeymapBase AskKeyMapDefault 24 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall KeymapBase MapRawKey 2a A19804
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall KeymapBase MapANSI 30 A190805
#endif /* __CLIB_PRAGMA_LIBCALL */

#endif /* PRAGMAS_KEYMAP_PRAGMAS_H */
