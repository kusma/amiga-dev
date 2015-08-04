/*
 * $Id$
 *
 * :ts=4
 *
 * Copyright © 2001-2013 by Olaf Barthel. All Rights Reserved.
 */

/* There is a deliberate incompatibility in the 'struct AnchorPath'
   definition between the AmigaOS 2.x/3.x and 4.x header files which
   we will need to work around here. */
#if defined(__amigaos4__)
#define ap_Buf ap_Buffer
#else
#define USE_OLD_ANCHORPATH
#endif /* __amigaos4__ */

/****************************************************************************/

#include <libraries/bsdsocket.h>

#include <intuition/intuition.h>

#include <exec/execbase.h>
#include <exec/memory.h>

#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <dos/anchorpath.h>
#include <dos/dosasl.h>
#include <dos/rdargs.h>
#include <dos/stdio.h>

#include <libraries/locale.h>

#include <workbench/startup.h>
#include <workbench/icon.h>

#include <clib/alib_protos.h>

/****************************************************************************/

/* These are options relevant for the prototype header files
   included below. */
#if !defined(__amigaos4__)
#define NO_INLINE_STDARG
#endif /* __amigaos4__ */

#define __NOLIBBASE__
#define __NOGLOBALIFACE__
#define __USE_INLINE__

/****************************************************************************/

#include <proto/bsdsocket.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/icon.h>
#include <proto/exec.h>
#include <proto/dos.h>

/****************************************************************************/

/* The AmigaOS 4.x header files have changed the name of the Flush()
   function, which may need to be fixed here. */
#if defined(__amigaos4__) && !defined(Flush)
#define Flush(fh) FFlush(fh)
#endif /* __amigaos4__ */

/****************************************************************************/

#include <stdarg.h>
#include <string.h>

/****************************************************************************/

#include "macros.h"

/****************************************************************************/

