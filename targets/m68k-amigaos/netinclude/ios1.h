#ifndef IOS1_H
#define IOS1_H \
       "$Id: ios1.h,v 4.1 1994/10/03 20:52:20 ppessi Exp $"
/*
 *      Extensions to SAS/C level 1 IO definition file include:ios1.h
 *
 *      Copyright © 1994 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

/*
 * Include SAS/C ios1.h
 */
#ifndef _IOS1_H
#include <include:ios1.h>
#endif

/*
 * Additional flag definitions for the ufbflg field
 * The given value was recommended by SAS.
 */
#ifndef UFB_SOCK
#define UFB_SOCK 0x100 /* AmiTCP/IP socket */
#endif

/*
 * Additional external definitions
 */
extern struct UFB *__allocufb(int *);
#define   allocufb    __allocufb

#endif
