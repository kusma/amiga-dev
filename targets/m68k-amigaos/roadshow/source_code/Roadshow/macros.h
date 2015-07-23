/*
 *	$Id: macros.h,v 1.6 2007-08-26 12:30:26 obarthel Exp $
 *
 *	:ts=4
 *
 *	Copyright © 2001-2007 by Olaf Barthel. All Rights Reserved.
 */

#ifndef _MACROS_H
#define _MACROS_H

/****************************************************************************/

#define OK (0)
#define SAME (0)
#define BUSY ((struct IORequest *)NULL)
#define CANNOT !
#define NOT !
#define NO !

/****************************************************************************/

#ifdef NULL
#undef NULL
#define NULL ((VOID *)0UL)
#endif /* NULL */

/****************************************************************************/

#ifndef ZERO
#define ZERO ((BPTR)NULL)
#endif /* ZERO */

/****************************************************************************/

#define UNIX_TIME_OFFSET 252460800UL

/****************************************************************************/

#if !defined(_COMPILER_H) && !defined(AMIGA_COMPILER_H)
#include "compiler.h"
#endif /* !_COMPILER_H && !AMIGA_COMPILER_H */

/****************************************************************************/

#define FLAG_IS_SET(v,f)	(((v) & (f)) == (f))
#define FLAG_IS_CLEAR(v,f)	(((v) & (f)) ==  0 )
#define SET_FLAG(v,f)		((VOID)((v) |= (f)))
#define CLEAR_FLAG(v,f)		((VOID)((v) &= ~((ULONG)(f))))

/****************************************************************************/

#define NUM_ENTRIES(t) (sizeof(t) / sizeof(t[0]))

/****************************************************************************/

#define MILLION 1000000

/****************************************************************************/

#endif /* _MACROS_H */