/* this macro lets us long-align structures on the stack */
#define D_S(type,name) \
	char a_##name[sizeof(type)+3]; \
    type *name = (type *)((ULONG)(a_##name+3) & ~3UL)

/****************************************************************************/

/* These two bits indicate the kind of address
   represented by an IEEE 802.3 MAC address. These
   are the two least significant bits of the first
   address byte. */
#define MAC_Is_Group_Address	0x01
#define MAC_Is_Logical_Address	0x02

/****************************************************************************/

#include "AddNetInterface_rev.h"

/****************************************************************************/

#ifndef AbsExecBase
#define AbsExecBase (*(struct ExecBase **)4)
#endif /* AbsExecBase */

/* This is missing in the AmigaOS 2.x/3.x header files. */
#ifndef FORMAT_DEF
#define FORMAT_DEF 4
#endif /* FORMAT_DEF */

/****************************************************************************/

typedef LONG *	NUMBER;
typedef LONG	SWITCH;
typedef STRPTR	KEY;

/****************************************************************************/

#define MAX_FILENAME_LEN 512

/****************************************************************************/

/* This context information is handed around for all the routines and
   subroutines below. */
struct CommandContext
{
	struct Library *		cc_SysBase;
	struct Library *		cc_DOSBase;
	struct Library *		cc_UtilityBase;
	struct Library *		cc_SocketBase;
	struct Library *		cc_LocaleBase;
	struct Library *		cc_IconBase;
	struct Library *		cc_IntuitionBase;

	#if defined(__amigaos4__)

	/************************************************************************/

	struct ExecIFace *		cc_IExec;
	struct DOSIFace *		cc_IDOS;
	struct UtilityIFace *	cc_IUtility;
	struct SocketIFace *	cc_ISocket;
	struct LocaleIFace *	cc_ILocale;
	struct IconIFace *		cc_IIcon;
	struct IntuitionIFace *	cc_IIntuition;

	/************************************************************************/

	#endif /* __amigaos4__ */

	struct Catalog *		cc_Catalog;
	UBYTE					cc_ProgramName[256];
	struct WBStartup *		cc_StartupMessage;
	struct RDArgs *			cc_RDA;
	BPTR					cc_File;
	struct AnchorPath *		cc_AnchorPath;
	BOOL					cc_Quiet;
	struct TagItem *		cc_Tags;
	LONG					cc_NumTags;
	LONG					cc_MaxTags;
	struct MsgPort *		cc_ReplyPort;
	struct AddressAllocationMessage
							cc_AllocationMessage;
	ULONG					cc_RouterTable[16];
	ULONG					cc_DNSTable[16];
	struct DateStamp		cc_LeaseExpires;
};

/****************************************************************************/

/* The following set of DECLARE_... macros are used to load the library
   bases in every routine that needs them. These are portable macros in
   the sense that they allow the same code to be built both for AmigaOS 2.x/3.x
   and 4.x. */

/****************************************************************************/

#if defined(__amigaos4__)

/****************************************************************************/

#define DECLARE_SYSBASE(cc) \
	struct ExecIFace * IExec = cc->cc_IExec; \
	struct Library * SysBase = cc->cc_SysBase

#define DECLARE_DOSBASE(cc) \
	struct DOSIFace * IDOS = cc->cc_IDOS; \
	struct Library * DOSBase = cc->cc_DOSBase

#define DECLARE_UTILITYBASE(cc) \
	struct UtilityIFace * IUtility = cc->cc_IUtility; \
	struct Library * UtilityBase = cc->cc_UtilityBase

#define DECLARE_LOCALEBASE(cc) \
	struct LocaleIFace * ILocale = cc->cc_ILocale; \
	struct Library * LocaleBase = cc->cc_LocaleBase

#define DECLARE_SOCKETBASE(cc) \
	struct SocketIFace * ISocket = cc->cc_ISocket; \
	struct Library * SocketBase = cc->cc_SocketBase

#define DECLARE_ICONBASE(cc) \
	struct IconIFace * IIcon = cc->cc_IIcon; \
	struct Library * IconBase = cc->cc_IconBase

#define DECLARE_INTUITIONBASE(cc) \
	struct IntuitionIFace * IIntuition = cc->cc_IIntuition; \
	struct Library * IntuitionBase = cc->cc_IntuitionBase

/****************************************************************************/

#else

/****************************************************************************/

#define DECLARE_SYSBASE(cc) \
	struct Library * SysBase = cc->cc_SysBase

#define DECLARE_DOSBASE(cc) \
	struct Library * DOSBase = cc->cc_DOSBase

#define DECLARE_UTILITYBASE(cc) \
	struct Library * UtilityBase = cc->cc_UtilityBase

#define DECLARE_LOCALEBASE(cc) \
	struct Library * LocaleBase = cc->cc_LocaleBase

#define DECLARE_SOCKETBASE(cc) \
	struct Library * SocketBase = cc->cc_SocketBase

#define DECLARE_ICONBASE(cc) \
	struct Library * IconBase = cc->cc_IconBase

#define DECLARE_INTUITIONBASE(cc) \
	struct Library * IntuitionBase = cc->cc_IntuitionBase

/****************************************************************************/

#endif /* __amigaos4__ */

/****************************************************************************/

/* This program performs two passes on the interfaces it has to take
   care of it. First it adds the interfaces, then it configures them.
   Here is a definition of what each pass does. */
enum add_interface_mode_t
{
	AIM_Add,
	AIM_Configure
};

/****************************************************************************/

LONG _start(VOID);

/****************************************************************************/

STATIC VOID close_libs(struct CommandContext *cc);
STATIC LONG open_libs(struct CommandContext *cc, struct Library *SysBase);
STATIC LONG cmd(struct CommandContext *cc);
STATIC LONG match_key(struct CommandContext *cc, STRPTR what);
STATIC LONG add_tag(struct CommandContext *cc, Tag tag, ULONG data);
STATIC BOOL is_blank_space(UBYTE c);
STATIC STRPTR get_next_token(STRPTR input, STRPTR string, LONG string_len);
STATIC LONG get_hex_value(struct CommandContext *cc, STRPTR str);
STATIC VOID get_hex_string(LONG value, STRPTR str);
STATIC VOID strip_unprintable_characters(STRPTR s);
STATIC VOID strip_extra_blank_spaces(STRPTR s);
STATIC LONG add_interface(struct CommandContext *cc, STRPTR name, enum add_interface_mode_t operating_mode, LONG timeout);
STATIC BOOL validate_ip_address(struct CommandContext *cc, STRPTR key, STRPTR address, LONG line_number, STRPTR file);
STATIC VOID get_errno_and_code(struct CommandContext *cc, LONG *errno_ptr, STRPTR *code_ptr);
STATIC BOOL substring_matches(struct CommandContext *cc, STRPTR pattern, STRPTR string);
STATIC VOID add_interface_node(struct CommandContext *cc, struct List *list, struct Node *new_node);
STATIC VOID VARARGS68K error_printf(struct CommandContext *cc, STRPTR format, ...);
STATIC VOID VARARGS68K warning_printf(struct CommandContext *cc, STRPTR format, ...);
STATIC VOID VARARGS68K info_printf(struct CommandContext *cc, STRPTR format, ...);
STATIC VOID info_print_fault(struct CommandContext *cc, LONG code);
STATIC VOID error_print_fault(struct CommandContext *cc, LONG code);
STATIC VOID error_print_fault_prefix(struct CommandContext *cc, LONG code, STRPTR prefix);
STATIC STRPTR get_builtin_str(LONG id);
STATIC STRPTR get_str(struct CommandContext *cc, LONG id);
STATIC LONG VARARGS68K Local_ConfigureInterfaceTags(struct CommandContext *cc, STRPTR interface_name, ...);
STATIC LONG VARARGS68K Local_SocketBaseTags(struct CommandContext *cc, ...);
STATIC LONG VARARGS68K Local_AddRouteTags(struct CommandContext *cc, ...);

/****************************************************************************/

/****** ROADSHOW/ADDNETINTERFACE **************************************************
*
*   NAME
*	AddNetInterface - Make network interfaces known to the protocol stack.
*
*   FORMAT
*	AddNetInterface [QUIET] [TIMEOUT=<n>] INTERFACE
*
*   TEMPLATE
*	INTERFACE/M,QUIET/S,TIMEOUT/K/N
*
*   PATH
*	C:ADDNETINTERFACE
*
*   FUNCTION
*	ADDNETINTERFACE starts the specified network interfaces, thus starting
*	the connection.
*
*   OPTIONS
*	INTERFACE/M
*	    The name of the interface to add; this can be a plain interface
*	    name, such as "Ariadne", or the fully qualified file name which
*	    contains the interface configuration information. The tool
*	    expects the name of the file in question (without the prefixed
*	    path) to become the name of the interface. For historic reasons
*	    interface names cannot be longer than 15 characters.
*
*	    For your convenience, a wild card pattern can be specified in
*	    place of the file name to use.
*
*	    If several interface names are specified, they will be sorted in
*	    alphabetical order before they are added. If the interface
*	    files have icons attached, you can use tool types such as
*	    "PRI=5" or "PRIORITY=5" to select the order in which the interfaces
*	    will be sorted. Higher priority entries will appear before lower
*	    priority entries. If the priorities for two entries is identical,
*	    then the interface names will be compared. If no priority is
*	    given, the value 0 will be used.
*
*	QUIET/S
*	    This option causes the program not to emit any error messages
*	    or progress reports. Also, if the program encounters an error
*	    it will flag this as failure code 5 which can be looked at
*	    using the "if warn" shell script command. If this option is
*	    not in effect, failure codes will be more severe and all sorts
*	    of progress information will be displayed.
*
*	TIMEOUT/K/N
*	    If you're going to use DHCP configuration for any of the
*	    interfaces, a default timeout value of 60 seconds will
*	    limit the time an interface can take to be configured.
*	    This parameter allows you to use a different timeout value.
*	    Note that due to how the configuration protocol works,
*	    the timeout cannot be shorter than ten seconds.
*
*	The 'AddNetInterface' command can be invoked from Workbench, too. It
*	operates on the same configuration files with the same keywords, etc.
*	To make it work, create an icon for your interface configuration file
*	(it must be a project icon) and put 'AddNetInterface' into its default
*	tool. Make sure that the project has enough stack space assigned (4000
*	bytes minimum), then double-click on the icon. If things should go
*	wrong, you will see an error requester pop up, and no further
*	initialization will be done. You can configure two options in the
*	project file's tool types: QUIET and TIMEOUT. These are identical to
*	the two parameters of the same name you could pass on the command
*	line; they define whether the command should print any error messages
*	(the default is to print them) and how long the command should wait
*	for DHCP configuration to conclude (default is a timeout of 60
*	seconds).
*
*   NOTES
*	This command is similar to the Unix "ifconfig" command.
*
*	The program makes two passes over the configuration files to be
*	taken into account. In the first pass information is gathered
*	on the interfaces to add, which is subsequently used to add those
*	interfaces found. In the second pass interfaces are configured,
*	setting their IP addresses, etc. If anything goes wrong in the
*	first pass, processing will stop and no second pass will be
*	done. If anything goes wrong in either the first or the second
*	pass, that pass will not be completed.
*
*   CONFIGURATION FILES
*	Interfaces are configured through files stored in the
*	"DEVS:NetInterfaces" or "SYS:Storage/NetInterfaces" directories.
*	These are text files whose contents are described below.
*
*	Each line of the file must correspond to an option; if a line is
*	introduced by a '#' or ';' character it will be ignored (so are empty
*	lines). The following options are supported:
*
*	    DEVICE/K
*	        Must be provided; the name of the SANA-II device driver. This
*	        should be the complete, fully qualified path to the driver. If
*	        no complete path is provided, the 'Devs:Networks' drawer will be
*	        checked. Thus, "DEVS:Networks/ariadne.device" is equivalent to
*	        "ariadne.device".
*
*	    UNIT/K/N
*	        Unit number of the device driver to open. The default is to
*	        use unit 0.
*
*	    IPTYPE/K/N
*	        You can use this parameter to override the packet type the
*	        stack uses when sending IP packets; default is 2048 (for
*	        Ethernet hardware).
*
*	    ARPTYPE/K/N
*	        You can use this parameter to override the packet type the
*	        stack uses when sending ARP packets. Default is 2054; this
*	        parameter only works with Ethernet hardware and should not be
*	        changed.
*
*	    IPREQUESTS/K/N
*	        The number of IP read requests to allocate and queue for the
*	        SANA-II device driver to use. The default value is 32, larger
*	        values can improve performance, especially with fast device
*	        drivers.
*
*	    WRITEREQUESTS/K/N
*	        The number of IP write requests to allocate and queue for the
*	        SANA-II device driver to use. The default value is 32, larger
*	        values can improve performance, especially with fast device
*	        drivers.
*
*	    ARPREQUESTS/K/N
*	        The number of ARP read requests to allocate and queue for the
*	        SANA-II device driver to use. The default value is 4.
*
*	    DEBUG/K  (possible parameters: YES or NO)
*	        You can enable debug output for this interface (don't worry,
*	        you can always disable it later) to help in tracking down
*	        configuration problems. At this time of writing, the debug
*	        mode will, if enabled, produce information on the progress of
*	        the DHCP configuration process.
*
*	    POINTTOPOINT/K  (possible parameters: YES or NO)
*	        This indicates that the device is used for point to point
*	        connections. The stack automatically figures out whether the
*	        SANA-II device driver is of the point to point type, so you
*	        should not need to specify this option.
*
*	    MULTICAST/K  (possible parameters: YES or NO)
*	        This tells the stack that this device can handle multicast
*	        packets. 'YES' only works with Ethernet hardware (where it's
*	        enabled by default anyway).
*
*	    DOWNGOESOFFLINE/K  (possible parameters: YES or NO)
*	        This option is useful with point to point devices, like
*	        'ppp.device'. When specified, bringing the interface 'down'
*	        (via the 'ConfigureNetInterface' program) or shutting down the
*	        stack will cause the associated SANA-II device driver to be
*	        switched offline (via the 'S2_OFFLINE' command).
*
*	    REPORTOFFLINE/K  (possible parameters: YES or NO)
*	        When a device is switched offline, you may want to know about
*	        it. This is helpful with SLIP/PPP connections which run over a
*	        serial link which accumulates costs while it is open. When the
*	        connection is broken and the device goes offline, you will
*	        receive a brief notification of what happened. However, if you
*	        tell the library itself to shut down, no notification that a
*	        device was switched offline will be shown.
*
*	    REQUIRESINITDELAY/K  (possible parameters: YES or NO)
*	        Some devices need a little time to settle after they have been
*	        opened or they will hickup and lose data after the first
*	        packet has been sent. The original 'Ariadne I' card is one
*	        such device. For these devices, the 'REQUIRESINITDELAY=YES'
*	        option will cause a delay of about a second before the first
*	        packet is sent.
*
*	        This option defaults to YES.
*
*	    COPYMODE/K  (possible parameters: SLOW or FAST)
*	        This option is for chasing subtle bugs in the driver interface
*	        with cards like the original 'Ariadne I'. Cards like these do
*	        not support writing to the hardware transmit buffer in units
*	        other than 16 bits a piece. Default is 'SLOW', which is
*	        compatible with the Ariadne I. But if you're feeling
*	        adventurous, try the 'FAST' option (and don't complain if it
*	        doesn't work for you!).
*
*	    FILTER/K  (possible parameters: OFF, LOCAL, IPANDARP or EVERYTHING)
*	        This option enables the use of the Berkeley packet filter for
*	        this particular interface. Possible choices for the key are:
*
*	            FILTER=OFF
*	                Disables the filter.
*
*	            FILTER=LOCAL
*	                Enables filtering on all IP and ARP packets that are
*	                intended for this particular interface. Packets
*	                intended for other interfaces or hosts are ignored.
*
*	            FILTER=IPANDARP
*	                Enables filtering on all IP and ARP packets that
*	                happen to fly by this interface, no matter whether the
*	                packets are intended for it or not. This requires that
*	                the underlying network device driver is opened for
*	                exclusive access in so-called 'promiscuous' mode. This
*	                may not work if other clients (Envoy, ACS) need to
*	                keep the driver opened.
*
*	            FILTER=EVERYTHING
*	                Identical to FILTER=IPANDARP, but will also filter all
*	                other kinds of packets that may show up.
*
*	        Default for this option is 'FILTER=LOCAL'. Note that by using
*	        this option you merely define what the filter mechanism can do
*	        and what it cannot do. The filter is not enabled when you add
*	        the interface.
*
*	    HARDWAREADDRESS/K
*	        You can specify the hardware address (layer 2 address, MAC
*	        address) this interface should respond to when it is first
*	        added and configured. This usually works only once for each
*	        interface, which means that once an address has been chosen
*	        you have to stick with it until the system is rebooted. And it
*	        also means that the first program to configure the address
*	        will manage to make its choice stick.
*
*	        The hardware address must be given as six bytes in hexadecimal
*	        notation, separated by colon characters, like this:
*
*	            HARDWAREADDRESS=00:60:30:00:11:22
*
*	        Take care, there are rules that apply to the choice of the
*	        hardware address, which means that you cannot simply pick a
*	        convenient number and get away with it. It is assumed that you
*	        will want to configure an IEEE 802.3 MAC address, which works for
*	        Ethernet hardware and is six bytes (48 bits) in size.
*
*	In addition to the purely static interface configuration information you
*   can also tell the configuration program to do something about the
*   interfaces once they have all been added. That's when the following
*   configuration file parameters will be taken into account:
*
*	    ADDRESS/K
*	        This configures the IP address of the interface. The parameter
*	        you supply should be an IP address in dotted-decimal notation
*	        ("192.168.0.1"). Don't pick a symbolic host name as the system
*	        may not yet be in a position to talk to name resolution server
*	        and translate the symbolic name.
*
*	        In place of the IP address you can also specify "DHCP"
*	        (Dynamic Host Configuration Protocol). As the name suggests,
*	        this will start a configuration process involving the DHCP
*	        protocol which should eventually yield the right IP address
*	        for this host. Note that this configuration procedure only
*	        works for Ethernet hardware.
*
*	    ALIAS/K/M
*	        In addition to the primary interface address you can assign
*	        several aliases to it. These must be specified in
*	        dotted-decimal notation ("192.168.0.1"). Alias addresses are
*	        added after the primary interface address has been configured.
*
*	    STATE/K
*	        By default, interfaces whose addresses are configured will
*	        switch automatically to 'up' state, making it possible for the
*	        TCP/IP stack to use them for network I/O. You can override
*	        this by using the 'STATE=DOWN' switch. The alternatives
*	        'online' (implies 'up', but tells the underlying network
*	        interface driver to go online first) and 'offline' (implies
*	        'down' but tells the driver to go offline first) are available
*	        as well.
*
*	    NETMASK/K
*	        This selects the subnet mask for the interface, which must be
*	        specified in dotted-decimal notation ("192.0.168.1").
*
*	        In place of the subnet mask you can also specify "DHCP"
*	        (Dynamic Host Configuration Protocol). As the name suggests,
*	        this will start a configuration process involving the DHCP
*	        protocol which should eventually yield the right subnet mask
*	        for this host. Note that this configuration procedure only
*	        works for Ethernet hardware.
*
*	    DESTINATION=DESTINATIONADDR/K
*	        The address of the point-to-point partner for this interface;
*	        must be specified in dotted-decimal notation ("192.168.0.1").
*	        Only works for point-to-point connections, such as PPP.
*
*	    METRIC/K/N 
*	        This configures the interface route metric value. Default
*	        is 0.
*
*	    MTU/K/N
*	        You can limit the maximum transmission size used by the TCP/IP
*	        stack to push data through the interface. The interface driver
*	        will have its own ideas about the maximum transmission size.
*	        You can therefore only suggest a smaller value than the
*	        driver's preferred hardware MTU size.
*
*	    CONFIGURE/K  (possible parameters: DHCP, AUTO or FASTAUTO)
*	        You can use DHCP configuration for this interface and protocol
*	        stack internals, namely the list of routers (and the default
*	        gateway) to use and the domain name servers. This option
*	        allows you to bring up the complete network configuration in
*	        one single step.
*
*	        You can request that a particular IP address is assigned to
*	        this interface by the DHCP process by specifying
*	        CONFIGURE=DHCP and your choice of ADDRESS=xxx.xxx.xxx.xxx.
*
*	        If your network has no DHCP server, you may choose
*	        CONFIGURE=AUTO to use automatic IPv4 address selection,
*	        based upon a protocol called ZeroConf. This protocol will
*	        select a currently unused address from a specially
*	        designated address range.
*
*	        If you choose automatic configuration in a wireless network,
*	        you might want to use CONFIGURE=FASTAUTO instead of
*	        CONFIGURE=AUTO.
*
*	        Note that only the CONFIGURE=DHCP option will attempt to
*	        set up a default route and a set of DNS servers for you to
*	        use. The alternatives of CONFIGURE=FASTAUTO and
*	        CONFIGURE=AUTO are restricted to selecting the network
*	        interface IPv4 addresses.
*
*	    LEASE/K
*	        This is a complex option which can be used to request how long
*	        an IP address should be bound to an interface, via the DHCP
*	        protocol. Several combinations of options are possible. Here
*	        is a short list:
*
*	            LEASE=300
*	            LEASE=300seconds
*	                This requests a lease of exactly 300 seconds, or
*	                five minutes.
*
*	            LEASE=30min
*	                This requests a lease of 30 minutes.
*
*	            LEASE=2hours
*	                This requests a lease of 2 hours.
*
*	            LEASE=1day
*	                This requests a lease of 1 day.
*
*	            LEASE=4weeks
*	                This requests a lease of 4 weeks.
*
*	            LEASE=infinite
*	                This requests that the IP address should be
*	                permanently bound.
*
*	        Blank spaces between the numbers and the qualifiers are
*	        supported. The qualifiers are tested using substring matching,
*	        which means for example that "30 minutes" is the same as "30
*	        min" and "30 m".
*
*	        Note that the requested lease time may be ignored by the DHCP
*	        server. After all, it is just a suggestion and not an order.
*
*	    ID/K
*	        This option works along with the CONFIGURE=DHCP process. It
*	        can be used to tell the DHCP server by which name the local
*	        host should be referred to. Some DHCP servers are on good
*	        terms with their local name resolution services and will add
*	        the name and the associated IP address to the local host
*	        database. The name you can supply here cannot be longer than
*	        255 characters and must be at least 2 characters long. Keep it
*	        brief: not all DHCP servers have room for the whole 255
*	        characters.
*
*	Unsupported keywords in the configuration file (or typos) will be
*	reported, along with the name of the file and the line number.
*
*	The name of the configuration file defines the name of the respective
*	interface. Interface names must be unique, and the case of the names
*	does not matter. For historic reasons interface names cannot be longer
*	than 15 characters. Beyond this no restrictions on naming conventions
*	apply.
*
*   DHCP PROTOCOL
*	A few words on DHCP (Dynamic Host Configuration Protocol). First, it
*	only works for Ethernet hardware, so please don't try it with PPP or
*	SLIP. Now it gets a bit technical. Unless you request an address to be
*	permanently assigned, DHCP will assign addresses only for a limited
*	period of time. This is called a 'lease'. Once an IP address has been
*	assigned through DHCP, the lease will be repeatedly extended. The DHCP
*	server may over time decide not to extend the lease or assign a new IP
*	address to the interface. To stop the lease from getting extended over
*	and over again, you must either change the interface's primary IP
*	address or mark it 'down'. The library will make a brave attempt to
*	get a DHCPRELEASE datagram out to notify the server that the
*	previously allocated IP address is no longer in use. Don't count on it
*	to work, though. First, the protocol stack might be going down so fast
*	that it cannot get the datagram out. Second, when you mark an
*	interface 'down' you will effectively pull it out of circulation, it
*	will not send any further datagrams. Third, DHCP rides on UDP whose
*	second name is 'unreliable datagram protocol', meaning that any
*	datagram may get lost or corrupted and nobody will hear about it; this
*	is rather hard on DHCP since the release message is sent only once.
*	Don't worry. Unless you request permanent leases, the leases will
*	eventually time out and the now unused IP address will finally return
*	to the pool of addresses available for allocation.
*
*   EXAMPLES
*	Start the interface called "DSL" and run quietly.
*
*	    1> AddNetInterface DSL QUIET
*
*	An example configuration file for the "Ariadne" interface, with
*	some options commented out:
*
*	    1> Type Devs:NetInterfaces/Ariadne
*	    device=ariadne.device
*	    unit=0
*	    #iprequests=64
*	    #writerequests=64
*	    copymode=fast
*	    #configure=dhcp
*	    address=192.168.0.1
*	    netmask=255.255.255.0
*	    #alias=192.168.0.9
*	    #hardwareaddress=00:60:30:00:11:22
*	    #id=a3000ux
*	    #debug=yes
*	    #filter=everything
*
*   SEE ALSO
*	ConfigureNetInterface
*	NetShutdown
*
******************************************************************************
*/

LONG
_start(VOID)
{
	struct Library *SysBase = (struct Library *)AbsExecBase;
	#if defined(__amigaos4__)
	struct ExecIFace * IExec = (struct ExecIFace *)((struct ExecBase *)SysBase)->MainInterface;
	#endif /* __amigaos4__ */

	struct CommandContext _cc,*cc = &_cc;
	LONG result = RETURN_FAIL;
	struct Process * pr;
	LONG error;

	memset(cc,0,sizeof(*cc));

	/* If launched from Workbench, pick up the startup message. */
	pr = (struct Process *)FindTask(NULL);
	if(pr->pr_CLI == ZERO)
	{
		struct MsgPort * mp = &pr->pr_MsgPort;

		WaitPort(mp);

		cc->cc_StartupMessage = (struct WBStartup *)GetMsg(mp);
	}

	/* Try to open the necessary libraries. */
	error = open_libs(cc,SysBase);
	if(error == OK)
	{
		DECLARE_LOCALEBASE(cc);
		DECLARE_DOSBASE(cc);

		/* If possible, open the catalog which contains the localized
		   program message. */
		if(cc->cc_LocaleBase != NULL)
			cc->cc_Catalog = OpenCatalogA(NULL,"roadshow.catalog",NULL);

		/* Do what the program needs to do. */
		result = cmd(cc);

		/* Clean up... */
		if(cc->cc_Catalog != NULL)
			CloseCatalog(cc->cc_Catalog);

		if(cc->cc_Tags != NULL)
			FreeVec(cc->cc_Tags);

		if(cc->cc_AnchorPath != NULL)
		{
			MatchEnd(cc->cc_AnchorPath);

			#if defined(__amigaos4__)
			{
				FreeDosObject(DOS_ANCHORPATH,cc->cc_AnchorPath);
			}
			#else
			{
				FreeVec(cc->cc_AnchorPath);
			}
			#endif /* __amigaos4__ */
		}

		if(cc->cc_RDA != NULL)
		{
			FreeArgs(cc->cc_RDA);
			FreeDosObject(DOS_RDARGS,cc->cc_RDA);
		}

		if(cc->cc_File != ZERO)
			Close(cc->cc_File);
	}
	else
	{
		pr->pr_Result2 = error;
	}

	close_libs(cc);

	/* Return the Workbench startup message. */
	if(cc->cc_StartupMessage != NULL)
	{
		Forbid();

		ReplyMsg((struct Message *)cc->cc_StartupMessage);
	}

	return(result);
}

/****************************************************************************/

/* Close all the libraries opened by open_libs(). */
STATIC VOID
close_libs(struct CommandContext * cc)
{
	DECLARE_SYSBASE(cc);

	#if defined(__amigaos4__)
	{
		if(cc->cc_IDOS != NULL)
			DropInterface((struct Interface *)cc->cc_IDOS);

		if(cc->cc_ILocale != NULL)
			DropInterface((struct Interface *)cc->cc_ILocale);

		if(cc->cc_ISocket != NULL)
			DropInterface((struct Interface *)cc->cc_ISocket);

		if(cc->cc_IUtility != NULL)
			DropInterface((struct Interface *)cc->cc_IUtility);

		if(cc->cc_IIcon != NULL)
			DropInterface((struct Interface *)cc->cc_IIcon);

		if(cc->cc_IIntuition != NULL)
			DropInterface((struct Interface *)cc->cc_IIntuition);
	}
	#endif /* __amigaos4__ */

	if(cc->cc_IntuitionBase != NULL)
		CloseLibrary(cc->cc_IntuitionBase);

	if(cc->cc_IconBase != NULL)
		CloseLibrary(cc->cc_IconBase);

	if(cc->cc_UtilityBase != NULL)
		CloseLibrary(cc->cc_UtilityBase);

	if(cc->cc_SocketBase != NULL)
		CloseLibrary(cc->cc_SocketBase);

	if(cc->cc_LocaleBase != NULL)
		CloseLibrary(cc->cc_LocaleBase);

	if(cc->cc_DOSBase != NULL)
		CloseLibrary(cc->cc_DOSBase);
}

/****************************************************************************/

/* Open all the required libraries. */
STATIC LONG
open_libs(struct CommandContext * cc,struct Library *SysBase)
{
	#if defined(__amigaos4__)
	struct ExecIFace * IExec = (struct ExecIFace *)((struct ExecBase *)SysBase)->MainInterface;
	#endif /* __amigaos4__ */

	LONG error;

	cc->cc_SysBase = SysBase;

	#if defined(__amigaos4__)
	{
		cc->cc_IExec = IExec;
	}
	#endif /* __amigaos4__ */

	cc->cc_DOSBase = OpenLibrary("dos.library",36);

	#if defined(__amigaos4__)
	{
		if(cc->cc_DOSBase != NULL)
		{
			cc->cc_IDOS = (struct DOSIFace *)GetInterface(cc->cc_DOSBase, "main", 1, 0);
			if(cc->cc_IDOS == NULL)
			{
				CloseLibrary(cc->cc_DOSBase);
				cc->cc_DOSBase = NULL;
			}
		}
	}
	#endif /* __amigaos4__ */

	if(cc->cc_DOSBase == NULL)
	{
		error = ERROR_INVALID_RESIDENT_LIBRARY;
		goto out;
	}

	cc->cc_LocaleBase = OpenLibrary("locale.library",38);

	#if defined(__amigaos4__)
	{
		if(cc->cc_LocaleBase != NULL)
		{
			cc->cc_ILocale = (struct LocaleIFace *)GetInterface(cc->cc_LocaleBase, "main", 1, 0);
			if(cc->cc_ILocale == NULL)
			{
				CloseLibrary(cc->cc_LocaleBase);
				cc->cc_LocaleBase = NULL;
			}
		}
	}
	#endif /* __amigaos4__ */

	cc->cc_UtilityBase = OpenLibrary("utility.library",37);

	#if defined(__amigaos4__)
	{
		if(cc->cc_UtilityBase != NULL)
		{
			cc->cc_IUtility = (struct UtilityIFace *)GetInterface(cc->cc_UtilityBase, "main", 1, 0);
			if(cc->cc_IUtility == NULL)
			{
				CloseLibrary(cc->cc_UtilityBase);
				cc->cc_UtilityBase = NULL;
			}
		}
	}
	#endif /* __amigaos4__ */

	cc->cc_IconBase = OpenLibrary("icon.library",37);

	#if defined(__amigaos4__)
	{
		if(cc->cc_IconBase != NULL)
		{
			cc->cc_IIcon = (struct IconIFace *)GetInterface(cc->cc_IconBase, "main", 1, 0);
			if(cc->cc_IIcon == NULL)
			{
				CloseLibrary(cc->cc_IconBase);
				cc->cc_IconBase = NULL;
			}
		}
	}
	#endif /* __amigaos4__ */

	cc->cc_IntuitionBase = OpenLibrary("intuition.library",37);

	#if defined(__amigaos4__)
	{
		if(cc->cc_IntuitionBase != NULL)
		{
			cc->cc_IIntuition = (struct IntuitionIFace *)GetInterface(cc->cc_IntuitionBase, "main", 1, 0);
			if(cc->cc_IIntuition == NULL)
			{
				CloseLibrary(cc->cc_IntuitionBase);
				cc->cc_IntuitionBase = NULL;
			}
		}
	}
	#endif /* __amigaos4__ */

	/* The following may be necessary to flush out an inoperable
	   bsdsocket.library which reached the end of the shutdown
	   process. */
	#if 1
	{
		struct ExecBase * ex = (struct ExecBase *)SysBase;
		struct Library * lib;

		Forbid();

		lib = (struct Library *)FindName(&ex->LibList,"bsdsocket.library");
		if(lib != NULL)
			RemLibrary(lib);

		Permit();
	}
	#endif

	cc->cc_SocketBase = OpenLibrary("bsdsocket.library",4);

	#if defined(__amigaos4__)
	{
		if(cc->cc_SocketBase != NULL)
		{
			cc->cc_ISocket = (struct SocketIFace *)GetInterface(cc->cc_SocketBase, "main", 1, 0);
			if(cc->cc_ISocket == NULL)
			{
				CloseLibrary(cc->cc_SocketBase);
				cc->cc_SocketBase = NULL;
			}
		}
	}
	#endif /* __amigaos4__ */

	error = OK;

 out:

	return(error);
}

/****************************************************************************/

#define CATCOMP_ARRAY
#define ADDNETINTERFACE_CATALOG_STRINGS
#include "roadshow.h"

/****************************************************************************/

/* This is the main() program, so to speak. */
STATIC LONG
cmd(struct CommandContext * cc)
{
	STRPTR devs_prefix = "DEVS:NetInterfaces";
	STRPTR storage_prefix = "SYS:Storage/NetInterfaces";

	struct
	{
		KEY *	Interface;
		SWITCH	Quiet;
		NUMBER	Timeout;
	} args;

	STRPTR args_template =
		"INTERFACE/M,"
		"QUIET/S,"
		"TIMEOUT/K/N"
		VERSTAG;

	DECLARE_SYSBASE(cc);
	DECLARE_DOSBASE(cc);
	DECLARE_UTILITYBASE(cc);
	DECLARE_ICONBASE(cc);
	DECLARE_SOCKETBASE(cc);

	struct List interface_list;
	ULONG interface_list_size;
	UBYTE * buf = NULL;
	LONG result = RETURN_FAIL;
	struct RDArgs * rda = NULL;
	LONG timeout = 60;
	BPTR old_cd = ZERO;
	BOOL old_cd_valid = FALSE;
	struct DiskObject * icon = NULL;
	BOOL configuration_trouble = FALSE;

	NewList(&interface_list);
	interface_list_size = 0;

	GetProgramName(cc->cc_ProgramName,sizeof(cc->cc_ProgramName));

	memset(&args,0,sizeof(args));

	/* If started from shell, read the command arguments. */
	if(cc->cc_StartupMessage == NULL)
	{
		rda = ReadArgs(args_template,(LONG *)&args,NULL);
		if(rda == NULL)
		{
			error_print_fault(cc,IoErr());
			goto out;
		}

		cc->cc_Quiet = (BOOL)(args.Quiet != 0);
	}

	/* Did we manage to open utility.library? */
	if(UtilityBase == NULL)
	{
		if(NOT cc->cc_Quiet)
			error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NO_UTILITY_TXT));

		goto out;
	}

	/* Did we manage to open bsdsocket.library? */
	if(SocketBase == NULL)
	{
		if(NOT cc->cc_Quiet)
			error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_CANNOT_OPEN_BSDSOCKET_TXT));

		goto out;
	}
	else
	{
		LONG have_interface_api = FALSE;
		LONG have_routing_api = FALSE;
		LONG have_address_conversion_api = FALSE;

		/* Check if the bsdsocket.library we managed to open supports the
		   various APIs we expect below. */
		if(Local_SocketBaseTags(cc,
		    SBTM_GETREF(SBTC_HAVE_INTERFACE_API),&have_interface_api,
		TAG_END) != 0)
		{
			have_interface_api = FALSE;
		}

		if(NOT have_interface_api)
		{
			if(NOT cc->cc_Quiet)
			{
				error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_BSDSOCKET_HAS_NO_INTERFACE_API_TXT),
					SocketBase->lib_Node.ln_Name, SocketBase->lib_Version, SocketBase->lib_Revision);
			}

			goto out;
		}

		if(Local_SocketBaseTags(cc,
		    SBTM_GETREF(SBTC_HAVE_ROUTING_API),&have_routing_api,
		TAG_END) != 0)
		{
			have_routing_api = FALSE;
		}

		if(NOT have_routing_api)
		{
			if(NOT cc->cc_Quiet)
			{
				error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_BSDSOCKET_HAS_NO_ROUTE_API_TXT),
					SocketBase->lib_Node.ln_Name, SocketBase->lib_Version, SocketBase->lib_Revision);
			}

			goto out;
		}

		if(Local_SocketBaseTags(cc,
		    SBTM_GETREF(SBTC_HAVE_ADDRESS_CONVERSION_API),&have_address_conversion_api,
		TAG_END) != 0)
		{
			have_address_conversion_api = FALSE;
		}

		if(NOT have_address_conversion_api)
		{
			if(NOT cc->cc_Quiet)
			{
				error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_HAS_NO_ADDRESS_CONVERSION_API_TXT),
					SocketBase->lib_Node.ln_Name, SocketBase->lib_Version, SocketBase->lib_Revision);
			}

			goto out;
		}
	}

	/* This is required by the code that reads the configuration
	   file. And we allocate this data here because if we had
	   allocated it before Intuition was opened, error display
	   would have been difficult. */
	cc->cc_RDA = AllocDosObject(DOS_RDARGS,NULL);
	if(cc->cc_RDA == NULL)
	{
		if(NOT cc->cc_Quiet)
			error_print_fault(cc,IoErr());

		goto out;
	}

	/* If started from shell, process the command line arguments. */
	if(cc->cc_StartupMessage == NULL)
	{
		KEY * interface_table;
		int operating_mode;
		STRPTR interface;
		struct Node * node;

		/* This is to be used for pattern matching when trying to find
		   the interfaces to add. */
		#if defined(__amigaos4__)
		{
			cc->cc_AnchorPath = AllocDosObjectTags(DOS_ANCHORPATH,
				ADO_Strlen,	MAX_FILENAME_LEN,
				ADO_Mask,	SIGBREAKF_CTRL_C,
			TAG_END);
		}
		#else
		{
			cc->cc_AnchorPath = AllocVec(sizeof(*cc->cc_AnchorPath) + MAX_FILENAME_LEN,MEMF_ANY|MEMF_PUBLIC);
			if(cc->cc_AnchorPath != NULL)
				memset(cc->cc_AnchorPath,0,sizeof(*cc->cc_AnchorPath));
		}
		#endif /* __amigaos4__ */

		if(cc->cc_AnchorPath == NULL)
		{
			if(NOT cc->cc_Quiet)
				error_print_fault(cc,IoErr());

			goto out;
		}

		if(args.Timeout != NULL && (*args.Timeout) > 0)
		{
			timeout = (*args.Timeout);
			if(timeout < AAM_TIMEOUT_MIN)
			{
				if(NOT cc->cc_Quiet)
				{
					warning_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_TIMEOUT_TOO_SHORT_TXT),
						timeout,AAM_TIMEOUT_MIN);
				}

				timeout = 10;
			}
		}

		/* There has to be at least one interface name we can work with. */
		if(args.Interface == NULL)
		{
			if(NOT cc->cc_Quiet)
				error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NO_INTERFACE_NAME_GIVEN_TXT));

			result = RETURN_ERROR;

			goto out;
		}

		result = RETURN_OK;

		/* Check each interface name given; if it's a wildcard pattern, expand it. */
		interface_table = args.Interface;

		while((interface = (*interface_table++)) != NULL)
		{
			/* Not an absolute path name? Look into DEVS: instead. */
			if(FilePart(interface) == interface)
			{
				LONG len = strlen(interface);
				LONG max_name_size = strlen(storage_prefix) + 1 + len + 1;
				BPTR file_lock;
				D_S(struct FileInfoBlock,fib);
				BOOL have_name = FALSE;
				LONG error = 0;

				if(CheckSignal(SIGBREAKF_CTRL_C))
				{
					if(NOT cc->cc_Quiet)
						error_print_fault(cc,ERROR_BREAK);
		
					goto out;
				}

				node = AllocVec(sizeof(*node) + max_name_size,MEMF_ANY|MEMF_PUBLIC);
				if(node == NULL)
				{
					if(NOT cc->cc_Quiet)
						error_print_fault(cc,IoErr());

					result = RETURN_ERROR;

					goto out;
				}

				node->ln_Name	= (char *)(node + 1);
				node->ln_Pri	= 0;

				/* Check if the interface file exists in the local directory.
				   Make sure that it's a file, too. */
				file_lock = Lock(interface,SHARED_LOCK);
				if(file_lock != ZERO && Examine(file_lock,fib))
				{
					if(fib->fib_DirEntryType < 0)
					{
						strcpy(node->ln_Name,interface);
						have_name = TRUE;
					}
					else
					{
						error = ERROR_OBJECT_WRONG_TYPE;
					}
				}
				else
				{
					error = IoErr();
				}

				UnLock(file_lock);

				/* If we still don't have a name, look into "DEVS:NetInterfaces". */
				if(NOT have_name && (error == ERROR_OBJECT_WRONG_TYPE || error == ERROR_OBJECT_NOT_FOUND))
				{
					strcpy(node->ln_Name,devs_prefix);
					AddPart(node->ln_Name,interface,max_name_size);

					file_lock = Lock(node->ln_Name,SHARED_LOCK);
					if(file_lock != ZERO && Examine(file_lock,fib))
					{
						if(fib->fib_DirEntryType < 0)
							have_name = TRUE;
						else
							error = ERROR_OBJECT_WRONG_TYPE;
					}
					else
					{
						error = IoErr();
					}

					UnLock(file_lock);
				}

				/* If we still don't have a name, look into "SYS:Storage/NetInterfaces". */
				if(NOT have_name && (error == ERROR_OBJECT_WRONG_TYPE || error == ERROR_OBJECT_NOT_FOUND))
				{
					strcpy(node->ln_Name,storage_prefix);
					AddPart(node->ln_Name,interface,max_name_size);

					file_lock = Lock(node->ln_Name,SHARED_LOCK);
					if(file_lock != ZERO && Examine(file_lock,fib))
					{
						if(fib->fib_DirEntryType < 0)
							have_name = TRUE;
						else
							error = ERROR_OBJECT_WRONG_TYPE;
					}
					else
					{
						error = IoErr();
					}

					UnLock(file_lock);
				}

				if(have_name)
				{
					/* Try to read the interface priority from the
					   icon attached to the file, if there is any. */
					if(IconBase != NULL)
					{
						struct DiskObject * icon;

						icon = GetDiskObject(node->ln_Name);
						if(icon != NULL)
						{
							STRPTR priority;

							priority = FindToolType(icon->do_ToolTypes,"PRI");
							if(priority == NULL)
								priority = FindToolType(icon->do_ToolTypes,"PRIORITY");

							if(priority != NULL)
							{
								LONG value;

								if(StrToLong(priority,&value) > 0)
								{
									if(-128 <= value && value <= 127)
										node->ln_Pri = value;
								}
							}

							FreeDiskObject(icon);
						}
					}

					add_interface_node(cc,&interface_list,node);
					interface_list_size++;
				}
				else
				{
					if(NOT cc->cc_Quiet)
						error_print_fault_prefix(cc,error,interface);

					result = RETURN_ERROR;

					goto out;
				}
			}
			else
			{
				static STRPTR info_suffix = ".info";
				const int info_suffix_len = strlen(info_suffix);
				LONG error;

				/* We may have to check an icon or two, so icon.library better be open. */
				if(IconBase == NULL)
				{
					if(NOT cc->cc_Quiet)
						error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NO_ICON_TXT));

					goto out;
				}

				MatchEnd(cc->cc_AnchorPath);

				#ifndef __amigaos4__
				{
					memset(cc->cc_AnchorPath,0,sizeof(*cc->cc_AnchorPath));

					cc->cc_AnchorPath->ap_Strlen	= MAX_FILENAME_LEN;
					cc->cc_AnchorPath->ap_BreakBits	= SIGBREAKF_CTRL_C;
				}
				#endif /* __amigaos4__ */

				/* Find all the files that match the pattern, if any. */
				error = MatchFirst(interface,cc->cc_AnchorPath);
				while(error == OK)
				{
					/* Careful there, only check files! */
					if(cc->cc_AnchorPath->ap_Info.fib_DirEntryType < 0)
					{
						int len = strlen(cc->cc_AnchorPath->ap_Buf);

						/* Files whose names end with ".info" are ignored. */
						if(len < info_suffix_len || Stricmp(&cc->cc_AnchorPath->ap_Buf[len - info_suffix_len],info_suffix) != SAME)
						{
							struct DiskObject * icon;

							node = AllocVec(sizeof(*node) + strlen(cc->cc_AnchorPath->ap_Buf)+1,MEMF_ANY);
							if(node == NULL)
							{
								error = ERROR_NO_FREE_STORE;
								break;
							}

							node->ln_Name	= (char *)(node + 1);
							node->ln_Pri	= 0;

							strcpy(node->ln_Name,cc->cc_AnchorPath->ap_Buf);

							icon = GetDiskObject(node->ln_Name);
							if(icon != NULL)
							{
								STRPTR priority;

								priority = FindToolType(icon->do_ToolTypes,"PRI");
								if(priority == NULL)
									priority = FindToolType(icon->do_ToolTypes,"PRIORITY");

								if(priority != NULL)
								{
									LONG value;

									if(StrToLong(priority,&value) > 0)
									{
										if(-128 <= value && value <= 127)
											node->ln_Pri = value;
									}
								}

								FreeDiskObject(icon);
							}

							add_interface_node(cc,&interface_list,node);
							interface_list_size++;
						}
					}

					error = MatchNext(cc->cc_AnchorPath);
				}

				if(error != OK && error != ERROR_NO_MORE_ENTRIES)
				{
					if(NOT cc->cc_Quiet)
						error_print_fault(cc,IoErr());

					result = RETURN_ERROR;

					goto out;
				}
			}
		}

		/* Now run through all the steps of the configuration process,
		   adding each interface in turn. */
		for(operating_mode = AIM_Add ; operating_mode <= AIM_Configure ; operating_mode++)
		{
			for(node = interface_list.lh_Head ; node->ln_Succ != NULL ; node = node->ln_Succ)
			{
				result = add_interface(cc,node->ln_Name,operating_mode,timeout);
				if(result != OK)
				{
					/* Fail only if there is one single interface involved. */
					if(interface_list_size == 1)
						goto out;

					configuration_trouble = TRUE;
				}
			}
		}
	}
	else
	{
		result = RETURN_OK;

		/* We may have to check an icon or two, so icon.library better be open. */
		if(IconBase == NULL)
		{
			if(NOT cc->cc_Quiet)
				error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NO_ICON_TXT));

			goto out;
		}

		/* We need at least one project icon to be invoked with this command. */
		if(cc->cc_StartupMessage->sm_NumArgs > 1)
		{
			D_S(struct FileInfoBlock,fib);
			int operating_mode;
			BOOL fib_valid;
			BPTR lock;
			LONG i;

			memset(fib,0,sizeof(fib));

			/* Run down the project icons. */
			for(i = 1 ; i < cc->cc_StartupMessage->sm_NumArgs ; i++)
			{
				/* These are the default settings which can be
				   overridden through icon tool types. */
				timeout = 60;
				cc->cc_Quiet = FALSE;

				/* Free the last icon we may have dealt with. */
				if(icon != NULL)
				{
					FreeDiskObject(icon);
					icon = NULL;
				}

				/* Remember the current directory we started with. It has
				   to be restored, eventually. */
				if(NOT old_cd_valid)
				{
					old_cd = CurrentDir(cc->cc_StartupMessage->sm_ArgList[i].wa_Lock);

					old_cd_valid = TRUE;
				}
				else
				{
					CurrentDir(cc->cc_StartupMessage->sm_ArgList[i].wa_Lock);
				}

				fib_valid = FALSE;

				/* Try to figure out what kind of object we are dealing
				   with and, if it's a file, check the icon tool types. */
				lock = Lock(cc->cc_StartupMessage->sm_ArgList[i].wa_Name,SHARED_LOCK);
				if(lock != ZERO)
				{
					if(Examine(lock,fib))
					{
						fib_valid = TRUE;

						if(icon == NULL)
						{
							icon = GetDiskObject(cc->cc_StartupMessage->sm_ArgList[i].wa_Name);
							if(icon != NULL)
							{
								STRPTR str;

								str = FindToolType(icon->do_ToolTypes,"QUIET");
								if(str != NULL)
									cc->cc_Quiet = TRUE;

								str = FindToolType(icon->do_ToolTypes,"TIMEOUT");
								if(str != NULL)
								{
									LONG val;

									if((StrToLong(str,&val) > 0) && (val > 0))
									{
										if(val < 10)
											val = 10;

										timeout = val;
									}
								}
							}
						}
					}

					UnLock(lock);
				}

				/* If we found a likely configuration file, take care of it. */
				if(fib_valid && (fib->fib_DirEntryType < 0))
				{
					/* Run through the configuration steps for this icon. */
					for(operating_mode = AIM_Add ; operating_mode <= AIM_Configure ; operating_mode++)
					{
						result = add_interface(cc,cc->cc_StartupMessage->sm_ArgList[i].wa_Name,operating_mode,timeout);
						if(result != OK)
						{
							/* Fail only if there is one single interface involved. */
							if(cc->cc_StartupMessage->sm_NumArgs == 1)
								goto out;

							configuration_trouble = TRUE;
						}
					}
				}
			}
		}
	}

	if(configuration_trouble)
		result = RETURN_WARN;

 out:

	if(NOT IsListEmpty(&interface_list))
	{
		struct Node * node;

		while((node = RemHead(&interface_list)) != NULL)
			FreeVec(node);
	}

	if(old_cd_valid)
		CurrentDir(old_cd);

	if(icon != NULL)
		FreeDiskObject(icon);

	DeleteMsgPort(cc->cc_ReplyPort);

	if(cc->cc_Quiet && result != RETURN_OK)
		result = RETURN_WARN;

	FreeVec(buf);

	if(rda != NULL)
		FreeArgs(rda);

	return(result);
}

