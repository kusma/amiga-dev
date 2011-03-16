#ifndef LIBRARIES_SERSCRIPT_H
#define LIBRARIES_SERSCRIPT_H \
       "$Id: serscript.h,v 1.1 1996/01/27 20:55:17 jraja Exp $"
/*
 *	Definitions of AmiTCP/IP serscript.library
 *
 *      Copyright © 1995 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef DEVICES_SERIAL_H
#include <devices/serial.h>
#endif

#ifndef SYS_CDEFS_H
#include <sys/cdefs.h>
#endif

#define SERSCRIPTNAME "AmiTCP:libs/serscript.library"

#if 0
/*
 * SerScriptContext type
 */
typedef void * SERSCRIPTCONTEXT;
#endif

/*
 * serscript.library callback prototype.
 * 
 * context  - copy of the value given to Dialing function, you can use this to
 *            track thing down on your code.
 * action   - what to do:
 *            SSCB_GET  - get the value of the variable name into result
 *            SSCB_SET  - set the value of the variable name to value
 *            SSCB_CALL - call an internal function name with arguments
 *                        value, return result in result
 *
 * name     - name of the object in question
 * value    - value (args) CSource buffer (valid from CurChr to Length)
 * result   - buffer for result of both SET and CALL.
 * 
 */
typedef ULONG (* ASM SerScriptCallback_t)(REG(a0) void * context,
					  REG(d0) ULONG action,
					  REG(a1) const UBYTE * name,
					  REG(a2) struct CSource * value,
					  REG(a3) struct CSource * result);

/* The action codes */
#define SSCB_EXIT 0 /* internal only */
#define SSCB_GET  1
#define SSCB_SET  2
#define SSCB_CALL 3

/*
 * Return codes for both the SerScriptRun and SerScriptCallback
 *
 * These values can be converted to english language strings with the 
 * SerScriptStrError() function.
 */
#define SS_OK		 0
#define SS_BREAK	 1
#define SS_TIMEOUT	 2
#define SS_NOCARRIER	 3
#define SS_NOMATCH	 4
#define SS_EOF		 5
#define SS_SYNTAX	 6
#define SS_UNKNOWN	 7
#define SS_BADARGS	 8
#define SS_TOOMANY	 9
#define SS_SCRIPTFAIL	10
#define SS_MEMORY	11
#define SS_VARIABLE	12
#define SS_IOERROR	13
#define SS_AREXX	14
#define SS_ILLACTION	15
#define SS_NOSCRIPT	16
#define SS_NOUSERGROUP	17
#define SS_INCOMPLETE	18
#define SS_INTERNAL	19
#define SS_FAIL		20

#endif /* LIBRARIES_SERSCRIPT_H */
