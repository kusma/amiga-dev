#ifndef PROTO_VIRTUAL_H
#define PROTO_VIRTUAL_H

/*
**	$VER: virtual.h 45.1 (11.12.2001)
**	Includes Release 45.1
**
**	Lattice `C' style prototype/pragma header file combo
**
**	(C) Copyright 2001 Amiga, Inc.
**	    All Rights Reserved
*/

#ifndef PRAGMAS_VIRTUAL_PRAGMAS_H
#include <pragmas/virtual_pragmas.h>
#endif

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

#ifndef __NOLIBBASE__
extern struct Library * VirtualBase;
#endif /* __NOLIBBASE__ */

#endif /* PROTO_VIRTUAL_H */
