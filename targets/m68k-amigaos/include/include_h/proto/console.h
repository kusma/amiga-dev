#ifndef PROTO_CONSOLE_H
#define PROTO_CONSOLE_H

/*
**	$VER: console.h 44.1 (1.11.1999)
**	Includes Release 45.1
**
**	Lattice `C' style prototype/pragma header file combo
**
**	(C) Copyright 2001 Amiga, Inc.
**	    All Rights Reserved
*/

#ifndef PRAGMAS_CONSOLE_PRAGMAS_H
#include <pragmas/console_pragmas.h>
#endif

#ifndef EXEC_DEVICES_H
#include <exec/devices.h>
#endif

#ifndef __NOLIBBASE__
extern struct Device * ConsoleDevice;
#endif /* __NOLIBBASE__ */

#endif /* PROTO_CONSOLE_H */
