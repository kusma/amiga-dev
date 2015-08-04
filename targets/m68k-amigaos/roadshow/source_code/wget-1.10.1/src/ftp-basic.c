/* Basic FTP routines.
   Copyright (C) 1995, 1996, 1997, 1998, 2000 Free Software Foundation, Inc.

This file is part of GNU Wget.

GNU Wget is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

GNU Wget is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wget; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

In addition, as a special exception, the Free Software Foundation
gives permission to link the code of its release of Wget with the
OpenSSL project's "OpenSSL" library (or with modified versions of it
that use the same license as the "OpenSSL" library), and distribute
the linked executables.  You must obey the GNU General Public License
in all respects for all of the code used other than "OpenSSL".  If you
modify this file, you may extend this exception to your version of the
file, but you are not obligated to do so.  If you do not wish to do
so, delete this exception statement from your version.  */

#include <config.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <sys/types.h>

#include "wget.h"
#include "utils.h"
#include "connect.h"
#include "host.h"
#include "ftp.h"
#include "retr.h"

char ftp_last_respline[128];


/* Get the response of FTP server and allocate enough room to handle
   it.  <CR> and <LF> characters are stripped from the line, and the
   line is 0-terminated.  All the response lines but the last one are
   skipped.  The last line is determined as described in RFC959.

   If the line is successfully read, FTPOK is returned, and *ret_line
   is assigned a freshly allocated line.  Otherwise, FTPRERR is
   returned, and the value of *ret_line should be ignored.  */

uerr_t
ftp_response (int fd, char **ret_line)
{
  while (1)
    {
      char *p;
      char *line = fd_read_line (fd);
      if (!line)
	return FTPRERR;

      /* Strip trailing CRLF before printing the line, so that
	 escnonprint doesn't include bogus \012 and \015. */
      p = strchr (line, '\0');
      if (p > line && p[-1] == '\n')
	*--p = '\0';
      if (p > line && p[-1] == '\r')
	*--p = '\0';

      if (opt.server_response)
	logprintf (LOG_NOTQUIET, "%s\n", escnonprint (line));
      else
        DEBUGP (("%s\n", escnonprint (line)));

      /* The last line of output is the one that begins with "ddd ". */
      if (ISDIGIT (line[0]) && ISDIGIT (line[1]) && ISDIGIT (line[2])
	  && line[3] == ' ')
	{
	  strncpy (ftp_last_respline, line, sizeof (ftp_last_respline));
	  ftp_last_respline[sizeof (ftp_last_respline) - 1] = '\0';
	  *ret_line = line;
	  return FTPOK;
	}
      xfree (line);
    }
}

/* Returns the malloc-ed FTP request, ending with <CR><LF>, printing
   it if printing is required.  If VALUE is NULL, just use
   command<CR><LF>.  */
static char *
ftp_request (const char *command, const char *value)
{
  char *res;
  if (value)
    {
      /* Check for newlines in VALUE (possibly injected by the %0A URL
	 escape) making the callers inadvertently send multiple FTP
	 commands at once.  Without this check an attacker could
	 intentionally redirect to ftp://server/fakedir%0Acommand.../
	 and execute arbitrary FTP command on a remote FTP server.  */
      if (strpbrk (value, "\r\n"))
	{
	  /* Copy VALUE to the stack and modify CR/LF to space. */
	  char *defanged, *p;
	  STRDUP_ALLOCA (defanged, value);
	  for (p = defanged; *p; p++)
	    if (*p == '\r' || *p == '\n')
	      *p = ' ';
	  DEBUGP (("\nDetected newlines in %s \"%s\"; changing to %s \"%s\"\n",
		   command, escnonprint (value), command, escnonprint (defanged)));
	  /* Make VALUE point to the defanged copy of the string. */
	  value = defanged;
	}
      res = concat_strings (command, " ", value, "\r\n", (char *) 0);
    }
  else
    res = concat_strings (command, "\r\n", (char *) 0);
  if (opt.server_response)
    {
      /* Hack: don't print out password.  */
      if (strncmp (res, "PASS", 4) != 0)
        logprintf (LOG_ALWAYS, "--> %s\n", res);
      else
        logputs (LOG_ALWAYS, "--> PASS Turtle Power!\n\n");
    }
  else
    DEBUGP (("\n--> %s\n", res));
  return res;
}

