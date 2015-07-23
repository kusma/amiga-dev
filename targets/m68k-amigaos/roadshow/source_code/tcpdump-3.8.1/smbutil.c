/*
 * Copyright (C) Andrew Tridgell 1995-1999
 *
 * This software may be distributed either under the terms of the
 * BSD-style license that accompanies tcpdump or the GNU GPL version 2
 * or later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef lint
static const char rcsid[] _U_ =
     "@(#) $Header: /Users/olsen/Code/cvs2svn-2010/os4-cvs-repository/contrib/roadshow/client-software/tcpdump-3.8.1/smbutil.c,v 1.2 2004-09-22 15:40:56 olsen Exp $";
#endif

#include <tcpdump-stdinc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interface.h"
#include "extract.h"
#include "smb.h"

extern const u_char *startbuf;

/*
 * interpret a 32 bit dos packed date/time to some parameters
 */
static void
interpret_dos_date(u_int32_t date, struct tm *tp)
{
    u_int32_t p0, p1, p2, p3;

    p0 = date & 0xFF;
    p1 = ((date & 0xFF00) >> 8) & 0xFF;
    p2 = ((date & 0xFF0000) >> 16) & 0xFF;
    p3 = ((date & 0xFF000000) >> 24) & 0xFF;

    tp->tm_sec = 2 * (p0 & 0x1F);
    tp->tm_min = ((p0 >> 5) & 0xFF) + ((p1 & 0x7) << 3);
    tp->tm_hour = (p1 >> 3) & 0xFF;
    tp->tm_mday = (p2 & 0x1F);
    tp->tm_mon = ((p2 >> 5) & 0xFF) + ((p3 & 0x1) << 3) - 1;
    tp->tm_year = ((p3 >> 1) & 0xFF) + 80;
}

/*
 * common portion:
 * create a unix date from a dos date
 */
static time_t
int_unix_date(u_int32_t dos_date)
{
    struct tm t;

    if (dos_date == 0)
	return(0);

    interpret_dos_date(dos_date, &t);
    t.tm_wday = 1;
    t.tm_yday = 1;
    t.tm_isdst = 0;

    return (mktime(&t));
}

/*
 * create a unix date from a dos date
 * in network byte order
 */
static time_t
make_unix_date(const u_char *date_ptr)
{
    u_int32_t dos_date = 0;

    dos_date = EXTRACT_LE_32BITS(date_ptr);

    return int_unix_date(dos_date);
}

/*
 * create a unix date from a dos date
 * in halfword-swapped network byte order!
 */
static time_t
make_unix_date2(const u_char *date_ptr)
{
    u_int32_t x, x2;

    x = EXTRACT_LE_32BITS(date_ptr);
    x2 = ((x & 0xFFFF) << 16) | ((x & 0xFFFF0000) >> 16);
    return int_unix_date(x2);
}

/*
 * interpret an 8 byte "filetime" structure to a time_t
 * It's originally in "100ns units since jan 1st 1601"
 */
static time_t
interpret_long_date(const u_char *p)
{
    double d;
    time_t ret;

    TCHECK2(p[4], 4);

    /* this gives us seconds since jan 1st 1601 (approx) */
    d = (EXTRACT_LE_32BITS(p + 4) * 256.0 + p[3]) * (1.0e-7 * (1 << 24));

    /* now adjust by 369 years to make the secs since 1970 */
    d -= 369.0 * 365.25 * 24 * 60 * 60;

    /* and a fudge factor as we got it wrong by a few days */
    d += (3 * 24 * 60 * 60 + 6 * 60 * 60 + 2);

    if (d < 0)
	return(0);

    ret = (time_t)d;

    return(ret);
trunc:
    return(0);
}

/*
 * interpret the weird netbios "name". Return the name type, or -1 if
 * we run past the end of the buffer
 */
static int
name_interpret(const u_char *in, const u_char *maxbuf, char *out)
{
    int ret;
    int len;

    if (in >= maxbuf)
	return(-1);	/* name goes past the end of the buffer */
    TCHECK2(*in, 1);
    len = (*in++) / 2;

    *out=0;

    if (len > 30 || len < 1)
	return(0);

    while (len--) {
	TCHECK2(*in, 2);
	if (in + 1 >= maxbuf)
	    return(-1);	/* name goes past the end of the buffer */
	if (in[0] < 'A' || in[0] > 'P' || in[1] < 'A' || in[1] > 'P') {
	    *out = 0;
	    return(0);
	}
	*out = ((in[0] - 'A') << 4) + (in[1] - 'A');
	in += 2;
	out++;
    }
    *out = 0;
    ret = out[-1];

    return(ret);

trunc:
    return(-1);
}

