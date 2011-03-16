#ifndef CLIB_SERSCRIPT_PROTOS_H
#define CLIB_SERSCRIPT_PROTOS_H
/*
**	$Filename: clib/serscript_protos.h $
**	$Release$
**	$Revision: 1.1 $
**	$Date: 1996/01/27 20:55:17 $
**
**	C prototypes for serscript.library
**
**	Copyright © 1995 AmiTCP/IP Group, 
**	                 Network Solutions Development Inc.
**	                 All rights reserved.
*/

#ifndef LIBRARIES_SERSCRIPT_H
#include <libraries/serscript.h>
#endif

/* serscript.library functions */

/*
 * SerScriptRun arguments:
 *
 * templreq - an initialized IOExtSer, which can be used to do serial IO.
 *            this is used as a template for creating temporary IO requests.
 *
 * context  - Caller private context pointer, which is not referenced by the 
 *            library, but passed to the callback when called.
 *
 * callback - callback for setting/getting variables and extended commands.
 *            Can be NULL if not supported.
 *
 * script   - the name of the serial script residing in AmiTCP:adrx/ if
 *            absolute path is not given.
 *
 * carrierdetect - sense carrier while doing serial IO.
 */
ULONG SerScriptRun(struct IOExtSer * templreq, void * context, 
        SerScriptCallback_t callback, const UBYTE * script, LONG carrierdetect);

/*
 * SerScriptStrError can be used to convert serscript.library error codes to
 * english language error strings.
 */
const UBYTE * SerScriptStrError(ULONG error);

#endif /* CLIB_SERSCRIPT_PROTOS_H */