/* Sends the USER and PASS commands to the server, to control
   connection socket csock.  */
uerr_t
ftp_login (int csock, const char *acc, const char *pass)
{
  uerr_t err;
  char *request, *respline;
  int nwritten;

  /* Get greeting.  */
  err = ftp_response (csock, &respline);
  if (err != FTPOK)
    return err;
  if (*respline != '2')
    {
      xfree (respline);
      return FTPSRVERR;
    }
  xfree (respline);
  /* Send USER username.  */
  request = ftp_request ("USER", acc);
  nwritten = fd_write (csock, request, strlen (request), -1.0);
  if (nwritten < 0)
    {
      xfree (request);
      return WRITEFAILED;
    }
  xfree (request);
  /* Get appropriate response.  */
  err = ftp_response (csock, &respline);
  if (err != FTPOK)
    return err;
  /* An unprobable possibility of logging without a password.  */
  if (*respline == '2')
    {
      xfree (respline);
      return FTPOK;
    }
  /* Else, only response 3 is appropriate.  */
  if (*respline != '3')
    {
      xfree (respline);
      return FTPLOGREFUSED;
    }
#ifdef ENABLE_OPIE
  {
    static const char *skey_head[] = {
      "331 s/key ",
      "331 opiekey "
    };
    int i;
    const char *seed = NULL;

    for (i = 0; i < countof (skey_head); i++)
      {
	int l = strlen (skey_head[i]);
        if (0 == strncasecmp (skey_head[i], respline, l))
	  {
	    seed = respline + l;
	    break;
	  }
      }
    if (seed)
      {
        int skey_sequence = 0;

	/* Extract the sequence from SEED.  */
	for (; ISDIGIT (*seed); seed++)
	  skey_sequence = 10 * skey_sequence + *seed - '0';
	if (*seed == ' ')
	  ++seed;
        else
          {
            xfree (respline);
            return FTPLOGREFUSED;
          }
	/* Replace the password with the SKEY response to the
	   challenge.  */
        pass = skey_response (skey_sequence, seed, pass);
      }
  }
#endif /* ENABLE_OPIE */
  xfree (respline);
  /* Send PASS password.  */
  request = ftp_request ("PASS", pass);
  nwritten = fd_write (csock, request, strlen (request), -1.0);
  if (nwritten < 0)
    {
      xfree (request);
      return WRITEFAILED;
    }
  xfree (request);
  /* Get appropriate response.  */
  err = ftp_response (csock, &respline);
  if (err != FTPOK)
    return err;
  if (*respline != '2')
    {
      xfree (respline);
      return FTPLOGINC;
    }
  xfree (respline);
  /* All OK.  */
  return FTPOK;
}

static void
ip_address_to_port_repr (const ip_address *addr, int port, char *buf, 
                         size_t buflen)
{
  unsigned char *ptr;

  assert (addr != NULL);
  assert (addr->type == IPV4_ADDRESS);
  assert (buf != NULL);
  /* buf must contain the argument of PORT (of the form a,b,c,d,e,f). */
  assert (buflen >= 6 * 4);

  ptr = ADDRESS_IPV4_DATA (addr);
  snprintf (buf, buflen, "%d,%d,%d,%d,%d,%d", ptr[0], ptr[1],
            ptr[2], ptr[3], (port & 0xff00) >> 8, port & 0xff);
  buf[buflen - 1] = '\0';
}

/* Bind a port and send the appropriate PORT command to the FTP
   server.  Use acceptport after RETR, to get the socket of data
   connection.  */
