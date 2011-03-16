#ifndef NET_SANA2ERRNO_H
#define NET_SANA2ERRNO_H \
       "$Id: sana2errno.h,v 4.1 1994/10/05 23:16:26 ppessi Exp $"
/*
 *      SANA-II error lists and printing
 *
 *      Copyright © 1994 AmiTCP/IP Group,
 *                       Network Solutions Development, Inc.
 *                       All rights reserved.
 */

extern const short io_nerr;
extern const char * const io_errlist[]; 
extern const short sana2io_nerr;
extern const char * const sana2io_errlist[];
extern const short sana2wire_nerr;
extern const char * const sana2wire_errlist[];

void sana2perror(const char *banner, struct IOSana2Req *ios2);
void Sana2PrintFault(const char *banner, struct IOSana2Req *ios2);

#endif
