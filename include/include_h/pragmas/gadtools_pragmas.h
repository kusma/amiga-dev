#ifndef PRAGMAS_GADTOOLS_PRAGMAS_H
#define PRAGMAS_GADTOOLS_PRAGMAS_H

/*
**	$VER: gadtools_pragmas.h 40.1 (17.5.1996)
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

#ifndef CLIB_GADTOOLS_PROTOS_H
#include <clib/gadtools_protos.h>
#endif /* CLIB_GADTOOLS_PROTOS_H */


/*--- functions in V36 or higher (Release 2.0) ---*/

/* Gadget Functions */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall GadToolsBase CreateGadgetA 1e A98004
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall GadToolsBase CreateGadget 1e A98004
 #endif /* __CLIB_PRAGMA_LIBCALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall GadToolsBase FreeGadgets 24 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall GadToolsBase GT_SetGadgetAttrsA 2a BA9804
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall GadToolsBase GT_SetGadgetAttrs 2a BA9804
 #endif /* __CLIB_PRAGMA_LIBCALL */
#endif /* __CLIB_PRAGMA_TAGCALL */

/* Menu functions */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall GadToolsBase CreateMenusA 30 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall GadToolsBase CreateMenus 30 9802
 #endif /* __CLIB_PRAGMA_LIBCALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall GadToolsBase FreeMenus 36 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall GadToolsBase LayoutMenuItemsA 3c A9803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall GadToolsBase LayoutMenuItems 3c A9803
 #endif /* __CLIB_PRAGMA_LIBCALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall GadToolsBase LayoutMenusA 42 A9803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall GadToolsBase LayoutMenus 42 A9803
 #endif /* __CLIB_PRAGMA_LIBCALL */
#endif /* __CLIB_PRAGMA_TAGCALL */

/* Misc Event-Handling Functions */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall GadToolsBase GT_GetIMsg 48 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall GadToolsBase GT_ReplyIMsg 4e 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall GadToolsBase GT_RefreshWindow 54 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall GadToolsBase GT_BeginRefresh 5a 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall GadToolsBase GT_EndRefresh 60 0802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall GadToolsBase GT_FilterIMsg 66 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall GadToolsBase GT_PostFilterIMsg 6c 901
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall GadToolsBase CreateContext 72 801
#endif /* __CLIB_PRAGMA_LIBCALL */

/* Rendering Functions */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall GadToolsBase DrawBevelBoxA 78 93210806
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall GadToolsBase DrawBevelBox 78 93210806
 #endif /* __CLIB_PRAGMA_LIBCALL */
#endif /* __CLIB_PRAGMA_TAGCALL */

/* Visuals Functions */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall GadToolsBase GetVisualInfoA 7e 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall GadToolsBase GetVisualInfo 7e 9802
 #endif /* __CLIB_PRAGMA_LIBCALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall GadToolsBase FreeVisualInfo 84 801
#endif /* __CLIB_PRAGMA_LIBCALL */

/*--- functions in V39 or higher (Release 3) ---*/

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall GadToolsBase GT_GetGadgetAttrsA ae BA9804
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall GadToolsBase GT_GetGadgetAttrs ae BA9804
 #endif /* __CLIB_PRAGMA_LIBCALL */
#endif /* __CLIB_PRAGMA_TAGCALL */


#endif /* PRAGMAS_GADTOOLS_PRAGMAS_H */
