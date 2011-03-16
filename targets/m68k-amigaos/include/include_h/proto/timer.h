#ifndef PROTO_TIMER_H
#define PROTO_TIMER_H

/*
**	$VER: timer.h 44.1 (1.11.1999)
**	Includes Release 45.1
**
**	Lattice `C' style prototype/pragma header file combo
**
**	(C) Copyright 2001 Amiga, Inc.
**	    All Rights Reserved
*/

#ifndef PRAGMAS_TIMER_PRAGMAS_H
#include <pragmas/timer_pragmas.h>
#endif

#ifndef EXEC_DEVICES_H
#include <exec/devices.h>
#endif

#ifndef __NOLIBBASE__
extern struct Device * TimerBase;
#endif /* __NOLIBBASE__ */

#endif /* PROTO_TIMER_H */
