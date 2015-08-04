/* Collect URLs from HTML source.
   Copyright (C) 1998, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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

#include <stdio.h>
#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include "wget.h"
#include "html-parse.h"
#include "url.h"
#include "utils.h"
#include "hash.h"
#include "convert.h"
#include "recur.h"		/* declaration of get_urls_html */

#ifndef errno
extern int errno;
#endif

struct map_context;

typedef void (*tag_handler_t) PARAMS ((int, struct taginfo *,
				       struct map_context *));

#define DECLARE_TAG_HANDLER(fun)					\
  static void fun PARAMS ((int, struct taginfo *, struct map_context *))

DECLARE_TAG_HANDLER (tag_find_urls);
DECLARE_TAG_HANDLER (tag_handle_base);
DECLARE_TAG_HANDLER (tag_handle_form);
DECLARE_TAG_HANDLER (tag_handle_link);
DECLARE_TAG_HANDLER (tag_handle_meta);

enum {
  TAG_A,
  TAG_APPLET,
  TAG_AREA,
  TAG_BASE,
  TAG_BGSOUND,
  TAG_BODY,
  TAG_EMBED,
  TAG_FIG,
  TAG_FORM,
  TAG_FRAME,
  TAG_IFRAME,
  TAG_IMG,
  TAG_INPUT,
  TAG_LAYER,
  TAG_LINK,
  TAG_META,
  TAG_OBJECT,
  TAG_OVERLAY,
  TAG_SCRIPT,
  TAG_TABLE,
  TAG_TD,
  TAG_TH
};

/* The list of known tags and functions used for handling them.  Most
   tags are simply harvested for URLs. */
static struct known_tag {
  int tagid;
  const char *name;
  tag_handler_t handler;
} known_tags[] = {
  { TAG_A,	 "a",		tag_find_urls },
  { TAG_APPLET,	 "applet",	tag_find_urls },
  { TAG_AREA,	 "area",	tag_find_urls },
  { TAG_BASE,	 "base",	tag_handle_base },
  { TAG_BGSOUND, "bgsound",	tag_find_urls },
  { TAG_BODY,	 "body",	tag_find_urls },
  { TAG_EMBED,	 "embed",	tag_find_urls },
  { TAG_FIG,	 "fig",		tag_find_urls },
  { TAG_FORM,	 "form",	tag_handle_form },
  { TAG_FRAME,	 "frame",	tag_find_urls },
  { TAG_IFRAME,	 "iframe",	tag_find_urls },
  { TAG_IMG,	 "img",		tag_find_urls },
  { TAG_INPUT,	 "input",	tag_find_urls },
  { TAG_LAYER,	 "layer",	tag_find_urls },
  { TAG_LINK,	 "link",	tag_handle_link },
  { TAG_META,	 "meta",	tag_handle_meta },
  { TAG_OBJECT,  "object",	tag_find_urls },
  { TAG_OVERLAY, "overlay",	tag_find_urls },
  { TAG_SCRIPT,	 "script",	tag_find_urls },
  { TAG_TABLE,	 "table",	tag_find_urls },
  { TAG_TD,	 "td",		tag_find_urls },
  { TAG_TH,	 "th",		tag_find_urls }
};

/* tag_url_attributes documents which attributes of which tags contain
   URLs to harvest.  It is used by tag_find_urls.  */

/* Defines for the FLAGS. */

/* The link is "inline", i.e. needs to be retrieved for this document
   to be correctly rendered.  Inline links include inlined images,
   stylesheets, children frames, etc.  */
#define ATTR_INLINE	1

/* The link is expected to yield HTML contents.  It's important not to
   try to follow HTML obtained by following e.g. <img src="...">
   regardless of content-type.  Doing this causes infinite loops for
   "images" that return non-404 error pages with links to the same
   image.  */
#define ATTR_HTML	2

/* For tags handled by tag_find_urls: attributes that contain URLs to
   download. */
