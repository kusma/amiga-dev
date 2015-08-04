/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code
 * distributions retain the above copyright notice and this paragraph
 * in its entirety, and (2) distributions including binary code include
 * the above copyright notice and this paragraph in its entirety in
 * the documentation or other materials provided with the distribution.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND
 * WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, WITHOUT
 * LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE.
 *
 * Original code by Hannes Gredler (hannes@juniper.net)
 */

#ifndef lint
static const char rcsid[] _U_ =
    "@(#) $Header: /Users/olsen/Code/cvs2svn-2010/os4-cvs-repository/contrib/roadshow/client-software/tcpdump-3.8.1/print-rsvp.c,v 1.2 2004-09-22 15:40:56 olsen Exp $";
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tcpdump-stdinc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interface.h"
#include "extract.h"
#include "addrtoname.h"
#include "ethertype.h"
#include "gmpls.h"

/*
 * RFC 2205 common header
 *
 *               0             1              2             3
 *        +-------------+-------------+-------------+-------------+
 *        | Vers | Flags|  Msg Type   |       RSVP Checksum       |
 *        +-------------+-------------+-------------+-------------+
 *        |  Send_TTL   | (Reserved)  |        RSVP Length        |
 *        +-------------+-------------+-------------+-------------+
 *
 */

struct rsvp_common_header {
    u_int8_t version_flags;
    u_int8_t msg_type;
    u_int8_t checksum[2];
    u_int8_t ttl;
    u_int8_t reserved;
    u_int8_t length[2];
};

/* 
 * RFC2205 object header
 *
 * 
 *               0             1              2             3
 *        +-------------+-------------+-------------+-------------+
 *        |       Length (bytes)      |  Class-Num  |   C-Type    |
 *        +-------------+-------------+-------------+-------------+
 *        |                                                       |
 *        //                  (Object contents)                   //
 *        |                                                       |
 *        +-------------+-------------+-------------+-------------+
 */

struct rsvp_object_header {
    u_int8_t length[2];
    u_int8_t class_num;
    u_int8_t ctype;
};

#define RSVP_VERSION            1
#define	RSVP_EXTRACT_VERSION(x) (((x)&0xf0)>>4) 
#define	RSVP_EXTRACT_FLAGS(x)   ((x)&0x0f)

#define	RSVP_MSGTYPE_PATH       1
#define	RSVP_MSGTYPE_RESV       2
#define	RSVP_MSGTYPE_PATHERR    3
#define	RSVP_MSGTYPE_RESVERR    4
#define	RSVP_MSGTYPE_PATHTEAR   5
#define	RSVP_MSGTYPE_RESVTEAR   6
#define	RSVP_MSGTYPE_RESVCONF   7
#define RSVP_MSGTYPE_AGGREGATE  12
#define RSVP_MSGTYPE_ACK        13
#define RSVP_MSGTYPE_HELLO_OLD  14      /* ancient Hellos */
#define RSVP_MSGTYPE_SREFRESH   15
#define	RSVP_MSGTYPE_HELLO      20

static const struct tok rsvp_msg_type_values[] = {
    { RSVP_MSGTYPE_PATH,	"Path" },
    { RSVP_MSGTYPE_RESV,	"Resv" },
    { RSVP_MSGTYPE_PATHERR,	"PathErr" },
    { RSVP_MSGTYPE_RESVERR,	"ResvErr" },
    { RSVP_MSGTYPE_PATHTEAR,	"PathTear" },
    { RSVP_MSGTYPE_RESVTEAR,	"ResvTear" },
    { RSVP_MSGTYPE_RESVCONF,	"ResvConf" },
    { RSVP_MSGTYPE_AGGREGATE,	"Aggregate" },
    { RSVP_MSGTYPE_ACK,	        "Acknowledgement" },
    { RSVP_MSGTYPE_HELLO_OLD,	"Hello (Old)" },
    { RSVP_MSGTYPE_SREFRESH,	"Refresh" },
    { RSVP_MSGTYPE_HELLO,	"Hello" },
    { 0, NULL}
};

static const struct tok rsvp_header_flag_values[] = {
    { 0x01,	              "Refresh reduction capable" }, /* rfc2961 */
    { 0, NULL}
};

#define	RSVP_OBJ_SESSION            1   /* rfc2205 */
#define	RSVP_OBJ_RSVP_HOP           3   /* rfc2205, rfc3473 */
#define	RSVP_OBJ_INTEGRITY          4
#define	RSVP_OBJ_TIME_VALUES        5   /* rfc2205 */
#define	RSVP_OBJ_ERROR_SPEC         6 
#define	RSVP_OBJ_SCOPE              7
#define	RSVP_OBJ_STYLE              8   /* rfc2205 */
#define	RSVP_OBJ_FLOWSPEC           9   /* rfc2215 */
#define	RSVP_OBJ_FILTERSPEC         10  /* rfc2215 */
#define	RSVP_OBJ_SENDER_TEMPLATE    11
#define	RSVP_OBJ_SENDER_TSPEC       12  /* rfc2215 */
#define	RSVP_OBJ_ADSPEC             13  /* rfc2215 */
#define	RSVP_OBJ_POLICY_DATA        14
#define	RSVP_OBJ_CONFIRM            15  /* rfc2205 */
#define	RSVP_OBJ_LABEL              16  /* rfc3209 */
#define	RSVP_OBJ_LABEL_REQ          19  /* rfc3209 */
#define	RSVP_OBJ_ERO                20  /* rfc3209 */
#define	RSVP_OBJ_RRO                21  /* rfc3209 */
#define	RSVP_OBJ_HELLO              22  /* rfc3209 */
#define	RSVP_OBJ_MESSAGE_ID         23
#define	RSVP_OBJ_MESSAGE_ID_ACK     24
#define	RSVP_OBJ_MESSAGE_ID_LIST    25
#define	RSVP_OBJ_RECOVERY_LABEL     34  /* rfc3473 */
#define	RSVP_OBJ_UPSTREAM_LABEL     35  /* rfc3473 */
#define	RSVP_OBJ_LABEL_SET          36  /* rfc3473 */
#define	RSVP_OBJ_PROTECTION         37  /* rfc3473 */
#define	RSVP_OBJ_DETOUR             63  /* draft-ietf-mpls-rsvp-lsp-fastreroute-01 */
#define	RSVP_OBJ_SUGGESTED_LABEL    129 /* rfc3473 */
#define	RSVP_OBJ_ACCEPT_LABEL_SET   130 /* rfc3473 */
#define	RSVP_OBJ_RESTART_CAPABILITY 131 /* rfc3473 */
#define	RSVP_OBJ_NOTIFY_REQ         195 /* rfc3473 */
#define	RSVP_OBJ_ADMIN_STATUS       196 /* rfc3473 */
#define	RSVP_OBJ_PROPERTIES         204 /* juniper proprietary */
#define	RSVP_OBJ_FASTREROUTE        205 /* draft-ietf-mpls-rsvp-lsp-fastreroute-01 */
#define	RSVP_OBJ_SESSION_ATTRIBUTE  207 /* rfc3209 */
#define RSVP_OBJ_CALL_ID            230 /* rfc3474 */
#define RSVP_OBJ_CALL_OPS           236 /* rfc3474 */

