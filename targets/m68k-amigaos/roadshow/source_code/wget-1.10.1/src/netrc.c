/* Read and parse the .netrc file to get hosts, accounts, and passwords.
   Copyright (C) 1996, Free Software Foundation, Inc.

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

/* This file used to be kept in synch with the code in Fetchmail, but
   the latter has diverged since.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif
#include <sys/types.h>
#include <errno.h>

#include "wget.h"
#include "utils.h"
#include "netrc.h"
#include "init.h"

#ifndef errno
extern int errno;
#endif

#define NETRC_FILE_NAME ".netrc"

acc_t *netrc_list;

static acc_t *parse_netrc PARAMS ((const char *));

/* Return the correct user and password, given the host, user (as
   given in the URL), and password (as given in the URL).  May return
   NULL.

   If SLACK_DEFAULT is set, allow looking for a "default" account.
   You will typically turn it off for HTTP.  */
void
search_netrc (const char *host, const char **acc, const char **passwd,
	      int slack_default)
{
  acc_t *l;
  static int processed_netrc;

  if (!opt.netrc)
    return;
  /* Find ~/.netrc.  */
  if (!processed_netrc)
    {
      char *home = home_dir ();

      netrc_list = NULL;
      processed_netrc = 1;
      if (home)
	{
	  int err;
	  struct_stat buf;
	  char *path = (char *)alloca (strlen (home) + 1
				       + strlen (NETRC_FILE_NAME) + 1);
	  sprintf (path, "%s/%s", home, NETRC_FILE_NAME);
	  xfree (home);
	  err = stat (path, &buf);
	  if (err == 0)
	    netrc_list = parse_netrc (path);
	}
    }
  /* If nothing to do...  */
  if (!netrc_list)
    return;
  /* Acc and password found; all OK.  */
  if (*acc && *passwd)
    return;
  /* Some data not given -- try finding the host.  */
  for (l = netrc_list; l; l = l->next)
    {
      if (!l->host)
	continue;
      else if (!strcasecmp (l->host, host))
	break;
    }
  if (l)
    {
      if (*acc)
	{
	  /* Looking for password in .netrc.  */
	  if (!strcmp (l->acc, *acc))
	    *passwd = l->passwd; /* usernames match; password OK */
	  else
	    *passwd = NULL;     /* usernames don't match */
	}
      else			/* NOT *acc */
	{
	  /* If password was given, use it.  The account is l->acc.  */
	  *acc = l->acc;
	  if (l->passwd)
	    *passwd = l->passwd;
	}
      return;
    }
  else
    {
      if (!slack_default)
	return;
      if (*acc)
	return;
      /* Try looking for the default account.  */
      for (l = netrc_list; l; l = l->next)
	if (!l->host)
	  break;
      if (!l)
	return;
      *acc = l->acc;
      if (!*passwd)
	*passwd = l->passwd;
      return;
    }
}


#ifdef STANDALONE

#include <assert.h>

/* Normally, these functions would be defined by your package.  */
# define xmalloc malloc
# define xfree free
# define xstrdup strdup

# define xrealloc realloc

/* Read a line from FP.  The function reallocs the storage as needed
   to accomodate for any length of the line.  Reallocs are done
   storage exponentially, doubling the storage after each overflow to
   minimize the number of calls to realloc() and fgets().  The newline
   character at the end of line is retained.

   After end-of-file is encountered without anything being read, NULL
   is returned.  NULL is also returned on error.  To distinguish
   between these two cases, use the stdio function ferror().  */

char *
read_whole_line (FILE *fp)
{
  int length = 0;
  int bufsize = 81;
  char *line = (char *)xmalloc (bufsize);

  while (fgets (line + length, bufsize - length, fp))
    {
      length += strlen (line + length);
      assert (length > 0);
      if (line[length - 1] == '\n')
	break;
      /* fgets() guarantees to read the whole line, or to use up the
         space we've given it.  We can double the buffer
         unconditionally.  */
      bufsize <<= 1;
      line = xrealloc (line, bufsize);
    }
  if (length == 0 || ferror (fp))
    {
      xfree (line);
      return NULL;
    }
  if (length + 1 < bufsize)
    /* Relieve the memory from our exponential greediness.  We say
       `length + 1' because the terminating \0 is not included in
       LENGTH.  We don't need to zero-terminate the string ourselves,
       though, because fgets() does that.  */
    line = xrealloc (line, length + 1);
  return line;
}
#endif /* STANDALONE */