/*
 * find a pointer to a netbios name
 */
static const u_char *
name_ptr(const u_char *buf, int ofs, const u_char *maxbuf)
{
    const u_char *p;
    u_char c;

    p = buf + ofs;
    if (p >= maxbuf)
	return(NULL);	/* name goes past the end of the buffer */
    TCHECK2(*p, 1);

    c = *p;

    /* XXX - this should use the same code that the DNS dissector does */
    if ((c & 0xC0) == 0xC0) {
	u_int16_t l = EXTRACT_16BITS(buf + ofs) & 0x3FFF;
	if (l == 0) {
	    /* We have a pointer that points to itself. */
	    return(NULL);
	}
	p = buf + l;
	if (p >= maxbuf)
	    return(NULL);	/* name goes past the end of the buffer */
	TCHECK2(*p, 1);
	return(buf + l);
    } else
	return(buf + ofs);

trunc:
    return(NULL);	/* name goes past the end of the buffer */
}

/*
 * extract a netbios name from a buf
 */
static int
name_extract(const u_char *buf, int ofs, const u_char *maxbuf, char *name)
{
    const u_char *p = name_ptr(buf, ofs, maxbuf);
    if (p == NULL)
	return(-1);	/* error (probably name going past end of buffer) */
    name[0] = '\0';
    return(name_interpret(p, maxbuf, name));
}


/*
 * return the total storage length of a mangled name
 */
static int
name_len(const unsigned char *s, const unsigned char *maxbuf)
{
    const unsigned char *s0 = s;
    unsigned char c;

    if (s >= maxbuf)
	return(-1);	/* name goes past the end of the buffer */
    TCHECK2(*s, 1);
    c = *s;
    if ((c & 0xC0) == 0xC0)
	return(2);
    while (*s) {
	if (s >= maxbuf)
	    return(-1);	/* name goes past the end of the buffer */
	TCHECK2(*s, 1);
	s += (*s) + 1;
    }
    return(PTR_DIFF(s, s0) + 1);

trunc:
    return(-1);	/* name goes past the end of the buffer */
}

static void
print_asc(const unsigned char *buf, int len)
{
    int i;
    for (i = 0; i < len; i++)
	safeputchar(buf[i]);
}

static const char *
name_type_str(int name_type)
{
    const char *f = NULL;

    switch (name_type) {
    case 0:    f = "Workstation"; break;
    case 0x03: f = "Client?"; break;
    case 0x20: f = "Server"; break;
    case 0x1d: f = "Master Browser"; break;
    case 0x1b: f = "Domain Controller"; break;
    case 0x1e: f = "Browser Server"; break;
    default:   f = "Unknown"; break;
    }
    return(f);
}

void
print_data(const unsigned char *buf, int len)
{
    int i = 0;

    if (len <= 0)
	return;
    printf("[%03X] ", i);
    for (i = 0; i < len; /*nothing*/) {
	printf("%02X ", buf[i] & 0xff);
	i++;
	if (i%8 == 0)
	    printf(" ");
	if (i % 16 == 0) {
	    print_asc(&buf[i - 16], 8);
	    printf(" ");
	    print_asc(&buf[i - 8], 8);
	    printf("\n");
	    if (i < len)
		printf("[%03X] ", i);
	}
    }
    if (i % 16) {
	int n;

	n = 16 - (i % 16);
	printf(" ");
	if (n>8)
	    printf(" ");
	while (n--)
	    printf("   ");

	n = SMBMIN(8, i % 16);
	print_asc(&buf[i - (i % 16)], n);
	printf(" ");
	n = (i % 16) - n;
	if (n > 0)
	    print_asc(&buf[i - n], n);
	printf("\n");
    }
}


static void
write_bits(unsigned int val, const char *fmt)
{
    const char *p = fmt;
    int i = 0;

    while ((p = strchr(fmt, '|'))) {
	size_t l = PTR_DIFF(p, fmt);
	if (l && (val & (1 << i)))
	    printf("%.*s ", (int)l, fmt);
	fmt = p + 1;
	i++;
    }
}