static const struct tok rsvp_obj_values[] = {
    { RSVP_OBJ_SESSION,            "Session" },
    { RSVP_OBJ_RSVP_HOP,           "RSVP Hop" },
    { RSVP_OBJ_INTEGRITY,          "Integrity" },
    { RSVP_OBJ_TIME_VALUES,        "Time Values" },
    { RSVP_OBJ_ERROR_SPEC,         "Error Spec" },
    { RSVP_OBJ_SCOPE,              "Scope" },
    { RSVP_OBJ_STYLE,              "Style" },
    { RSVP_OBJ_FLOWSPEC,           "Flowspec" },
    { RSVP_OBJ_FILTERSPEC,         "FilterSpec" },
    { RSVP_OBJ_SENDER_TEMPLATE,    "Sender Template" },
    { RSVP_OBJ_SENDER_TSPEC,       "Sender TSpec" },
    { RSVP_OBJ_ADSPEC,             "Adspec" },
    { RSVP_OBJ_POLICY_DATA,        "Policy Data" },
    { RSVP_OBJ_CONFIRM,            "Confirm" },
    { RSVP_OBJ_LABEL,              "Label" },
    { RSVP_OBJ_LABEL_REQ,          "Label Request" },
    { RSVP_OBJ_ERO,                "ERO" },
    { RSVP_OBJ_RRO,                "RRO" },
    { RSVP_OBJ_HELLO,              "Hello" },
    { RSVP_OBJ_MESSAGE_ID,         "Message ID" },
    { RSVP_OBJ_MESSAGE_ID_ACK,     "Message ID Ack" },
    { RSVP_OBJ_MESSAGE_ID_LIST,    "Message ID List" },
    { RSVP_OBJ_RECOVERY_LABEL,     "Recovery Label" },
    { RSVP_OBJ_UPSTREAM_LABEL,     "Upstream Label" },
    { RSVP_OBJ_LABEL_SET,          "Label Set" },
    { RSVP_OBJ_ACCEPT_LABEL_SET,   "Acceptable Label Set" },
    { RSVP_OBJ_DETOUR,             "Detour" },
    { RSVP_OBJ_SUGGESTED_LABEL,    "Suggested Label" },
    { RSVP_OBJ_PROPERTIES,         "Properties" },
    { RSVP_OBJ_FASTREROUTE,        "Fast Re-Route" },
    { RSVP_OBJ_SESSION_ATTRIBUTE,  "Session Attribute" },
    { RSVP_OBJ_CALL_ID,            "Call-ID" },
    { RSVP_OBJ_CALL_OPS,           "Call Capability" },
    { RSVP_OBJ_RESTART_CAPABILITY, "Restart Capability" },
    { RSVP_OBJ_NOTIFY_REQ,         "Notify Request" },
    { RSVP_OBJ_PROTECTION,         "Protection" },
    { RSVP_OBJ_ADMIN_STATUS,       "Administrative Status" },
    { 0, NULL}
};

#define	RSVP_CTYPE_IPV4        1
#define	RSVP_CTYPE_IPV6        2
#define	RSVP_CTYPE_TUNNEL_IPV4 7
#define	RSVP_CTYPE_TUNNEL_IPV6 8
#define RSVP_CTYPE_1           1
#define RSVP_CTYPE_2           2
#define RSVP_CTYPE_3           3
#define RSVP_CTYPE_4           4

/*
 * the ctypes are not globally unique so for
 * translating it to strings we build a table based
 * on objects offsetted by the ctype
 */

