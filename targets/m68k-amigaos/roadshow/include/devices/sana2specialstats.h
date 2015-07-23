#ifndef SANA2_SANA2SPECIALSTATS_H
#define SANA2_SANA2SPECIALSTATS_H 1
/*
**	$Id: sana2specialstats.h,v 1.1 2003/08/06 15:15:40 obarthel Exp $
**	Includes Release 50.1
**
**	Defined IDs for SANA-II special statistics.
**
**	(C) Copyright 1991-2003 Amiga, Inc.
**	    All Rights Reserved
*/

#ifndef SANA2_SANA2DEVICE_H
#include <devices/sana2.h>
#endif  /* !SANA2_SANA2DEVICE_H */

/****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
   #ifdef __PPC__
    #pragma pack(2)
   #endif
#elif defined(__VBCC__)
   #pragma amiga-align
#endif

/****************************************************************************/

/*
** The SANA-II special statistic identifier is an unsigned 32 number.
** The upper 16 bits identify the type of network wire type to which
** the statistic applies and the lower 16 bits identify the particular
** statistic.
**
** If you desire to add a new statistic identifier, contacts CATS.
*/



/*
** defined ethernet special statistics
*/

#define S2SS_ETHERNET_BADMULTICAST      ((((S2WireType_Ethernet)&0xffff)<<16)|0x0000)
/*
** This count will record the number of times a received packet tripped
** the hardware's multicast filtering mechanism but was not actually in
** the current multicast table.
*/

#define S2SS_ETHERNET_RETRIES           ((((S2WireType_Ethernet)&0xffff)<<16)|0x0001)
/*
** This count records the total number of retries which have resulted
** from transmissions on this board.
*/


#define S2SS_ETHERNET_FIFO_UNDERRUNS    ((((S2WireType_Ethernet)&0xffff)<<16)|0x0002)
/*
** This count records an error condition which indoicates that the host
** computer did not feed the network interface card at a high enough rate.
*/

/****************************************************************************/

#ifdef __GNUC__
   #ifdef __PPC__
    #pragma pack()
   #endif
#elif defined(__VBCC__)
   #pragma default-align
#endif

#ifdef __cplusplus
}
#endif

/****************************************************************************/

#endif  /* SANA2_SANA2SPECIALSTATS_H */
