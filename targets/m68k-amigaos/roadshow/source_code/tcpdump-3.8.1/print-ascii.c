/*	$NetBSD: print-ascii.c,v 1.1 1999/09/30 14:49:12 sjg Exp $ 	*/

/*-
 * Copyright (c) 1997, 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Alan Barrett and Simon J. Gerraty.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef lint
static const char rcsid[] _U_ =
     "@(#) $Header: /Users/olsen/Code/cvs2svn-2010/os4-cvs-repository/contrib/roadshow/client-software/tcpdump-3.8.1/print-ascii.c,v 1.2 2004-09-22 15:40:55 olsen Exp $";
#endif
#include <tcpdump-stdinc.h>
#include <stdio.h>

#include "interface.h"

#define ASCII_LINELENGTH 300
#define HEXDUMP_BYTES_PER_LINE 16
#define HEXDUMP_SHORTS_PER_LINE (HEXDUMP_BYTES_PER_LINE / 2)
#define HEXDUMP_HEXSTUFF_PER_SHORT 5 /* 4 hex digits and a space */
#define HEXDUMP_HEXSTUFF_PER_LINE \
		(HEXDUMP_HEXSTUFF_PER_SHORT * HEXDUMP_SHORTS_PER_LINE)

void
ascii_print_with_offset(register const u_char *cp, register u_int length,
			register u_int oset)
{
	register u_int i;
	register int s1, s2;
	register int nshorts;
	char hexstuff[HEXDUMP_SHORTS_PER_LINE*HEXDUMP_HEXSTUFF_PER_SHORT+1], *hsp;
	char asciistuff[ASCII_LINELENGTH+1], *asp;
	u_int maxlength = (Aflag ? ASCII_LINELENGTH : HEXDUMP_SHORTS_PER_LINE);

	nshorts = length / sizeof(u_short);
	i = 0;
	hsp = hexstuff; asp = asciistuff;
	if (Aflag) *(asp++) = '\n';
	while (--nshorts >= 0) {
		s1 = *cp++;
		s2 = *cp++;
		if (Aflag) {
			i += 2;
			*(asp++) = (isgraph(s1) ? s1 : (s1 != '\t' && s1 != ' ' && s1 != '\n' && s1 != '\r' ? '.' : s1) );
			*(asp++) = (isgraph(s2) ? s2 : (s2 != '\t' && s2 != ' ' && s2 != '\n' && s2 != '\r' ? '.' : s2) );
			if (s1 == '\n' || s2 == '\n') i = maxlength;

		} else {
			(void)snprintf(hsp, sizeof(hexstuff) - (hsp - hexstuff),
			    " %02x%02x", s1, s2);
			hsp += HEXDUMP_HEXSTUFF_PER_SHORT;
			*(asp++) = (isgraph(s1) ? s1 : '.');
			*(asp++) = (isgraph(s2) ? s2 : '.');
			i++;
		}
		if (i >= maxlength) {
			*hsp = *asp = '\0';
			if (Aflag) {
				(void)printf("%s", asciistuff);
			} else {
				(void)printf("\n0x%04x\t%-*s\t%s",
				    oset, HEXDUMP_HEXSTUFF_PER_LINE,
				    hexstuff, asciistuff);
			}
			i = 0; hsp = hexstuff; asp = asciistuff;
			oset += HEXDUMP_BYTES_PER_LINE;
		}
	}
	if (length & 1) {
		s1 = *cp++;
		if (Aflag) {
			*(asp++) = (isgraph(s1) ? s1 : (s1 != '\t' && s1 != ' ' && s1 != '\n' && s1 != '\r' ? '.' : s1) );
		} else {
			(void)snprintf(hsp, sizeof(hexstuff) - (hsp - hexstuff),
			    " %02x", s1);
			hsp += 3;
			*(asp++) = (isgraph(s1) ? s1 : '.');
		}
		++i;
	}
	if (i > 0) {
		*hsp = *asp = '\0';
		if (Aflag) {
			(void)printf("\n%s", asciistuff);
		} else {
			(void)printf("\n0x%04x\t%-*s\t%s",
			     oset, HEXDUMP_HEXSTUFF_PER_LINE,
			     hexstuff, asciistuff);
		}
	}
}

void
ascii_print(register const u_char *cp, register u_int length)
{
	ascii_print_with_offset(cp, length, 0);
}

/*
 * telnet_print() wants this.  It is essentially default_print_unaligned()
 */
void
hex_print_with_offset(register const u_char *cp, register u_int length,
		      register u_int oset)
{
	register u_int i, s;
	register int nshorts;

	nshorts = (u_int) length / sizeof(u_short);
	i = 0;
	while (--nshorts >= 0) {
		if ((i++ % 8) == 0) {
			(void)printf("\n0x%04x\t", oset);
			oset += HEXDUMP_BYTES_PER_LINE;
		}
		s = *cp++;
		(void)printf(" %02x%02x", s, *cp++);
	}
	if (length & 1) {
		if ((i % 8) == 0)
			(void)printf("\n0x%04x\t", oset);
		(void)printf(" %02x", *cp);
	}
}

/*
 * just for completeness
 */
void
hex_print(register const u_char *cp, register u_int length)
{
	hex_print_with_offset(cp, length, 0);
}

#ifdef MAIN
int
main(int argc, char *argv[])
{
	hex_print("Hello, World!\n", 14);
	printf("\n");
	ascii_print("Hello, World!\n", 14);
	printf("\n");
#define TMSG "Now is the winter of our discontent...\n"
	ascii_print_with_offset(TMSG, sizeof(TMSG) - 1, 0x100);
	printf("\n");
	exit(0);
}
#endif /* MAIN */