static struct {
  int tagid;
  const char *attr_name;
  int flags;
} tag_url_attributes[] = {
  { TAG_A,		"href",		ATTR_HTML },
  { TAG_APPLET,		"code",		ATTR_INLINE },
  { TAG_AREA,		"href",		ATTR_HTML },
  { TAG_BGSOUND,	"src",		ATTR_INLINE },
  { TAG_BODY,		"background",	ATTR_INLINE },
  { TAG_EMBED,		"href",		ATTR_HTML },
  { TAG_EMBED,		"src",		ATTR_INLINE | ATTR_HTML },
  { TAG_FIG,		"src",		ATTR_INLINE },
  { TAG_FRAME,		"src",		ATTR_INLINE | ATTR_HTML },
  { TAG_IFRAME,		"src",		ATTR_INLINE | ATTR_HTML },
  { TAG_IMG,		"href",		ATTR_INLINE },
  { TAG_IMG,		"lowsrc",	ATTR_INLINE },
  { TAG_IMG,		"src",		ATTR_INLINE },
  { TAG_INPUT,		"src",		ATTR_INLINE },
  { TAG_LAYER,		"src",		ATTR_INLINE | ATTR_HTML },
  { TAG_OBJECT,		"data",		ATTR_INLINE },
  { TAG_OVERLAY,	"src",		ATTR_INLINE | ATTR_HTML },
  { TAG_SCRIPT,		"src",		ATTR_INLINE },
  { TAG_TABLE,		"background",	ATTR_INLINE },
  { TAG_TD,		"background",	ATTR_INLINE },
  { TAG_TH,		"background",	ATTR_INLINE }
};

/* The lists of interesting tags and attributes are built dynamically,
   from the information above.  However, some places in the code refer
   to the attributes not mentioned here.  We add them manually.  */
static const char *additional_attributes[] = {
  "rel",			/* used by tag_handle_link */
  "http-equiv",			/* used by tag_handle_meta */
  "name",			/* used by tag_handle_meta */
  "content",			/* used by tag_handle_meta */
  "action"			/* used by tag_handle_form */
};

struct hash_table *interesting_tags;
struct hash_table *interesting_attributes;

static void
init_interesting (void)
{
  /* Init the variables interesting_tags and interesting_attributes
     that are used by the HTML parser to know which tags and
     attributes we're interested in.  We initialize this only once,
     for performance reasons.

     Here we also make sure that what we put in interesting_tags
     matches the user's preferences as specified through --ignore-tags
     and --follow-tags.  */

  int i;
  interesting_tags = make_nocase_string_hash_table (countof (known_tags));

  /* First, add all the tags we know hot to handle, mapped to their
     respective entries in known_tags.  */
  for (i = 0; i < countof (known_tags); i++)
    hash_table_put (interesting_tags, known_tags[i].name, known_tags + i);

  /* Then remove the tags ignored through --ignore-tags.  */
  if (opt.ignore_tags)
    {
      char **ignored;
      for (ignored = opt.ignore_tags; *ignored; ignored++)
	hash_table_remove (interesting_tags, *ignored);
    }

  /* If --follow-tags is specified, use only those tags.  */
  if (opt.follow_tags)
    {
      /* Create a new table intersecting --follow-tags and known_tags,
	 and use it as interesting_tags.  */
      struct hash_table *intersect = make_nocase_string_hash_table (0);
      char **followed;
      for (followed = opt.follow_tags; *followed; followed++)
	{
	  struct known_tag *t = hash_table_get (interesting_tags, *followed);
	  if (!t)
	    continue;		/* ignore unknown --follow-tags entries. */
	  hash_table_put (intersect, *followed, t);
	}
      hash_table_destroy (interesting_tags);
      interesting_tags = intersect;
    }

  /* Add the attributes we care about. */
  interesting_attributes = make_nocase_string_hash_table (10);
  for (i = 0; i < countof (additional_attributes); i++)
    hash_table_put (interesting_attributes, additional_attributes[i], "1");
  for (i = 0; i < countof (tag_url_attributes); i++)
    hash_table_put (interesting_attributes,
		    tag_url_attributes[i].attr_name, "1");
}

/* Find the value of attribute named NAME in the taginfo TAG.  If the
   attribute is not present, return NULL.  If ATTRIND is non-NULL, the
   index of the attribute in TAG will be stored there.  */

static char *
find_attr (struct taginfo *tag, const char *name, int *attrind)
{
  int i;
  for (i = 0; i < tag->nattrs; i++)
    if (!strcasecmp (tag->attrs[i].name, name))
      {
	if (attrind)
	  *attrind = i;
	return tag->attrs[i].value;
      }
  return NULL;
}

struct map_context {
  char *text;			/* HTML text. */
  char *base;			/* Base URI of the document, possibly
				   changed through <base href=...>. */
  const char *parent_base;	/* Base of the current document. */
  const char *document_file;	/* File name of this document. */
  int nofollow;			/* whether NOFOLLOW was specified in a
                                   <meta name=robots> tag. */

  struct urlpos *head, *tail;	/* List of URLs that is being
				   built. */
};

/* Append LINK_URI to the urlpos structure that is being built.

   LINK_URI will be merged with the current document base.  TAG and
   ATTRIND are the necessary context to store the position and
   size.  */