/* convert a UCS2 string into iso-8859-1 string */
static const char *
unistr(const u_char *s, int *len)
{
    static char buf[1000];
    int l=0;
    static int use_unicode = -1;

    if (use_unicode == -1) {
	char *p = getenv("USE_UNICODE");
	if (p && (atoi(p) == 1))
	    use_unicode = 1;
	else
	    use_unicode = 0;
    }

    /* maybe it isn't unicode - a cheap trick */
    if (!use_unicode || (s[0] && s[1])) {
	*len = strlen((const char *)s) + 1;
	return (const char *)s;
    }

    *len = 0;

    if (s[0] == 0 && s[1] != 0) {
	s++;
	*len = 1;
    }

    while (l < (int)(sizeof(buf) - 1) && s[0] && s[1] == 0) {
	buf[l] = s[0];
	s += 2;
	l++;
	*len += 2;
    }
    buf[l] = 0;
    *len += 2;
    return buf;
}

static const u_char *
smb_fdata1(const u_char *buf, const char *fmt, const u_char *maxbuf)
{
    int reverse = 0;
    const char *attrib_fmt = "READONLY|HIDDEN|SYSTEM|VOLUME|DIR|ARCHIVE|";
    int len;

    while (*fmt && buf<maxbuf) {
	switch (*fmt) {
	case 'a':
	    write_bits(buf[0], attrib_fmt);
	    buf++;
	    fmt++;
	    break;

	case 'A':
	    write_bits(EXTRACT_LE_16BITS(buf), attrib_fmt);
	    buf += 2;
	    fmt++;
	    break;

	case '{':
	  {
	    char bitfmt[128];
	    char *p;
	    int l;

	    p = strchr(++fmt, '}');
	    l = PTR_DIFF(p, fmt);

	    if ((unsigned int)l > sizeof(bitfmt) - 1)
		    l = sizeof(bitfmt)-1;

	    strncpy(bitfmt, fmt, l);
	    bitfmt[l] = '\0';
	    fmt = p + 1;
	    write_bits(buf[0], bitfmt);
	    buf++;
	    break;
	  }

	case 'P':
	  {
	    int l = atoi(fmt + 1);
	    buf += l;
	    fmt++;
	    while (isdigit((unsigned char)*fmt))
		fmt++;
	    break;
	  }
	case 'r':
	    reverse = !reverse;
	    fmt++;
	    break;
	case 'D':
	  {
	    unsigned int x;

	    TCHECK2(buf[0], 4);
	    x = reverse ? EXTRACT_32BITS(buf) : EXTRACT_LE_32BITS(buf);
	    printf("%d (0x%x)", x, x);
	    buf += 4;
	    fmt++;
	    break;
	  }
	case 'L':
	  {
	    unsigned int x1, x2;

	    TCHECK2(buf[4], 4);
	    x1 = reverse ? EXTRACT_32BITS(buf) :
			   EXTRACT_LE_32BITS(buf);
	    x2 = reverse ? EXTRACT_32BITS(buf + 4) :
			   EXTRACT_LE_32BITS(buf + 4);
	    if (x2)
		printf("0x%08x:%08x", x2, x1);
	    else
		printf("%d (0x%08x%08x)", x1, x2, x1);
	    buf += 8;
	    fmt++;
	    break;
	  }
	case 'd':
	  {
	    unsigned int x;
	    TCHECK2(buf[0], 2);
	    x = reverse ? EXTRACT_16BITS(buf) :
			  EXTRACT_LE_16BITS(buf);
	    printf("%d (0x%x)", x, x);
	    buf += 2;
	    fmt++;
	    break;
	  }
	case 'W':
	  {
	    unsigned int x;
	    TCHECK2(buf[0], 4);
	    x = reverse ? EXTRACT_32BITS(buf) :
			  EXTRACT_LE_32BITS(buf);
	    printf("0x%X", x);
	    buf += 4;
	    fmt++;
	    break;
	  }
	case 'w':
	  {
	    unsigned int x;
	    TCHECK2(buf[0], 2);
	    x = reverse ? EXTRACT_16BITS(buf) :
			  EXTRACT_LE_16BITS(buf);
	    printf("0x%X", x);
	    buf += 2;
	    fmt++;
	    break;
	  }
	case 'B':
	  {
	    unsigned int x;
	    TCHECK(buf[0]);
	    x = buf[0];
	    printf("0x%X", x);
	    buf += 1;
	    fmt++;
	    break;
	  }
	case 'b':
	  {
	    unsigned int x;
	    TCHECK(buf[0]);
	    x = buf[0];
	    printf("%u (0x%x)", x, x);
	    buf += 1;
	    fmt++;
	    break;
	  }
	case 'S':
	  {
	    /*XXX unistr() */
	    printf("%.*s", (int)PTR_DIFF(maxbuf, buf), unistr(buf, &len));
	    buf += len;
	    fmt++;
	    break;
	  }
	case 'Z':
	  {
	    if (*buf != 4 && *buf != 2)
		printf("Error! ASCIIZ buffer of type %u (safety=%lu)\n", *buf,
		    (unsigned long)PTR_DIFF(maxbuf, buf));
	    printf("%.*s", (int)PTR_DIFF(maxbuf, buf + 1),
		unistr(buf + 1, &len));
	    buf += len + 1;
	    fmt++;
	    break;
	  }
	case 's':
	  {
	    int l = atoi(fmt + 1);
	    printf("%-*.*s", l, l, buf);
	    buf += l;
	    fmt++;
	    while (isdigit((unsigned char)*fmt))
		fmt++;
	    break;
	  }
	case 'h':
	  {
	    int l = atoi(fmt + 1);
	    while (l--)
		printf("%02x", *buf++);
	    fmt++;
	    while (isdigit((unsigned char)*fmt))
		fmt++;
	    break;
	  }
	case 'n':
	  {
	    int t = atoi(fmt+1);
	    char nbuf[255];
	    int name_type;
	    int len;

	    switch (t) {
	    case 1:
		name_type = name_extract(startbuf, PTR_DIFF(buf, startbuf),
		    maxbuf, nbuf);
		if (name_type < 0)
		    goto trunc;
		len = name_len(buf, maxbuf);
		if (len < 0)
		    goto trunc;
		buf += len;
		printf("%-15.15s NameType=0x%02X (%s)", nbuf, name_type,
		    name_type_str(name_type));
		break;
	    case 2:
		name_type = buf[15];
		printf("%-15.15s NameType=0x%02X (%s)", buf, name_type,
		    name_type_str(name_type));
		buf += 16;
		break;
	    }
	    fmt++;
	    while (isdigit((unsigned char)*fmt))
		fmt++;
	    break;
	  }
	case 'T':
	  {
	    time_t t;
	    struct tm *lt;
	    const char *tstring;
	    u_int32_t x;
	    x = EXTRACT_LE_32BITS(buf);

	    switch (atoi(fmt + 1)) {
	    case 1:
		if (x == 0 || x == 0xFFFFFFFF)
		    t = 0;
		else
		    t = make_unix_date(buf);
		buf += 4;
		break;
	    case 2:
		if (x == 0 || x == 0xFFFFFFFF)
		    t = 0;
		else
		    t = make_unix_date2(buf);
		buf += 4;
		break;
	    case 3:
		t = interpret_long_date(buf);
		buf += 8;
		break;
	    }
	    if (t != 0) {
		lt = localtime(&t);
		if (lt != NULL)
		    tstring = asctime(lt);
		else
		    tstring = "(Can't convert time)\n";
	    } else
		tstring = "NULL\n";
	    printf("%s", tstring);
	    fmt++;
	    while (isdigit((unsigned char)*fmt))
		fmt++;
	    break;
	  }
	default:
	    putchar(*fmt);
	    fmt++;
	    break;
	}
    }

    if (buf >= maxbuf && *fmt)
	printf("END OF BUFFER\n");

    return(buf);

trunc:
    printf("\n");
    printf("WARNING: Short packet. Try increasing the snap length\n");
    return(NULL);
}

