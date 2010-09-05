#ifndef PRAGMAS_LAYOUT_PRAGMAS_H
#define PRAGMAS_LAYOUT_PRAGMAS_H

/*
**	$VER: layout_pragmas.h 1.1 (6.10.1999)
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

#ifndef CLIB_LAYOUT_PROTOS_H
#include <clib/layout_protos.h>
#endif /* CLIB_LAYOUT_PROTOS_H */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall LayoutBase LAYOUT_GetClass 1e 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall LayoutBase ActivateLayoutGadget 24 0A9804
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall LayoutBase FlushLayoutDomainCache 2a 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall LayoutBase RethinkLayout 30 0A9804
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall LayoutBase LayoutLimits 36 BA9804
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall LayoutBase PAGE_GetClass 3c 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall LayoutBase SetPageGadgetAttrsA 42 CBA9805
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall LayoutBase SetPageGadgetAttrs 42 CBA9805
 #endif /* __CLIB_PRAGMA_LIBCALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall LayoutBase RefreshPageGadget 48 BA9804
#endif /* __CLIB_PRAGMA_LIBCALL */

#endif /* PRAGMAS_LAYOUT_PRAGMAS_H */