uerr_t
ftp_port (int csock, int *local_sock)
{
  uerr_t err;
  char *request, *respline;
  ip_address addr;
  int nwritten;
  int port;
  /* Must contain the argument of PORT (of the form a,b,c,d,e,f). */
  char bytes[6 * 4 + 1];

  /* Get the address of this side of the connection. */
  if (!socket_ip_address (csock, &addr, ENDPOINT_LOCAL))
    return FTPSYSERR;

  assert (addr.type == IPV4_ADDRESS);

  /* Setting port to 0 lets the system choose a free port.  */
  port = 0;

  /* Bind the port.  */
  *local_sock = bind_local (&addr, &port);
  if (*local_sock < 0)
    return FTPSYSERR;

  /* Construct the argument of PORT (of the form a,b,c,d,e,f). */
  ip_address_to_port_repr (&addr, port, bytes, sizeof (bytes));

  /* Send PORT request.  */
  request = ftp_request ("PORT", bytes);
  nwritten = fd_write (csock, request, strlen (request), -1.0);
  if (nwritten < 0)
    {
      xfree (request);
      fd_close (*local_sock);
      return WRITEFAILED;
    }
  xfree (request);

  /* Get appropriate response.  */
  err = ftp_response (csock, &respline);
  if (err != FTPOK)
    {
      fd_close (*local_sock);
      return err;
    }
  if (*respline != '2')
    {
      xfree (respline);
      fd_close (*local_sock);
      return FTPPORTERR;
    }
  xfree (respline);
  return FTPOK;
}

#ifdef ENABLE_IPV6
static void
ip_address_to_lprt_repr (const ip_address *addr, int port, char *buf, 
                         size_t buflen)
{
  unsigned char *ptr;

  assert (addr != NULL);
  assert (addr->type == IPV4_ADDRESS || addr->type == IPV6_ADDRESS);
  assert (buf != NULL);
  /* buf must contain the argument of LPRT (of the form af,n,h1,h2,...,hn,p1,p2). */
  assert (buflen >= 21 * 4);

  /* Construct the argument of LPRT (of the form af,n,h1,h2,...,hn,p1,p2). */
  switch (addr->type) 
    {
      case IPV4_ADDRESS: 
	ptr = ADDRESS_IPV4_DATA (addr);
        snprintf (buf, buflen, "%d,%d,%d,%d,%d,%d,%d,%d,%d", 4, 4, 
                  ptr[0], ptr[1], ptr[2], ptr[3], 2,
                  (port & 0xff00) >> 8, port & 0xff);
        buf[buflen - 1] = '\0';
        break;
      case IPV6_ADDRESS: 
	ptr = ADDRESS_IPV6_DATA (addr);
	snprintf (buf, buflen, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
	          6, 16, ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5], ptr[6], ptr[7], 
		  ptr[8], ptr[9], ptr[10], ptr[11], ptr[12], ptr[13], ptr[14], ptr[15], 2,
		  (port & 0xff00) >> 8, port & 0xff);
	buf[buflen - 1] = '\0';
	break;
    }
}

/* Bind a port and send the appropriate PORT command to the FTP
   server.  Use acceptport after RETR, to get the socket of data
   connection.  */
uerr_t
ftp_lprt (int csock, int *local_sock)
{
  uerr_t err;
  char *request, *respline;
  ip_address addr;
  int nwritten;
  int port;
  /* Must contain the argument of LPRT (of the form af,n,h1,h2,...,hn,p1,p2). */
  char bytes[21 * 4 + 1];

  /* Get the address of this side of the connection. */
  if (!socket_ip_address (csock, &addr, ENDPOINT_LOCAL))
    return FTPSYSERR;

  assert (addr.type == IPV4_ADDRESS || addr.type == IPV6_ADDRESS);

  /* Setting port to 0 lets the system choose a free port.  */
  port = 0;

  /* Bind the port.  */
  *local_sock = bind_local (&addr, &port);
  if (*local_sock < 0)
    return FTPSYSERR;

  /* Construct the argument of LPRT (of the form af,n,h1,h2,...,hn,p1,p2). */
  ip_address_to_lprt_repr (&addr, port, bytes, sizeof (bytes));

  /* Send PORT request.  */
  request = ftp_request ("LPRT", bytes);
  nwritten = fd_write (csock, request, strlen (request), -1.0);
  if (nwritten < 0)
    {
      xfree (request);
      fd_close (*local_sock);
      return WRITEFAILED;
    }
  xfree (request);
  /* Get appropriate response.  */
  err = ftp_response (csock, &respline);
  if (err != FTPOK)
    {
      fd_close (*local_sock);
      return err;
    }
  if (*respline != '2')
    {
      xfree (respline);
      fd_close (*local_sock);
      return FTPPORTERR;
    }
  xfree (respline);
  return FTPOK;
}