const u_char *
smb_fdata(const u_char *buf, const char *fmt, const u_char *maxbuf)
{
    static int depth = 0;
    char s[128];
    char *p;

    while (*fmt) {
	switch (*fmt) {
	case '*':
	    fmt++;
	    while (buf < maxbuf) {
		const u_char *buf2;
		depth++;
		buf2 = smb_fdata(buf, fmt, maxbuf);
		depth--;
		if (buf2 == NULL)
		    return(NULL);
		if (buf2 == buf)
		    return(buf);
		buf = buf2;
	    }
	    return(buf);

	case '|':
	    fmt++;
	    if (buf >= maxbuf)
		return(buf);
	    break;

	case '%':
	    fmt++;
	    buf = maxbuf;
	    break;

	case '#':
	    fmt++;
	    return(buf);
	    break;

	case '[':
	    fmt++;
	    if (buf >= maxbuf)
		return(buf);
	    memset(s, 0, sizeof(s));
	    p = strchr(fmt, ']');
	    if ((size_t)(p - fmt + 1) > sizeof(s)) {
		/* overrun */
		return(buf);
	    }
	    strncpy(s, fmt, p - fmt);
	    s[p - fmt] = '\0';
	    fmt = p + 1;
	    buf = smb_fdata1(buf, s, maxbuf);
	    if (buf == NULL)
		return(NULL);
	    break;

	default:
	    putchar(*fmt);
	    fmt++;
	    fflush(stdout);
	    break;
	}
    }
    if (!depth && buf < maxbuf) {
	size_t len = PTR_DIFF(maxbuf, buf);
	printf("Data: (%lu bytes)\n", (unsigned long)len);
	print_data(buf, len);
	return(buf + len);
    }
    return(buf);
}

