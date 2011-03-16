/*
 * AmiTCP/IP compatibility headers for PosixLib
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2005
 *
 * $Id: radix.h,v 1.1 2005/04/23 22:10:21 phx Exp $
 */

#ifndef _NET_RADIX_H_
#define	_NET_RADIX_H_

/*
 * Radix search tree node layout.
 */

struct radix_node {
        struct  radix_mask *rn_mklist;  /* list of masks contained in subtree */
        struct  radix_node *rn_p;       /* parent */
        short   rn_b;                   /* bit offset; -1-index(netmask) */
        char    rn_bmask;               /* node: mask for bit test*/
        u_char  rn_flags;               /* enumerated next */
#define RNF_NORMAL      1               /* leaf contains normal route */
#define RNF_ROOT        2               /* leaf is root leaf for tree */
#define RNF_ACTIVE      4               /* This node is alive (for rtfree) */
        union {
                struct {                        /* leaf only data: */
                        caddr_t rn_Key;         /* object of search */
                        caddr_t rn_Mask;        /* netmask, if present */
                        struct  radix_node *rn_Dupedkey;
                } rn_leaf;
                struct {                        /* node only data: */
                        int     rn_Off;         /* where to start compare */
                        struct  radix_node *rn_L;/* progeny */
                        struct  radix_node *rn_R;/* progeny */
                } rn_node;
        } rn_u;
};

#define rn_dupedkey rn_u.rn_leaf.rn_Dupedkey
#define rn_key rn_u.rn_leaf.rn_Key
#define rn_mask rn_u.rn_leaf.rn_Mask
#define rn_off rn_u.rn_node.rn_Off
#define rn_l rn_u.rn_node.rn_L
#define rn_r rn_u.rn_node.rn_R

/*
 * Annotations to tree concerning potential routes applying to subtrees.
 */

struct radix_mask {
        short   rm_b;                   /* bit offset; -1-index(netmask) */
        char    rm_unused;              /* cf. rn_bmask */
        u_char  rm_flags;               /* cf. rn_flags */
        struct  radix_mask *rm_mklist;  /* more masks to try */
        union   {
                caddr_t rmu_mask;               /* the mask */
                struct  radix_node *rmu_leaf;   /* for normal routes */
        } rm_rmu;
        int     rm_refs;                /* # of references to this struct */
};

#define rm_mask rm_rmu.rmu_mask
#define rm_leaf rm_rmu.rmu_leaf		/* extra field would make 32 bytes */

struct radix_node_head {
        struct  radix_node *rnh_treetop;
        int     rnh_addrsize;           /* permit, but not require fixed keys */
        int     rnh_pktsize;            /* permit, but not require fixed keys */
        struct  radix_node *(*rnh_addaddr)      /* add based on sockaddr */
                (void *v, void *mask,
                     struct radix_node_head *head, struct radix_node nodes[]);
        struct  radix_node *(*rnh_addpkt)       /* add based on packet hdr */
                (void *v, void *mask,
                     struct radix_node_head *head, struct radix_node nodes[]);
        struct  radix_node *(*rnh_deladdr)      /* remove based on sockaddr */
                (void *v, void *mask, struct radix_node_head *head);
        struct  radix_node *(*rnh_delpkt)       /* remove based on packet hdr */
                (void *v, void *mask, struct radix_node_head *head);
        struct  radix_node *(*rnh_matchaddr)    /* locate based on sockaddr */
                (void *v, struct radix_node_head *head);
        struct  radix_node *(*rnh_lookup)       /* locate based on sockaddr */
                (void *v, void *mask, struct radix_node_head *head);
        struct  radix_node *(*rnh_matchpkt)     /* locate based on packet hdr */
                (void *v, struct radix_node_head *head);
        int     (*rnh_walktree)                 /* traverse tree */
                (struct radix_node_head *,
                     int (*)(struct radix_node *, void *), void *);
        struct  radix_node rnh_nodes[3];        /* empty tree for common case */
};


#endif /* _NET_RADIX_H_ */
