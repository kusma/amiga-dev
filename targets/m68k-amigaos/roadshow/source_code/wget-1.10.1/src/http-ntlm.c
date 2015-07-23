/* NTLM code.
   Copyright (C) 2005 Free Software Foundation, Inc.
   Donated by Daniel Stenberg.

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

/* NTLM details:
   
   http://davenport.sourceforge.net/ntlm.html
   http://www.innovation.ch/java/ntlm.html

*/

/* -- WIN32 approved -- */
#include <stdio.h>
#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif
#include <stdlib.h>

#include <openssl/des.h>
#include <openssl/md4.h>

#include "wget.h"
#include "utils.h"
#include "http-ntlm.h"

#if OPENSSL_VERSION_NUMBER < 0x00907001L
#define DES_key_schedule des_key_schedule
#define DES_cblock des_cblock
#define DES_set_odd_parity des_set_odd_parity
#define DES_set_key des_set_key
#define DES_ecb_encrypt des_ecb_encrypt

/* This is how things were done in the old days */
#define DESKEY(x) x
#define DESKEYARG(x) x
#else
/* Modern version */
#define DESKEYARG(x) *x
#define DESKEY(x) &x
#endif

/* Define this to make the type-3 message include the NT response message */
#define USE_NTRESPONSES 1

/* Flag bits definitions available at on
   http://davenport.sourceforge.net/ntlm.html */

#define NTLMFLAG_NEGOTIATE_UNICODE               (1<<0)
#define NTLMFLAG_NEGOTIATE_OEM                   (1<<1)
#define NTLMFLAG_REQUEST_TARGET                  (1<<2)
/* unknown (1<<3) */
#define NTLMFLAG_NEGOTIATE_SIGN                  (1<<4)
#define NTLMFLAG_NEGOTIATE_SEAL                  (1<<5)
#define NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE        (1<<6)
#define NTLMFLAG_NEGOTIATE_LM_KEY                (1<<7)
#define NTLMFLAG_NEGOTIATE_NETWARE               (1<<8)
#define NTLMFLAG_NEGOTIATE_NTLM_KEY              (1<<9)
/* unknown (1<<10) */
/* unknown (1<<11) */
#define NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED       (1<<12)
#define NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED  (1<<13)
#define NTLMFLAG_NEGOTIATE_LOCAL_CALL            (1<<14)
#define NTLMFLAG_NEGOTIATE_ALWAYS_SIGN           (1<<15)
#define NTLMFLAG_TARGET_TYPE_DOMAIN              (1<<16)
#define NTLMFLAG_TARGET_TYPE_SERVER              (1<<17)
#define NTLMFLAG_TARGET_TYPE_SHARE               (1<<18)
#define NTLMFLAG_NEGOTIATE_NTLM2_KEY             (1<<19)
#define NTLMFLAG_REQUEST_INIT_RESPONSE           (1<<20)
#define NTLMFLAG_REQUEST_ACCEPT_RESPONSE         (1<<21)
#define NTLMFLAG_REQUEST_NONNT_SESSION_KEY       (1<<22)
#define NTLMFLAG_NEGOTIATE_TARGET_INFO           (1<<23)
/* unknown (1<24) */
/* unknown (1<25) */
/* unknown (1<26) */
/* unknown (1<27) */
/* unknown (1<28) */
#define NTLMFLAG_NEGOTIATE_128                   (1<<29)
#define NTLMFLAG_NEGOTIATE_KEY_EXCHANGE          (1<<30)
#define NTLMFLAG_NEGOTIATE_56                    (1<<31)

/*
  (*) = A "security buffer" is a triplet consisting of two shorts and one
  long:

  1. a 'short' containing the length of the buffer in bytes
  2. a 'short' containing the allocated space for the buffer in bytes
  3. a 'long' containing the offset to the start of the buffer from the
     beginning of the NTLM message, in bytes.
*/