typedef struct {
    const char *name;
    int code;
    const char *message;
} err_code_struct;

/* Dos Error Messages */
static err_code_struct dos_msgs[] = {
    { "ERRbadfunc", 1, "Invalid function." },
    { "ERRbadfile", 2, "File not found." },
    { "ERRbadpath", 3, "Directory invalid." },
    { "ERRnofids", 4, "No file descriptors available" },
    { "ERRnoaccess", 5, "Access denied." },
    { "ERRbadfid", 6, "Invalid file handle." },
    { "ERRbadmcb", 7, "Memory control blocks destroyed." },
    { "ERRnomem", 8, "Insufficient server memory to perform the requested function." },
    { "ERRbadmem", 9, "Invalid memory block address." },
    { "ERRbadenv", 10, "Invalid environment." },
    { "ERRbadformat", 11, "Invalid format." },
    { "ERRbadaccess", 12, "Invalid open mode." },
    { "ERRbaddata", 13, "Invalid data." },
    { "ERR", 14, "reserved." },
    { "ERRbaddrive", 15, "Invalid drive specified." },
    { "ERRremcd", 16, "A Delete Directory request attempted  to  remove  the  server's  current directory." },
    { "ERRdiffdevice", 17, "Not same device." },
    { "ERRnofiles", 18, "A File Search command can find no more files matching the specified criteria." },
    { "ERRbadshare", 32, "The sharing mode specified for an Open conflicts with existing  FIDs  on the file." },
    { "ERRlock", 33, "A Lock request conflicted with an existing lock or specified an  invalid mode,   or an Unlock requested attempted to remove a lock held by another process." },
    { "ERRfilexists", 80, "The file named in a Create Directory,  Make  New  File  or  Link  request already exists." },
    { "ERRbadpipe", 230, "Pipe invalid." },
    { "ERRpipebusy", 231, "All instances of the requested pipe are busy." },
    { "ERRpipeclosing", 232, "Pipe close in progress." },
    { "ERRnotconnected", 233, "No process on other end of pipe." },
    { "ERRmoredata", 234, "There is more data to be returned." },
    { NULL, -1, NULL }
 };