/* Maybe add NEWENTRY to the account information list, LIST.  NEWENTRY is
   set to a ready-to-use acc_t, in any event.  */
static void
maybe_add_to_list (acc_t **newentry, acc_t **list)
{
  acc_t *a, *l;
  a = *newentry;
  l = *list;

  /* We need an account name in order to add the entry to the list.  */
  if (a && ! a->acc)
    {
      /* Free any allocated space.  */
      xfree_null (a->host);
      xfree_null (a->acc);
      xfree_null (a->passwd);
    }
  else
    {
      if (a)
	{
	  /* Add the current machine into our list.  */
	  a->next = l;
	  l = a;
	}

      /* Allocate a new acc_t structure.  */
      a = (acc_t *)xmalloc (sizeof (acc_t));
    }

  /* Zero the structure, so that it is ready to use.  */
  memset (a, 0, sizeof(*a));

  /* Return the new pointers.  */
  *newentry = a;
  *list = l;
  return;
}

/* Helper function for the parser, shifts contents of
   null-terminated string once character to the left.
   Used in processing \ and " constructs in the netrc file */
static void
shift_left(char *string)
{
  char *p;
  
  for (p=string; *p; ++p)
    *p = *(p+1);
}

/* Parse a .netrc file (as described in the ftp(1) manual page).  */
static acc_t *
parse_netrc (const char *path)
{
  FILE *fp;
  char *line, *p, *tok;
  const char *premature_token;
  acc_t *current, *retval;
  int ln, quote;

  /* The latest token we've seen in the file.  */
  enum
  {
    tok_nothing, tok_account, tok_login, tok_macdef, tok_machine, tok_password
  } last_token = tok_nothing;

  current = retval = NULL;

  fp = fopen (path, "r");
  if (!fp)
    {
      fprintf (stderr, _("%s: Cannot read %s (%s).\n"), exec_name,
	       path, strerror (errno));
      return retval;
    }

  /* Initialize the file data.  */
  ln = 0;
  premature_token = NULL;

  /* While there are lines in the file...  */
  while ((line = read_whole_line (fp)) != NULL)
    {
      ln ++;

      /* Parse the line.  */
      p = line;
      quote = 0;

      /* Skip leading whitespace.  */
      while (*p && ISSPACE (*p))
	p ++;

      /* If the line is empty, then end any macro definition.  */
      if (last_token == tok_macdef && !*p)
	/* End of macro if the line is empty.  */
	last_token = tok_nothing;

      /* If we are defining macros, then skip parsing the line.  */
      while (*p && last_token != tok_macdef)
	{
	  /* Skip any whitespace.  */
	  while (*p && ISSPACE (*p))
	    p ++;

	  /* Discard end-of-line comments; also, stop processing if
	     the above `while' merely skipped trailing whitespace.  */
	  if (*p == '#' || !*p)
	    break;

	  /* If the token starts with quotation mark, note this fact,
	     and squash the quotation character */
	  if (*p == '"'){
	    quote = 1;
	    shift_left (p);
	  }

	  tok = p;

	  /* Find the end of the token, handling quotes and escapes.  */
	  while (*p && (quote ? *p != '"' : !ISSPACE (*p))){
	    if (*p == '\\')
	      shift_left (p);
	    p ++;
	  }

	  /* If field was quoted, squash the trailing quotation mark
	     and reset quote flag.  */
	  if (quote)
	    {
	      shift_left (p);
	      quote = 0;
	    }

	  /* Null-terminate the token, if it isn't already.  */
	  if (*p)
	    *p ++ = '\0';

	  switch (last_token)
	    {
	    case tok_login:
	      if (current)
		current->acc = xstrdup (tok);
	      else
		premature_token = "login";
	      break;

	    case tok_machine:
	      /* Start a new machine entry.  */
	      maybe_add_to_list (&current, &retval);
	      current->host = xstrdup (tok);
	      break;

	    case tok_password:
	      if (current)
		current->passwd = xstrdup (tok);
	      else
		premature_token = "password";
	      break;

	      /* We handle most of tok_macdef above.  */
	    case tok_macdef:
	      if (!current)
		premature_token = "macdef";
	      break;

	      /* We don't handle the account keyword at all.  */
	    case tok_account:
	      if (!current)
		premature_token = "account";
	      break;

	      /* We handle tok_nothing below this switch.  */
	    case tok_nothing:
	      break;
	    }

	  if (premature_token)
	    {
	      fprintf (stderr, _("\
%s: %s:%d: warning: \"%s\" token appears before any machine name\n"),
		       exec_name, path, ln, premature_token);
	      premature_token = NULL;
	    }

	  if (last_token != tok_nothing)
	    /* We got a value, so reset the token state.  */
	    last_token = tok_nothing;
	  else
	    {
	      /* Fetch the next token.  */
	      if (!strcmp (tok, "account"))
		last_token = tok_account;
	      else if (!strcmp (tok, "default"))
		{
		  maybe_add_to_list (&current, &retval);
		}
	      else if (!strcmp (tok, "login"))
		last_token = tok_login;

	      else if (!strcmp (tok, "macdef"))
		last_token = tok_macdef;

	      else if (!strcmp (tok, "machine"))
		last_token = tok_machine;

	      else if (!strcmp (tok, "password"))
		last_token = tok_password;

	      else
		fprintf (stderr, _("%s: %s:%d: unknown token \"%s\"\n"),
			 exec_name, path, ln, tok);
	    }
	}

      xfree (line);
    }

  fclose (fp);

  /* Finalize the last machine entry we found.  */
  maybe_add_to_list (&current, &retval);
  xfree (current);

  /* Reverse the order of the list so that it appears in file order.  */
  current = retval;
  retval = NULL;
  while (current)
    {
      acc_t *saved_reference;

      /* Change the direction of the pointers.  */
      saved_reference = current->next;
      current->next = retval;

      /* Advance to the next node.  */
      retval = current;
      current = saved_reference;
    }

  return retval;
}


