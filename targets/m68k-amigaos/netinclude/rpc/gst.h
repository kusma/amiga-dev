/*
 * $Id: gst.h,v 4.1 1994/09/26 08:09:11 jraja Exp jraja $
 *
 * Include file for the SAS/C Global Symbol Table generation
 *
 * Copyright © 1994 AmiTCP/IP Group,
 *                  Network Solutions Development Inc.
 *                  All rights reserved.
 *
 */
struct sockaddr_in;

#include <sys/param.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
/* #include <sys/socket.h> \* includes inline functions */
#include <sys/time.h>
/* #include <netdb.h> \* includes inline functions */
#include <errno.h>
/* #include <arpa/inet.h> \* includes inline functions */
/* #include <net/if.h> \* depends on sys/socket.h */
#include <proto/usergroup.h>
#include <pragmas/socket_pragmas.h>

#include <rpc/rpc.h>
#include <rpc/pmap_prot.h>
#include <rpc/pmap_clnt.h>
#include <rpc/pmap_rmt.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <unistd.h>
