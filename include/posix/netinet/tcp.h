/*
 * AmiTCP/IP compatibility headers for PosixLib
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2007
 *
 * $Id: tcp.h,v 1.1 2007/10/10 19:25:24 phx Exp $
 */

#ifndef _NETINET_TCP_H_
#define _NETINET_TCP_H_

typedef u_long  tcp_seq;

/*
 * TCP header.
 * Per RFC 793, September, 1981.
 */
#pragma dontwarn 51
struct tcphdr {
  u_short th_sport;   /* source port */
  u_short th_dport;   /* destination port */
  tcp_seq th_seq;     /* sequence number */
  tcp_seq th_ack;     /* acknowledgement number */
  u_char  th_off:4;   /* data offset */
  u_char  th_x2:4;    /* (unused) */
  u_char  th_flags;
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20
  u_short th_win;     /* window */
  u_short th_sum;     /* checksum */
  u_short th_urp;     /* urgent pointer */
};
#pragma popwarn

#define TCPOPT_EOL  0
#define TCPOPT_NOP  1
#define TCPOPT_MAXSEG 2

/*
 * Default maximum segment size for TCP.
 * With an IP MSS of 576, this is 536,
 * but 512 is probably more convenient.
 * This should be defined as MIN(512, IP_MSS - sizeof (struct tcpiphdr)).
 */
#define TCP_MSS 512

#define TCP_MAXWIN  65535   /* largest value for window */

/*
 * User-settable options (used with setsockopt).
 */
#define TCP_NODELAY 0x01    /* don't delay send to coalesce packets */
#define TCP_MAXSEG  0x02    /* set maximum segment size */

#endif /* _NETINET_TCP_H_ */
