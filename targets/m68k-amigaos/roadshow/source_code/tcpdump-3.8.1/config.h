/* config.h.  Generated automatically by configure.  */
/* config.h.in.  Generated automatically from configure.in by autoheader.  */
/* "generated automatically" means DO NOT MAKE CHANGES TO config.h.in --
 * make them to acconfig.h and rerun autoheader */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE void

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#define TIME_WITH_SYS_TIME 1

/* Define if you enable IPv6 support */
/* #undef INET6 */

/* Define if you enable support for the libsmi. */
/* #undef LIBSMI */

/* define if you have struct __res_state_ext */
/* #undef HAVE_RES_STATE_EXT */

/* define if your struct __res_state has the nsort member */
/* #undef HAVE_NEW_RES_STATE */

/* define if you have the addrinfo function. */
/* #undef HAVE_ADDRINFO */

/* define if you need to include missing/addrinfoh.h. */
#define NEED_ADDRINFO_H 1

/* define ifyou have the h_errno variable. */
/* #undef HAVE_H_ERRNO */

/* define if IN6ADDRSZ is defined (XXX not used!) */
/* #undef HAVE_IN6ADDRSZ */

/* define if INADDRSZ is defined (XXX not used!) */
/* #undef HAVE_INADDRSZ */

/* define if this is a development version, to use additional prototypes. */
/* #undef HAVE_OS_PROTO_H */

/* define if RES_USE_INET6 is defined */
/* #undef HAVE_RES_USE_INET6 */

/* define if struct sockaddr has the sa_len member */
#define HAVE_SOCKADDR_SA_LEN 1

/* define if you have struct sockaddr_storage */
/* #undef HAVE_SOCKADDR_STORAGE */

/* define if you have ether_ntohost() and it works */
/* #undef USE_ETHER_NTOHOST */

/* define if libpcap has pcap_version */
#define HAVE_PCAP_VERSION 1

/* define if libpcap has pcap_debug */
/* #undef HAVE_PCAP_DEBUG */

/* define if libpcap has yydebug */
/* #undef HAVE_YYDEBUG */

/* define if libpcap has pcap_list_datalinks() */
#define HAVE_PCAP_LIST_DATALINKS 1

/* define if libpcap has pcap_set_datalink() */
#define HAVE_PCAP_SET_DATALINK 

/* define if libpcap has pcap_datalink_name_to_val() */
#define HAVE_PCAP_DATALINK_NAME_TO_VAL 1

/* define if libpcap has pcap_datalink_val_to_description() */
#define HAVE_PCAP_DATALINK_VAL_TO_DESCRIPTION 1

/* define if unaligned memory accesses fail */
#define LBL_ALIGN 1

/* The successful return value from signal (?)XXX */
#define RETSIGVAL 

/* Define this on IRIX */
/* #undef _BSD_SIGNALS */

/* For HP/UX ANSI compiler? */
/* #undef _HPUX_SOURCE */

/* AIX hack. */
/* #undef _SUN */

/* Workaround for missing sized types */
/* XXX this should move to the more standard uint*_t */
#define int8_t signed char
#define int16_t short
#define int32_t int
#define u_int16_t u_short
#define u_int32_t u_int
#define u_int8_t u_char

/* Whether or not to include the possibly-buggy SMB printer */
#define TCPDUMP_DO_SMB 1

/* Long story short: aclocal.m4 depends on autoconf 2.13
 * implementation details wrt "const"; newer versions
 * have different implementation details so for now we
 * put "const" here.  This may cause duplicate definitions
 * in config.h but that should be OK since they're the same.
 */
/* #undef const */

/* Define if you have the dnet_htoa function.  */
/* #undef HAVE_DNET_HTOA */

/* Define if you have a dnet_htoa declaration in <netdnet/dnetdb.h>.  */
/* #undef HAVE_NETDNET_DNETDB_H_DNET_HTOA */

/* The number of bytes in a char.  */
#define SIZEOF_CHAR 1

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* The number of bytes in a long.  */
#define SIZEOF_LONG 4

/* The number of bytes in a short.  */
#define SIZEOF_SHORT 2

/* Define if you have the bpf_dump function.  */
#define HAVE_BPF_DUMP 1

/* Define if you have the ether_ntohost function.  */
/* #undef HAVE_ETHER_NTOHOST */

/* Define if you have the getaddrinfo function.  */
/* #undef HAVE_GETADDRINFO */

/* Define if you have the getnameinfo function.  */
/* #undef HAVE_GETNAMEINFO */

/* Define if you have the pcap_breakloop function.  */
#define HAVE_PCAP_BREAKLOOP 1

/* Define if you have the pcap_dump_flush function.  */
#define HAVE_PCAP_DUMP_FLUSH 1

/* Define if you have the pcap_findalldevs function.  */
#define HAVE_PCAP_FINDALLDEVS 1

/* Define if you have the pcap_lib_version function.  */
#define HAVE_PCAP_LIB_VERSION 1

/* Define if you have the pfopen function.  */
/* #undef HAVE_PFOPEN */

/* Define if you have the setlinebuf function.  */
/* #undef HAVE_SETLINEBUF */

/* Define if you have the sigaction function.  */
/* #undef HAVE_SIGACTION */

/* Define if you have the sigset function.  */
/* #undef HAVE_SIGSET */

/* Define if you have the snprintf function.  */
#define HAVE_SNPRINTF 1

/* Define if you have the strcasecmp function.  */
#define HAVE_STRCASECMP 1

/* Define if you have the strdup function.  */
#define HAVE_STRDUP 1

/* Define if you have the strftime function.  */
#define HAVE_STRFTIME 1

/* Define if you have the strlcat function.  */
/* #undef HAVE_STRLCAT */

/* Define if you have the strlcpy function.  */
/* #undef HAVE_STRLCPY */

/* Define if you have the strsep function.  */
/* #undef HAVE_STRSEP */

/* Define if you have the vfprintf function.  */
#define HAVE_VFPRINTF 1

/* Define if you have the vsnprintf function.  */
#define HAVE_VSNPRINTF 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <netdnet/dnetdb.h> header file.  */
/* #undef HAVE_NETDNET_DNETDB_H */

/* Define if you have the <netinet/ether.h> header file.  */
/* #undef HAVE_NETINET_ETHER_H */

/* Define if you have the <netinet/if_ether.h> header file.  */
#define HAVE_NETINET_IF_ETHER_H 1

/* Define if you have the <openssl/evp.h> header file.  */
/* #undef HAVE_OPENSSL_EVP_H */

/* Define if you have the <rpc/rpcent.h> header file.  */
/* #undef HAVE_RPC_RPCENT_H */

/* Define if you have the <smi.h> header file.  */
/* #undef HAVE_SMI_H */

/* Define if you have the crypto library (-lcrypto).  */
/* #undef HAVE_LIBCRYPTO */

/* Define if you have the rpc library (-lrpc).  */
/* #undef HAVE_LIBRPC */

/* Define if you have the smi library (-lsmi).  */
/* #undef HAVE_LIBSMI */

/* Define as token for inline if inlining supported */
#define inline inline

/* define if your compiler has __attribute__ */
#define HAVE___ATTRIBUTE__ 1