/****************************************************************************/

/* Check if a string reads as YES/NO or ON/OFF. */
STATIC LONG
match_key(struct CommandContext * cc,STRPTR what)
{
	DECLARE_UTILITYBASE(cc);

	LONG result;

	if(Stricmp(what,"YES") == SAME ||
	   Stricmp(what,"ON") == SAME)
	{
		result = TRUE;
	}
	else if (Stricmp(what,"NO") == SAME ||
	         Stricmp(what,"OFF") == SAME)
	{
		result = FALSE;
	}
	else
	{
		result = -1;
	}

	return(result);
}

/****************************************************************************/

/* Add a new tag item to a list being built. */
STATIC LONG
add_tag(struct CommandContext * cc,Tag tag,ULONG data)
{
	DECLARE_SYSBASE(cc);

	LONG error = OK;
	LONG which = -1;
	LONG i;

	for(i = 0 ; i < cc->cc_NumTags ; i++)
	{
		if(cc->cc_Tags[i].ti_Tag == tag)
		{
			which = i;
			break;
		}
	}

	if(which == -1 && cc->cc_NumTags + 1 >= cc->cc_MaxTags)
	{
		struct TagItem * new_tags;

		new_tags = AllocVec(sizeof(*new_tags) * (cc->cc_MaxTags+10),MEMF_ANY|MEMF_PUBLIC);
		if(new_tags == NULL)
		{
			error = ERROR_NO_FREE_STORE;
			goto out;
		}

		if(cc->cc_Tags != NULL)
			CopyMem(cc->cc_Tags,new_tags,sizeof(*new_tags) * cc->cc_NumTags);

		FreeVec(cc->cc_Tags);
		cc->cc_Tags = new_tags;
		cc->cc_MaxTags += 10;
	}

	if(which == -1)
		which = cc->cc_NumTags++;

	cc->cc_Tags[which].ti_Tag	= tag;
	cc->cc_Tags[which].ti_Data	= data;

 out:

	return(error);
}

