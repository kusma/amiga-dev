/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: unistd.h,v 1.2 2005/02/27 13:50:47 phx Exp $
 */

#ifndef _SYS_UNISTD_H_
#define _SYS_UNISTD_H_

/* access function */
#define F_OK            0       /* test for existence of file */
#define X_OK            0x01    /* test for execute or search permission */
#define W_OK            0x02    /* test for write permission */
#define R_OK            0x04    /* test for read permission */

/* whence values for lseek(2) */
#ifdef __VBCC__
#define SEEK_SET      (-1)      /* set file offset to offset */
#define SEEK_CUR        0       /* set file offset to current plus offset */
#define SEEK_END        1       /* set file offset to EOF plus offset */
#else
#define SEEK_SET        0       /* set file offset to offset */
#define SEEK_CUR        1       /* set file offset to current plus offset */
#define SEEK_END        2       /* set file offset to EOF plus offset */
#endif

#endif  /* _SYS_UNISTD_H_ */
