/*
 * POSIX Compatibility Library for AmigaOS
 *
 * Written by Frank Wille <frank@phoenix.owl.de> in 2003
 *
 * $Id: getopt.c,v 1.2 2005/02/27 13:45:24 phx Exp $
 */

/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Dieter Baron and Thomas Klausner.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <getopt.h>

#define IGNORE_FIRST  (*options == '-' || *options == '+')
#define PRINT_ERROR ((opterr) && ((*options != ':') \
                     || (IGNORE_FIRST && options[1] != ':')))
#define EMSG  ""

/* return values */
#define BADCH (int)'?'
#define BADARG  (int)':'
#define INORDER (int)1


int opterr = 1;     /* if error message should be printed */
int optind = 1;     /* index into parent argv vector */
int optopt = BADCH; /* character checked for validity */
int optreset;       /* reset getopt */
char *optarg;       /* argument associated with option */

static char *place = EMSG; /* option letter processing */
static int nonopt_start = -1; /* first non option argument (for permute) */
static int nonopt_end = -1;   /* first option after non options (for permute) */

/* Error messages */
static const char recargchar[] = "option requires an argument -- %c\n";
static const char recargstring[] = "option requires an argument -- %s\n";
static const char ambig[] = "ambiguous option -- %.*s\n";
static const char noarg[] = "option doesn't take an argument -- %.*s\n";
static const char illoptchar[] = "illegal option -- %c\n";
static const char illoptstring[] = "illegal option -- %s\n";



static int gcd(int a,int b)
{
  int c = a % b;

  while (c != 0) {
    a = b;
    b = c;
    c = a % b;
  }     
  return b;
}


static void permute_args(int nonopt_start,int nonopt_end,int opt_end,
                         char * const *nargv)
{
  int cstart, cyclelen, i, j, ncycle, nnonopts, nopts, pos;
  char *swap;

  nnonopts = nonopt_end - nonopt_start;
  nopts = opt_end - nonopt_end;
  ncycle = gcd(nnonopts, nopts);
  cyclelen = (opt_end - nonopt_start) / ncycle;

  for (i=0; i<ncycle; i++) {
    cstart = nonopt_end+i;
    pos = cstart;
    for (j=0; j<cyclelen; j++) {
      if (pos >= nonopt_end)
        pos -= nnonopts;
      else
        pos += nopts;
      swap = nargv[pos];
      ((char **)nargv)[pos] = nargv[cstart];
      ((char **)nargv)[cstart] = swap;
    }
  }
}


static int getopt_internal(int nargc,char * const *nargv,const char *options)
{
  char *oli;   /* option letter list index */
  int optchar;

  optarg = NULL;

  if (optind == 0)
    optind = 1;

  if (optreset)
    nonopt_start = nonopt_end = -1;

  if (optreset || !*place) {        /* update scanning pointer */
    optreset = 0;
    if (optind >= nargc) {          /* end of argument vector */
      place = EMSG;
      if (nonopt_end != -1) {
        /* do permutation, if we have to */
        permute_args(nonopt_start, nonopt_end, optind, nargv);
        optind -= nonopt_end - nonopt_start;
      }
      else if (nonopt_start != -1) {  /* skipped non-options */
        optind = nonopt_start;
      }
      nonopt_start = nonopt_end = -1;
      return -1;
    }
    if (*(place = nargv[optind]) != '-') {  /* found non-option */
      place = EMSG;
      return -1;
    }
    if (nonopt_start != -1 && nonopt_end == -1)
      nonopt_end = optind;
    if (place[1] && *++place == '-') {  /* found "--" */
      place++;
      return -2;
    }
  }

  if ((optchar = (int)*place++) == (int)':' ||
      (oli = strchr(options + (IGNORE_FIRST ? 1 : 0), optchar)) == NULL) {
    /* option letter unknown or ':' */
    if (!*place)
      ++optind;
    if (PRINT_ERROR)
      fprintf(stderr, illoptchar, optchar);
    optopt = optchar;
    return BADCH;
  }

  if (optchar == 'W' && oli[1] == ';') {    /* -W long-option */
    if (*place) 
      return -2;

    if (++optind >= nargc) {  /* no arg */
      place = EMSG;
      if (PRINT_ERROR)
        fprintf(stderr, recargchar, optchar);
      optopt = optchar;
      return BADARG;
    }
    else        /* white space */
      place = nargv[optind];
    /*
     * Handle -W arg the same as --arg (which causes getopt to
     * stop parsing).
     */
    return -2;
  }

  if (*++oli != ':') {      /* doesn't take argument */
    if (!*place)
      ++optind;
  }
  else {        /* takes (optional) argument */
    optarg = NULL;
    if (*place)     /* no white space */
      optarg = place;
    else if (oli[1] != ':') {   /* arg not optional */
      if (++optind >= nargc) {  /* no arg */
        place = EMSG;
        if (PRINT_ERROR)
          fprintf(stderr, recargchar, optchar);
        optopt = optchar;
        return BADARG;
      }
      else
        optarg = nargv[optind];
    }
    place = EMSG;
    ++optind;
  }

  /* dump back option letter */
  return optchar;
}