static void
ip_address_to_eprt_repr (const ip_address *addr, int port, char *buf, 
                         size_t buflen)
{
  int afnum;

  assert (addr != NULL);
  assert (addr->type == IPV4_ADDRESS || addr->type == IPV6_ADDRESS);
  assert (buf != NULL);
  /* buf must contain the argument of EPRT (of the form |af|addr|port|). 
   * 4 chars for the | separators, INET6_ADDRSTRLEN chars for addr  
   * 1 char for af (1-2) and 5 chars for port (0-65535) */
  assert (buflen >= 4 + INET6_ADDRSTRLEN + 1 + 5); 

  /* Construct the argument of EPRT (of the form |af|addr|port|). */
  afnum = (addr->type == IPV4_ADDRESS ? 1 : 2);
  snprintf (buf, buflen, "|%d|%s|%d|", afnum, pretty_print_address (addr), port);
  buf[buflen - 1] = '\0';
}

/* Bind a port and send the appropriate PORT command to the FTP
   server.  Use acceptport after RETR, to get the socket of data
   connection.  */
uerr_t
ftp_eprt (int csock, int *local_sock)
{
  uerr_t err;
  char *request, *respline;
  ip_address addr;
  int nwritten;
  int port;
  /* Must contain the argument of EPRT (of the form |af|addr|port|). 
   * 4 chars for the | separators, INET6_ADDRSTRLEN chars for addr  
   * 1 char for af (1-2) and 5 chars for port (0-65535) */
  char bytes[4 + INET6_ADDRSTRLEN + 1 + 5 + 1];

  /* Get the address of this side of the connection. */
  if (!socket_ip_address (csock, &addr, ENDPOINT_LOCAL))
    return FTPSYSERR;

  assert (addr.type == IPV4_ADDRESS || addr.type == IPV6_ADDRESS);

  /* Setting port to 0 lets the system choose a free port.  */
  port = 0;

  /* Bind the port.  */
  *local_sock = bind_local (&addr, &port);
  if (*local_sock < 0)
    return FTPSYSERR;

  /* Construct the argument of EPRT (of the form |af|addr|port|). */
  ip_address_to_eprt_repr (&addr, port, bytes, sizeof (bytes));

  /* Send PORT request.  */
  request = ftp_request ("EPRT", bytes);
  nwritten = fd_write (csock, request, strlen (request), -1.0);
  if (nwritten < 0)
    {
      xfree (request);
      fd_close (*local_sock);
      return WRITEFAILED;
    }
  xfree (request);
  /* Get appropriate response.  */
  err = ftp_response (csock, &respline);
  if (err != FTPOK)
    {
      fd_close (*local_sock);
      return err;
    }
  if (*respline != '2')
    {
      xfree (respline);
      fd_close (*local_sock);
      return FTPPORTERR;
    }
  xfree (respline);
  return FTPOK;
}
#endif

/* Similar to ftp_port, but uses `PASV' to initiate the passive FTP
   transfer.  Reads the response from server and parses it.  Reads the
   host and port addresses and returns them.  */
