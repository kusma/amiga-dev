#ifndef PROTO_DOS_H
#define PROTO_DOS_H

/*
**	$VER: dos.h 44.1 (1.11.1999)
**	Includes Release 45.1
**
**	Lattice `C' style prototype/pragma header file combo
**
**	(C) Copyright 2001 Amiga, Inc.
**	    All Rights Reserved
*/

#ifndef PRAGMAS_DOS_PRAGMAS_H
#include <pragmas/dos_pragmas.h>
#endif

#ifndef DOS_DOSEXTENS_H
#include <dos/dosextens.h>
#endif

#ifndef __NOLIBBASE__
extern struct DosLibrary * DOSBase;
#endif /* __NOLIBBASE__ */

#endif /* PROTO_DOS_H */
