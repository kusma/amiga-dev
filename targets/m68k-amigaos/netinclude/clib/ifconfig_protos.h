#ifndef CLIB_IFCONFIG_PROTOS_H
#define CLIB_IFCONFIG_PROTOS_H
/*
**	$Filename: clib/ifconfig_protos.h $
**	$Release$
**	$Revision: 1.2 $
**	$Date: 1996/02/25 21:17:40 $
**
**	C prototypes for ifconfig.library
**
**	Copyright © 1995 AmiTCP/IP Group, 
**	                 Network Solutions Development Inc.
**	                 All rights reserved.
*/

#ifndef LIBRARIES_IFCONFIG_H
#include <libraries/ifconfig.h>
#endif

/* ifconfig.library functions */

/*
 * Reserve the first entry for possible ARexx interface
 */
LONG Reserved(void);

/*
 * Find the interface configuration for interface ifname.
 */
LONG IfConfigFind(const char *ifname, struct ifconfig ** ifcp);

/*
 * Free the configuration got from IfConfigFind().
 */
void IfConfigFree(struct ifconfig * config);

/*
 * Find all interface configurations matching the filter. filter can be NULL. 
 */
LONG IfConfigFindAll(struct List ** ifclist, struct TagItem * filter);

/*
 * Free a list of configuration entries returned by IfConfigFindAll().
 */
void IfConfigFreeList(struct List * list);

/*
 * Convert ifconfig.library error codes to english language error strings.
 */
const UBYTE * IfConfigStrError(ULONG ioerr, char * buf, ULONG bufsiz);

/*
 * Return (english) name of the tag
 */
const struct ifc_confitem * IfConfigGetTagInfo(ULONG tagcode);

#endif /* CLIB_IFCONFIG_PROTOS_H */