uerr_t
ftp_pasv (int csock, ip_address *addr, int *port)
{
  char *request, *respline, *s;
  int nwritten, i;
  uerr_t err;
  unsigned char tmp[6];

  assert (addr != NULL);
  assert (port != NULL);

  xzero (*addr);

  /* Form the request.  */
  request = ftp_request ("PASV", NULL);
  /* And send it.  */
  nwritten = fd_write (csock, request, strlen (request), -1.0);
  if (nwritten < 0)
    {
      xfree (request);
      return WRITEFAILED;
    }
  xfree (request);
  /* Get the server response.  */
  err = ftp_response (csock, &respline);
  if (err != FTPOK)
    return err;
  if (*respline != '2')
    {
      xfree (respline);
      return FTPNOPASV;
    }
  /* Parse the request.  */
  s = respline;
  for (s += 4; *s && !ISDIGIT (*s); s++);
  if (!*s)
    return FTPINVPASV;
  for (i = 0; i < 6; i++)
    {
      tmp[i] = 0;
      for (; ISDIGIT (*s); s++)
        tmp[i] = (*s - '0') + 10 * tmp[i];
      if (*s == ',')
        s++;
      else if (i < 5)
        {
          /* When on the last number, anything can be a terminator.  */
          xfree (respline);
          return FTPINVPASV;
        }
    }
  xfree (respline);

  addr->type = IPV4_ADDRESS;
  memcpy (ADDRESS_IPV4_DATA (addr), tmp, 4);
  *port = ((tmp[4] << 8) & 0xff00) + tmp[5];

  return FTPOK;
}

#ifdef ENABLE_IPV6
/* Similar to ftp_lprt, but uses `LPSV' to initiate the passive FTP
   transfer.  Reads the response from server and parses it.  Reads the
   host and port addresses and returns them.  */
uerr_t
ftp_lpsv (int csock, ip_address *addr, int *port)
{
  char *request, *respline, *s;
  int nwritten, i, af, addrlen, portlen;
  uerr_t err;
  unsigned char tmp[16];
  unsigned char tmpprt[2];

  assert (addr != NULL);
  assert (port != NULL);

  xzero (*addr);

  /* Form the request.  */
  request = ftp_request ("LPSV", NULL);

  /* And send it.  */
  nwritten = fd_write (csock, request, strlen (request), -1.0);
  if (nwritten < 0)
    {
      xfree (request);
      return WRITEFAILED;
    }
  xfree (request);

  /* Get the server response.  */
  err = ftp_response (csock, &respline);
  if (err != FTPOK)
    return err;
  if (*respline != '2')
    {
      xfree (respline);
      return FTPNOPASV;
    }  

  /* Parse the response.  */
  s = respline;
  for (s += 4; *s && !ISDIGIT (*s); s++);
  if (!*s)
    return FTPINVPASV;

  /* First, get the address family */
  af = 0;
  for (; ISDIGIT (*s); s++)
    af = (*s - '0') + 10 * af;

  if (af != 4 && af != 6)
    {
      xfree (respline);
      return FTPINVPASV;
    }

  if (!*s || *s++ != ',')
    {
      xfree (respline);
      return FTPINVPASV;
    }

  /* Then, get the address length */
  addrlen = 0;
  for (; ISDIGIT (*s); s++)
    addrlen = (*s - '0') + 10 * addrlen;

  if (!*s || *s++ != ',')
    {
      xfree (respline);
      return FTPINVPASV;
    }

  if (addrlen > 16)
    {
      xfree (respline);
      return FTPINVPASV;
    }

  if ((af == 4 && addrlen != 4)
      || (af == 6 && addrlen != 16))
    {
      xfree (respline);
      return FTPINVPASV;
    }

  /* Now, we get the actual address */
  for (i = 0; i < addrlen; i++)
    {
      tmp[i] = 0;
      for (; ISDIGIT (*s); s++)
        tmp[i] = (*s - '0') + 10 * tmp[i];
      if (*s == ',')
        s++;
      else
        {
          xfree (respline);
          return FTPINVPASV;
        }
    }

  /* Now, get the port length */
  portlen = 0;
  for (; ISDIGIT (*s); s++)
    portlen = (*s - '0') + 10 * portlen;

  if (!*s || *s++ != ',')
    {
      xfree (respline);
      return FTPINVPASV;
    }

  if (portlen > 2)
    {
      xfree (respline);
      return FTPINVPASV;
    }

  /* Finally, we get the port number */
  tmpprt[0] = 0;
  for (; ISDIGIT (*s); s++)
    tmpprt[0] = (*s - '0') + 10 * tmpprt[0];

  if (!*s || *s++ != ',')
    {
      xfree (respline);
      return FTPINVPASV;
    }

  tmpprt[1] = 0;
  for (; ISDIGIT (*s); s++)
    tmpprt[1] = (*s - '0') + 10 * tmpprt[1];

  assert (s != NULL);

  if (af == 4)
    {
      addr->type = IPV4_ADDRESS;
      memcpy (ADDRESS_IPV4_DATA (addr), tmp, 4);
      *port = ((tmpprt[0] << 8) & 0xff00) + tmpprt[1];
      DEBUGP (("lpsv addr is: %s\n", pretty_print_address(addr)));
      DEBUGP (("tmpprt[0] is: %d\n", tmpprt[0]));
      DEBUGP (("tmpprt[1] is: %d\n", tmpprt[1]));
      DEBUGP (("*port is: %d\n", *port));
    }
  else
    {
      assert (af == 6);
      addr->type = IPV6_ADDRESS;
      memcpy (ADDRESS_IPV6_DATA (addr), tmp, 16);
      *port = ((tmpprt[0] << 8) & 0xff00) + tmpprt[1];
      DEBUGP (("lpsv addr is: %s\n", pretty_print_address(addr)));
      DEBUGP (("tmpprt[0] is: %d\n", tmpprt[0]));
      DEBUGP (("tmpprt[1] is: %d\n", tmpprt[1]));
      DEBUGP (("*port is: %d\n", *port));
    }

  xfree (respline);
  return FTPOK;
}