/* return 1 on success, 0 otherwise */
int
ntlm_input (struct ntlmdata *ntlm, const char *header)
{
  if (0 != strncmp (header, "NTLM", 4))
    return 0;

  header += 4;
  while (*header && ISSPACE(*header))
    header++;

  if (*header)
    {
      /* We got a type-2 message here:

         Index   Description         Content
         0       NTLMSSP Signature   Null-terminated ASCII "NTLMSSP"
                                     (0x4e544c4d53535000)
         8       NTLM Message Type   long (0x02000000)
         12      Target Name         security buffer(*)
         20      Flags               long
         24      Challenge           8 bytes
         (32)    Context (optional)  8 bytes (two consecutive longs)
         (40)    Target Information  (optional) security buffer(*)
         32 (48) start of data block
      */
      int size;
      char *buffer = (char *) alloca (strlen (header));

      DEBUGP (("Received a type-2 NTLM message.\n"));

      size = base64_decode (header, buffer);
      if (size < 0)
	return 0;		/* malformed base64 from server */

      ntlm->state = NTLMSTATE_TYPE2; /* we got a type-2 */

      if (size >= 48)
        /* the nonce of interest is index [24 .. 31], 8 bytes */
        memcpy (ntlm->nonce, &buffer[24], 8);

      /* at index decimal 20, there's a 32bit NTLM flag field */
    }
  else
    {
      if (ntlm->state >= NTLMSTATE_TYPE1)
	{
	  DEBUGP (("Unexpected empty NTLM message.\n"));
	  return 0; /* this is an error */
	}

      DEBUGP (("Empty NTLM message, starting transaction.\n"));
      ntlm->state = NTLMSTATE_TYPE1; /* we should sent away a type-1 */
    }

  return 1;
}

/*
 * Turns a 56 bit key into the 64 bit, odd parity key and sets the key.  The
 * key schedule ks is also set.
 */
static void
setup_des_key(unsigned char *key_56,
	      DES_key_schedule DESKEYARG(ks))
{
  DES_cblock key;

  key[0] = key_56[0];
  key[1] = ((key_56[0] << 7) & 0xFF) | (key_56[1] >> 1);
  key[2] = ((key_56[1] << 6) & 0xFF) | (key_56[2] >> 2);
  key[3] = ((key_56[2] << 5) & 0xFF) | (key_56[3] >> 3);
  key[4] = ((key_56[3] << 4) & 0xFF) | (key_56[4] >> 4);
  key[5] = ((key_56[4] << 3) & 0xFF) | (key_56[5] >> 5);
  key[6] = ((key_56[5] << 2) & 0xFF) | (key_56[6] >> 6);
  key[7] =  (key_56[6] << 1) & 0xFF;

  DES_set_odd_parity(&key);
  DES_set_key(&key, ks);
}

 /*
  * takes a 21 byte array and treats it as 3 56-bit DES keys. The
  * 8 byte plaintext is encrypted with each key and the resulting 24
  * bytes are stored in the results array.
  */
static void
calc_resp(unsigned char *keys, unsigned char *plaintext, unsigned char *results)
{
  DES_key_schedule ks;

  setup_des_key(keys, DESKEY(ks));
  DES_ecb_encrypt((DES_cblock*) plaintext, (DES_cblock*) results,
                  DESKEY(ks), DES_ENCRYPT);

  setup_des_key(keys+7, DESKEY(ks));
  DES_ecb_encrypt((DES_cblock*) plaintext, (DES_cblock*) (results+8),
                  DESKEY(ks), DES_ENCRYPT);

  setup_des_key(keys+14, DESKEY(ks));
  DES_ecb_encrypt((DES_cblock*) plaintext, (DES_cblock*) (results+16),
                  DESKEY(ks), DES_ENCRYPT);
}

/*
 * Set up lanmanager and nt hashed passwords
 */
static void
mkhash(const char *password,
       unsigned char *nonce,	/* 8 bytes */
       unsigned char *lmresp	/* must fit 0x18 bytes */
#ifdef USE_NTRESPONSES
       , unsigned char *ntresp  /* must fit 0x18 bytes */
#endif
  )
{
  unsigned char lmbuffer[21];
#ifdef USE_NTRESPONSES
  unsigned char ntbuffer[21];
#endif
  unsigned char *pw;
  static const unsigned char magic[] = {
    0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25
  };
  int i;
  int len = strlen(password);

  /* make it fit at least 14 bytes */
  pw = (unsigned char *) alloca (len < 7 ? 14 : len * 2);

  if (len > 14)
    len = 14;
  
  for (i=0; i<len; i++)
    pw[i] = TOUPPER (password[i]);

  for (; i<14; i++)
    pw[i] = 0;

  {
    /* create LanManager hashed password */
    DES_key_schedule ks;

    setup_des_key(pw, DESKEY(ks));
    DES_ecb_encrypt((DES_cblock *)magic, (DES_cblock *)lmbuffer,
                    DESKEY(ks), DES_ENCRYPT);
  
    setup_des_key(pw+7, DESKEY(ks));
    DES_ecb_encrypt((DES_cblock *)magic, (DES_cblock *)(lmbuffer+8),
                    DESKEY(ks), DES_ENCRYPT);

    memset(lmbuffer+16, 0, 5);
  }
  /* create LM responses */
  calc_resp(lmbuffer, nonce, lmresp);

#ifdef USE_NTRESPONSES
  {
    /* create NT hashed password */
    MD4_CTX MD4;

    len = strlen(password);

    for (i=0; i<len; i++) {
      pw[2*i]   = password[i];
      pw[2*i+1] = 0;
    }

    MD4_Init(&MD4);
    MD4_Update(&MD4, pw, 2*len);
    MD4_Final(ntbuffer, &MD4);

    memset(ntbuffer+16, 0, 5);
  }

  calc_resp(ntbuffer, nonce, ntresp);
#endif
}

