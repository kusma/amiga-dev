        IFND    SANA2_SANA2SPECIALSTATS_I
SANA2_SANA2SPECIALSTATS_I       SET     1
**
**      $Filename: devices/sana2specialstats.i $
**      $Revision: 1.4 $
**      $Date: 93/01/04 12:13:20 $
**
**      Defined ids for SANA-II special statistics.
**
**
**      (C) Copyright 1991 Commodore-Amiga Inc.
**              All Rights Reserved
**


        IFND    SANA2_SANA2DEVICE_I
        INCLUDE "sana2/sana2device.i"
        ENDC    !SANA2_SANA2DEVICE_I

;
; The SANA-II special statistic identifier is an unsigned 32 number.
; The upper 16 bits identify the type of network wire type to which
; the statistic applies and the lower 16 bits identify the particular
; statistic.
;

; If you'd like to add new statistic identifiers, contact CATS.

;
; defined ethernet special statistics
;

S2SS_ETHERNET_BADMULTICAST      EQU     ((((S2WIRETYPE_ETHERNET)&$ffff)<<16)|$0000)
;
; This count will record the number of times a received packet tripped
; the hardware's multicast filtering mechanism but was not actually in
; the current multicast table.
;

S2SS_ETHERNET_RETRIES           EQU     ((((S2WIRETYPE_ETHERNET)&$ffff)<<16)|$0001)
;
; This count records the total number of retries which have resulted
; from transmissions on this board.
;

S2SS_ETHERNET_FIFO_UNDERRUNS    EQU     ((((S2WIRETYPE_ETHERNET)&$ffff)<<16)|$0002)
;
; This count records an error condition which indicates that the host
; computer did not feed the network interface data at a high enough
; rate.


        ENDC    SANA2_SANA2SPECIALSTATS_I