/* Similar to ftp_eprt, but uses `EPSV' to initiate the passive FTP
   transfer.  Reads the response from server and parses it.  Reads the
   host and port addresses and returns them.  */
uerr_t
ftp_epsv (int csock, ip_address *ip, int *port)
{
  char *request, *respline, *start, delim, *s;
  int nwritten, i;
  uerr_t err;
  int tport;

  assert (ip != NULL);
  assert (port != NULL);

  /* IP already contains the IP address of the control connection's
     peer, so we don't need to call socket_ip_address here.  */

  /* Form the request.  */
  /* EPSV 1 means that we ask for IPv4 and EPSV 2 means that we ask for IPv6. */
  request = ftp_request ("EPSV", (ip->type == IPV4_ADDRESS ? "1" : "2"));

  /* And send it.  */
  nwritten = fd_write (csock, request, strlen (request), -1.0);
  if (nwritten < 0)
    {
      xfree (request);
      return WRITEFAILED;
    }
  xfree (request);

  /* Get the server response.  */
  err = ftp_response (csock, &respline);
  if (err != FTPOK)
    return err;
  if (*respline != '2')
    {
      xfree (respline);
      return FTPNOPASV;
    }  

  assert (respline != NULL);

  DEBUGP(("respline is %s\n", respline));

  /* Parse the response.  */
  s = respline;

  /* Skip the useless stuff and get what's inside the parentheses */
  start = strchr (respline, '(');
  if (start == NULL)
    {
      xfree (respline);
      return FTPINVPASV;
    }  

  /* Skip the first two void fields */
  s = start + 1;
  delim = *s++;
  if (delim < 33 || delim > 126)
    {
      xfree (respline);
      return FTPINVPASV;
    }  

  for (i = 0; i < 2; i++)
    {
      if (*s++ != delim) 
        {
          xfree (respline);
        return FTPINVPASV;
        }  
    }

  /* Finally, get the port number */
  tport = 0; 
  for (i = 1; ISDIGIT (*s); s++) 
    {
      if (i > 5)
        {
          xfree (respline);
          return FTPINVPASV;
        }  
      tport = (*s - '0') + 10 * tport;
    }

  /* Make sure that the response terminates correcty */
  if (*s++ != delim)
    {
      xfree (respline);
      return FTPINVPASV;
    }  

  if (*s++ != ')')
    {
      xfree (respline);
      return FTPINVPASV;
    }  

  *port = tport;

  xfree (respline);
  return FTPOK;
}
#endif