static const struct tok rsvp_ctype_values[] = {
    { 256*RSVP_OBJ_RSVP_HOP+RSVP_CTYPE_IPV4,	             "IPv4" },
    { 256*RSVP_OBJ_RSVP_HOP+RSVP_CTYPE_IPV6,	             "IPv6" },
    { 256*RSVP_OBJ_RSVP_HOP+RSVP_CTYPE_3,	             "IPv4 plus opt. TLVs" },
    { 256*RSVP_OBJ_RSVP_HOP+RSVP_CTYPE_4,	             "IPv6 plus opt. TLVs" },
    { 256*RSVP_OBJ_NOTIFY_REQ+RSVP_CTYPE_IPV4,	             "IPv4" },
    { 256*RSVP_OBJ_NOTIFY_REQ+RSVP_CTYPE_IPV6,	             "IPv6" },
    { 256*RSVP_OBJ_CONFIRM+RSVP_CTYPE_IPV4,	             "IPv4" },
    { 256*RSVP_OBJ_CONFIRM+RSVP_CTYPE_IPV6,	             "IPv6" },
    { 256*RSVP_OBJ_TIME_VALUES+RSVP_CTYPE_1,	             "1" },
    { 256*RSVP_OBJ_FLOWSPEC+RSVP_CTYPE_1,	             "obsolete" },
    { 256*RSVP_OBJ_FLOWSPEC+RSVP_CTYPE_2,	             "IntServ" },
    { 256*RSVP_OBJ_SENDER_TSPEC+RSVP_CTYPE_2,	             "IntServ" },
    { 256*RSVP_OBJ_ADSPEC+RSVP_CTYPE_2,	                     "IntServ" },
    { 256*RSVP_OBJ_FILTERSPEC+RSVP_CTYPE_IPV4,	             "IPv4" },
    { 256*RSVP_OBJ_FILTERSPEC+RSVP_CTYPE_IPV6,	             "IPv6" },
    { 256*RSVP_OBJ_FILTERSPEC+RSVP_CTYPE_3,	             "IPv6 Flow-label" },
    { 256*RSVP_OBJ_FILTERSPEC+RSVP_CTYPE_TUNNEL_IPV4,	     "Tunnel IPv4" },
    { 256*RSVP_OBJ_SESSION+RSVP_CTYPE_IPV4,	             "IPv4" },
    { 256*RSVP_OBJ_SESSION+RSVP_CTYPE_IPV6,	             "IPv6" },
    { 256*RSVP_OBJ_SESSION+RSVP_CTYPE_TUNNEL_IPV4,           "Tunnel IPv4" },
    { 256*RSVP_OBJ_SENDER_TEMPLATE+RSVP_CTYPE_IPV4,          "IPv4" },
    { 256*RSVP_OBJ_SENDER_TEMPLATE+RSVP_CTYPE_IPV6,          "IPv6" },
    { 256*RSVP_OBJ_SENDER_TEMPLATE+RSVP_CTYPE_TUNNEL_IPV4,   "Tunnel IPv4" },
    { 256*RSVP_OBJ_STYLE+RSVP_CTYPE_1,                       "1" },
    { 256*RSVP_OBJ_HELLO+RSVP_CTYPE_1,                       "Hello Request" },
    { 256*RSVP_OBJ_HELLO+RSVP_CTYPE_2,                       "Hello Ack" },
    { 256*RSVP_OBJ_LABEL_REQ+RSVP_CTYPE_1,	             "without label range" },
    { 256*RSVP_OBJ_LABEL_REQ+RSVP_CTYPE_2,	             "with ATM label range" },
    { 256*RSVP_OBJ_LABEL_REQ+RSVP_CTYPE_3,                   "with FR label range" },
    { 256*RSVP_OBJ_LABEL_REQ+RSVP_CTYPE_4,                   "Generalized Label" },
    { 256*RSVP_OBJ_LABEL+RSVP_CTYPE_1,                       "Label" },
    { 256*RSVP_OBJ_LABEL+RSVP_CTYPE_2,                       "Generalized Label" },
    { 256*RSVP_OBJ_LABEL+RSVP_CTYPE_3,                       "Waveband Switching" },
    { 256*RSVP_OBJ_SUGGESTED_LABEL+RSVP_CTYPE_1,             "Label" },
    { 256*RSVP_OBJ_SUGGESTED_LABEL+RSVP_CTYPE_2,             "Generalized Label" },
    { 256*RSVP_OBJ_SUGGESTED_LABEL+RSVP_CTYPE_3,             "Waveband Switching" },
    { 256*RSVP_OBJ_UPSTREAM_LABEL+RSVP_CTYPE_1,              "Label" },
    { 256*RSVP_OBJ_UPSTREAM_LABEL+RSVP_CTYPE_2,              "Generalized Label" },
    { 256*RSVP_OBJ_UPSTREAM_LABEL+RSVP_CTYPE_3,              "Waveband Switching" },
    { 256*RSVP_OBJ_RECOVERY_LABEL+RSVP_CTYPE_1,              "Label" },
    { 256*RSVP_OBJ_RECOVERY_LABEL+RSVP_CTYPE_2,              "Generalized Label" },
    { 256*RSVP_OBJ_RECOVERY_LABEL+RSVP_CTYPE_3,              "Waveband Switching" },
    { 256*RSVP_OBJ_ERO+RSVP_CTYPE_IPV4,                      "IPv4" },
    { 256*RSVP_OBJ_RRO+RSVP_CTYPE_IPV4,                      "IPv4" },
    { 256*RSVP_OBJ_ERROR_SPEC+RSVP_CTYPE_IPV4,               "IPv4" },
    { 256*RSVP_OBJ_ERROR_SPEC+RSVP_CTYPE_IPV6,               "IPv6" },
    { 256*RSVP_OBJ_ERROR_SPEC+RSVP_CTYPE_3,                  "IPv4 plus opt. TLVs" },
    { 256*RSVP_OBJ_ERROR_SPEC+RSVP_CTYPE_4,                  "IPv6 plus opt. TLVs" },
    { 256*RSVP_OBJ_RESTART_CAPABILITY+RSVP_CTYPE_1,          "IPv4" },
    { 256*RSVP_OBJ_SESSION_ATTRIBUTE+RSVP_CTYPE_TUNNEL_IPV4, "Tunnel IPv4" },
    { 256*RSVP_OBJ_FASTREROUTE+RSVP_CTYPE_TUNNEL_IPV4,       "Tunnel IPv4" },
    { 256*RSVP_OBJ_DETOUR+RSVP_CTYPE_TUNNEL_IPV4,            "Tunnel IPv4" },
    { 256*RSVP_OBJ_PROPERTIES+RSVP_CTYPE_1,                  "1" },
    { 0, NULL}
};

#define RSVP_OBJ_XRO_MASK_SUBOBJ(x)   ((x)&0x7f)
#define RSVP_OBJ_XRO_MASK_LOOSE(x)    ((x)&0x80)

#define	RSVP_OBJ_XRO_RES       0
#define	RSVP_OBJ_XRO_IPV4      1
#define	RSVP_OBJ_XRO_IPV6      2
#define	RSVP_OBJ_XRO_ASN       32
#define	RSVP_OBJ_XRO_MPLS      64

static const struct tok rsvp_obj_xro_values[] = {
    { RSVP_OBJ_XRO_RES,	      "Reserved" },
    { RSVP_OBJ_XRO_IPV4,      "IPv4 prefix" },
    { RSVP_OBJ_XRO_IPV6,      "IPv6 prefix" },
    { RSVP_OBJ_XRO_ASN,       "Autonomous system number" },
    { RSVP_OBJ_XRO_MPLS,      "MPLS label switched path termination" },
    { 0, NULL}
};

