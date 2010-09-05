#ifndef PROTO_EXEC_H
#define PROTO_EXEC_H

/*
**	$VER: exec.h 44.1 (1.11.1999)
**	Includes Release 45.1
**
**	Lattice `C' style prototype/pragma header file combo
**
**	(C) Copyright 2001 Amiga, Inc.
**	    All Rights Reserved
*/

#ifndef EXEC_EXECBASE_H
#include <exec/execbase.h>
#endif

#if defined(_USEOLDEXEC_) || !defined(__USE_SYSBASE)
#ifndef PRAGMAS_EXEC_PRAGMAS_H
#include <pragmas/exec_pragmas.h>
#endif /* PRAGMAS_EXEC_PRAGMAS_H */
#else
#ifndef __NOLIBBASE__
extern struct ExecBase * SysBase;
#endif /* __NOLIBBASE__ */
#ifndef PRAGMAS_EXEC_SYSBASE_PRAGMAS_H
#include <pragmas/exec_sysbase_pragmas.h>
#endif /* PRAGMAS_EXEC_SYSBASE_PRAGMAS_H */
#endif /* defined(_USEOLDEXEC_) || !defined(__USE_SYSBASE) */

#endif /* PROTO_EXEC_H */