/* Sends the TYPE request to the server.  */
uerr_t
ftp_type (int csock, int type)
{
  char *request, *respline;
  int nwritten;
  uerr_t err;
  char stype[2];

  /* Construct argument.  */
  stype[0] = type;
  stype[1] = 0;
  /* Send TYPE request.  */
  request = ftp_request ("TYPE", stype);
  nwritten = fd_write (csock, request, strlen (request), -1.0);
  if (nwritten < 0)
    {
      xfree (request);
      return WRITEFAILED;
    }
  xfree (request);
  /* Get appropriate response.  */
  err = ftp_response (csock, &respline);
  if (err != FTPOK)
    return err;
  if (*respline != '2')
    {
      xfree (respline);
      return FTPUNKNOWNTYPE;
    }
  xfree (respline);
  /* All OK.  */
  return FTPOK;
}

/* Changes the working directory by issuing a CWD command to the
   server.  */
uerr_t
ftp_cwd (int csock, const char *dir)
{
  char *request, *respline;
  int nwritten;
  uerr_t err;

  /* Send CWD request.  */
  request = ftp_request ("CWD", dir);
  nwritten = fd_write (csock, request, strlen (request), -1.0);
  if (nwritten < 0)
    {
      xfree (request);
      return WRITEFAILED;
    }
  xfree (request);
  /* Get appropriate response.  */
  err = ftp_response (csock, &respline);
  if (err != FTPOK)
    return err;
  if (*respline == '5')
    {
      xfree (respline);
      return FTPNSFOD;
    }
  if (*respline != '2')
    {
      xfree (respline);
      return FTPRERR;
    }
  xfree (respline);
  /* All OK.  */
  return FTPOK;
}

/* Sends REST command to the FTP server.  */
uerr_t
ftp_rest (int csock, wgint offset)
{
  char *request, *respline;
  int nwritten;
  uerr_t err;

  request = ftp_request ("REST", number_to_static_string (offset));
  nwritten = fd_write (csock, request, strlen (request), -1.0);
  if (nwritten < 0)
    {
      xfree (request);
      return WRITEFAILED;
    }
  xfree (request);
  /* Get appropriate response.  */
  err = ftp_response (csock, &respline);
  if (err != FTPOK)
    return err;
  if (*respline != '3')
    {
      xfree (respline);
      return FTPRESTFAIL;
    }
  xfree (respline);
  /* All OK.  */
  return FTPOK;
}

/* Sends RETR command to the FTP server.  */
uerr_t
ftp_retr (int csock, const char *file)
{
  char *request, *respline;
  int nwritten;
  uerr_t err;

  /* Send RETR request.  */
  request = ftp_request ("RETR", file);
  nwritten = fd_write (csock, request, strlen (request), -1.0);
  if (nwritten < 0)
    {
      xfree (request);
      return WRITEFAILED;
    }
  xfree (request);
  /* Get appropriate response.  */
  err = ftp_response (csock, &respline);
  if (err != FTPOK)
    return err;
  if (*respline == '5')
    {
      xfree (respline);
      return FTPNSFOD;
    }
  if (*respline != '1')
    {
      xfree (respline);
      return FTPRERR;
    }
  xfree (respline);
  /* All OK.  */
  return FTPOK;
}

/* Sends the LIST command to the server.  If FILE is NULL, send just
   `LIST' (no space).  */
uerr_t
ftp_list (int csock, const char *file)
{
  char *request, *respline;
  int nwritten;
  uerr_t err;

  /* Send LIST request.  */
  request = ftp_request ("LIST", file);
  nwritten = fd_write (csock, request, strlen (request), -1.0);
  if (nwritten < 0)
    {
      xfree (request);
      return WRITEFAILED;
    }
  xfree (request);
  /* Get appropriate respone.  */
  err = ftp_response (csock, &respline);
  if (err != FTPOK)
    return err;
  if (*respline == '5')
    {
      xfree (respline);
      return FTPNSFOD;
    }
  if (*respline != '1')
    {
      xfree (respline);
      return FTPRERR;
    }
  xfree (respline);
  /* All OK.  */
  return FTPOK;
}