/****************************************************************************/

/* Check if a character is considered a blank space. This includes
   the ' ', '\t' and '\240' characters. */
STATIC BOOL
is_blank_space(UBYTE c)
{
	BOOL result;

	result = (BOOL)(c == ' ' || c == '\t' || c == (UBYTE)'\240');

	return(result);
}

/****************************************************************************/

/* Get the next token from a string; tokens are separated by blank spaces
   or the colon character. This is used for configuring the hardware
   address of an Ethernet interface. */
STATIC STRPTR
get_next_token(
	STRPTR	input,
	STRPTR	string,
	LONG	string_len)
{
	if((*input) == '\0')
	{
		input = NULL;
	}
	else
	{
		UBYTE c;

		while(is_blank_space(*input) || (*input) == ':')
			input++;

		while(TRUE)
		{
			c = (*input++);

			/* Stop at the end of the string or when a blank
			   space is found. */
			if(c == '\0')
			{
				input--;
				break;
			}
			else if (is_blank_space(c) || c == ':')
			{
				break;
			}

			if(string_len > 1)
			{
				(*string++) = c;
				string_len--;
			}
		}

		if(string_len > 0)
			(*string) = '\0';
	}

	return(input);
}

/****************************************************************************/

/* Convert a hexadecimal number into a 32 bit integer. */
STATIC LONG
get_hex_value(struct CommandContext * cc,STRPTR str)
{
	DECLARE_UTILITYBASE(cc);

	LONG result = -1;
	LONG value = 0;
	LONG len = 0;
	UBYTE c;

	while((c = ToUpper(*str++)) != '\0')
	{
		len++;
		if(len > 2)
			goto out;

		if('0' <= c && c <= '9')
			c = c - '0';
		else if ('A' <= c && c <= 'F')
			c = c - 'A' + 10;
		else
			goto out;

		value = (16 * value) + c;
	}

	result = value;

 out:

	return(result);
}

/****************************************************************************/

/* Turn an 8 bit integer into a hexadecimal string. */
STATIC VOID
get_hex_string(LONG value,STRPTR str)
{
	STATIC UBYTE code[] = "0123456789ABCDEF";

	str[0] = code[(value & 0xF0) >> 4];
	str[1] = code[(value & 0x0F)     ];
	str[2] = '\0';
}

/****************************************************************************/

/* Remove all unprintable characters from a string. */
STATIC VOID
strip_unprintable_characters(STRPTR s)
{
	STRPTR t = s;
	UBYTE c;

	while((c = (*s++)) != '\0')
	{
		if((c >= ' ' || c == '\t') && (c < '\177' || c >= (UBYTE)'\240'))
			(*t++) = c;
	}

	(*t) = '\0';
}

/****************************************************************************/

/* Strip leading and trailing blank spaces from a string. */
STATIC VOID
strip_extra_blank_spaces(STRPTR s)
{
	LONG num_leading_spaces;
	LONG num_trailing_spaces;
	LONG len,i;

	len = strlen(s);

	num_leading_spaces = 0;

	for(i = 0 ; i < len ; i++)
	{
		if(NOT is_blank_space(s[i]))
			break;

		num_leading_spaces++;
	}

	num_trailing_spaces = 0;

	for(i = len-1 ; i >= 0 ; i--)
	{
		if(NOT is_blank_space(s[i]))
			break;

		num_trailing_spaces++;
	}

	if(num_trailing_spaces > 0)
	{
		len -= num_trailing_spaces;
		s[len] = '\0';
	}

	if(num_leading_spaces > 0)
		memmove(s,&s[num_leading_spaces],len+1);
}

/****************************************************************************/

