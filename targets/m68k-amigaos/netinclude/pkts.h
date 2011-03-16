/*
 * $Id: pkts.h,v 1.1 1996/01/21 01:14:57 jraja Exp $
 *
 * Simplified double buffered AmigaDOS I/O.
 *
 * Author: Tomi Ollila <Tomi.Ollila@nsdi.fi>
 *
 * 	Copyright (c) 1994 Network Solutions Development Inc.
 *			All rights reserved
 *
 * HISTORY 
 * $Log: pkts.h,v $
 * Revision 1.1  1996/01/21 01:14:57  jraja
 * Initial revision
 *
 */

#ifndef PKTS_H
#define PKTS_H

struct pktinfo {
  struct MsgPort * fhType;
  struct MsgPort * port;
  struct DosPacket * pkt;
  char * bufs[2];
  int    whichbuf;
};

int initPktInfo(struct pktinfo * pkti, int fd,
		char * buf1, char * buf2, int size);
int deInitPktInfo(struct pktinfo * pkti);
int readPkt(struct pktinfo * pkti, char ** buf);
int writePkt(struct pktinfo * pkti, char ** buf, int len);

#endif /*  PKTS_H */