/* Free a netrc list.  */
void
free_netrc(acc_t *l)
{
  acc_t *t;

  while (l)
    {
      t = l->next;
      xfree_null (l->acc);
      xfree_null (l->passwd);
      xfree_null (l->host);
      xfree (l);
      l = t;
    }
}

#ifdef STANDALONE
#include <sys/types.h>
#include <sys/stat.h>

int
main (int argc, char **argv)
{
  struct_stat sb;
  char *program_name, *file, *target;
  acc_t *head, *a;

  if (argc < 2 || argc > 3)
    {
      fprintf (stderr, _("Usage: %s NETRC [HOSTNAME]\n"), argv[0]);
      exit (1);
    }

  program_name = argv[0];
  file = argv[1];
  target = argv[2];

  if (stat (file, &sb))
    {
      fprintf (stderr, _("%s: cannot stat %s: %s\n"), argv[0], file,
	       strerror (errno));
      exit (1);
    }

  head = parse_netrc (file);
  a = head;
  while (a)
    {
      /* Skip if we have a target and this isn't it.  */
      if (target && a->host && strcmp (target, a->host))
	{
	  a = a->next;
	  continue;
	}

      if (!target)
	{
	  /* Print the host name if we have no target.  */
	  if (a->host)
	    fputs (a->host, stdout);
	  else
	    fputs ("DEFAULT", stdout);

	  fputc (' ', stdout);
	}

      /* Print the account name.  */
      fputs (a->acc, stdout);

      if (a->passwd)
	{
	  /* Print the password, if there is any.  */
	  fputc (' ', stdout);
	  fputs (a->passwd, stdout);
	}

      fputc ('\n', stdout);

      /* Exit if we found the target.  */
      if (target)
	exit (0);
      a = a->next;
    }

  /* Exit with failure if we had a target, success otherwise.  */
  if (target)
    exit (1);

  exit (0);
}
#endif /* STANDALONE */