static struct urlpos *
append_url (const char *link_uri,
	    struct taginfo *tag, int attrind, struct map_context *ctx)
{
  int link_has_scheme = url_has_scheme (link_uri);
  struct urlpos *newel;
  const char *base = ctx->base ? ctx->base : ctx->parent_base;
  struct url *url;

  if (!base)
    {
      DEBUGP (("%s: no base, merge will use \"%s\".\n",
	       ctx->document_file, link_uri));

      if (!link_has_scheme)
	{
	  /* Base URL is unavailable, and the link does not have a
	     location attached to it -- we have to give up.  Since
	     this can only happen when using `--force-html -i', print
	     a warning.  */
	  logprintf (LOG_NOTQUIET,
		     _("%s: Cannot resolve incomplete link %s.\n"),
		     ctx->document_file, link_uri);
	  return NULL;
	}

      url = url_parse (link_uri, NULL);
      if (!url)
	{
	  DEBUGP (("%s: link \"%s\" doesn't parse.\n",
		   ctx->document_file, link_uri));
	  return NULL;
	}
    }
  else
    {
      /* Merge BASE with LINK_URI, but also make sure the result is
	 canonicalized, i.e. that "../" have been resolved.
	 (parse_url will do that for us.) */

      char *complete_uri = uri_merge (base, link_uri);

      DEBUGP (("%s: merge(\"%s\", \"%s\") -> %s\n",
	       ctx->document_file, base, link_uri, complete_uri));

      url = url_parse (complete_uri, NULL);
      if (!url)
	{
	  DEBUGP (("%s: merged link \"%s\" doesn't parse.\n",
		   ctx->document_file, complete_uri));
	  xfree (complete_uri);
	  return NULL;
	}
      xfree (complete_uri);
    }

  DEBUGP (("appending \"%s\" to urlpos.\n", url->url));

  newel = xnew0 (struct urlpos);
  newel->url = url;
  newel->pos = tag->attrs[attrind].value_raw_beginning - ctx->text;
  newel->size = tag->attrs[attrind].value_raw_size;

  /* A URL is relative if the host is not named, and the name does not
     start with `/'.  */
  if (!link_has_scheme && *link_uri != '/')
    newel->link_relative_p = 1;
  else if (link_has_scheme)
    newel->link_complete_p = 1;

  if (ctx->tail)
    {
      ctx->tail->next = newel;
      ctx->tail = newel;
    }
  else
    ctx->tail = ctx->head = newel;

  return newel;
}

/* All the tag_* functions are called from collect_tags_mapper, as
   specified by KNOWN_TAGS.  */

/* Default tag handler: collect URLs from attributes specified for
   this tag by tag_url_attributes.  */

static void
tag_find_urls (int tagid, struct taginfo *tag, struct map_context *ctx)
{
  int i, attrind;
  int first = -1;

  for (i = 0; i < countof (tag_url_attributes); i++)
    if (tag_url_attributes[i].tagid == tagid)
      {
	/* We've found the index of tag_url_attributes where the
	   attributes of our tag begin.  */
	first = i;
	break;
      }
  assert (first != -1);

  /* Loop over the "interesting" attributes of this tag.  In this
     example, it will loop over "src" and "lowsrc".

       <img src="foo.png" lowsrc="bar.png">

     This has to be done in the outer loop so that the attributes are
     processed in the same order in which they appear in the page.
     This is required when converting links.  */

  for (attrind = 0; attrind < tag->nattrs; attrind++)
    {
      /* Find whether TAG/ATTRIND is a combination that contains a
	 URL. */
      char *link = tag->attrs[attrind].value;
      const int size = countof (tag_url_attributes);

      /* If you're cringing at the inefficiency of the nested loops,
	 remember that they both iterate over a very small number of
	 items.  The worst-case inner loop is for the IMG tag, which
	 has three attributes.  */
      for (i = first; i < size && tag_url_attributes[i].tagid == tagid; i++)
	{
	  if (0 == strcasecmp (tag->attrs[attrind].name,
			       tag_url_attributes[i].attr_name))
	    {
	      struct urlpos *up = append_url (link, tag, attrind, ctx);
	      if (up)
		{
		  int flags = tag_url_attributes[i].flags;
		  if (flags & ATTR_INLINE)
		    up->link_inline_p = 1;
		  if (flags & ATTR_HTML)
		    up->link_expect_html = 1;
		}
	    }
	}
    }
}

/* Handle the BASE tag, for <base href=...>. */