/* Server Error Messages */
err_code_struct server_msgs[] = {
    { "ERRerror", 1, "Non-specific error code." },
    { "ERRbadpw", 2, "Bad password - name/password pair in a Tree Connect or Session Setup are invalid." },
    { "ERRbadtype", 3, "reserved." },
    { "ERRaccess", 4, "The requester does not have  the  necessary  access  rights  within  the specified  context for the requested function. The context is defined by the TID or the UID." },
    { "ERRinvnid", 5, "The tree ID (TID) specified in a command was invalid." },
    { "ERRinvnetname", 6, "Invalid network name in tree connect." },
    { "ERRinvdevice", 7, "Invalid device - printer request made to non-printer connection or  non-printer request made to printer connection." },
    { "ERRqfull", 49, "Print queue full (files) -- returned by open print file." },
    { "ERRqtoobig", 50, "Print queue full -- no space." },
    { "ERRqeof", 51, "EOF on print queue dump." },
    { "ERRinvpfid", 52, "Invalid print file FID." },
    { "ERRsmbcmd", 64, "The server did not recognize the command received." },
    { "ERRsrverror", 65, "The server encountered an internal error,  e.g.,  system file unavailable." },
    { "ERRfilespecs", 67, "The file handle (FID) and pathname parameters contained an invalid  combination of values." },
    { "ERRreserved", 68, "reserved." },
    { "ERRbadpermits", 69, "The access permissions specified for a file or directory are not a valid combination.  The server cannot set the requested attribute." },
    { "ERRreserved", 70, "reserved." },
    { "ERRsetattrmode", 71, "The attribute mode in the Set File Attribute request is invalid." },
    { "ERRpaused", 81, "Server is paused." },
    { "ERRmsgoff", 82, "Not receiving messages." },
    { "ERRnoroom", 83, "No room to buffer message." },
    { "ERRrmuns", 87, "Too many remote user names." },
    { "ERRtimeout", 88, "Operation timed out." },
    { "ERRnoresource", 89, "No resources currently available for request." },
    { "ERRtoomanyuids", 90, "Too many UIDs active on this session." },
    { "ERRbaduid", 91, "The UID is not known as a valid ID on this session." },
    { "ERRusempx", 250, "Temp unable to support Raw,  use MPX mode." },
    { "ERRusestd", 251, "Temp unable to support Raw,  use standard read/write." },
    { "ERRcontmpx", 252, "Continue in MPX mode." },
    { "ERRreserved", 253, "reserved." },
    { "ERRreserved", 254, "reserved." },
    { "ERRnosupport", 0xFFFF, "Function not supported." },
    { NULL, -1, NULL }
};

/* Hard Error Messages */
err_code_struct hard_msgs[] = {
    { "ERRnowrite", 19, "Attempt to write on write-protected diskette." },
    { "ERRbadunit", 20, "Unknown unit." },
    { "ERRnotready", 21, "Drive not ready." },
    { "ERRbadcmd", 22, "Unknown command." },
    { "ERRdata", 23, "Data error (CRC)." },
    { "ERRbadreq", 24, "Bad request structure length." },
    { "ERRseek", 25 , "Seek error." },
    { "ERRbadmedia", 26, "Unknown media type." },
    { "ERRbadsector", 27, "Sector not found." },
    { "ERRnopaper", 28, "Printer out of paper." },
    { "ERRwrite", 29, "Write fault." },
    { "ERRread", 30, "Read fault." },
    { "ERRgeneral", 31, "General failure." },
    { "ERRbadshare", 32, "A open conflicts with an existing open." },
    { "ERRlock", 33, "A Lock request conflicted with an existing lock or specified an invalid mode,  or an Unlock requested attempted to remove a lock held by another process." },
    { "ERRwrongdisk", 34, "The wrong disk was found in a drive." },
    { "ERRFCBUnavail", 35, "No FCBs are available to process request." },
    { "ERRsharebufexc", 36, "A sharing buffer has been exceeded." },
    { NULL, -1, NULL }
};

static struct {
    int code;
    const char *class;
    err_code_struct *err_msgs;
} err_classes[] = {
    { 0, "SUCCESS", NULL },
    { 0x01, "ERRDOS", dos_msgs },
    { 0x02, "ERRSRV", server_msgs },
    { 0x03, "ERRHRD", hard_msgs },
    { 0x04, "ERRXOS", NULL },
    { 0xE1, "ERRRMX1", NULL },
    { 0xE2, "ERRRMX2", NULL },
    { 0xE3, "ERRRMX3", NULL },
    { 0xFF, "ERRCMD", NULL },
    { -1, NULL, NULL }
};

/*
 * return a SMB error string from a SMB buffer
 */
char *
smb_errstr(int class, int num)
{
    static char ret[128];
    int i, j;

    ret[0] = 0;

    for (i = 0; err_classes[i].class; i++)
	if (err_classes[i].code == class) {
	    if (err_classes[i].err_msgs) {
		err_code_struct *err = err_classes[i].err_msgs;
		for (j = 0; err[j].name; j++)
		    if (num == err[j].code) {
			snprintf(ret, sizeof(ret), "%s - %s (%s)",
			    err_classes[i].class, err[j].name, err[j].message);
			return ret;
		    }
	    }

	    snprintf(ret, sizeof(ret), "%s - %d", err_classes[i].class, num);
	    return ret;
	}

    snprintf(ret, sizeof(ret), "ERROR: Unknown error (%d,%d)", class, num);
    return(ret);
}
