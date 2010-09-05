/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: dirent.h,v 1.2 2005/02/27 13:50:46 phx Exp $
 */

#ifndef _SYS_DIRENT_H_
#define _SYS_DIRENT_H_

struct dirent {
    u_int32_t d_fileno;         /* file number of entry */
    u_int16_t d_reclen;         /* length of this record */
    u_int8_t  d_type;           /* file type, see below */
    u_int8_t  d_namlen;         /* length of string in d_name */
    char      d_name[255 + 1];  /* name must be no longer than this */
};

/*
 * File types
 */
#define DT_UNKNOWN       0
#define DT_FIFO          1
#define DT_CHR           2
#define DT_DIR           4
#define DT_BLK           6
#define DT_REG           8
#define DT_LNK          10
#define DT_SOCK         12
#define DT_WHT          14

#endif /* _SYS_DIRENT_H_ */
