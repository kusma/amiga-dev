#ifndef PROTO_REXXSYSLIB_H
#define PROTO_REXXSYSLIB_H

/*
**	$VER: rexxsyslib.h 44.1 (1.11.1999)
**	Includes Release 45.1
**
**	Lattice `C' style prototype/pragma header file combo
**
**	(C) Copyright 2001 Amiga, Inc.
**	    All Rights Reserved
*/

#ifndef PRAGMAS_REXXSYSLIB_PRAGMAS_H
#include <pragmas/rexxsyslib_pragmas.h>
#endif

#ifndef REXX_RXSLIB_H
#include <rexx/rxslib.h>
#endif

#ifndef __NOLIBBASE__
extern struct RxsLib * RexxSysBase;
#endif /* __NOLIBBASE__ */

#endif /* PROTO_REXXSYSLIB_H */