/* draft-ietf-mpls-rsvp-lsp-fastreroute-02.txt */
static const struct tok rsvp_obj_rro_flag_values[] = {
    { 0x01,	              "Local protection available" },
    { 0x02,                   "Local protection in use" },
    { 0x04,                   "Bandwidth protection" },
    { 0x08,                   "Node protection" },
    { 0, NULL}
};

static const struct tok rsvp_resstyle_values[] = {
    { 17,	              "Wildcard Filter" },
    { 10,                     "Fixed Filter" },
    { 18,                     "Shared Explicit" },
    { 0, NULL}
};

#define RSVP_OBJ_INTSERV_GUARANTEED_SERV 2
#define RSVP_OBJ_INTSERV_CONTROLLED_LOAD 5

static const struct tok rsvp_intserv_service_type_values[] = {
    { 1,                                "Default/Global Information" },
    { RSVP_OBJ_INTSERV_GUARANTEED_SERV, "Guaranteed Service" },
    { RSVP_OBJ_INTSERV_CONTROLLED_LOAD,	"Controlled Load" },
    { 0, NULL}
};

static const struct tok rsvp_intserv_parameter_id_values[] = {
    { 4,                     "IS hop cnt" },
    { 6,                     "Path b/w estimate" },
    { 8,                     "Minimum path latency" },
    { 10,                    "Composed MTU" },
    { 127,                   "Token Bucket TSpec" },
    { 130,                   "Guaranteed Service RSpec" },
    { 133,                   "End-to-end composed value for C" },
    { 134,                   "End-to-end composed value for D" },
    { 135,                   "Since-last-reshaping point composed C" },
    { 136,                   "Since-last-reshaping point composed D" },
    { 0, NULL}
};

static struct tok rsvp_session_attribute_flag_values[] = {
    { 0x01,	              "Local Protection desired" },
    { 0x02,                   "Label Recording desired" },
    { 0x04,                   "SE Style desired" },
    { 0x08,                   "Bandwidth protection desired" }, /* draft-ietf-mpls-rsvp-lsp-fastreroute-02.txt */
    { 0x10,                   "Node protection desired" },      /* draft-ietf-mpls-rsvp-lsp-fastreroute-02.txt */
    { 0, NULL}
};

static struct tok rsvp_obj_prop_tlv_values[] = {
    { 0x01,                   "Cos" },
    { 0x02,                   "Metric 1" },
    { 0x04,                   "Metric 2" },
    { 0x08,                   "CCC Status" },
    { 0x10,                   "Path Type" },
    { 0, NULL}
};

#define RSVP_OBJ_ERROR_SPEC_CODE_ROUTING 24
#define RSVP_OBJ_ERROR_SPEC_CODE_NOTIFY  25

static struct tok rsvp_obj_error_code_values[] = {
    { RSVP_OBJ_ERROR_SPEC_CODE_ROUTING, "Routing Problem" },
    { RSVP_OBJ_ERROR_SPEC_CODE_NOTIFY,  "Notify Error" },
    { 0, NULL}
};

static struct tok rsvp_obj_error_code_routing_values[] = {
    { 1,                      "Bad EXPLICIT_ROUTE object" },
    { 2,                      "Bad strict node" },
    { 3,                      "Bad loose node" },
    { 4,                      "Bad initial subobject" },
    { 5,                      "No route available toward destination" },
    { 6,                      "Unacceptable label value" },
    { 7,                      "RRO indicated routing loops" },
    { 8,                      "non-RSVP-capable router in the path" },
    { 9,                      "MPLS label allocation failure" },
    { 10,                     "Unsupported L3PID" },
    { 0, NULL}
};

#define FALSE 0
#define TRUE  1


int rsvp_intserv_print(const u_char *);

/* 
 * this is a dissector for all the intserv defined
 * specs as defined per rfc2215
 * it is called from various rsvp objects;
 * returns the amount of bytes being processed
 */
int
rsvp_intserv_print(const u_char *tptr) {

    int parameter_id,parameter_length;
    union {
	float f;
	u_int32_t i;
    } bw;

    parameter_id = *(tptr);
    parameter_length = EXTRACT_16BITS(tptr+2)<<2; /* convert wordcount to bytecount */

    printf("\n\t      Parameter ID: %s (%u), length: %u, Flags: [0x%02x]",
           tok2str(rsvp_intserv_parameter_id_values,"unknown",parameter_id),
           parameter_id,
           parameter_length,
           *(tptr+1));

    switch(parameter_id) { /* parameter_id */

    case 4:
       /*
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        * |    4 (e)      |    (f)        |           1 (g)               |
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        * |        IS hop cnt (32-bit unsigned integer)                   |
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        */
        printf("\n\t\tIS hop count: %u", EXTRACT_32BITS(tptr+4));
        break;

    case 6:
       /*
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        * |    6 (h)      |    (i)        |           1 (j)               |
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        * |  Path b/w estimate  (32-bit IEEE floating point number)       |
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        */
        bw.i = EXTRACT_32BITS(tptr+4);
        printf("\n\t\tPath b/w estimate: %.10g Mbps", bw.f/125000);
        break;

    case 8:
       /*
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        * |     8 (k)     |    (l)        |           1 (m)               |
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        * |        Minimum path latency (32-bit integer)                  |
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        */
        printf("\n\t\tMinimum path latency: ");
        if (EXTRACT_32BITS(tptr+4) == 0xffffffff)
            printf("don't care");
        else
            printf("%u", EXTRACT_32BITS(tptr+4));
        break;

    case 10:

       /*
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        * |     10 (n)    |      (o)      |           1 (p)               |
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        * |      Composed MTU (32-bit unsigned integer)                   |
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        */
        printf("\n\t\tComposed MTU: %u bytes", EXTRACT_32BITS(tptr+4));
        break;
    case 127:
       /* 
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        * |   127 (e)     |    0 (f)      |             5 (g)             |
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        * |  Token Bucket Rate [r] (32-bit IEEE floating point number)    |
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        * |  Token Bucket Size [b] (32-bit IEEE floating point number)    |
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        * |  Peak Data Rate [p] (32-bit IEEE floating point number)       |
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        * |  Minimum Policed Unit [m] (32-bit integer)                    |
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        * |  Maximum Packet Size [M]  (32-bit integer)                    |
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        */

        bw.i = EXTRACT_32BITS(tptr+4);
        printf("\n\t\tToken Bucket Rate: %.10g Mbps", bw.f/125000);
        bw.i = EXTRACT_32BITS(tptr+8);
        printf("\n\t\tToken Bucket Size: %.10g bytes", bw.f);
        bw.i = EXTRACT_32BITS(tptr+12);
        printf("\n\t\tPeak Data Rate: %.10g Mbps", bw.f/125000);
        printf("\n\t\tMinimum Policed Unit: %u bytes", EXTRACT_32BITS(tptr+16));
        printf("\n\t\tMaximum Packet Size: %u bytes", EXTRACT_32BITS(tptr+20));
        break;

    case 130:
       /* 
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        * |     130 (h)   |    0 (i)      |            2 (j)              |
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        * |  Rate [R]  (32-bit IEEE floating point number)                |
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        * |  Slack Term [S]  (32-bit integer)                             |
        * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        */

        bw.i = EXTRACT_32BITS(tptr+4);
        printf("\n\t\tRate: %.10g Mbps", bw.f/125000);
        printf("\n\t\tSlack Term: %u", EXTRACT_32BITS(tptr+8));
        break;

    case 133:
    case 134:
    case 135:
    case 136:
        printf("\n\t\tValue: %u", EXTRACT_32BITS(tptr+4));
        break;

    default:
        if (vflag <= 1)
            print_unknown_data(tptr+4,"\n\t\t",parameter_length);
    }
    return (parameter_length+4); /* header length 4 bytes */
}