static void
tag_handle_base (int tagid, struct taginfo *tag, struct map_context *ctx)
{
  struct urlpos *base_urlpos;
  int attrind;
  char *newbase = find_attr (tag, "href", &attrind);
  if (!newbase)
    return;

  base_urlpos = append_url (newbase, tag, attrind, ctx);
  if (!base_urlpos)
    return;
  base_urlpos->ignore_when_downloading = 1;
  base_urlpos->link_base_p = 1;

  if (ctx->base)
    xfree (ctx->base);
  if (ctx->parent_base)
    ctx->base = uri_merge (ctx->parent_base, newbase);
  else
    ctx->base = xstrdup (newbase);
}

/* Mark the URL found in <form action=...> for conversion. */

static void
tag_handle_form (int tagid, struct taginfo *tag, struct map_context *ctx)
{
  int attrind;
  char *action = find_attr (tag, "action", &attrind);
  if (action)
    {
      struct urlpos *up = append_url (action, tag, attrind, ctx);
      if (up)
	up->ignore_when_downloading = 1;
    }
}

/* Handle the LINK tag.  It requires special handling because how its
   links will be followed in -p mode depends on the REL attribute.  */

static void
tag_handle_link (int tagid, struct taginfo *tag, struct map_context *ctx)
{
  int attrind;
  char *href = find_attr (tag, "href", &attrind);

  /* All <link href="..."> link references are external, except those
     known not to be, such as style sheet and shortcut icon:

       <link rel="stylesheet" href="...">
       <link rel="shortcut icon" href="...">
  */
  if (href)
    {
      struct urlpos *up = append_url (href, tag, attrind, ctx);
      if (up)
	{
	  char *rel = find_attr (tag, "rel", NULL);
	  if (rel
	      && (0 == strcasecmp (rel, "stylesheet")
		  || 0 == strcasecmp (rel, "shortcut icon")))
	    up->link_inline_p = 1;
	  else
	    /* The external ones usually point to HTML pages, such as
	       <link rel="next" href="..."> */
	    up->link_expect_html = 1;
	}
    }
}

/* Handle the META tag.  This requires special handling because of the
   refresh feature and because of robot exclusion.  */

static void
tag_handle_meta (int tagid, struct taginfo *tag, struct map_context *ctx)
{
  char *name = find_attr (tag, "name", NULL);
  char *http_equiv = find_attr (tag, "http-equiv", NULL);

  if (http_equiv && 0 == strcasecmp (http_equiv, "refresh"))
    {
      /* Some pages use a META tag to specify that the page be
	 refreshed by a new page after a given number of seconds.  The
	 general format for this is:

	   <meta http-equiv=Refresh content="NUMBER; URL=index2.html">

	 So we just need to skip past the "NUMBER; URL=" garbage to
	 get to the URL.  */

      struct urlpos *entry;
      int attrind;
      int timeout = 0;
      char *p;

      char *refresh = find_attr (tag, "content", &attrind);
      if (!refresh)
	return;

      for (p = refresh; ISDIGIT (*p); p++)
	timeout = 10 * timeout + *p - '0';
      if (*p++ != ';')
	return;

      while (ISSPACE (*p))
	++p;
      if (!(   TOUPPER (*p)       == 'U'
	    && TOUPPER (*(p + 1)) == 'R'
	    && TOUPPER (*(p + 2)) == 'L'
	    &&          *(p + 3)  == '='))
	return;
      p += 4;
      while (ISSPACE (*p))
	++p;

      entry = append_url (p, tag, attrind, ctx);
      if (entry)
	{
	  entry->link_refresh_p = 1;
	  entry->refresh_timeout = timeout;
	  entry->link_expect_html = 1;
	}
    }
  else if (name && 0 == strcasecmp (name, "robots"))
    {
      /* Handle stuff like:
	 <meta name="robots" content="index,nofollow"> */
      char *content = find_attr (tag, "content", NULL);
      if (!content)
	return;
      if (!strcasecmp (content, "none"))
	ctx->nofollow = 1;
      else
	{
	  while (*content)
	    {
	      /* Find the next occurrence of ',' or the end of
		 the string.  */
	      char *end = strchr (content, ',');
	      if (end)
		++end;
	      else
		end = content + strlen (content);
	      if (!strncasecmp (content, "nofollow", end - content))
		ctx->nofollow = 1;
	      content = end;
	    }
	}
    }
}

/* Dispatch the tag handler appropriate for the tag we're mapping
   over.  See known_tags[] for definition of tag handlers.  */

static void
collect_tags_mapper (struct taginfo *tag, void *arg)
{
  struct map_context *ctx = (struct map_context *)arg;

  /* Find the tag in our table of tags.  This must not fail because
     map_html_tags only returns tags found in interesting_tags.  */
  struct known_tag *t = hash_table_get (interesting_tags, tag->name);
  assert (t != NULL);

  t->handler (t->tagid, tag, ctx);
}