/* This is the big one. It reads the configuration file and acts upon what it finds. */
STATIC LONG
add_interface(struct CommandContext * cc,STRPTR name,enum add_interface_mode_t operating_mode,LONG timeout)
{
	/* This is where the configuration file data will go. */
	struct
	{
		KEY		Device;
		NUMBER	Unit;
		NUMBER	IPType;
		NUMBER	ARPType;
		NUMBER	IPRequests;
		NUMBER	WriteRequests;
		NUMBER	ARPRequests;
		KEY		HardwareType; /* This is always ignored; it's only for the prefs editor to use! */
		KEY		Debug;
		KEY		Filter;
		KEY		PointToPoint;
		KEY		Multicast;
		KEY		DownGoesOffline;
		KEY		ReportOffline;
		KEY		RequiresInitDelay;
		KEY		CopyMode;
		KEY		Address;
		KEY *	Alias;
		KEY		NetMask;
		KEY		State;
		KEY		BroadcastAddress;
		KEY		DestinationAddress;
		KEY		HardwareAddress;
		NUMBER	Metric;
		NUMBER	MTU;
		KEY		Configure;
		KEY		Lease;
		KEY		ID;
	} args;

	/* This command template corresponds to the parameter definitions above. */
	STRPTR args_template =
		"DEVICE/K,"
		"UNIT/K/N,"
		"IPTYPE/K/N,"
		"ARPTYPE/K/N,"
		"IPREQUESTS/K/N,"
		"WRITEREQUESTS/K/N,"
		"ARPREQUESTS/K/N,"
		"HARDWARETYPE/K,"
		"DEBUG/K,"
		"FILTER/K,"
		"POINTTOPOINT/K,"
		"MULTICAST/K,"
		"DOWNGOESOFFLINE/K,"
		"REPORTOFFLINE/K,"
		"REQUIRESINITDELAY/K,"
		"COPYMODE/K,"
		"ADDRESS/K,"
		"ALIAS/K/M,"
		"NETMASK/K,"
		"STATE/K,"
		"BROADCASTADDRESS/K,"
		"DESTINATION=DESTINATIONADDRESS/K,"
		"HARDWAREADDRESS/K,"
		"METRIC/K/N,"
		"MTU/K/N,"
		"CONFIGURE/K,"
		"LEASE/K,"
		"ID/K";

	DECLARE_SYSBASE(cc);
	DECLARE_DOSBASE(cc);
	DECLARE_UTILITYBASE(cc);
	DECLARE_SOCKETBASE(cc);

	STRPTR hardware_address = NULL;
	struct InterfaceHardwareAddress iha;
	LONG interface_state = SM_Up;
	STRPTR address = NULL;
	STRPTR net_mask = NULL;
	STRPTR broadcast_address = NULL;
	STRPTR destination_address = NULL;
	LONG metric = 0;
	STRPTR device_name = NULL;
	LONG device_unit = 0;
	LONG result = RETURN_FAIL;
	STRPTR interface_name = FilePart(name);
	UBYTE default_domain_name[256];
	UBYTE str[512];
	UBYTE *s;
	UBYTE *t;
	UBYTE *v;
	LONG line_number;
	LONG error;
	LONG len;
	LONG i;
	LONG separator_index;
	UBYTE separator_char;
	BOOL configure_dynamic = FALSE;
	BOOL configure_auto = FALSE;
	BOOL configure_slow_auto = FALSE;
	ULONG lease_time = DHCP_DEFAULT_LEASE_TIME;
	STRPTR client_id = NULL;
	struct List alias_list;
	struct Node * node;

	NewList(&alias_list);

	memset(&iha,0,sizeof(iha));

	/* Try to open the configuration file. */
	cc->cc_File = Open(name,MODE_OLDFILE);
	if(cc->cc_File == ZERO)
	{
		if(NOT cc->cc_Quiet)
		{
			error = IoErr();

			Fault(error,NULL,str,sizeof(str));

			error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_CANNOT_OPEN_FILE_TXT),
				name,str);
		}

		goto out;
	}

	/* Use a larger buffer to make reading from the configuration
	   file a little more efficient. */
	SetVBuf(cc->cc_File,NULL,BUF_LINE,sizeof(str));

	/* Free any configuration items allocated before. */
	FreeVec(cc->cc_Tags);
	cc->cc_Tags = NULL;

	cc->cc_MaxTags = cc->cc_NumTags = 0;

	line_number = 0;

	/* Read the configuration file line by line. */
	while(FGets(cc->cc_File,str,sizeof(str)-2) != NULL)
	{
		line_number++;

		/* Drop anything from the line that we don't want. */
		strip_unprintable_characters(str);

		/* Skip leading blank spaces. */
		s = str;
		while(is_blank_space(*s))
			s++;

		/* A # or ; introduces a comment. We just ignore that. */
		if((*s) == '#' || (*s) == ';')
			continue;

		/* Drop trailing blank spaces. */
		len = strlen(s);
		while(len > 0 && (is_blank_space(s[len-1]) || s[len-1] == '\r' || s[len-1] == '\n'))
			len--;

		if(len == 0)
			continue;

		s[len] = '\0';

		/* There's a key and a value in this line. The key and the value
		   are separated by blank spaces or a '=' character. */
		t = v = s;

		/* Isolate the key; the key is everything that's not a blank
	       space or a '=' character. */
		while(NOT is_blank_space(*t) && (*t) != '=' && (*t) != '\0')
		{
			v++;
			t++;
		}

		/* Find the value. */
		if((*t) != '\0')
		{
			(*v++) = ' ';

			t++;

			while(is_blank_space(*t) || (*t) == '=')
				t++;

			while((*t) != '\0')
				(*v++) = (*t++);
		}

		(*v) = '\0';

		/* Figure out where the separator character (blank space or '=') is. */
		len = strlen(s);

		separator_index = -1;
		separator_char = '\0';

		for(i = 0 ; i < len ; i++)
		{
			if(is_blank_space(s[i]) || s[i] == '=')
			{
				separator_index	= i;
				separator_char	= s[i];

				/* Chop off the string after the key. */
				s[i] = '\0';
				break;
			}
		}

		/* Check if we know the key for this line. */
		if(FindArg(args_template,s) == -1)
		{
			/* We don't know it, so we'll ignore it. */
			if(NOT cc->cc_Quiet)
			{
				warning_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_UNKNOWN_KEYWORD_TXT),
					s,line_number,name);
			}

			continue;
		}

		/* Restore the character following the key. */
		if(separator_index != -1)
			s[separator_index] = separator_char;

		s[len++]	= '\n';
		s[len]		= '\0';

		/* Process what's in this line. */
		FreeArgs(cc->cc_RDA);
		memset(&args,0,sizeof(args));

		cc->cc_RDA->RDA_Source.CS_Buffer = s;
		cc->cc_RDA->RDA_Source.CS_Length = len;
		cc->cc_RDA->RDA_Source.CS_CurChr = 0;
		cc->cc_RDA->RDA_Flags |= RDAF_NOPROMPT;

		if(CANNOT ReadArgs((STRPTR)args_template,(LONG *)&args,cc->cc_RDA))
		{
			if(NOT cc->cc_Quiet)
			{
				error = IoErr();

				Fault(error,NULL,str,sizeof(str));

				error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_PARSE_ERROR_TXT),
					line_number,name,str);
			}

			goto out;
		}

		/* Did we get a device name? */
		if(args.Device != NULL)
		{
			FreeVec(device_name);

			device_name = AllocVec(strlen(args.Device)+1,MEMF_ANY|MEMF_PUBLIC);
			if(device_name == NULL)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
						line_number,name);
				}

				goto out;
			}

			strcpy(device_name,args.Device);
		}

		/* Did we get a unit number? */
		if(args.Unit != NULL)
			device_unit = (*args.Unit);

		/* Did we get an IP packet type? */
		if(args.IPType != NULL)
		{
			error = add_tag(cc,IFA_IPType,(*args.IPType));
			if(error != OK)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
						line_number,name);
				}

				goto out;
			}
		}

		/* Did we get an ARP packet type? */
		if(args.ARPType != NULL)
		{
			error = add_tag(cc,IFA_ARPType,(*args.ARPType));
			if(error != OK)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
						line_number,name);
				}

				goto out;
			}
		}

		/* Did we get a number for the total number of IP requests? */
		if(args.IPRequests != NULL)
		{
			error = add_tag(cc,IFA_NumReadRequests,(*args.IPRequests));
			if(error != OK)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
						line_number,name);
				}

				goto out;
			}
		}

		/* Did we get a number for the total number of write requests? */
		if(args.WriteRequests != NULL)
		{
			error = add_tag(cc,IFA_NumWriteRequests,(*args.WriteRequests));
			if(error != OK)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
						line_number,name);
				}

				goto out;
			}
		}

		/* Did we get a number for the total number of ARP requests? */
		if(args.ARPRequests != NULL)
		{
			error = add_tag(cc,IFA_NumARPRequests,(*args.ARPRequests));
			if(error != OK)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
						line_number,name);
				}

				goto out;
			}
		}

		/* Did we get a filter mode? */
		if(args.Filter != NULL)
		{
			LONG mode;

			/* What is the filter set to? */
			if(Stricmp(args.Filter,"OFF") == SAME)
				mode = PFM_Nothing;
			else if (Stricmp(args.Filter,"LOCAL") == SAME)
				mode = PFM_Local;
			else if (Stricmp(args.Filter,"IPANDARP") == SAME)
				mode = PFM_IPandARP;
			else if (Stricmp(args.Filter,"EVERYTHING") == SAME)
				mode = PFM_Everything;
			else
				mode = -1;

			if(mode < 0)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_UNKNOWN_PARAMETER_TXT),
						args.Filter,line_number,name);
				}

				goto out;
			}

			error = add_tag(cc,IFA_PacketFilterMode,mode);
			if(error != OK)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
						line_number,name);
				}

				goto out;
			}
		}

		/* Is this a point-to-point device? */
		if(args.PointToPoint != NULL)
		{
			LONG mode;

			mode = match_key(cc,args.PointToPoint);
			if(mode < 0)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_UNKNOWN_PARAMETER_TXT),
						args.PointToPoint,line_number,name);
				}

				goto out;
			}

			error = add_tag(cc,IFA_PointToPoint,mode);
			if(error != OK)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
						line_number,name);
				}

				goto out;
			}
		}

		/* Enable or disable debug mode on this interface? */
		if(args.Debug != NULL)
		{
			LONG mode;

			mode = match_key(cc,args.Debug);
			if(mode < 0)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_UNKNOWN_PARAMETER_TXT),
						args.Debug,line_number,name);
				}

				goto out;
			}

			error = add_tag(cc,IFA_SetDebugMode,mode);
			if(error != OK)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
						line_number,name);
				}

				goto out;
			}
		}

		/* Is this interface capable of multicasts? */
		if(args.Multicast != NULL)
		{
			LONG mode;

			mode = match_key(cc,args.Multicast);
			if(mode < 0)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_UNKNOWN_PARAMETER_TXT),
						args.Multicast,line_number,name);
				}

				goto out;
			}

			error = add_tag(cc,IFA_Multicast,mode);
			if(error != OK)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
						line_number,name);
				}

				goto out;
			}
		}

		/* Should marking the interface "down" also take it offline? */
		if(args.DownGoesOffline != NULL)
		{
			LONG mode;

			mode = match_key(cc,args.DownGoesOffline);
			if(mode < 0)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_UNKNOWN_PARAMETER_TXT),
						args.DownGoesOffline,line_number,name);
				}

				goto out;
			}

			error = add_tag(cc,IFA_DownGoesOffline,mode);
			if(error != OK)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
						line_number,name);
				}

				goto out;
			}
		}

		/* Report when the interface goes offline? */
		if(args.ReportOffline != NULL)
		{
			LONG mode;

			mode = match_key(cc,args.ReportOffline);
			if(mode < 0)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_UNKNOWN_PARAMETER_TXT),
						args.ReportOffline,line_number,name);
				}

				goto out;
			}

			error = add_tag(cc,IFA_ReportOffline,mode);
			if(error != OK)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
						line_number,name);
				}

				goto out;
			}
		}

		/* Is a small delay required after initializing the interface? */
		if(args.RequiresInitDelay != NULL)
		{
			LONG mode;

			mode = match_key(cc,args.RequiresInitDelay);
			if(mode < 0)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_UNKNOWN_PARAMETER_TXT),
						args.RequiresInitDelay,line_number,name);
				}

				goto out;
			}

			error = add_tag(cc,IFA_RequiresInitDelay,mode);
			if(error != OK)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
						line_number,name);
				}

				goto out;
			}
		}

		/* Use a particular copying mode? */
		if(args.CopyMode != NULL)
		{
			LONG mode;

			if(Stricmp(args.CopyMode,"SLOW") == SAME)
				mode = CM_SlowWordCopy;
			else if (Stricmp(args.CopyMode,"FAST") == SAME)
				mode = CM_FastWordCopy;
			else
				mode = -1;

			if(mode < 0)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_UNKNOWN_PARAMETER_TXT),
						args.CopyMode,line_number,name);
				}

				goto out;
			}

			error = add_tag(cc,IFA_CopyMode,mode);
			if(error != OK)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
						line_number,name);
				}

				goto out;
			}
		}

		/* Use a particular IP address, or DHCP? */
		if(args.Address != NULL)
		{
			struct in_addr in;

			/* Is this an IP address? */
			if(Stricmp(args.Address,"DHCP") != SAME)
			{
				/* Is is a well-formed address? */
				if(CANNOT validate_ip_address(cc,"ADDRESS",args.Address,line_number,name))
					goto out;
			}

			/* If this is an IP address, have a closer look at it. */
			if(inet_aton(args.Address,&in))
			{
				/* Neither 0 nor the broadcast address are permitted. */
				if(in.s_addr == 0 || in.s_addr == ~0UL)
				{
					if(NOT cc->cc_Quiet)
					{
						error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_INVALID_ADDRESS_TXT),
							args.Address,line_number,name);
					}

					goto out;
				}

				/* And no reserved range either. */
				if(((in.s_addr >> 24) & 0xFF) == 169 &&
				   ((in.s_addr >> 16) & 0xFF) == 254)
				{
					warning_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_DYNAMIC_ADDRESS_SHOULD_NOT_BE_BOUND_TXT),
						args.Address,line_number,name);
				}
			}

			FreeVec(address);

			address = AllocVec(strlen(args.Address)+1,MEMF_ANY|MEMF_PUBLIC);
			if(address == NULL)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
						line_number,name);
				}

				goto out;
			}

			strcpy(address,args.Address);
		}

		/* Set the interface to a particular state? */
		if(args.State != NULL)
		{
			if(Stricmp(args.State,"UP") == SAME)
			{
				interface_state = SM_Up;
			}
			else if (Stricmp(args.State,"DOWN") == SAME)
			{
				interface_state = SM_Down;
			}
			else if (Stricmp(args.State,"ONLINE") == SAME)
			{
				interface_state = SM_Online;
			}
			else if (Stricmp(args.State,"OFFLINE") == SAME)
			{
				interface_state = SM_Offline;
			}
			else
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_UNKNOWN_STATE_PARAMETER_TXT),
						args.State,line_number);
				}

				goto out;
			}
		}

		/* Use a particular broadcast address? */
		if(args.BroadcastAddress != NULL)
		{
			if(CANNOT validate_ip_address(cc,"BROADCASTADDRESS",args.BroadcastAddress,line_number,name))
				goto out;

			FreeVec(broadcast_address);

			broadcast_address = AllocVec(strlen(args.BroadcastAddress)+1,MEMF_ANY|MEMF_PUBLIC);
			if(broadcast_address == NULL)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
						line_number,name);
				}

				goto out;
			}

			strcpy(broadcast_address,args.BroadcastAddress);
		}

		/* For a point-to-point address, use a particular destination address for the peer? */
		if(args.DestinationAddress != NULL)
		{
			if(CANNOT validate_ip_address(cc,"DESTINATIONADDRESS",args.DestinationAddress,line_number,name))
				goto out;

			FreeVec(destination_address);

			destination_address = AllocVec(strlen(args.DestinationAddress)+1,MEMF_ANY|MEMF_PUBLIC);
			if(destination_address == NULL)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
						line_number,name);
				}

				goto out;
			}

			strcpy(destination_address,args.DestinationAddress);
		}

		/* Use a particular hardware address? */
		if(args.HardwareAddress != NULL)
		{
			UBYTE token[40];
			UBYTE buf[10];
			STRPTR arg;
			LONG num_tokens;
			LONG i;

			/* Collect individual tokens; these are octet separated by
			   blank spaces or ':' characters. */
			num_tokens = 0;

			arg = args.HardwareAddress;
			while((arg = get_next_token(arg,token,sizeof(token))) != NULL)
			{
				if(get_hex_value(cc,token) < 0)
				{
					if(NOT cc->cc_Quiet)
					{
						error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_INVALID_HARDWARE_ADDRESS_TXT),
							args.HardwareAddress,line_number,name);
					}

					goto out;
				}

				num_tokens++;
			}

			/* Complain if we didn't get anything. */
			if(num_tokens == 0)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_MISSING_HARDWARE_ADDRESS_TXT),
						line_number,name);
				}

				goto out;
			}

			FreeVec(iha.iha_Address);
			FreeVec(hardware_address);

			/* Allocate memory for the address. */
			iha.iha_Length = num_tokens * 8;
			iha.iha_Address = AllocVec(num_tokens,MEMF_ANY|MEMF_PUBLIC|MEMF_CLEAR);
			if(iha.iha_Address == NULL)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_FOR_HARDWARE_ADDRESS_TXT),
						args.HardwareAddress);
				}

				goto out;
			}

			hardware_address = AllocVec(3 * num_tokens,MEMF_ANY|MEMF_PUBLIC|MEMF_CLEAR);
			if(hardware_address == NULL)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_FOR_HARDWARE_ADDRESS_TXT),
						args.HardwareAddress);
				}

				goto out;
			}

			/* Now parse the individual octets. */
			i = 0;
			arg = args.HardwareAddress;
			while((arg = get_next_token(arg,token,sizeof(token))) != NULL)
			{
				iha.iha_Address[i] = get_hex_value(cc,token);

				get_hex_string(iha.iha_Address[i],buf);
				if(i != 0)
					strcat(hardware_address,":");

				strcat(hardware_address,buf);

				i++;
			}

			/* Assuming that this is probably an IEEE 802.3 family
			   address, we're going to check for a few likely
			   configuration errors due to poor choice of
			   address. */
			if(NOT cc->cc_Quiet)
			{
				if(num_tokens < 6)
				{
					warning_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_HARDWARE_ADDRESS_TOO_SHORT_TXT),
						args.HardwareAddress,line_number,name);
				}
				else if (num_tokens > 6)
				{
					warning_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_HARDWARE_ADDRESS_TOO_LONG_TXT),
						args.HardwareAddress,line_number,name);
				}

				if(FLAG_IS_SET(iha.iha_Address[0],MAC_Is_Group_Address))
				{
					BOOL is_broadcast_address = TRUE;

					for(i = 0 ; i < num_tokens ; i++)
					{
						if(iha.iha_Address[i] != 0xFF)
						{
							is_broadcast_address = FALSE;
							break;
						}
					}

					if(is_broadcast_address)
					{
						warning_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_HARDWARE_ADDRESS_IS_BROADCAST_TXT),
							args.HardwareAddress,line_number,name);
					}
					else
					{
						warning_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_HARDWARE_ADDRESS_IS_GROUP_TXT),
							args.HardwareAddress,line_number,name);
					}
				}
			}
		}

		/* Use a particular netmask, or DHCP? */
		if(args.NetMask != NULL)
		{
			/* Is this an IP address? */
			if(Stricmp(args.NetMask,"DHCP") != SAME)
			{
				/* Is is a well-formed address? */
				if(CANNOT validate_ip_address(cc,"NETMASK",args.NetMask,line_number,name))
					goto out;
			}

			FreeVec(net_mask);

			net_mask = AllocVec(strlen(args.NetMask)+1,MEMF_ANY|MEMF_PUBLIC);
			if(net_mask == NULL)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
						line_number,name);
				}

				goto out;
			}

			strcpy(net_mask,args.NetMask);
		}

		/* Use a particular routing metric? */
		if(args.Metric != NULL)
			metric = (*args.Metric);

		/* Use a particular MTU value? */
		if(args.MTU != NULL && (*args.MTU) > 0)
		{
			error = add_tag(cc,IFA_LimitMTU,(*args.MTU));
			if(error != OK)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
						line_number,name);
				}

				goto out;
			}
		}

		/* Use a particular configuration method? */
		if(args.Configure != NULL)
		{
			if(Stricmp(args.Configure,"DHCP") == SAME)
			{
				/* Dynamic Host Configuration Protocol */
				configure_dynamic = TRUE;
				configure_auto = FALSE;
			}
			else if (Stricmp(args.Configure,"SLOWAUTO") == SAME || Stricmp(args.Configure,"AUTO") == SAME)
			{
				/* ZeroConf; "slow" method for spanning tree routing. */
				configure_dynamic = configure_auto = configure_slow_auto = TRUE;
			}
			else if (Stricmp(args.Configure,"FASTAUTO") == SAME)
			{
				/* ZeroConf; "fast" method for wireless applications, etc. */
				configure_dynamic = configure_auto = TRUE;
				configure_slow_auto = FALSE;
			}
			else
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_UNKNOWN_PARAMETER_TXT),
						args.Configure,line_number,name);
				}

				goto out;
			}
		}

		/* Ask for a particular lease time? */
		if(args.Lease != NULL)
		{
			STRPTR s = args.Lease;
			STRPTR key = NULL;
			LONG number;
			LONG len;

			/* Skip all leading blank spaces. */
			while(is_blank_space(*s))
				s++;

			/* If it's a number, convert it now. */
			len = 0;
			number = 0;
			while('0' <= (*s) && (*s) <= '9')
			{
				number = (10 * number) + (*s) - '0';
				len++;
				s++;
			}

			/* Skip all following blank spaces, if any. */
			while(is_blank_space(*s))
				s++;

			if(s[0] != '\0')
				key = s;

			/* Check if there's a keyword instead. */
			if(len == 0 && key != NULL)
			{
				/* This should be "infinite", "infinity" or
				   simply "inf". */
				if(substring_matches(cc,"INF",key) == SAME)
				{
					lease_time = DHCP_INFINITE_LEASE_TIME;
				}
				else
				{
					if(NOT cc->cc_Quiet)
					{
						error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_UNKNOWN_LEASE_PARAMETER_TXT),
							key,line_number,name);
					}

					goto out;
				}
			}
			else if (len > 0)
			{
				lease_time = number;

				/* This could be qualifier, which stands for seconds,
				   minutes, hours, days or weeks. */
				if(key != NULL)
				{
					if(substring_matches(cc,"SECOND",key) == SAME)
					{
						/* This is the default unit */
					}
					else if (substring_matches(cc,"MINUTE",key) == SAME)
					{
						lease_time *= 60;
					}
					else if (substring_matches(cc,"HOUR",key) == SAME)
					{
						lease_time *= 60 * 60;
					}
					else if (substring_matches(cc,"DAY",key) == SAME)
					{
						lease_time *= 24 * 60 * 60;
					}
					else if (substring_matches(cc,"WEEK",key) == SAME)
					{
						lease_time *= 7 * 24 * 60 * 60;
					}
					else
					{
						if(NOT cc->cc_Quiet)
						{
							error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_UNKNOWN_LEASE_PARAMETER_TXT),
								key,line_number,name);
						}

						goto out;
					}
				}
			}
			else
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_INVALID_LEASE_PARAMETER_TXT),
						args.Lease,line_number,name);
				}

				goto out;
			}
		}

		/* Use a particular client ID (for DHCP)? */
		if(args.ID != NULL)
		{
			LONG len;

			len = strlen(args.ID);

			if(len > 255 && NOT cc->cc_Quiet)
			{
				warning_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_ID_TOO_LONG_TXT),
					args.ID,line_number,name);
			}

			if(len < 2)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_ID_TOO_SHORT_TXT),
						args.ID,line_number,name);
				}

				goto out;
			}

			FreeVec(client_id);

			client_id = AllocVec(len+1,MEMF_ANY|MEMF_PUBLIC);
			if(client_id == NULL)
			{
				if(NOT cc->cc_Quiet)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_INTERFACE_NOT_CONFIGURED_NO_MEMORY_TXT),
						interface_name);
				}

				goto out;
			}

			strncpy(client_id,args.ID,len);
			client_id[len] = '\0';
		}

		/* Use an alias for this interface? */
		if(args.Alias != NULL)
		{
			STRPTR * alias = (STRPTR *)args.Alias;
			struct in_addr in;
			STRPTR address;

			while((address = (*alias++)) != NULL)
			{
				/* Check if the IP address is well-formed. */
				if(CANNOT validate_ip_address(cc,"ALIAS",address,line_number,name))
					goto out;

				/* Have a closer look at the address. */
				if(inet_aton(address,&in))
				{
					/* Don't permit 0 or the broadcast address. */
					if(in.s_addr == 0 || in.s_addr == ~0UL)
					{
						if(NOT cc->cc_Quiet)
						{
							error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_INVALID_ADDRESS_TXT),
								address,line_number,name);
						}

						goto out;
					}

					/* Don't use the reserved range. */
					if(((in.s_addr >> 24) & 0xFF) == 169 &&
					   ((in.s_addr >> 16) & 0xFF) == 254)
					{
						warning_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_DYNAMIC_ADDRESS_SHOULD_NOT_BE_BOUND_TXT),
							address,line_number,name);
					}
				}

				node = AllocVec(sizeof(*node) + strlen(address)+1,MEMF_ANY|MEMF_PUBLIC);
				if(node == NULL)
				{
					if(NOT cc->cc_Quiet)
					{
						error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_PARSE_TXT),
							line_number,name);
					}

					goto out;
				}

				node->ln_Name = (char *)(node + 1);
				strcpy(node->ln_Name,address);

				AddTail(&alias_list,node);
			}
		}
	}

	/* When adding the interface, use the provided hardware address. */
	if(operating_mode == AIM_Add && iha.iha_Address != NULL)
	{
		error = add_tag(cc,IFA_HardwareAddress,(ULONG)&iha);
		if(error != OK)
		{
			if(NOT cc->cc_Quiet)
			{
				error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_CONFIGURE_HARDWARE_ADDRESS_TXT),
					interface_name);
			}

			goto out;
		}
	}

	/* This concludes the parameter list. */
	error = add_tag(cc,TAG_END,0);
	if(error != OK)
	{
		if(NOT cc->cc_Quiet)
		{
			error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_ADD_INTERFACE_TXT),
				name);
		}

		goto out;
	}

	/* Stop right here if no device name was given. */
	if(device_name == NULL)
	{
		if(NOT cc->cc_Quiet)
		{
			error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NO_DEVICE_NAME_PROVIDED_TXT),
				name);
		}

		goto out;
	}

	/* In the "add" stage, add the interface with the parameters given. */
	if(operating_mode == AIM_Add)
	{
		if(AddInterfaceTagList(interface_name,device_name,device_unit,cc->cc_Tags) != 0)
		{
			if(NOT cc->cc_Quiet)
			{
				LONG errno;
				STRPTR code;
	
				get_errno_and_code(cc,&errno,&code);
	
				if(code != NULL && errno > 0)
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_COULD_NOT_ADD_INTERFACE_ERROR_CODE_TXT),
						interface_name,code);
				}
				else
				{
					error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_COULD_NOT_ADD_INTERFACE_TXT),
						interface_name);
				}
			}
	
			goto out;
		}

		if(NOT cc->cc_Quiet)
		{
			info_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_INTERFACE_ADDED_TXT),
				interface_name);

			if(hardware_address != NULL)
			{
				info_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_INTERFACE_HARDWARE_ADDRESS_SET_TO_TXT),
					interface_name,hardware_address);
			}
		}
	}
	else if (operating_mode == AIM_Configure)
	{
		/* In the configure state, set up the interface address and aliases. */
		if(address != NULL || configure_dynamic)
		{
			struct AddressAllocationMessage * aam = &cc->cc_AllocationMessage;
			BOOL want_address = (BOOL)(address != NULL && Stricmp(address,"DHCP") == SAME);
			BOOL want_netmask = (BOOL)(net_mask != NULL && Stricmp(net_mask,"DHCP") == SAME);

			/* Do we need to configure anything dynamically? */
			if(configure_dynamic || want_address || want_netmask)
			{
				LONG signals;

				/* Remember the client identifier. */
				aam->aam_ClientIdentifier = client_id;

				/* We have to have the reply port ready. */
				if(cc->cc_ReplyPort == NULL)
				{
					cc->cc_ReplyPort = CreateMsgPort();
					if(cc->cc_ReplyPort == NULL)
					{
						if(NOT cc->cc_Quiet)
							error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_NOT_ENOUGH_MEMORY_TO_CONFIGURE_INTERFACE_TXT),name);

						goto out;
					}
				}

				/* Before we begin, we will need to mark this
				   interface 'up' so that the protocol stack will
				   send messages through it. For some devices it
				   is not sufficient to mark the interface as up.
				   These also have to be switched online, too. */
				if(Local_ConfigureInterfaceTags(cc,interface_name,
					IFC_State,SM_Online,
				TAG_DONE) != 0)
				{
					if(NOT cc->cc_Quiet)
					{
						LONG errno;
						STRPTR code;

						get_errno_and_code(cc,&errno,&code);

						if(code != NULL && errno > 0)
						{
							error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_CANNOT_MARK_INTERFACE_UP_ERROR_CODE_TXT),
								interface_name,code);
						}
						else
						{
							error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_CANNOT_MARK_INTERFACE_UP_TXT),
								interface_name);
						}
					}

					goto out;
				}

				if(NOT cc->cc_Quiet)
				{
					if(cc->cc_StartupMessage == NULL)
					{
						info_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_TRYING_INTERFACE_CONFIGURATION_TXT),
							interface_name);

						Flush(Output());
					}
				}

				/* Initialize the message, then send it. */
				aam->aam_Message.mn_Node.ln_Type	= NT_REPLYMSG;
				aam->aam_Message.mn_ReplyPort		= cc->cc_ReplyPort;
				aam->aam_Message.mn_Length			= sizeof(*aam);

				memcpy(aam->aam_InterfaceName,interface_name,sizeof(aam->aam_InterfaceName)-1);
				aam->aam_InterfaceName[sizeof(aam->aam_InterfaceName)-1] = '\0';

				if(configure_auto)
				{
					if(configure_slow_auto)
						aam->aam_Protocol = AAMP_SLOWAUTO;
					else
						aam->aam_Protocol = AAMP_FASTAUTO;
				}
				else
				{
					aam->aam_Protocol = AAMP_DHCP;
				}

				aam->aam_Version			= AAM_VERSION;
				aam->aam_Timeout			= timeout;
				aam->aam_LeaseTime			= lease_time;
				aam->aam_RouterTable		= cc->cc_RouterTable;
				aam->aam_RouterTableSize	= NUM_ENTRIES(cc->cc_RouterTable);
				aam->aam_DNSTable			= cc->cc_DNSTable;
				aam->aam_DNSTableSize		= NUM_ENTRIES(cc->cc_DNSTable);
				aam->aam_LeaseExpires		= &cc->cc_LeaseExpires;
				aam->aam_DomainName			= default_domain_name;
				aam->aam_DomainNameSize		= sizeof(default_domain_name);

				strcpy(default_domain_name,"");

				if(address != NULL && Stricmp(address,"DHCP") != SAME)
					aam->aam_RequestedAddress = inet_addr(address);

				BeginInterfaceConfig(aam);

				/* Wait for something to happen. */
				signals = Wait(SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_F | (1UL << cc->cc_ReplyPort->mp_SigBit));

				/* ^C means "stop everything". */
				if(signals & SIGBREAKF_CTRL_C)
				{
					AbortInterfaceConfig(aam);
					WaitPort(cc->cc_ReplyPort);
					GetMsg(cc->cc_ReplyPort);

					if(NOT cc->cc_Quiet)
					{
						info_printf(cc,"\n");

						info_print_fault(cc,ERROR_BREAK);
					}

					goto out;
				}
				else if (signals & SIGBREAKF_CTRL_F)
				{
					/* ^F means "stop configuring this interface". */
					AbortInterfaceConfig(aam);
					WaitPort(cc->cc_ReplyPort);
					GetMsg(cc->cc_ReplyPort);

					if(NOT cc->cc_Quiet)
					{
						info_printf(cc,"\n");

						warning_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_INTERFACE_CONFIGURATION_ABORTED_TXT),
							interface_name);
					}

					result = RETURN_OK;
					goto out;
				}
				else
				{
					if(NOT cc->cc_Quiet)
						info_printf(cc,"\n");

					WaitPort(cc->cc_ReplyPort);
					GetMsg(cc->cc_ReplyPort);

					/* If that didn't work out, print an error message. */
					if(aam->aam_Result != AAMR_Success && NOT cc->cc_Quiet)
					{
						STATIC CONST error_map_table[][2] =
						{
							{ AAMR_Aborted,				MSG_ADDNETINTERFACE_INTERFACE_CONFIGURATION_ABORTED_TXT },
							{ AAMR_Timeout,				MSG_ADDNETINTERFACE_INTERFACE_CONFIGURATION_ATTEMPT_TIMED_OUT_TXT },
							{ AAMR_InterfaceNotKnown,	MSG_ADDNETINTERFACE_INTERFACE_NOT_CONFIGURED_TXT },
							{ AAMR_InterfaceWrongType,	MSG_ADDNETINTERFACE_INTERFACE_WRONG_TYPE_TXT },
							{ AAMR_AddressKnown,		MSG_ADDNETINTERFACE_INTERFACE_ALREADY_CONFIGURED_TXT },
							{ AAMR_VersionUnknown,		MSG_ADDNETINTERFACE_INTERFACE_CONFIGURATION_VERSION_CONFLICT_TXT },
							{ AAMR_NoMemory,			MSG_ADDNETINTERFACE_INTERFACE_NOT_CONFIGURED_NO_MEMORY_TXT },
							{ AAMR_AddressInUse,		MSG_ADDNETINTERFACE_INTERFACE_NOT_CONFIGURED_ADDRESS_IN_USE_TXT },
							{ AAMR_AddrChangeFailed,	MSG_ADDNETINTERFACE_INTERFACE_NOT_CONFIGURED_ADDRESS_CHANGE_FAILED_TXT },
							{ AAMR_MaskChangeFailed,	MSG_ADDNETINTERFACE_INTERFACE_NOT_CONFIGURED_MASK_CHANGE_FAILED_TXT },
							{ AAMR_Busy,				MSG_ADDNETINTERFACE_INTERFACE_IS_BUSY_TXT },
							{ -1,						-1}
						};

						LONG message_code;
						size_t i;

						message_code = MSG_ADDNETINTERFACE_INTERFACE_NOT_CONFIGURED_UNKNOWN_ERROR_TXT;

						for(i = 0 ; error_map_table[i][0] != -1 ; i++)
						{
							if(aam->aam_Result == error_map_table[i][0])
							{
								message_code = error_map_table[i][1];
								break;
							}
						}

						error_printf(cc,get_str(cc,message_code),
							interface_name,aam->aam_Result);
					}

					/* Even if an error occured, don't stop the show. */
					if(aam->aam_Result != AAMR_Success)
					{
						result = RETURN_OK;
						goto out;
					}

					/* Check if we got the required IP address. */
					if(configure_dynamic || want_address)
					{
						UBYTE str[20];

						/* Did we really get something? */
						if(aam->aam_Address == 0)
						{
							if(NOT cc->cc_Quiet)
							{
								error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_INTERFACE_NOT_CONFIGURED_INVALID_ADDRESS_TXT),
									interface_name);
							}

							goto out;
						}

						/* Remember the address. */
						strcpy(str,Inet_NtoA(aam->aam_Address));

						FreeVec(address);
						address = AllocVec(strlen(str)+1,MEMF_ANY|MEMF_PUBLIC);
						if(address == NULL)
						{
							if(NOT cc->cc_Quiet)
							{
								error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_INTERFACE_NOT_CONFIGURED_NO_MEMORY_TXT),
									interface_name);
							}

							goto out;
						}

						strcpy(address,str);
					}

					/* Check if we got the required subnet mask. */
					if(configure_dynamic || want_netmask)
					{
						/* Did we really get something? */
						if(aam->aam_SubnetMask == 0x00000000 ||
						   aam->aam_SubnetMask == 0xFFFFFFFF)
						{
							FreeVec(net_mask);
							net_mask = NULL;
						}
						else
						{
							UBYTE str[20];

							/* Remember the subnet mask. */
							strcpy(str,Inet_NtoA(aam->aam_SubnetMask));

							FreeVec(net_mask);
							net_mask = AllocVec(strlen(str)+1,MEMF_ANY|MEMF_PUBLIC);
							if(net_mask == NULL)
							{
								if(NOT cc->cc_Quiet)
								{
									error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_INTERFACE_NOT_CONFIGURED_NO_MEMORY_TXT),
										interface_name);
								}

								goto out;
							}

							strcpy(net_mask,str);
						}
					}

					/* Validate the router and DNS address tables. */
					if(configure_dynamic)
					{
						LONG i,n;

						n = 0;
						for(i = 0 ; i < aam->aam_RouterTableSize ; i++)
						{
							if(aam->aam_RouterTable[i] != 0)
								n++;
						}

						if(n == 0)
							aam->aam_RouterTable = NULL;

						n = 0;
						for(i = 0 ; i < aam->aam_DNSTableSize ; i++)
						{
							if(aam->aam_DNSTable[i] != 0)
								n++;
						}

						if(n == 0)
							aam->aam_DNSTable = NULL;
					}
				}
			}

			/* Finally, configure the interface using the information obtained above. */
			if(Local_ConfigureInterfaceTags(cc,interface_name,
				(NOT (configure_dynamic || want_address) && address != NULL) ?	IFC_Address : TAG_IGNORE,	address,
				(NOT (configure_dynamic || want_netmask) && net_mask != NULL) ?	IFC_NetMask : TAG_IGNORE,	net_mask,

				(broadcast_address != NULL) ?	IFC_BroadcastAddress : TAG_IGNORE,		broadcast_address,
				(destination_address != NULL) ?	IFC_DestinationAddress : TAG_IGNORE,	destination_address,
				(metric != 0) ?					IFC_Metric : TAG_IGNORE,				metric,
												IFC_State,								interface_state,
				IFC_Complete,					TRUE,
			TAG_DONE) != 0)
			{
				if(NOT cc->cc_Quiet)
				{
					LONG errno;
					STRPTR code;

					get_errno_and_code(cc,&errno,&code);

					if(code != NULL && errno > 0)
					{
						error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_COULD_NOT_CONFIGURE_INTERFACE_ERROR_CODE_TXT),
							interface_name,code);
					}
					else
					{
						error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_COULD_NOT_CONFIGURE_INTERFACE_TXT),
							interface_name);
					}
				}

				goto out;
			}

			/* Unless we shouldn't do so, say what we did above. */
			if(NOT cc->cc_Quiet)
			{
				info_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_INTERFACE_CONFIGURED_TXT),
					interface_name);

				if(address != NULL)
					info_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_INTERFACE_CONFIGURED_ADDRESS_TXT),address);

				if(net_mask != NULL)
					info_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_INTERFACE_CONFIGURED_MASK_TXT),net_mask);

				if(broadcast_address != NULL)
					info_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_INTERFACE_CONFIGURED_BROADCAST_ADDRESS_TXT),broadcast_address);

				if(destination_address != NULL)
					info_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_INTERFACE_CONFIGURED_DESTINATION_ADDRESS_TXT),destination_address);

				if(metric != 0)
					info_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_INTERFACE_CONFIGURED_METRIC_TXT),metric);

				info_printf(cc,".\n");
			}

			/* Check if we got any router information from the server. */
			if(configure_dynamic && aam->aam_RouterTable != NULL)
			{
				LONG i,n;

				n = 0;
				for(i = 0 ; i < aam->aam_RouterTableSize ; i++)
				{
					if(aam->aam_RouterTable[i] != 0)
					{
						UBYTE str[20];
						LONG error = OK;
						STRPTR code = NULL;

						/* Use this router address. */
						strcpy(str,Inet_NtoA(aam->aam_RouterTable[i]));

						/* The first is the default gateway address. */
						if(n == 0)
						{
							if(Local_AddRouteTags(cc,
								RTA_DefaultGateway,str,
							TAG_DONE) != OK)
							{
								get_errno_and_code(cc,&error,&code);
							}

							/* Say what we did. */
							if(error == OK && NOT cc->cc_Quiet)
								info_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_DEFAULT_ROUTE_TXT),str);
						}
						else
						{
							/* The second is a destination address. */
							if(Local_AddRouteTags(cc,
								RTA_Destination,str,
							TAG_DONE) != OK)
							{
								get_errno_and_code(cc,&error,&code);
							}

							/* Say what we did. */
							if(error == OK && NOT cc->cc_Quiet)
								info_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_ROUTE_TXT),str);
						}

						/* Print an error message if that didn't work. */
						if(error != OK)
						{
							if(NOT cc->cc_Quiet)
							{
								LONG message_code;

								if(code != NULL && error > 0)
									message_code = MSG_ADDNETINTERFACE_COULD_NOT_ADD_ROUTE_ERROR_CODE_TXT;
								else
									message_code = MSG_ADDNETINTERFACE_COULD_NOT_ADD_ROUTE_TXT;

								error_printf(cc,get_str(cc,message_code),str,code);
							}

							goto out;
						}

						n++;
					}
				}
			}

			/* Check if we got a default domain name to use for DNS lookups. */
			if(configure_dynamic)
			{
				STRPTR domain_name;

				if(aam->aam_DomainName != NULL && aam->aam_DomainName[0] != '\0')
					domain_name = aam->aam_DomainName;
				else
					domain_name = "";

				SetDefaultDomainName(domain_name);

				if(domain_name[0] != '\0' && NOT cc->cc_Quiet)
				{
					info_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_DEFAULT_DOMAIN_NAME_TXT),
						domain_name);
				}
			}

			/* Check if we got a DNS server table to use. */
			if(configure_dynamic && aam->aam_DNSTable != NULL)
			{
				LONG i;

				for(i = 0 ; i < aam->aam_DNSTableSize ; i++)
				{
					if(aam->aam_DNSTable[i] != 0)
					{
						UBYTE str[20];
						LONG error = OK;
						STRPTR code = NULL;

						strcpy(str,Inet_NtoA(aam->aam_DNSTable[i]));

						if(AddDomainNameServer(str) != OK)
							get_errno_and_code(cc,&error,&code);

						/* Say what we did. */
						if(error == OK && NOT cc->cc_Quiet)
							info_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_DNS_TXT),str);

						if(error != OK)
						{
							if(NOT cc->cc_Quiet)
							{
								LONG message_code;

								if(code != NULL && error > 0)
									message_code = MSG_ADDNETINTERFACE_COULD_NOT_ADD_DNS_ERROR_CODE_TXT;
								else
									message_code = MSG_ADDNETINTERFACE_COULD_NOT_ADD_DNS_TXT;

								error_printf(cc,get_str(cc,message_code),str,code);
							}

							goto out;
						}
					}
				}
			}

			/* If we received a lease from a DHCP server, say how long it will last. */
			if((configure_dynamic || want_address) && aam->aam_Protocol == AAMP_DHCP && NOT cc->cc_Quiet)
			{
				info_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_ADDRESS_TXT),
					interface_name,address);

				if(cc->cc_LeaseExpires.ds_Days		== 0 &&
				   cc->cc_LeaseExpires.ds_Minute	== 0 &&
				   cc->cc_LeaseExpires.ds_Tick		== 0)
				{
					info_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_LEASED_PERMANENTLY_TXT));
				}
				else
				{
					struct DateTime dat;
					UBYTE date[LEN_DATSTRING+1];
					UBYTE time[LEN_DATSTRING+1];

					memset(&dat,0,sizeof(dat));

					dat.dat_Stamp	= cc->cc_LeaseExpires;
					dat.dat_Format	= FORMAT_DEF;
					dat.dat_StrDate	= date;
					dat.dat_StrTime	= time;

					DateToStr(&dat);

					strip_extra_blank_spaces(date);
					strip_extra_blank_spaces(time);

					info_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_LEASED_UNTIL_TXT),
						date,time);
				}
			}
		}

		/* Finally, try to add alias addresses for this interface. */
		if(NOT IsListEmpty(&alias_list))
		{
			for(node = alias_list.lh_Head ;
			    node->ln_Succ != NULL ;
			    node = node->ln_Succ)
			{
				if(Local_ConfigureInterfaceTags(cc,interface_name,
					IFC_AddAliasAddress,node->ln_Name,
				TAG_DONE) != 0)
				{
					if(NOT cc->cc_Quiet)
					{
						LONG message_code;
						LONG errno;
						STRPTR code;

						get_errno_and_code(cc,&errno,&code);

						if(code != NULL && errno > 0)
							message_code = MSG_ADDNETINTERFACE_COULD_NOT_ADD_ALIAS_ERROR_CODE_TXT;
						else
							message_code = MSG_ADDNETINTERFACE_COULD_NOT_ADD_ALIAS_TXT;

						error_printf(cc,get_str(cc,message_code),interface_name,node->ln_Name,code);
					}

					goto out;
				}

				/* Say what we did. */
				if(NOT cc->cc_Quiet)
					info_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_ALIAS_TXT),node->ln_Name);
			}
		}
	}

	result = RETURN_OK;

 out:

	/* Clean up the lot... */
	while((node = RemHead(&alias_list)) != NULL)
		FreeVec(node);

	FreeVec(client_id);
	FreeVec(address);
	FreeVec(net_mask);
	FreeVec(broadcast_address);
	FreeVec(destination_address);

	FreeVec(device_name);

	FreeVec(hardware_address);
	FreeVec(iha.iha_Address);

	if(cc->cc_File != ZERO)
	{
		Close(cc->cc_File);
		cc->cc_File = ZERO;
	}

	return(result);
}