/* Sends the SYST command to the server. */
uerr_t
ftp_syst (int csock, enum stype *server_type)
{
  char *request, *respline;
  int nwritten;
  uerr_t err;

  /* Send SYST request.  */
  request = ftp_request ("SYST", NULL);
  nwritten = fd_write (csock, request, strlen (request), -1.0);
  if (nwritten < 0)
    {
      xfree (request);
      return WRITEFAILED;
    }
  xfree (request);

  /* Get appropriate response.  */
  err = ftp_response (csock, &respline);
  if (err != FTPOK)
    return err;
  if (*respline == '5')
    {
      xfree (respline);
      return FTPSRVERR;
    }

  /* Skip the number (215, but 200 (!!!) in case of VMS) */
  strtok (respline, " ");

  /* Which system type has been reported (we are interested just in the
     first word of the server response)?  */
  request = strtok (NULL, " ");

  if (!strcasecmp (request, "VMS"))
    *server_type = ST_VMS;
  else if (!strcasecmp (request, "UNIX"))
    *server_type = ST_UNIX;
  else if (!strcasecmp (request, "WINDOWS_NT")
	   || !strcasecmp (request, "WINDOWS2000"))
    *server_type = ST_WINNT;
  else if (!strcasecmp (request, "MACOS"))
    *server_type = ST_MACOS;
  else if (!strcasecmp (request, "OS/400"))
    *server_type = ST_OS400;
  else
    *server_type = ST_OTHER;

  xfree (respline);
  /* All OK.  */
  return FTPOK;
}

/* Sends the PWD command to the server. */
uerr_t
ftp_pwd (int csock, char **pwd)
{
  char *request, *respline;
  int nwritten;
  uerr_t err;

  /* Send PWD request.  */
  request = ftp_request ("PWD", NULL);
  nwritten = fd_write (csock, request, strlen (request), -1.0);
  if (nwritten < 0)
    {
      xfree (request);
      return WRITEFAILED;
    }
  xfree (request);
  /* Get appropriate response.  */
  err = ftp_response (csock, &respline);
  if (err != FTPOK)
    return err;
  if (*respline == '5')
    {
    err:
      xfree (respline);
      return FTPSRVERR;
    }

  /* Skip the number (257), leading citation mark, trailing citation mark
     and everything following it. */
  strtok (respline, "\"");
  request = strtok (NULL, "\"");
  if (!request)
    /* Treat the malformed response as an error, which the caller has
       to handle gracefully anyway.  */
    goto err;

  /* Has the `pwd' been already allocated?  Free! */
  xfree_null (*pwd);

  *pwd = xstrdup (request);

  xfree (respline);
  /* All OK.  */
  return FTPOK;
}

/* Sends the SIZE command to the server, and returns the value in 'size'.
 * If an error occurs, size is set to zero. */
uerr_t
ftp_size (int csock, const char *file, wgint *size)
{
  char *request, *respline;
  int nwritten;
  uerr_t err;

  /* Send PWD request.  */
  request = ftp_request ("SIZE", file);
  nwritten = fd_write (csock, request, strlen (request), -1.0);
  if (nwritten < 0)
    {
      xfree (request);
      *size = 0;
      return WRITEFAILED;
    }
  xfree (request);
  /* Get appropriate response.  */
  err = ftp_response (csock, &respline);
  if (err != FTPOK)
    {
      *size = 0;
      return err;
    }
  if (*respline == '5')
    {
      /* 
       * Probably means SIZE isn't supported on this server.
       * Error is nonfatal since SIZE isn't in RFC 959 
       */
      xfree (respline);
      *size = 0;
      return FTPOK;
    }

  errno = 0;
  *size = str_to_wgint (respline + 4, NULL, 10);
  if (errno)
    {
      /* 
       * Couldn't parse the response for some reason.  On the (few)
       * tests I've done, the response is 213 <SIZE> with nothing else -
       * maybe something a bit more resilient is necessary.  It's not a
       * fatal error, however.
       */
      xfree (respline);
      *size = 0;
      return FTPOK;
    }

  xfree (respline);
  /* All OK.  */
  return FTPOK;
}

/* If URL's params are of the form "type=X", return character X.
   Otherwise, return 'I' (the default type).  */
char
ftp_process_type (const char *params)
{
  if (params
      && 0 == strncasecmp (params, "type=", 5)
      && params[5] != '\0')
    return TOUPPER (params[5]);
  else
    return 'I';
}