#define SHORTPAIR(x) ((x) & 0xff), ((x) >> 8)
#define LONGQUARTET(x) ((x) & 0xff), (((x) >> 8)&0xff), \
  (((x) >>16)&0xff), ((x)>>24)

/* this is for creating ntlm header output */
char *
ntlm_output (struct ntlmdata *ntlm, const char *user, const char *passwd,
	     int *ready)
{
  const char *domain=""; /* empty */
  const char *host=""; /* empty */
  int domlen=strlen(domain);
  int hostlen = strlen(host);
  int hostoff; /* host name offset */
  int domoff;  /* domain name offset */
  int size;
  char *base64;
  char ntlmbuf[256]; /* enough, unless the host/domain is very long */

  /* point to the address of the pointer that holds the string to sent to the
     server, which is for a plain host or for a HTTP proxy */
  char *output;

  *ready = 0;

  /* not set means empty */
  if(!user)
    user="";

  if(!passwd)
    passwd="";
  
  switch(ntlm->state) {
  case NTLMSTATE_TYPE1:
  default: /* for the weird cases we (re)start here */
    hostoff = 32;
    domoff = hostoff + hostlen;

    DEBUGP (("Creating a type-1 NTLM message.\n"));
    
    /* Create and send a type-1 message:

    Index Description          Content
    0     NTLMSSP Signature    Null-terminated ASCII "NTLMSSP"
                               (0x4e544c4d53535000)
    8     NTLM Message Type    long (0x01000000)
    12    Flags                long
    16    Supplied Domain      security buffer(*)
    24    Supplied Workstation security buffer(*)
    32    start of data block

    Format string (merged for pre-ANSI compilers):
      "NTLMSSP%c"
      "\x01%c%c%c" 32-bit type = 1
      "%c%c%c%c"   32-bit NTLM flag field
      "%c%c"  domain length
      "%c%c"  domain allocated space
      "%c%c"  domain name offset
      "%c%c"  2 zeroes
      "%c%c"  host length
      "%c%c"  host allocated space
      "%c%c"  host name offset
      "%c%c"  2 zeroes
      "%s"    host name
      "%s"   domain string
    */

    snprintf(ntlmbuf, sizeof(ntlmbuf),
	     "NTLMSSP%c\001%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%s%s",
             0,     /* trailing zero */
             0,0,0, /* part of type-1 long */

             LONGQUARTET(
               NTLMFLAG_NEGOTIATE_OEM|      /*   2 */
               NTLMFLAG_NEGOTIATE_NTLM_KEY  /* 200 */
               /* equals 0x0202 */
               ),
             SHORTPAIR(domlen),
             SHORTPAIR(domlen),
             SHORTPAIR(domoff),
             0,0,
             SHORTPAIR(hostlen),
             SHORTPAIR(hostlen),
             SHORTPAIR(hostoff),
             0,0,
             host, domain);

    /* initial packet length */
    size = 32 + hostlen + domlen;

    base64 = (char *) alloca (BASE64_LENGTH (size) + 1);
    base64_encode (ntlmbuf, size, base64);

    output = concat_strings ("NTLM ", base64, (char *) 0);
    break;
    
  case NTLMSTATE_TYPE2:
    /* We received the type-2 already, create a type-3 message:

    Index   Description            Content
    0       NTLMSSP Signature      Null-terminated ASCII "NTLMSSP"
                                   (0x4e544c4d53535000)
    8       NTLM Message Type      long (0x03000000)
    12      LM/LMv2 Response       security buffer(*)
    20      NTLM/NTLMv2 Response   security buffer(*)
    28      Domain Name            security buffer(*)
    36      User Name              security buffer(*)
    44      Workstation Name       security buffer(*)
    (52)    Session Key (optional) security buffer(*)
    (60)    Flags (optional)       long
    52 (64) start of data block

    */
  
  {
    int lmrespoff;
    int ntrespoff;
    int useroff;
    unsigned char lmresp[0x18]; /* fixed-size */
#ifdef USE_NTRESPONSES
    unsigned char ntresp[0x18]; /* fixed-size */
#endif
    const char *usr;
    int userlen;

    DEBUGP (("Creating a type-3 NTLM message.\n"));

    usr = strchr(user, '\\');
    if(!usr)
      usr = strchr(user, '/');

    if (usr) {
      domain = user;
      domlen = usr - domain;
      usr++;
    }
    else
      usr = user;
    userlen = strlen(usr);

    mkhash(passwd, &ntlm->nonce[0], lmresp
#ifdef USE_NTRESPONSES
           , ntresp
#endif
      );

    domoff = 64; /* always */
    useroff = domoff + domlen;
    hostoff = useroff + userlen;
    lmrespoff = hostoff + hostlen;
    ntrespoff = lmrespoff + 0x18;

    /* Create the big type-3 message binary blob:
	 "NTLMSSP%c"
	 "\x03%c%c%c"  type-3, 32 bits 

	 "%c%c%c%c"  LanManager length + allocated space 
	 "%c%c"      LanManager offset 
	 "%c%c"      2 zeroes 

	 "%c%c"  NT-response length 
	 "%c%c"  NT-response allocated space 
	 "%c%c"  NT-response offset 
	 "%c%c"  2 zeroes 

	 "%c%c"  domain length 
	 "%c%c"  domain allocated space 
	 "%c%c"  domain name offset 
	 "%c%c"  2 zeroes 

	 "%c%c"  user length 
	 "%c%c"  user allocated space 
	 "%c%c"  user offset 
	 "%c%c"  2 zeroes 

	 "%c%c"  host length 
	 "%c%c"  host allocated space 
	 "%c%c"  host offset 
	 "%c%c%c%c%c%c" 6 zeroes 

	 "\xff\xff"   message length 
	 "%c%c"   2 zeroes 

	 "\x01\x82"  flags 
	 "%c%c"   2 zeroes */

    size = snprintf(ntlmbuf, sizeof(ntlmbuf),
		    "NTLMSSP%c\003%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\377\377%c%c\001\202%c%c",
                    0, /* zero termination */
                    0,0,0, /* type-3 long, the 24 upper bits */

                    SHORTPAIR(0x18),  /* LanManager response length, twice */
                    SHORTPAIR(0x18),
                    SHORTPAIR(lmrespoff),
                    0x0, 0x0,
                    
#ifdef USE_NTRESPONSES
                    SHORTPAIR(0x18),  /* NT-response length, twice */
                    SHORTPAIR(0x18),
#else
                    0x0, 0x0,
                    0x0, 0x0,
#endif
                    SHORTPAIR(ntrespoff),
                    0x0, 0x0,

                    SHORTPAIR(domlen),
                    SHORTPAIR(domlen),
                    SHORTPAIR(domoff),
                    0x0, 0x0,

                    SHORTPAIR(userlen),
                    SHORTPAIR(userlen),
                    SHORTPAIR(useroff),
                    0x0, 0x0,
                    
                    SHORTPAIR(hostlen),
                    SHORTPAIR(hostlen),
                    SHORTPAIR(hostoff),
                    0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
             
                    0x0, 0x0,

                    0x0, 0x0);

    /* size is now 64 */
    size=64;
    ntlmbuf[62]=ntlmbuf[63]=0;

    memcpy(&ntlmbuf[size], domain, domlen);
    size += domlen;

    memcpy(&ntlmbuf[size], usr, userlen);
    size += userlen;

    /* we append the binary hashes to the end of the blob */
    if(size < ((int)sizeof(ntlmbuf) - 0x18)) {
      memcpy(&ntlmbuf[size], lmresp, 0x18);
      size += 0x18;
    }

#ifdef USE_NTRESPONSES
    if(size < ((int)sizeof(ntlmbuf) - 0x18)) {      
      memcpy(&ntlmbuf[size], ntresp, 0x18);
      size += 0x18;
    }
#endif

    ntlmbuf[56] = size & 0xff;
    ntlmbuf[57] = size >> 8;

    /* convert the binary blob into base64 */
    base64 = (char *) alloca (BASE64_LENGTH (size) + 1);
    base64_encode (ntlmbuf, size, base64);

    output = concat_strings ("NTLM ", base64, (char *) 0);

    ntlm->state = NTLMSTATE_TYPE3; /* we sent a type-3 */
    *ready = 1;
  }
  break;

  case NTLMSTATE_TYPE3:
    /* connection is already authenticated,
     * don't send a header in future requests */
    *ready = 1;
    output = NULL;
    break;
  }

  return output;
}
