        IFND    SANA2_SANA2DEVICE_I
SANA2_SANA2DEVICE_I     SET     1
**
**      $Filename: devices/sana2.i $
**      $Revision: 1.16 $
**      $Date: 94/02/17 14:28:38 $
**
**      Structure definitions for SANA-II devices.
**
**      (C) Copyright 1991 Commodore-Amiga Inc.
**              All Rights Reserved
**


        IFND    EXEC_TYPES_I
        INCLUDE "exec/types.i"
        ENDC

        IFND    EXEC_PORTS_I
        INCLUDE "exec/ports.i"
        ENDC

        IFND    EXEC_IO_I
        INCLUDE "exec/io.i"
        ENDC

        IFND    EXEC_ERRORS_I
        INCLUDE "exec/errors.i"
        ENDC

        IFND    DEVICES_TIMER_I
        INCLUDE "devices/timer.i"
        ENDC

        IFND    UTILITY_TAGITEM_I
        INCLUDE "utility/tagitem.i"
        ENDC


SANA2_MAX_ADDR_BITS     EQU     128
SANA2_MAX_ADDR_BYTES    EQU     ((SANA2_MAX_ADDR_BITS+7)/8)


 STRUCTURE IOSana2Req,0
        STRUCT  IOS2_REQ,IO_SIZE
        ULONG   IOS2_WIREERROR          ; wire type specific error
        ULONG   IOS2_PACKETTYPE         ; packet type
        STRUCT  IOS2_SRCADDR,SANA2_MAX_ADDR_BYTES       ; source address
        STRUCT  IOS2_DSTADDR,SANA2_MAX_ADDR_BYTES       ; dest address
        ULONG   IOS2_DATALENGTH         ; length of packet data
        APTR    IOS2_DATA               ; packet data (not touched by driver!)
        APTR    IOS2_STATDATA           ; statistics data pointer
        APTR    IOS2_BUFFERMANAGEMENT   ; see SANA-II OpenDevice adoc

        LABEL   IOS2_SIZE


;
; equates for the IO_FLAGS field
;

SANA2IOB_RAW    EQU     7               ; raw packet IO requested
SANA2IOF_RAW    EQU     (1<<SANA2IOB_RAW)

SANA2IOB_BCAST  EQU     6               ; broadcast packet (received)
SANA2IOF_BCAST  EQU     (1<<SANA2IOB_BCAST)

SANA2IOB_MCAST  EQU     5               ; multicast packet (received)
SANA2IOF_MCAST  EQU     (1<<SANA2IOB_MCAST)

SANA2IOB_QUICK  EQU     IOB_QUICK       ; quick IO requested (0)
SANA2IOF_QUICK  EQU     IOF_QUICK


;
; equates for OpenDevice()
;

SANA2OPB_MINE   EQU     0               ; exclusive access requested
SANA2OPF_MINE   EQU     (1<<SANA2OPB_MINE)

SANA2OPB_PROM   EQU     1               ; promiscuous mode requested
SANA2OPF_PROM   EQU     (1<<SANA2OPB_PROM)

S2_Dummy        EQU     (TAG_USER+$B0000)
S2_COPYTOBUFF   EQU     S2_Dummy+1
S2_COPYFROMBUFF EQU     S2_Dummy+2
S2_PACKETFILTER EQU     S2_Dummy+3

 STRUCTURE SANA2DEVICEQUERY,0
        ; Standard information
        ULONG   S2DQ_SIZEAVAILABLE      ; bytes available
        ULONG   S2DQ_SIZESUPPLIED       ; bytes supplied
        ULONG    S2DQ_FORMAT             ; this is type 0
        ULONG    S2DQ_DEVICELEVEL        ; this document is level 0

        ; Common information
        UWORD   S2DQ_ADDRFIELDSIZE           ; address size in bits
        ULONG   S2DQ_MTU                ; maximum packet data size
        ULONG    S2DQ_BPS                ; line rate (bits/sec)
        ULONG    S2DQ_HARDWARETYPE       ; what the wire is

        ; Format specific information
        LABEL   S2DQ_SIZE


;
; defined SANA-II hardware types
;

S2WIRETYPE_ETHERNET             EQU     1
S2WIRETYPE_IEEE802              EQU     6
S2WIRETYPE_ARCNET               EQU     7
S2WIRETYPE_LOCALTALK            EQU     11
S2WIRETYPE_DYLAN                EQU     12

S2WIRETYPE_AMOKNET              EQU     200

S2WIRETYPE_LIANA		EQU	202

S2WIRETYPE_PPP                  EQU     253
S2WIRETYPE_SLIP                 EQU     254
S2WIRETYPE_CSLIP                EQU     255