/****************************************************************************/

/* Try to convert an IP address specification into a number an complain
   if that doesn't work. */
STATIC BOOL
validate_ip_address(struct CommandContext * cc,STRPTR key,STRPTR address,LONG line_number,STRPTR file)
{
	DECLARE_SOCKETBASE(cc);

	struct in_addr in;
	BOOL result;

	if(inet_aton(address,&in))
	{
		result = TRUE;
	}
	else
	{
		if(NOT cc->cc_Quiet)
			error_printf(cc,get_str(cc,MSG_ADDNETINTERFACE_INVALID_IP_ADDRESS_AT_TXT),key,address,line_number,file);

		result = FALSE;
	}

	return(result);
}

/****************************************************************************/

/* Get the error message text corresponding to an error number. */
STATIC VOID
get_errno_and_code(struct CommandContext * cc,LONG * errno_ptr,STRPTR * code_ptr)
{
	LONG errno = 0;
	LONG code;

	Local_SocketBaseTags(cc,
	    SBTM_GETREF(SBTC_ERRNO),&errno,
	TAG_END);

	code = errno;

	if(Local_SocketBaseTags(cc,
	    SBTM_GETREF(SBTC_ERRNOSTRPTR),&code,
	TAG_END) != 0)
	{
		code = 0;
	}

	(*errno_ptr)	= errno;
	(*code_ptr)		= (STRPTR)code;
}

