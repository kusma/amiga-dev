/*
 * Copyright (C) 2001 WIDE Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef lint
static const char rcsid[] _U_ =
    "@(#) $Header: /Users/olsen/Code/cvs2svn-2010/os4-cvs-repository/contrib/roadshow/client-software/tcpdump-3.8.1/print-mpls.c,v 1.2 2004-09-22 15:40:56 olsen Exp $ (LBL)";
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tcpdump-stdinc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "addrtoname.h"
#include "interface.h"
#include "extract.h"			/* must come after interface.h */

#define LABEL_MASK	0xfffff000
#define LABEL_SHIFT	12
#define	EXP_MASK	0x00000e00
#define EXP_SHIFT	9
#define	STACK_MASK	0x00000100
#define STACK_SHIFT	8
#define TTL_MASK	0x000000ff
#define TTL_SHIFT	0

#define MPLS_LABEL(x)	(((x) & LABEL_MASK) >> LABEL_SHIFT)
#define MPLS_EXP(x)	(((x) & EXP_MASK) >> EXP_SHIFT)
#define MPLS_STACK(x)	(((x) & STACK_MASK) >> STACK_SHIFT)
#define MPLS_TTL(x)	(((x) & TTL_MASK) >> TTL_SHIFT)

static const char *mpls_labelname[] = {
/*0*/	"IPv4 explicit NULL", "router alert", "IPv6 explicit NULL",
	"implicit NULL", "rsvd",
/*5*/	"rsvd", "rsvd", "rsvd", "rsvd", "rsvd",
/*10*/	"rsvd", "rsvd", "rsvd", "rsvd", "rsvd",
/*15*/	"rsvd",
};

/*
 * RFC3032: MPLS label stack encoding
 */
void
mpls_print(const u_char *bp, u_int length)
{
	const u_char *p;
	u_int32_t v;

	p = bp;
	printf("MPLS");
	do {
		TCHECK2(*p, sizeof(v));
		v = EXTRACT_32BITS(p);
		printf(" (");	/*)*/
		printf("label %u", MPLS_LABEL(v));
		if (vflag &&
		    MPLS_LABEL(v) < sizeof(mpls_labelname) / sizeof(mpls_labelname[0]))
			printf(" (%s)", mpls_labelname[MPLS_LABEL(v)]);
		printf(", exp %u", MPLS_EXP(v));
		if (MPLS_STACK(v))
			printf(", [S]");
		printf(", ttl %u", MPLS_TTL(v));
		/*(*/
		printf(")");

		p += sizeof(v);
	} while (!MPLS_STACK(v));

	switch (MPLS_LABEL(v)) {
	case 0:	/* IPv4 explicit NULL label */
        case 3:	/* IPv4 implicit NULL label */
		ip_print(p, length - (p - bp));
		break;
#ifdef INET6
	case 2:	/* IPv6 explicit NULL label */
		ip6_print(p, length - (p - bp));
		break;
#endif
	default:
		/*
		 * Generally there's no indication of protocol in MPLS label
		 * encoding, however draft-hsmit-isis-aal5mux-00.txt describes
                 * a technique that looks at the first payload byte if the BOS (bottom of stack)
                 * bit is set and tries to determine the network layer protocol
                 * 0x45-0x4f is IPv4
                 * 0x60-0x6f is IPv6
                 * 0x81-0x83 is OSI (CLNP,ES-IS,IS-IS)
                 * this technique is sometimes known as NULL encapsulation
                 * and decoding is particularly useful for control-plane traffic [BGP]
                 * which cisco by default sends MPLS encapsulated
		 */

                if (MPLS_STACK(v)) { /* only do this if the stack bit is set */
                    switch(*p) {
                    case 0x45:
                    case 0x46:
                    case 0x47:
                    case 0x48:
                    case 0x49:
                    case 0x4a:
                    case 0x4b:
                    case 0x4c:
                    case 0x4d:
                    case 0x4e:
                    case 0x4f:
		        if (vflag>0) {
                            printf("\n\t");
                            ip_print(p, length - (p - bp));
			    }
                        else printf(", IP, length: %u",length);
                        break;
#ifdef INET6
                    case 0x60:
                    case 0x61:
                    case 0x62:
                    case 0x63:
                    case 0x64:
                    case 0x65:
                    case 0x66:
                    case 0x67:
                    case 0x68:
                    case 0x69:
                    case 0x6a:
                    case 0x6b:
                    case 0x6c:
                    case 0x6d:
                    case 0x6e:
                    case 0x6f:
		        if (vflag>0) {
                            printf("\n\t");
                            ip6_print(p, length - (p - bp));
                            }
			else printf(", IPv6, length: %u",length);
                        break;
#endif
                    case 0x81:
                    case 0x82:
                    case 0x83:
		        if (vflag>0) {
                            printf("\n\t");
                            isoclns_print(p, length - (p - bp), length - (p - bp));
			    }
			else printf(", OSI, length: %u",length);
                        break;
                    default:
                        /* ok bail out - we did not figure out what it is*/
                        break;
                    }
                }
                return;
	}

trunc:
	printf("[|MPLS]");
}

/*
 * draft-ietf-mpls-lsp-ping-02.txt
 */
void
mpls_lsp_ping_print(const u_char *pptr, u_int length)
{
    printf("UDP, LSP-PING, length: %u", length);
    if (vflag >1)
	print_unknown_data(pptr,"\n\t  ", length);
}
