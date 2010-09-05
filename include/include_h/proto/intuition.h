#ifndef PROTO_INTUITION_H
#define PROTO_INTUITION_H

/*
**	$VER: intuition.h 44.1 (1.11.1999)
**	Includes Release 45.1
**
**	Lattice `C' style prototype/pragma header file combo
**
**	(C) Copyright 2001 Amiga, Inc.
**	    All Rights Reserved
*/

#ifndef PRAGMAS_INTUITION_PRAGMAS_H
#include <pragmas/intuition_pragmas.h>
#endif

#ifndef INTUITION_INTUITIONBASE_H
#include <intuition/intuitionbase.h>
#endif

#ifndef __NOLIBBASE__
extern struct IntuitionBase * IntuitionBase;
#endif /* __NOLIBBASE__ */

#endif /* PROTO_INTUITION_H */
