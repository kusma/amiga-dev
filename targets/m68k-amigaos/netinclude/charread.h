#ifndef CHARREAD_H
#define CHARREAD_H \
       "$Id: charread.h,v 4.2 1994/10/14 17:10:35 too Exp $"
/*
 *      Macros for efficient interface to socket receiving
 *
 *      Copyright © 1994 AmiTCP/IP Group, Network Solutions Development, Inc.
 *                       All rights reserved.
 */


#ifndef RC_BUFSIZE
#define RC_BUFSIZE 1024
#endif

struct CharRead {
  int	rc_Fd;
  int	rc_Size;
  int	rc_Curr;
  unsigned char	rc_Buffer[RC_BUFSIZE];
};

#define RC_DO_SELECT	-3
#define RC_EOF		-2
#define RC_ERROR	-1

#define initCharRead(rc, fd) do { \
				  (rc)->rc_Fd = fd;  \
			          (rc)->rc_Size = 0; \
				  (rc)->rc_Curr = 1; \
			     } while(0)

#ifdef AMIGA
#define RC_R_E_A_D(a, b, c) recv(a, b, c, 0)
#else
#define RC_R_E_A_D(a, b, c) read(a, b, c)
#endif

#define charRead(rc) \
	((rc)->rc_Curr >= (rc)->rc_Size ? \
		(rc)->rc_Curr++ == (rc)->rc_Size ? \
			RC_DO_SELECT: \
			((rc)->rc_Size = RC_R_E_A_D((rc)->rc_Fd, \
						    (rc)->rc_Buffer, \
						    RC_BUFSIZE)) <= 0 ? \
				(rc)->rc_Size == 0 ? \
					RC_EOF:	\
					RC_ERROR: \
				(rc)->rc_Buffer[((rc)->rc_Curr = 1) - 1]: \
	        (rc)->rc_Buffer[(rc)->rc_Curr++])

#endif /* _CHARREAD_H_ */
