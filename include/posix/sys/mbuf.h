/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: mbuf.h,v 1.1 2005/04/23 22:10:21 phx Exp $
 */

#ifndef _SYS_MBUF_H_
#define _SYS_MBUF_H_

#ifndef _SYS_TYPES_H_
#include <sys/types.h>
#endif
#ifndef _MACHINE_PARAM_H_
#include <machine/param.h>
#endif

/*
 * Mbufs are of a single size, MSIZE (machine/param.h), which
 * includes overhead.  An mbuf may add a single "mbuf cluster" of size
 * MCLBYTES (also in machine/param.h), which has no additional overhead
 * and is used instead of the internal data area; this is done when
 * at least MINCLSIZE of data must be stored.
 */

#define MLEN            (MSIZE - sizeof(struct m_hdr))  /* normal data len */
#define MHLEN           (MLEN - sizeof(struct pkthdr))  /* data len w/pkthdr */

#define MINCLSIZE       (MHLEN+MLEN+1)  /* smallest amount to put in cluster */
#define M_MAXCOMPRESS   (MHLEN / 2)     /* max amount to copy for compression *

/*
 * Macros for type conversion
 * mtod(m,t) -  convert mbuf pointer to data pointer of correct type
 */
#define mtod(m,t)       ((t)((m)->m_data))

/* header at beginning of each mbuf: */
struct m_hdr {
        struct  mbuf *mh_next;          /* next buffer in chain */
        struct  mbuf *mh_nextpkt;       /* next chain in queue/record */
        caddr_t mh_data;                /* location of data */
        int     mh_len;                 /* amount of data in this mbuf */
        short   mh_type;                /* type of data in this mbuf */
        short   mh_flags;               /* flags; see below */
};

/*
 * record/packet header in first mbuf of chain; valid if M_PKTHDR set
 *
 * A note about csum_data: For the out-bound direction, this indicates the
 * offset after the L4 header where the final L4 checksum value is to be
 * stored.  For the in-bound direction, it is only valid if the M_CSUM_DATA
 * flag is set.  In this case, an L4 checksum has been calculated by
 * hardware, but it is up to software to perform final verification.
 *
 * Note for in-bound TCP/UDP checksums, we expect the csum_data to NOT
 * be bit-wise inverted (the final step in the calculation of an IP
 * checksum) -- this is so we can accumulate the checksum for fragmented
 * packets during reassembly.
 */
struct  pkthdr {
        struct  ifnet *rcvif;           /* rcv interface */
        int     len;                    /* total packet length */
};

/* description of external storage mapped into mbuf, valid if M_EXT set */
struct m_ext {
        caddr_t ext_buf;                /* start of buffer */
        void    (*ext_free)             /* free routine if not the usual */
                (struct mbuf *, caddr_t, u_int, void *);
        u_int   ext_size;               /* size of buffer, for ext_free */
};

struct mbuf {
        struct  m_hdr m_hdr;
        union {
                struct {
                        struct  pkthdr MH_pkthdr;       /* M_PKTHDR set */
                        union {
                                struct  m_ext MH_ext;   /* M_EXT set */
                                char    MH_databuf[MHLEN];
                        } MH_dat;
                } MH;
                char    M_databuf[MLEN];                /* !M_PKTHDR, !M_EXT */
        } M_dat;
};

#define m_next          m_hdr.mh_next
#define m_len           m_hdr.mh_len
#define m_data          m_hdr.mh_data
#define m_type          m_hdr.mh_type
#define m_flags         m_hdr.mh_flags
#define m_nextpkt       m_hdr.mh_nextpkt
#define m_pkthdr        M_dat.MH.MH_pkthdr
#define m_ext           M_dat.MH.MH_dat.MH_ext
#define m_pktdat        M_dat.MH.MH_dat.MH_databuf
#define m_dat           M_dat.M_databuf

/* mbuf flags */
#define M_EXT           0x0001  /* has associated external storage */
#define M_PKTHDR        0x0002  /* start of record */
#define M_EOR           0x0004  /* end of record */

/* mbuf pkthdr flags, also in m_flags */
#define M_BCAST         0x0100  /* send/received as link-level broadcast */
#define M_MCAST         0x0200  /* send/received as link-level multicast */

/* flags copied when copying m_pkthdr */
#define M_COPYFLAGS     (M_PKTHDR|M_EOR|M_BCAST|M_MCAST)

/* length to m_copy to copy all */
#define	M_COPYALL       1000000000

/*
 * Mbuf statistics.
 * For statistics related to mbuf and cluster allocations, see also the
 * pool headers (mbpool and mclpool).
 */
struct mbstat {
        u_long  m_mbufs;        /* formerly m_mbufs */
        u_long  m_clusters;     /* formerly m_clusters */
        u_long  m_spare;        /* spare field */
        u_long  m_clfree;       /* formely m_clfree - free clusters */
        u_long  m_drops;        /* times failed to find space */
        u_long  m_wait;         /* times waited for space */
        u_long  m_drain;        /* times drained protocols for space */
};


#endif  /* _SYS_MBUF_H_ */