int getopt(int nargc,char * const *nargv,const char *options)
{
  int retval;

  if ((retval = getopt_internal(nargc, nargv, options)) == -2) {
    ++optind;
    if (nonopt_end != -1) {
      permute_args(nonopt_start, nonopt_end, optind, nargv);
      optind -= nonopt_end - nonopt_start;
    }
    nonopt_start = nonopt_end = -1;
    retval = -1;
  }
  return retval;
}


int getopt_long(int nargc,char * const *nargv,const char *options,
                const struct option *long_options,int *idx)
{
  int retval;

  if ((retval = getopt_internal(nargc, nargv, options)) == -2) {
    char *current_argv, *has_equal;
    size_t current_argv_len;
    int i, match;

    current_argv = place;
    match = -1;
    optind++;
    place = EMSG;

    if (*current_argv == '\0') {    /* found "--" */
      if (nonopt_end != -1) {
        permute_args(nonopt_start, nonopt_end,
            optind, nargv);
        optind -= nonopt_end - nonopt_start;
      }
      nonopt_start = nonopt_end = -1;
      return -1;
    }
    if ((has_equal = strchr(current_argv, '=')) != NULL) {
      /* argument found (--option=arg) */
      current_argv_len = has_equal - current_argv;
      has_equal++;
    }
    else
      current_argv_len = strlen(current_argv);

    for (i=0; long_options[i].name; i++) {
      /* find matching long option */
      if (strncmp(current_argv, long_options[i].name, current_argv_len))
        continue;

      if (strlen(long_options[i].name) == (unsigned)current_argv_len) {
        /* exact match */
        match = i;
        break;
      }

      if (match == -1)    /* partial match */
        match = i;
      else {
        /* ambiguous abbreviation */
        if (PRINT_ERROR)
          fprintf(stderr, ambig, (int)current_argv_len, current_argv);
        optopt = 0;
        return BADCH;
      }
    }

    if (match != -1) {      /* option found */
      if (long_options[match].has_arg == no_argument && has_equal) {
        if (PRINT_ERROR)
          fprintf(stderr, noarg, (int)current_argv_len, current_argv);
        if (long_options[match].flag == NULL)
          optopt = long_options[match].val;
        else
          optopt = 0;
        return BADARG;
      }

      if (long_options[match].has_arg == required_argument ||
          long_options[match].has_arg == optional_argument) {
        if (has_equal)
          optarg = has_equal;
        else if (long_options[match].has_arg == required_argument)
          optarg = nargv[optind++];
      }

      if ((long_options[match].has_arg == required_argument)
          && (optarg == NULL)) {
        if (PRINT_ERROR)
          fprintf(stderr, recargstring, current_argv);
        if (long_options[match].flag == NULL)
          optopt = long_options[match].val;
        else
          optopt = 0;
        --optind;
        return BADARG;
      }
    }
    else {      /* unknown option */
      if (PRINT_ERROR)
        fprintf(stderr, illoptstring, current_argv);
      optopt = 0;
      return BADCH;
    }

    if (long_options[match].flag) {
      *long_options[match].flag = long_options[match].val;
      retval = 0;
    }
    else 
      retval = long_options[match].val;

    if (idx)
      *idx = match;
  }
  return retval;
}
