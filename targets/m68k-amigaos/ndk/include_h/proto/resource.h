#ifndef PROTO_RESOURCE_H
#define PROTO_RESOURCE_H

/*
**	$VER: resource.h 44.1 (1.11.1999)
**	Includes Release 45.1
**
**	Lattice `C' style prototype/pragma header file combo
**
**	(C) Copyright 2001 Amiga, Inc.
**	    All Rights Reserved
*/

#ifndef PRAGMAS_RESOURCE_PRAGMAS_H
#include <pragmas/resource_pragmas.h>
#endif

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

#ifndef __NOLIBBASE__
extern struct Library * ResourceBase;
#endif /* __NOLIBBASE__ */

#endif /* PROTO_RESOURCE_H */
