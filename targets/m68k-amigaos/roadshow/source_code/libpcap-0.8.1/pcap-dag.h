/*
 * pcap-dag.c: Packet capture interface for Endace DAG card.
 *
 * The functionality of this code attempts to mimic that of pcap-linux as much
 * as possible.  This code is only needed when compiling in the DAG card code
 * at the same time as another type of device.
 *
 * Author: Richard Littin, Sean Irvine ({richard,sean}@reeltwo.com)
 *
 * @(#) $Header: /Users/olsen/Code/cvs2svn-2010/os4-cvs-repository/contrib/roadshow/client-software/libpcap-0.8.1/pcap-dag.h,v 1.2 2004-09-22 15:28:25 olsen Exp $ (LBL)
 */

pcap_t *dag_open_live(const char *device, int snaplen, int promisc, int to_ms, char *ebuf);

