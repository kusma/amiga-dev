/*
 *	$Id: macros.h,v 1.3 2005/01/07 13:32:03 obarthel Exp $
 *
 *	:ts=4
 *
 *	Copyright © 2001-2006 by Olaf Barthel. All Rights Reserved.
 */

#ifndef _MACROS_H
#define _MACROS_H

/****************************************************************************/

#ifdef NULL
#undef NULL
#endif /* NULL */

#define NULL ((VOID *)0UL)

/****************************************************************************/

#define OK (0)
#define SAME (0)
#define BUSY ((struct IORequest *)NULL)
#define CANNOT !
#define NOT !
#define NO !
#define NOTHING ((VOID)0)

/****************************************************************************/

#ifndef ZERO
#define ZERO ((BPTR)NULL)
#endif /* ZERO */

/****************************************************************************/

#define UNIX_TIME_OFFSET 252460800

/****************************************************************************/

#ifndef _COMPILER_H
#include "compiler.h"
#endif /* _COMPILER_H */

/****************************************************************************/

#define PORT_SIG_MASK(mp) (1UL << (mp)->mp_SigBit)

/****************************************************************************/

#define FLAG_IS_SET(v,f)	(((v) & (f)) == (f))
#define FLAG_IS_CLEAR(v,f)	(((v) & (f)) ==  0 )
#define SET_FLAG(v,f)		((VOID)((v) |= (f)))
#define CLEAR_FLAG(v,f)		((VOID)((v) &= ~((ULONG)(f))))
#define BIT_IS_SET(v,b)		FLAG_IS_SET(v,(1UL << (b)))
#define BIT_IS_CLEAR(v,b)	FLAG_IS_CLEAR(v,(1UL << (b)))
#define IS_EVEN(v)			(((v) & 1) == 0)
#define IS_ODD(v)			(((v) & 1) != 0)

/****************************************************************************/

#define NUM_ENTRIES(t) (sizeof(t) / sizeof(t[0]))

/****************************************************************************/

#endif /* _MACROS_H */
