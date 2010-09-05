#ifndef PROTO_ASL_H
#define PROTO_ASL_H

/*
**	$VER: asl.h 44.1 (1.11.1999)
**	Includes Release 45.1
**
**	Lattice `C' style prototype/pragma header file combo
**
**	(C) Copyright 2001 Amiga, Inc.
**	    All Rights Reserved
*/

#ifndef PRAGMAS_ASL_PRAGMAS_H
#include <pragmas/asl_pragmas.h>
#endif

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

#ifndef __NOLIBBASE__
extern struct Library * AslBase;
#endif /* __NOLIBBASE__ */

#endif /* PROTO_ASL_H */