/****************************************************************************/

/* Check if part of a string matches a shorter string. */
STATIC BOOL
substring_matches(struct CommandContext * cc,STRPTR pattern,STRPTR string)
{
	DECLARE_UTILITYBASE(cc);

	BOOL result = FALSE;
	LONG i,len,pattern_len;

	len = strlen(string);
	pattern_len = strlen(pattern);

	for(i = 0 ; i <= len - pattern_len ; i++)
	{
		if(Strnicmp(&string[len],pattern,pattern_len) == SAME)
		{
			result = TRUE;
			break;
		}
	}

	return(result);
}

/****************************************************************************/

/* Add a new interface node to the list, sorting its contents by priority
   and name. The list is sorted by descending priority, and where the priority
   is identical, by name (in lexical order). */
STATIC VOID
add_interface_node(struct CommandContext * cc,struct List * list,struct Node * new_node)
{
	DECLARE_UTILITYBASE(cc);
	DECLARE_DOSBASE(cc);

	struct Node * list_node;

	/* Find a node on the list that's "larger" than the node
	   we are about to add. */
	for(list_node = list->lh_Head ;
	    list_node->ln_Succ != NULL ;
	    list_node = list_node->ln_Succ)
	{
		if((list_node->ln_Pri < new_node->ln_Pri) || (list_node->ln_Pri == new_node->ln_Pri && Stricmp(FilePart(list_node->ln_Name),FilePart(new_node->ln_Name)) > 0))
			break;
	}

	/* Now insert the new node in the right position,
	   in front of the list node found above. */
	new_node->ln_Succ			= (struct Node *)list_node;
	new_node->ln_Pred			= (struct Node *)list_node->ln_Pred;

	list_node->ln_Pred			= (struct Node *)new_node;
	new_node->ln_Pred->ln_Succ	= (struct Node *)new_node;
}