/* Analyze HTML tags FILE and construct a list of URLs referenced from
   it.  It merges relative links in FILE with URL.  It is aware of
   <base href=...> and does the right thing.  */

struct urlpos *
get_urls_html (const char *file, const char *url, int *meta_disallow_follow)
{
  struct file_memory *fm;
  struct map_context ctx;
  int flags;

  /* Load the file. */
  fm = read_file (file);
  if (!fm)
    {
      logprintf (LOG_NOTQUIET, "%s: %s\n", file, strerror (errno));
      return NULL;
    }
  DEBUGP (("Loaded %s (size %s).\n", file, number_to_static_string (fm->length)));

  ctx.text = fm->content;
  ctx.head = ctx.tail = NULL;
  ctx.base = NULL;
  ctx.parent_base = url ? url : opt.base_href;
  ctx.document_file = file;
  ctx.nofollow = 0;

  if (!interesting_tags)
    init_interesting ();

  /* Specify MHT_TRIM_VALUES because of buggy HTML generators that
     generate <a href=" foo"> instead of <a href="foo"> (browsers
     ignore spaces as well.)  If you really mean space, use &32; or
     %20.  MHT_TRIM_VALUES also causes squashing of embedded newlines,
     e.g. in <img src="foo.[newline]html">.  Such newlines are also
     ignored by IE and Mozilla and are presumably introduced by
     writing HTML with editors that force word wrap.  */
  flags = MHT_TRIM_VALUES;
  if (opt.strict_comments)
    flags |= MHT_STRICT_COMMENTS;

  map_html_tags (fm->content, fm->length, collect_tags_mapper, &ctx, flags,
		 interesting_tags, interesting_attributes);

  DEBUGP (("no-follow in %s: %d\n", file, ctx.nofollow));
  if (meta_disallow_follow)
    *meta_disallow_follow = ctx.nofollow;

  xfree_null (ctx.base);
  read_file_free (fm);
  return ctx.head;
}

/* This doesn't really have anything to do with HTML, but it's similar
   to get_urls_html, so we put it here.  */

struct urlpos *
get_urls_file (const char *file)
{
  struct file_memory *fm;
  struct urlpos *head, *tail;
  const char *text, *text_end;

  /* Load the file.  */
  fm = read_file (file);
  if (!fm)
    {
      logprintf (LOG_NOTQUIET, "%s: %s\n", file, strerror (errno));
      return NULL;
    }
  DEBUGP (("Loaded %s (size %s).\n", file, number_to_static_string (fm->length)));

  head = tail = NULL;
  text = fm->content;
  text_end = fm->content + fm->length;
  while (text < text_end)
    {
      int up_error_code;
      char *url_text;
      struct urlpos *entry;
      struct url *url;

      const char *line_beg = text;
      const char *line_end = memchr (text, '\n', text_end - text);
      if (!line_end)
	line_end = text_end;
      else
	++line_end;
      text = line_end;

      /* Strip whitespace from the beginning and end of line. */
      while (line_beg < line_end && ISSPACE (*line_beg))
	++line_beg;
      while (line_end > line_beg && ISSPACE (*(line_end - 1)))
	--line_end;

      if (line_beg == line_end)
	continue;

      /* The URL is in the [line_beg, line_end) region. */

      /* We must copy the URL to a zero-terminated string, and we
	 can't use alloca because we're in a loop.  *sigh*.  */
      url_text = strdupdelim (line_beg, line_end);

      if (opt.base_href)
	{
	  /* Merge opt.base_href with URL. */
	  char *merged = uri_merge (opt.base_href, url_text);
	  xfree (url_text);
	  url_text = merged;
	}

      url = url_parse (url_text, &up_error_code);
      if (!url)
	{
	  logprintf (LOG_NOTQUIET, _("%s: Invalid URL %s: %s\n"),
		     file, url_text, url_error (up_error_code));
	  xfree (url_text);
	  continue;
	}
      xfree (url_text);

      entry = xnew0 (struct urlpos);
      entry->next = NULL;
      entry->url = url;

      if (!head)
	head = entry;
      else
	tail->next = entry;
      tail = entry;
    }
  read_file_free (fm);
  return head;
}

void
cleanup_html_url (void)
{
  /* Destroy the hash tables.  The hash table keys and values are not
     allocated by this code, so we don't need to free them here.  */
  if (interesting_tags)
    hash_table_destroy (interesting_tags);
  if (interesting_attributes)
    hash_table_destroy (interesting_attributes);
}
