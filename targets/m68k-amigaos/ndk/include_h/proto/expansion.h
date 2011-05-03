#ifndef PROTO_EXPANSION_H
#define PROTO_EXPANSION_H

/*
**	$VER: expansion.h 44.1 (1.11.1999)
**	Includes Release 45.1
**
**	Lattice `C' style prototype/pragma header file combo
**
**	(C) Copyright 2001 Amiga, Inc.
**	    All Rights Reserved
*/

#ifndef PRAGMAS_EXPANSION_PRAGMAS_H
#include <pragmas/expansion_pragmas.h>
#endif

#ifndef LIBRARIES_EXPANSIONBASE_H
#include <libraries/expansionbase.h>
#endif

#ifndef __NOLIBBASE__
extern struct ExpansionBase * ExpansionBase;
#endif /* __NOLIBBASE__ */

#endif /* PROTO_EXPANSION_H */