/****************************************************************************/

/* Print an error message or show an error requester instead. */
STATIC VOID VARARGS68K
error_printf(struct CommandContext * cc,STRPTR format,...)
{
	DECLARE_DOSBASE(cc);
	DECLARE_SYSBASE(cc);
	DECLARE_INTUITIONBASE(cc);

	va_list args;

	if(cc->cc_StartupMessage == NULL)
	{
		STRPTR program_name = cc->cc_ProgramName;
		BPTR fh;

		/* Try to use the error output stream associated with the shell. */
		#if defined(__amigaos4__)
		{
			fh = ErrorOutput();
		}
		#else
		{
			struct Process * this_process = (struct Process *)FindTask(NULL);

			fh = this_process->pr_CES;
		}
		#endif /* __amigaos4__ */

		if(fh == ZERO)
			fh = Output();

		VFPrintf(fh,"%s: ",&program_name);

		#if defined(__amigaos4__)
		{
			va_startlinear(args,format);
			VFPrintf(fh,format,va_getlinearva(args,APTR));
			va_end(args);
		}
		#else
		{
			va_start(args,format);
			VFPrintf(fh,format,args);
			va_end(args);
		}
		#endif /* __amigaos4__ */

		VFPrintf(fh,"\n",NULL);
	}
	else
	{
		if(IntuitionBase != NULL)
		{
			struct EasyStruct es;

			memset(&es,0,sizeof(es));

			es.es_StructSize	= sizeof(es);
			es.es_Title			= get_str(cc,MSG_ADDNETINTERFACE_ERROR_MESSAGE_TITLE_TXT);
			es.es_TextFormat	= format;
			es.es_GadgetFormat	= get_str(cc,MSG_ADDNETINTERFACE_ERROR_MESSAGE_BUTTON_LABEL_TXT);

			#if defined(__amigaos4__)
			{
				va_startlinear(args,format);
				EasyRequestArgs(NULL,&es,NULL,va_getlinearva(args,APTR));
				va_end(args);
			}
			#else
			{
				va_start(args,format);
				EasyRequestArgs(NULL,&es,NULL,args);
				va_end(args);
			}
			#endif /* __amigaos4__ */
		}
	}
}

/****************************************************************************/

/* Print a warning message in the shell window. */
STATIC VOID VARARGS68K
warning_printf(struct CommandContext * cc,STRPTR format,...)
{
	DECLARE_DOSBASE(cc);

	if(cc->cc_StartupMessage == NULL)
	{
		STRPTR program_name = cc->cc_ProgramName;
		va_list args;

		VPrintf("%s: ",&program_name);

		#if defined(__amigaos4__)
		{
			va_startlinear(args,format);
			VPrintf(format,va_getlinearva(args,APTR));
			va_end(args);
		}
		#else
		{
			va_start(args,format);
			VPrintf(format,args);
			va_end(args);
		}
		#endif /* __amigaos4__ */

		VPrintf("\n",NULL);
	}
}

/****************************************************************************/

/* Print an informational message in the shell window. */
STATIC VOID VARARGS68K
info_printf(struct CommandContext * cc,STRPTR format,...)
{
	DECLARE_DOSBASE(cc);

	if(cc->cc_StartupMessage == NULL)
	{
		va_list args;

		#if defined(__amigaos4__)
		{
			va_startlinear(args,format);
			VPrintf(format,va_getlinearva(args,APTR));
			va_end(args);
		}
		#else
		{
			va_start(args,format);
			VPrintf(format,args);
			va_end(args);
		}
		#endif /* __amigaos4__ */
	}
}

/****************************************************************************/

/* Print an error string in the shell window; this is supposed to be
   an informational message only. */
STATIC VOID
info_print_fault(struct CommandContext * cc,LONG code)
{
	DECLARE_DOSBASE(cc);

	if(cc->cc_StartupMessage == NULL)
	{
		UBYTE str[100];

		Fault(code,NULL,str,sizeof(str));

		info_printf(cc,"%s: %s\n",cc->cc_ProgramName,str);
	}
}

/****************************************************************************/

/* Print an error string in the shell window, corresponding to a code. */
STATIC VOID
error_print_fault(struct CommandContext * cc,LONG code)
{
	DECLARE_DOSBASE(cc);

	if(cc->cc_StartupMessage == NULL)
	{
		PrintFault(code,cc->cc_ProgramName);
	}
	else
	{
		UBYTE str[100];

		Fault(code,NULL,str,sizeof(str));

		error_printf(cc,"%s",str);
	}
}

/****************************************************************************/

/* Print an error string in the shell window, corresponding to a code, using
   a special prefix. */
STATIC VOID
error_print_fault_prefix(struct CommandContext * cc,LONG code,STRPTR prefix)
{
	DECLARE_DOSBASE(cc);

	if(cc->cc_StartupMessage == NULL)
	{
		PrintFault(code,prefix);
	}
	else
	{
		UBYTE str[100];

		Fault(code,NULL,str,sizeof(str));

		error_printf(cc,"%s: %s",prefix,str);
	}
}

/****************************************************************************/

/* This looks up a locale string ID in the builtin database; adapted
   from CygnusEd because I couldn't find my own implementation for this
   application... */
STATIC STRPTR
get_builtin_str(LONG id)
{
	LONG top,middle,bottom;
	STRPTR builtin_string;

	/* The search area is all those message between bottom and top, inclusive. */
	bottom = 0;
	top = NUM_ENTRIES(CatCompArray) - 1;

	/* Binary search through the CatCompArray to find the requested string.
	   Note that this assumes that the strings are sorted. Catcomp defaults
	   to creating sorted arrays, but it does _not_ force it. If in the .cd
	   file you specify out of order string numbers this routine will fail. */
	while(bottom != top)
	{
		middle = (bottom + top) / 2;

		if(CatCompArray[middle].cca_ID >= id)
			top = middle;
		else
			bottom = middle + 1;
	}

	/* The only time this error message should occur is if you've passed
	   a garbage number OR if the CatCompArray is not sorted. */
	if(CatCompArray[bottom].cca_ID == id)
		builtin_string = (STRPTR)CatCompArray[bottom].cca_Str;
	else
		builtin_string = "";

	return(builtin_string);
}

STATIC STRPTR
get_str(struct CommandContext * cc, LONG id)
{
	DECLARE_LOCALEBASE(cc);

	STRPTR builtin_string;
	STRPTR result;

	builtin_string = get_builtin_str(id);

	if(cc->cc_Catalog != NULL)
		result = (STRPTR)GetCatalogStr(cc->cc_Catalog,id,builtin_string);
	else
		result = builtin_string;

	return(result);
}

/****************************************************************************/

/* The following are varargs stubs for several bsdsocket.library routines
   which are necessary for the 68k GCC build. */
STATIC LONG VARARGS68K
Local_ConfigureInterfaceTags(struct CommandContext * cc,STRPTR interface_name,...)
{
	DECLARE_SOCKETBASE(cc);

	va_list args;
	LONG result;

	#if defined(__amigaos4__)
	{
		va_startlinear(args,interface_name);
		result = ConfigureInterfaceTagList(interface_name,va_getlinearva(args,struct TagItem *));
		va_end(args);
	}
	#else
	{
		va_start(args,interface_name);
		result = ConfigureInterfaceTagList(interface_name,(struct TagItem *)args);
		va_end(args);
	}
	#endif /* __amigaos4__ */

	return(result);
}

STATIC LONG VARARGS68K
Local_SocketBaseTags(struct CommandContext * cc,...)
{
	DECLARE_SOCKETBASE(cc);

	va_list args;
	LONG result;

	#if defined(__amigaos4__)
	{
		va_startlinear(args,cc);
		result = SocketBaseTagList(va_getlinearva(args,struct TagItem *));
		va_end(args);
	}
	#else
	{
		va_start(args,cc);
		result = SocketBaseTagList((struct TagItem *)args);
		va_end(args);
	}
	#endif /* __amigaos4__ */

	return(result);
}

STATIC LONG VARARGS68K
Local_AddRouteTags(struct CommandContext * cc,...)
{
	DECLARE_SOCKETBASE(cc);

	va_list args;
	LONG result;

	#if defined(__amigaos4__)
	{
		va_startlinear(args,cc);
		result = AddRouteTagList(va_getlinearva(args,struct TagItem *));
		va_end(args);
	}
	#else
	{
		va_start(args,cc);
		result = AddRouteTagList((struct TagItem *)args);
		va_end(args);
	}
	#endif /* __amigaos4__ */

	return(result);
}