S2WIRETYPE_PLIP			EQU	420

 STRUCTURE SANA2PACKETTYPESTATS,0
        ULONG    S2PTS_TXPACKETS         ; transmitted count
        ULONG    S2PTS_RXPACKETS         ; received count
        ULONG    S2PTS_TXBYTES           ; bytes transmitted count
        ULONG    S2PTS_RXBYTES           ; bytes received count
        ULONG    S2PTS_PACKETSDROPPED    ; packets dropped count
        LABEL   S2PTS_SIZE


 STRUCTURE SANA2SPECIALSTATRECORD,0
        ULONG   S2SSR_TYPE              ; statistic identifier
        ULONG    S2SSR_COUNT             ; the statistic
        APTR    S2SSR_STRING            ; statistic name
        LABEL   S2SSR_SIZE


 STRUCTURE SANA2SPECIALSTATHEADER,0
        ULONG   S2SSH_RECORDCOUNTMAX            ; room available
        ULONG   S2SSH_RECORDCOUNTSUPPLIED       ; number supplied
        LABEL   S2SSH_SIZE


 STRUCTURE SANA2DEVICESTATS,0
        ULONG    S2DS_PACKETSRECEIVED           ; received count
        ULONG    S2DS_PACKETSSENT               ; transmitted count
        ULONG    S2DS_BADDATA                   ; bad packets received
        ULONG    S2DS_OVERRUNS                  ; hardware miss count
        ULONG    S2DS_UNUSED                    ; currently unused field
        ULONG    S2DS_UNKNOWNTYPESRECEIVED      ; orphan count
        ULONG    S2DS_RECONFIGURATIONS          ; network reconfigurations
        STRUCT  S2DS_LASTSTART,TV_SIZE          ; time of last online
        LABEL   S2DS_SIZE


;
; Device Commands
;

S2_START                EQU     (CMD_NONSTD)

S2_DEVICEQUERY          EQU     (S2_START+0)
S2_GETSTATIONADDRESS    EQU     (S2_START+1)
S2_CONFIGINTERFACE      EQU     (S2_START+2)
S2_ADDMULTICASTADDRESS  EQU     (S2_START+5)
S2_DELMULTICASTADDRESS  EQU     (S2_START+6)
S2_MULTICAST            EQU     (S2_START+7)
S2_BROADCAST            EQU     (S2_START+8)
S2_TRACKTYPE            EQU     (S2_START+9)
S2_UNTRACKTYPE          EQU     (S2_START+10)
S2_GETTYPESTATS         EQU     (S2_START+11)
S2_GETSPECIALSTATS      EQU     (S2_START+12)
S2_GETGLOBALSTATS       EQU     (S2_START+13)
S2_ONEVENT              EQU     (S2_START+14)
S2_READORPHAN           EQU     (S2_START+15)
S2_ONLINE               EQU     (S2_START+16)
S2_OFFLINE              EQU     (S2_START+17)

S2_END                  EQU     (S2_START+18)


;
; defined errors for IO_ERROR
;

S2ERR_NO_ERROR          EQU     0       ; peachy-keen
S2ERR_NO_RESOURCES      EQU     1       ; resource allocation failure
S2ERR_BAD_ARGUMENT      EQU     3       ; garbage somewhere
S2ERR_BAD_STATE         EQU     4       ; inappropriate state
S2ERR_BAD_ADDRESS       EQU     5       ; who?
S2ERR_MTU_EXCEEDED      EQU     6       ; too much to chew
S2ERR_NOT_SUPPORTED     EQU     8       ; command not supported by hardware
S2ERR_SOFTWARE          EQU     9       ; software error detected
S2ERR_OUTOFSERVICE      EQU     10      ; driver is offline
S2ERR_TX_FAILURE        EQU     11      ; transmission attempt failed
;SEE ALSO <exec/errors.i>

;
; defined errors for IOS2_WIREERROR
;

S2WERR_GENERIC_ERROR    EQU     0       ; no specific info available
S2WERR_NOT_CONFIGURED   EQU     1       ; unit not configured
S2WERR_UNIT_ONLINE      EQU     2       ; unit is currently online
S2WERR_UNIT_OFFLINE     EQU     3       ; unit is currently offline
S2WERR_ALREADY_TRACKED  EQU     4       ; protocol already tracked
S2WERR_NOT_TRACKED      EQU     5       ; protocol not tracked
S2WERR_BUFF_ERROR       EQU     6       ; buffer mgmt func returned error
S2WERR_SRC_ADDRESS      EQU     7       ; source address problem
S2WERR_DST_ADDRESS      EQU     8       ; destination address problem
S2WERR_BAD_BROADCAST    EQU     9       ; broadcast address problem
S2WERR_BAD_MULTICAST    EQU     10      ; multicast address problem
S2WERR_MULTICAST_FULL   EQU     11      ; multicast address list full
S2WERR_BAD_EVENT        EQU     12      ; unsupported event class
S2WERR_BAD_STATDATA     EQU     13      ; statdata failed sanity check
S2WERR_IS_CONFIGURED    EQU     15      ; attempt to config twice
S2WERR_NULL_POINTER     EQU     16      ; null pointer detected
S2WERR_TOO_MANY_RETRIES EQU     17      ; tx failed due to too many retries
S2WERR_RCVREL_HDW_ERR   EQU     18      ; driver fixable hdw error


;
; defined events
;

S2EVENT_ERROR       equ 1      ; error catch all
S2EVENT_TX          equ 2      ; transmitter error catch all
S2EVENT_RX          equ 4      ; receiver error catch all
S2EVENT_ONLINE      equ 8      ; unit is in service
S2EVENT_OFFLINE     equ 16     ; unit is not in service
S2EVENT_BUFF        equ 32     ; buffer mgmt function error catch all
S2EVENT_HARDWARE    equ 64     ; hardware error catch all
S2EVENT_SOFTWARE    equ 128    ; software error catch all


        ENDC    SANA2_SANA2DEVICE_I

