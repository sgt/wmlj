/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001,2002 Sergei Barbarash <sgt@livejournal.com>
 *
 * $Id: network.c,v 1.16 2002/02/07 16:35:27 sgt Exp $
 */

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include "md5.h"
#include "dlg.h"
#include "network.h"
#include "rc.h"
#include "wmlj.h"

typedef struct {
  CURL *curl;
  CURLcode res;
  GString *buf;
} Request;

/* a dialog for showing network errors */
static void
network_error(gchar *msg) {
  GtkWidget *dialog, *vbox;

  dialog = dlg_new(NULL, "wmlj network error");

  vbox = gtk_vbox_new(FALSE, 5); {
    GtkWidget *label;

    label = gtk_label_new(g_strdup_printf("wmlj network error:\n\n%s", msg));
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);

    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, FALSE, 0);
  }

  dlg_set_contents(dialog, vbox);
  dlg_add_close(dialog, NULL);
  gtk_widget_show(dialog);
}

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
hash_curl_error() {
  GHashTable *hash = g_hash_table_new(g_str_hash, g_str_equal);

  g_hash_table_insert(hash, "http-error", "error");

  return hash;
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
request_run(gchar *postfields) {
  Request *req;
  GString *url;
  GHashTable *hash;
  gchar errorbuf[CURL_ERROR_SIZE];

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

    /* proxy, if needed */
    if (conf.use_proxy) {
      curl_easy_setopt(req->curl, CURLOPT_PROXY, conf.proxy_server);
      curl_easy_setopt(req->curl, CURLOPT_PROXYPORT, conf.proxy_port);
    }

    /* self-promotion */
    curl_easy_setopt(req->curl, CURLOPT_USERAGENT, USERAGENT);

    /* add a cookie for the fast server */
    if (conf.use_fast)
      curl_easy_setopt(req->curl, CURLOPT_COOKIE, "ljfastserver=1;");

   /* the data */
    curl_easy_setopt(req->curl, CURLOPT_POSTFIELDS, postfields);

    /* be verbose for debugging purposes */
    if (DEBUG)
      curl_easy_setopt(req->curl, CURLOPT_VERBOSE, 1);

    /* here we can get a fancy error message, eh? */
    curl_easy_setopt(req->curl, CURLOPT_ERRORBUFFER, errorbuf);

    /* do the magic */
    req->res = curl_easy_perform(req->curl);

    if (req->res != CURLE_OK) {
      network_error(errorbuf);
      hash = hash_curl_error();
    }
    else {
      hash = parse_response(req->buf->str);
    }

    /* always cleanup */
    curl_easy_cleanup(req->curl);

    g_string_free(req->buf, TRUE);

    g_free(req);

    return hash;
  }
}

static gboolean
request_succeeded(GHashTable *result) {
  char *success;

  if (result == NULL)
    return FALSE;

  success = g_hash_table_lookup(result, "success");
  if (success == NULL)
    return FALSE;

  return (g_strcasecmp(success, "OK") == 0);
}

static void
md5_hash(char *src, char *dest) {
  struct MD5Context context;
  unsigned char checksum[16];
  int i;

  MD5Init(&context);
  MD5Update(&context, (const unsigned char*)src, strlen(src));
  MD5Final(checksum, &context);
  for (i = 0; i < 16; i++)
    sprintf(&dest[i*2], "%02x", (unsigned int)checksum[i]);
}

static GString*
lj_protocol_string_new(gchar *mode) {
  GString *str;
  char hpassword[33];

  str = g_string_sized_new(64);

  g_string_sprintfa(str, "user=%s&", conf.user);
  /* g_string_sprintfa(str, "password=%s&", conf.password); */
  md5_hash(conf.password, hpassword);
  g_string_sprintfa(str, "hpassword=%s&", hpassword);
  g_string_sprintfa(str, "mode=%s&", mode);

  return str;
}

gboolean
check_friends() {
  GHashTable *hash;
  GString *req_str;
  gint ok_interval;

  if (wmlj.cf.lastupdate == NULL)
    wmlj.cf.lastupdate = g_strdup("");

  req_str = lj_protocol_string_new("checkfriends");
  g_string_sprintfa(req_str, "lastupdate=%s&", wmlj.cf.lastupdate);

  hash = request_run(req_str->str);

  g_string_free(req_str, TRUE);

  if (!request_succeeded(hash)) {
    if (DEBUG)
      fprintf(stderr, "check_friends: Request failed.\n");

    if (g_hash_table_lookup(hash, "http-error") == NULL) {
      network_error("Friendlist check failed. "
		    "Please review your login information.");
    }

    return FALSE;
  }

  wmlj.cf.lastupdate = g_strdup(g_hash_table_lookup(hash, "lastupdate"));
  wmlj.cf.new_messages = atoi(g_hash_table_lookup(hash, "new"));

  /* interval shouldn't be less than one that's required by the server */
  ok_interval = MAX(conf.interval,
		    atoi(g_hash_table_lookup(hash, "interval")));
  if (conf.interval < ok_interval)
    conf.interval = ok_interval;

  /* adjust the interval in case it has been changed */
  wmlj_cf_timeout_remove();

  /* if there are new messages, let's start the animation */
  if (wmlj.cf.new_messages > 0)
    wmlj_anim_timeout_add();
  else
    /* resume polling (possibly, with adjusted interval) */
    wmlj_cf_timeout_add();

  if (DEBUG)
    fprintf(stderr, "check_friends: new '%d'; last '%s'; interval '%s'\n",
	    wmlj.cf.new_messages, wmlj.cf.lastupdate,
	    (char*)g_hash_table_lookup(hash, "interval"));

  hash_free(hash);

  return TRUE;
}

/* perform a client login */
gboolean
lj_login() {
  GHashTable *hash;
  GString *req_str;
  gpointer fastserver;

  req_str = lj_protocol_string_new("login");
  g_string_sprintfa(req_str, "clientversion=%s&", USERAGENT);
  /* g_string_sprintfa(req_str, "getmenus=%d&", 1); */

  hash = request_run(req_str->str);

  g_string_free(req_str, TRUE);

  if (!request_succeeded(hash)) {
    if (DEBUG)
      fprintf(stderr, "lj_login: Request failed.\n");

    if (g_hash_table_lookup(hash, "http-error") == NULL) {
      network_error("Login failed. "
		    "Please review your login information.");
    }

    return FALSE;
  }

  /* check if we can use the fast server */
  fastserver = g_hash_table_lookup(hash, "fastserver");
  if (fastserver != NULL)
    conf.use_fast = (atoi(fastserver) != 0);
  else
    conf.use_fast = FALSE;

  /* add the web links */
  /* wmlj_menu_add_web(hash); */

  hash_free(hash);

  return TRUE;
}

void*
login_check_friends_thread() {
  pthread_mutex_lock(&wmlj.network_mutex);
  lj_login();
  check_friends();
  pthread_mutex_unlock(&wmlj.network_mutex);

  return FALSE;
}

void
network_exec_thread() {
  pthread_create(&wmlj.network_thread, NULL,
		 login_check_friends_thread, NULL);
}
