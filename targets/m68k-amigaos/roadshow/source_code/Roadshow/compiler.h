/*
 *	$Id: compiler.h,v 1.3 2007-08-26 12:30:26 obarthel Exp $
 *
 *	:ts=4
 *
 *	Copyright © 2001-2007 by Olaf Barthel. All Rights Reserved.
 */

#ifndef _COMPILER_H
#define _COMPILER_H

/****************************************************************************/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif /* EXEC_TYPES_H */

/****************************************************************************/

/* The following definitions are redundant in the V50 AmigaOS header files. */
#ifndef AMIGA_COMPILER_H

/****************************************************************************/

#if defined(__SASC)
#define ASM __asm
#define REG(r,p) register __##r p
#define INLINE __inline
#define INTERRUPT __interrupt
#define FAR __far
#define STDARGS __stdargs
#elif defined(__GNUC__)
#define ASM
#define REG(r,p) p __asm(#r)
#define INLINE __inline__
#define INTERRUPT __attribute__((__interrupt__))
#define FAR
#define STDARGS __attribute__((__stkparm__))
#else
#define ASM
#define REG(x)
#define INLINE
#define INTERRUPT
#define FAR
#define STDARGS
#endif /* __SASC */

#define VARARGS68K

/****************************************************************************/

#endif /* AMIGA_COMPILER_H */

/****************************************************************************/

#endif /* _COMPILER_H */