void
rsvp_print(register const u_char *pptr, register u_int len) {

    const struct rsvp_common_header *rsvp_com_header;
    const struct rsvp_object_header *rsvp_obj_header;
    const u_char *tptr,*obj_tptr;
    u_short tlen,rsvp_obj_len,rsvp_obj_ctype,obj_tlen,intserv_serv_tlen;
    int hexdump,processed,padbytes,error_code,error_value;
    union {
	float f;
	u_int32_t i;
    } bw;

    tptr=pptr;
    rsvp_com_header = (const struct rsvp_common_header *)pptr;
    TCHECK(*rsvp_com_header);

    /*
     * Sanity checking of the header.
     */
    if (RSVP_EXTRACT_VERSION(rsvp_com_header->version_flags) != RSVP_VERSION) {
	printf("RSVP version %u packet not supported",
               RSVP_EXTRACT_VERSION(rsvp_com_header->version_flags));
	return;
    }

    /* in non-verbose mode just lets print the basic Message Type*/
    if (vflag < 1) {
        printf("RSVP %s Message, length: %u",
               tok2str(rsvp_msg_type_values, "unknown (%u)",rsvp_com_header->msg_type),
               len);
        return;
    }

    /* ok they seem to want to know everything - lets fully decode it */

    tlen=EXTRACT_16BITS(rsvp_com_header->length);

    printf("RSVP\n\tv: %u, msg-type: %s, Flags: [%s], length: %u, ttl: %u, checksum: 0x%04x",
           RSVP_EXTRACT_VERSION(rsvp_com_header->version_flags),
           tok2str(rsvp_msg_type_values, "unknown, type: %u",rsvp_com_header->msg_type),
           bittok2str(rsvp_header_flag_values,"none",RSVP_EXTRACT_FLAGS(rsvp_com_header->version_flags)),
           tlen,
           rsvp_com_header->ttl,
           EXTRACT_16BITS(rsvp_com_header->checksum));

    tptr+=sizeof(const struct rsvp_common_header);
    tlen-=sizeof(const struct rsvp_common_header);

    while(tlen>0) {
        /* did we capture enough for fully decoding the object header ? */
        if (!TTEST2(*tptr, sizeof(struct rsvp_object_header)))
            goto trunc;

        rsvp_obj_header = (const struct rsvp_object_header *)tptr;
        rsvp_obj_len=EXTRACT_16BITS(rsvp_obj_header->length);
        rsvp_obj_ctype=rsvp_obj_header->ctype;

        if(rsvp_obj_len % 4 || rsvp_obj_len < 4)
            return;

        printf("\n\t  %s Object (%u) Flags: [%s",
               tok2str(rsvp_obj_values,
                       "Unknown",
                       rsvp_obj_header->class_num),
               rsvp_obj_header->class_num,
               ((rsvp_obj_header->class_num)&0x80) ? "ignore" : "reject");

        if (rsvp_obj_header->class_num > 128)
            printf(" %s",
                   ((rsvp_obj_header->class_num)&0x40) ? "and forward" : "silently");

        printf(" if unknown], Class-Type: %s (%u), length: %u",
               tok2str(rsvp_ctype_values,
                       "Unknown",
                       ((rsvp_obj_header->class_num)<<8)+rsvp_obj_ctype),
               rsvp_obj_ctype,
               rsvp_obj_len);

        obj_tptr=tptr+sizeof(struct rsvp_object_header);
        obj_tlen=rsvp_obj_len-sizeof(struct rsvp_object_header);

        /* did we capture enough for fully decoding the object ? */
        if (!TTEST2(*tptr, rsvp_obj_len))
            goto trunc;
        hexdump=FALSE;

        switch(rsvp_obj_header->class_num) {
        case RSVP_OBJ_SESSION:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_IPV4:
                printf("\n\t    IPv4 DestAddress: %s, Protocol ID: 0x%02x",
                       ipaddr_string(obj_tptr),
                       *(obj_tptr+4));
                printf("\n\t    Flags: [0x%02x], DestPort %u",
                       *(obj_tptr+5),
                       EXTRACT_16BITS(obj_tptr+6));
                obj_tlen-=8;
                obj_tptr+=8;                
                break;
#ifdef INET6
            case RSVP_CTYPE_IPV6:
                printf("\n\t    IPv6 DestAddress: %s, Protocol ID: 0x%02x",
                       ip6addr_string(obj_tptr),
                       *(obj_tptr+16));
                printf("\n\t    Flags: [0x%02x], DestPort %u",
                       *(obj_tptr+17),
                       EXTRACT_16BITS(obj_tptr+18));
                obj_tlen-=20;
                obj_tptr+=20;                
                break;

            case RSVP_CTYPE_TUNNEL_IPV6:
                printf("\n\t    IPv6 Tunnel EndPoint: %s, Tunnel ID: 0x%04x, Extended Tunnel ID: %s",
                       ip6addr_string(obj_tptr),
                       EXTRACT_16BITS(obj_tptr+18),
                       ip6addr_string(obj_tptr+20));
                obj_tlen-=36;
                obj_tptr+=36;                
                break;
#endif
            case RSVP_CTYPE_TUNNEL_IPV4:
                printf("\n\t    IPv4 Tunnel EndPoint: %s, Tunnel ID: 0x%04x, Extended Tunnel ID: %s",
                       ipaddr_string(obj_tptr),
                       EXTRACT_16BITS(obj_tptr+6),
                       ipaddr_string(obj_tptr+8));
                obj_tlen-=12;
                obj_tptr+=12;                
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_CONFIRM:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_IPV4:
                printf("\n\t    IPv4 Receiver Address: %s",
                       ipaddr_string(obj_tptr));
                obj_tlen-=4;
                obj_tptr+=4;                
                break;
#ifdef INET6
            case RSVP_CTYPE_IPV6:
                printf("\n\t    IPv6 Receiver Address: %s",
                       ip6addr_string(obj_tptr));
                obj_tlen-=16;
                obj_tptr+=16;                
                break;
#endif
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_NOTIFY_REQ:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_IPV4:
                printf("\n\t    IPv4 Notify Node Address: %s",
                       ipaddr_string(obj_tptr));
                obj_tlen-=4;
                obj_tptr+=4;                
                break;
#ifdef INET6
            case RSVP_CTYPE_IPV6:
                printf("\n\t    IPv6 Notify Node Address: %s",
                       ip6addr_string(obj_tptr));
                obj_tlen-=16;
                obj_tptr+=16;                
                break;
#endif
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_SUGGESTED_LABEL: /* fall through */
        case RSVP_OBJ_UPSTREAM_LABEL:  /* fall through */
        case RSVP_OBJ_RECOVERY_LABEL:  /* fall through */
        case RSVP_OBJ_LABEL:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_1:
                while(obj_tlen >= 4 ) {
                    printf("\n\t    Label: %u", EXTRACT_32BITS(obj_tptr));
                    obj_tlen-=4;
                    obj_tptr+=4;
                }
                break;
            case RSVP_CTYPE_2:
                printf("\n\t    Generalized Label: %u",
                       EXTRACT_32BITS(obj_tptr));
                obj_tlen-=4;
                obj_tptr+=4;
                break;
            case RSVP_CTYPE_3:
                printf("\n\t    Waveband ID: %u\n\t    Start Label: %u, Stop Label: %u",
                       EXTRACT_32BITS(obj_tptr),
                       EXTRACT_32BITS(obj_tptr+4),
                       EXTRACT_32BITS(obj_tptr+8));
                obj_tlen-=12;
                obj_tptr+=12;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_STYLE:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_1:
                printf("\n\t    Reservation Style: %s, Flags: [0x%02x]",
                       tok2str(rsvp_resstyle_values,
                               "Unknown",
                               EXTRACT_24BITS(obj_tptr+1)),
                       *(obj_tptr));
                obj_tlen-=4;
                obj_tptr+=4;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_SENDER_TEMPLATE:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_IPV4:
                printf("\n\t    Source Address: %s, Source Port: %u",
                       ipaddr_string(obj_tptr),
                       EXTRACT_16BITS(obj_tptr+6));
                obj_tlen-=8;
                obj_tptr+=8;
                break;
#ifdef INET6
            case RSVP_CTYPE_IPV6:
                printf("\n\t    Source Address: %s, Source Port: %u",
                       ip6addr_string(obj_tptr),
                       EXTRACT_16BITS(obj_tptr+18));
                obj_tlen-=20;
                obj_tptr+=20;
                break;
#endif
            case RSVP_CTYPE_TUNNEL_IPV4:
                printf("\n\t    IPv4 Tunnel Sender Address: %s, LSP-ID: 0x%04x",
                       ipaddr_string(obj_tptr),
                       EXTRACT_16BITS(obj_tptr+6));
                obj_tlen-=8;
                obj_tptr+=8;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_LABEL_REQ:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_1:
                while(obj_tlen >= 4 ) {
                    printf("\n\t    L3 Protocol ID: %s",
                           tok2str(ethertype_values,
                                   "Unknown Protocol (0x%04x)",
                                   EXTRACT_16BITS(obj_tptr+2)));
                    obj_tlen-=4;
                    obj_tptr+=4;
                }
                break;
            case RSVP_CTYPE_2:
                printf("\n\t    L3 Protocol ID: %s",
                       tok2str(ethertype_values,
                               "Unknown Protocol (0x%04x)",
                               EXTRACT_16BITS(obj_tptr+2)));
                printf(",%s merge capability",((*(obj_tptr+4))&0x80) ? "no" : "" );
                printf("\n\t    Minimum VPI/VCI: %u/%u",
                       (EXTRACT_16BITS(obj_tptr+4))&0xfff,
                       (EXTRACT_16BITS(obj_tptr+6))&0xfff);
                printf("\n\t    Maximum VPI/VCI: %u/%u",
                       (EXTRACT_16BITS(obj_tptr+8))&0xfff,
                       (EXTRACT_16BITS(obj_tptr+10))&0xfff);
                obj_tlen-=12;
                obj_tptr+=12;
                break;
            case RSVP_CTYPE_3:
                printf("\n\t    L3 Protocol ID: %s",
                       tok2str(ethertype_values,
                               "Unknown Protocol (0x%04x)",
                               EXTRACT_16BITS(obj_tptr+2)));
                printf("\n\t    Minimum/Maximum DLCI: %u/%u, %s%s bit DLCI",
                       (EXTRACT_32BITS(obj_tptr+4))&0x7fffff,
                       (EXTRACT_32BITS(obj_tptr+8))&0x7fffff,
                       (((EXTRACT_16BITS(obj_tptr+4)>>7)&3) == 0 ) ? "10" : "",
                       (((EXTRACT_16BITS(obj_tptr+4)>>7)&3) == 2 ) ? "23" : "");
                obj_tlen-=12;
                obj_tptr+=12;
                break;
            case RSVP_CTYPE_4:
                printf("\n\t    LSP Encoding Type: %s (%u)",
                       tok2str(gmpls_encoding_values,
                               "Unknown",
                               *obj_tptr),
		       *obj_tptr);
                printf("\n\t    Switching Type: %s (%u), Payload ID: %s (0x%04x)",
                       tok2str(gmpls_switch_cap_values,
                               "Unknown",
                               *(obj_tptr+1)),
		       *(obj_tptr+1),
                       tok2str(gmpls_payload_values,
                               "Unknown",
                               EXTRACT_16BITS(obj_tptr+2)),
		       EXTRACT_16BITS(obj_tptr+2));
                obj_tlen-=8;
                obj_tptr+=8;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_RRO:
        case RSVP_OBJ_ERO:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_IPV4:
                while(obj_tlen >= 4 ) {
                    printf("\n\t    Subobject Type: %s",
                           tok2str(rsvp_obj_xro_values,
                                   "Unknown %u",
                                   RSVP_OBJ_XRO_MASK_SUBOBJ(*obj_tptr)));                
                    switch(RSVP_OBJ_XRO_MASK_SUBOBJ(*obj_tptr)) {
                    case RSVP_OBJ_XRO_IPV4:
                        printf(", %s, %s/%u, Flags: [%s]",
                               RSVP_OBJ_XRO_MASK_LOOSE(*obj_tptr) ? "Loose" : "Strict",
                               ipaddr_string(obj_tptr+2),
                               *(obj_tptr+6),
                               bittok2str(rsvp_obj_rro_flag_values,
                                   "none",
                                   *(obj_tptr+7))); /* rfc3209 says that this field is rsvd. */
                    }
                    obj_tlen-=*(obj_tptr+1);
                    obj_tptr+=*(obj_tptr+1);
                }
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_HELLO:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_1:
            case RSVP_CTYPE_2:
                printf("\n\t    Source Instance: 0x%08x, Destination Instance: 0x%08x",
                       EXTRACT_32BITS(obj_tptr),
                       EXTRACT_32BITS(obj_tptr+4));
                obj_tlen-=8;
                obj_tptr+=8;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_RESTART_CAPABILITY:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_1:
                printf("\n\t    Restart  Time: %ums, Recovery Time: %ums",
                       EXTRACT_16BITS(obj_tptr),
                       EXTRACT_16BITS(obj_tptr+4));
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_SESSION_ATTRIBUTE:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_TUNNEL_IPV4:
                printf("\n\t    Session Name: %s",(obj_tptr+4));
                printf("\n\t    Setup Priority: %u, Holding Priority: %u, Flags: [%s]",
                       (int)*obj_tptr,
                       (int)*(obj_tptr+1),
                       tok2str(rsvp_session_attribute_flag_values,
                                  "none",
                                  *(obj_tptr+2)));

                obj_tlen-=4+*(obj_tptr+3);
                obj_tptr+=4+*(obj_tptr+3);
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_RSVP_HOP:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_3: /* fall through - FIXME add TLV parser */
            case RSVP_CTYPE_IPV4:
                printf("\n\t    Previous/Next Interface: %s, Logical Interface Handle: 0x%08x",
                       ipaddr_string(obj_tptr),
                       EXTRACT_32BITS(obj_tptr+4));
                obj_tlen-=8;
                obj_tptr+=8;
                hexdump=TRUE; /* unless we have a TLV parser lets just hexdump */
                break;
#ifdef INET6
            case RSVP_CTYPE_4: /* fall through - FIXME add TLV parser */
            case RSVP_CTYPE_IPV6:
                printf("\n\t    Previous/Next Interface: %s, Logical Interface Handle: 0x%08x",
                       ip6addr_string(obj_tptr),
                       EXTRACT_32BITS(obj_tptr+16));
                obj_tlen-=20;
                obj_tptr+=20;
                hexdump=TRUE; /* unless we have a TLV parser lets just hexdump */
                break;
#endif
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_TIME_VALUES:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_1:
                printf("\n\t    Refresh Period: %ums",
                       EXTRACT_32BITS(obj_tptr));
                obj_tlen-=4;
                obj_tptr+=4;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        /* those three objects do share the same semantics */
        case RSVP_OBJ_SENDER_TSPEC:
        case RSVP_OBJ_ADSPEC:
        case RSVP_OBJ_FLOWSPEC:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_2:
                printf("\n\t    Msg-Version: %u, length: %u",
                       (*obj_tptr & 0xf0) >> 4,
                       EXTRACT_16BITS(obj_tptr+2)<<2);
                obj_tptr+=4; /* get to the start of the service header */
                obj_tlen-=4;

                while (obj_tlen >= 4) {
                    intserv_serv_tlen=EXTRACT_16BITS(obj_tptr+2)<<2;
                    printf("\n\t    Service Type: %s (%u), break bit %s set, Service length: %u",
                           tok2str(rsvp_intserv_service_type_values,"unknown",*(obj_tptr)),
                           *(obj_tptr),
                           (*(obj_tptr+1)&0x80) ? "" : "not",
                           intserv_serv_tlen);
                    
                    obj_tptr+=4; /* get to the start of the parameter list */
                    obj_tlen-=4;

                    while (intserv_serv_tlen>=4) {
                        processed = rsvp_intserv_print(obj_tptr);
                        if (processed == 0)
                            break;
                        obj_tlen-=processed;
                        intserv_serv_tlen-=processed;
                        obj_tptr+=processed;
                    }
                }
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_FILTERSPEC:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_IPV4:
                printf("\n\t    Source Address: %s, Source Port: %u",
                       ipaddr_string(obj_tptr),
                       EXTRACT_16BITS(obj_tptr+6));
                obj_tlen-=8;
                obj_tptr+=8;
                break;
#ifdef INET6
            case RSVP_CTYPE_IPV6:
                printf("\n\t    Source Address: %s, Source Port: %u",
                       ip6addr_string(obj_tptr),
                       EXTRACT_16BITS(obj_tptr+18));
                obj_tlen-=20;
                obj_tptr+=20;
                break;
            case RSVP_CTYPE_3:
                printf("\n\t    Source Address: %s, Flow Label: %u",
                       ip6addr_string(obj_tptr),
                       EXTRACT_24BITS(obj_tptr+17));
                obj_tlen-=20;
                obj_tptr+=20;
                break;
            case RSVP_CTYPE_TUNNEL_IPV6:
                printf("\n\t    Source Address: %s, LSP-ID: 0x%04x",
                       ipaddr_string(obj_tptr),
                       EXTRACT_16BITS(obj_tptr+18));
                obj_tlen-=20;
                obj_tptr+=20;
                break;
#endif
            case RSVP_CTYPE_TUNNEL_IPV4:
                printf("\n\t    Source Address: %s, LSP-ID: 0x%04x",
                       ipaddr_string(obj_tptr),
                       EXTRACT_16BITS(obj_tptr+6));
                obj_tlen-=8;
                obj_tptr+=8;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_FASTREROUTE:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_TUNNEL_IPV4:
                bw.i = EXTRACT_32BITS(obj_tptr+4);
                printf("\n\t    Setup Priority: %u, Holding Priority: %u, Hop-limit: %u, Bandwidth: %.10g Mbps",
                       (int)*obj_tptr,
                       (int)*(obj_tptr+1),
                       (int)*(obj_tptr+2),
                        bw.f*8/1000000);              
                printf("\n\t    Include Colors: 0x%08x, Exclude Colors: 0x%08x",
                       EXTRACT_32BITS(obj_tptr+8),
                       EXTRACT_32BITS(obj_tptr+12));
                obj_tlen-=16;
                obj_tptr+=16;
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_DETOUR:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_TUNNEL_IPV4:
                while(obj_tlen >= 8) {
                    printf("\n\t    PLR-ID: %s, Avoid-Node-ID: %s",
                           ipaddr_string(obj_tptr),
                           ipaddr_string(obj_tptr+4));              
                    obj_tlen-=8;
                    obj_tptr+=8;
                }
                break;
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_ERROR_SPEC:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_3: /* fall through - FIXME add TLV parser */
            case RSVP_CTYPE_IPV4:
                error_code=*(obj_tptr+5);
                error_value=EXTRACT_16BITS(obj_tptr+6);
                printf("\n\t    Error Node Adress: %s, Flags: [0x%02x]\n\t    Error Code: %s (%u)",
                       ipaddr_string(obj_tptr),
                       *(obj_tptr+4),
                       tok2str(rsvp_obj_error_code_values,"unknown",error_code),
                       error_code);
                switch (error_code) {
                case RSVP_OBJ_ERROR_SPEC_CODE_ROUTING:
                    printf(", Error Value: %s (%u)",
                           tok2str(rsvp_obj_error_code_routing_values,"unknown",error_value),
                           error_value);
                    break;
                default:
                    printf(", Unknown Error Value (%u)", error_value);
                    break;
                }
                break;
#ifdef INET6
            case RSVP_CTYPE_4: /* fall through - FIXME add TLV parser */
            case RSVP_CTYPE_IPV6:
                error_code=*(obj_tptr+17);
                error_value=EXTRACT_16BITS(obj_tptr+18);
                printf("\n\t    Error Node Adress: %s, Flags: [0x%02x]\n\t    Error Code: %s (%u)",
                       ip6addr_string(obj_tptr),
                       *(obj_tptr+16),
                       tok2str(rsvp_obj_error_code_values,"unknown",error_code),
                       error_code);

                switch (error_code) {
                case RSVP_OBJ_ERROR_SPEC_CODE_ROUTING:
                    printf(", Error Value: %s (%u)",
                           tok2str(rsvp_obj_error_code_routing_values,"unknown",error_value),
			   error_value);
                    break;
                default:
                    break;
                }

                break;
#endif
            default:
                hexdump=TRUE;
            }
            break;

        case RSVP_OBJ_PROPERTIES:
            switch(rsvp_obj_ctype) {
            case RSVP_CTYPE_1:
                padbytes = EXTRACT_16BITS(obj_tptr+2);
                printf("\n\t    TLV count: %u, padding bytes: %u",
                       EXTRACT_16BITS(obj_tptr),
                       padbytes);
                       obj_tlen-=4;
                       obj_tptr+=4;
                /* loop through as long there is anything longer than the TLV header (2) */
                while(obj_tlen >= 2 + padbytes) {
                    printf("\n\t      %s TLV (0x%02x), length: %u", /* length includes header */
                           tok2str(rsvp_obj_prop_tlv_values,"unknown",*obj_tptr),
                           *obj_tptr,
                           *(obj_tptr+1));
                    print_unknown_data(obj_tptr+2,"\n\t\t",*(obj_tptr+1)-2);
                    obj_tlen-=*(obj_tptr+1);
                    obj_tptr+=*(obj_tptr+1);
                }
                break;
            default:
                hexdump=TRUE;
            }
            break;

        /*
         *  FIXME those are the defined objects that lack a decoder
         *  you are welcome to contribute code ;-)
         */

        case RSVP_OBJ_INTEGRITY:
        case RSVP_OBJ_SCOPE:
        case RSVP_OBJ_POLICY_DATA:
        case RSVP_OBJ_MESSAGE_ID:
        case RSVP_OBJ_MESSAGE_ID_ACK:
        case RSVP_OBJ_MESSAGE_ID_LIST:
        case RSVP_OBJ_LABEL_SET:
        case RSVP_OBJ_ACCEPT_LABEL_SET:
        case RSVP_OBJ_PROTECTION:
        default:
            if (vflag <= 1)
                print_unknown_data(obj_tptr,"\n\t    ",obj_tlen);
            break;
        }
        /* do we want to see an additionally hexdump ? */
        if (vflag > 1 || hexdump==TRUE)
            print_unknown_data(tptr+sizeof(sizeof(struct rsvp_object_header)),"\n\t    ",
                               rsvp_obj_len-sizeof(struct rsvp_object_header));

        tptr+=rsvp_obj_len;
        tlen-=rsvp_obj_len;
    }
    return;
trunc:
    printf("\n\t\t packet exceeded snapshot");
}
