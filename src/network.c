/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001, Sergei Barbarash <sgt@outline.ru>
 *
 * $Id: network.c,v 1.1 2002/01/05 15:31:18 sgt Exp $
 */

#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include "network.h"
#include "rc.h"
#include "wmlj.h"

typedef struct {
  CURL *curl;
  CURLcode res;
  GString *buf;
} Request;

static size_t curlwrite(void *ptr, size_t size, size_t nmemb, void *stream) {
  Request *req = stream;

  g_string_append(req->buf, ptr);

  return size*nmemb;
}

static char*
skipline(char *text) {
  while (*text != 0) {
    if (*text == '\n') {
      text++;
      break;
    }
    text++;
  }
  return text;
}

static char*
getline(char *text) {
  int len;
  char *str;

  for (len = 0; text[len] != 0; len++)
    if (text[len] == '\r' || text[len] == '\n') break;

  str = g_new(char, len+1);
  memcpy(str, text, len+1);
  str[len] = 0;
  return str;
}

static gboolean
free_all_hash_cb(gpointer key, gpointer val, gpointer user_data) {
  g_free(key); g_free(val);
  return TRUE;
}

static void
hash_free(GHashTable *hash) {
  g_hash_table_foreach_remove(hash, free_all_hash_cb, NULL);
  g_hash_table_destroy(hash);
}

static GHashTable*
parse_response(char *buf) {
  char *key, *val;
  GHashTable *hash = g_hash_table_new(g_str_hash, g_str_equal);
  int read_keys = 0;

  while (*buf) {
    key = getline(buf);
    buf = skipline(buf);
    if (*buf) {
      val = getline(buf);
      buf = skipline(buf);

      g_hash_table_insert(hash, g_strdup(key), g_strdup(val));

      if (DEBUG)
	fprintf(stderr, "parse_response: key '%s'; val '%s'\n",
		key, val);

      read_keys++;
    }
  }
  if (read_keys == 0) { /* error. */
    hash_free(hash);
    return NULL;
  }
  return hash;
}

static GHashTable*
request_run(char *postfields) {
  Request *req;
  GString *url;
  GHashTable *hash;

  req = g_new0(Request, 1);

  req->buf = g_string_sized_new(2048);

  req->curl = curl_easy_init(); {

    if ((req->curl) == NULL)
      return NULL;

    /* specify server URL */
    url = g_string_sized_new(64);
    g_string_sprintf(url, "http://%s:%d/%s", conf.lj_server, conf.lj_port,
		     SERVER_PATH);
    curl_easy_setopt(req->curl, CURLOPT_URL, url->str);

    /* passing request structure to writefunction */
    curl_easy_setopt(req->curl, CURLOPT_FILE, req);

    /* specify writefunction handler */
    curl_easy_setopt(req->curl, CURLOPT_WRITEFUNCTION, curlwrite);

    /* it's POST we're trying to make */
    curl_easy_setopt(req->curl, CURLOPT_POST, 1);

    /* self-promotion */
    curl_easy_setopt(req->curl, CURLOPT_USERAGENT, USERAGENT);

    /* the data */
    curl_easy_setopt(req->curl, CURLOPT_POSTFIELDS, postfields);

    /* be verbose for debugging purposes */
    if (DEBUG)
      curl_easy_setopt(req->curl, CURLOPT_VERBOSE, 1);

    /* do the magic */
    req->res = curl_easy_perform(req->curl);

    /* always cleanup */
    curl_easy_cleanup(req->curl);

    hash = parse_response(req->buf->str);

    g_string_free(req->buf, TRUE);

    g_free(req);

    return hash;
  }
}

gboolean
check_friends() {
  GHashTable *hash;
  GString *req_str;
  gint ok_interval;

  gdk_threads_enter();

  req_str = g_string_sized_new(1024);

  if (cf.lastupdate == NULL)
    cf.lastupdate = g_strdup("");

  g_string_sprintfa(req_str, "user=%s&", conf.user);
  g_string_sprintfa(req_str, "password=%s&", conf.password);
  g_string_sprintfa(req_str, "mode=%s&", "checkfriends");
  g_string_sprintfa(req_str, "lastupdate=%s&", cf.lastupdate);

  hash = request_run(req_str->str);

  g_string_free(req_str, TRUE);

  cf.lastupdate = g_strdup(g_hash_table_lookup(hash, "lastupdate"));
  cf.new_messages = atoi(g_hash_table_lookup(hash, "new"));

  /* interval shouldn't be less than one that's required by the server */
  ok_interval = MAX(conf.interval,
		    atoi(g_hash_table_lookup(hash, "interval")));
  if (conf.interval < ok_interval) {
    conf.interval = ok_interval;
    wmlj_cf_timeout_remove();
    wmlj_cf_timeout_add();
  }

  /* if there are new messages, let's start the animation */
  if (cf.new_messages > 0)
    wmlj_anim_timeout_add();

  if (DEBUG)
    fprintf(stderr, "check_friends: new '%d'; last '%s'; interval '%s'\n",
	    cf.new_messages, cf.lastupdate,
	    (char*)g_hash_table_lookup(hash, "interval"));

  hash_free(hash);

  gdk_threads_leave();

  return TRUE;
}
