/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001,2002 Sergei Barbarash <sgt@livejournal.com>
 *
 * $Id: rc.c,v 1.9 2002/01/08 17:03:03 sgt Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#include "wmlj.h"
#include "rc.h"

Config conf; /* global var for configuration storage */

static void
rc_space_skip(FILE *file, gboolean to_newline) {
  int c;

  do {
    c = fgetc(file);
  } while (isspace(c) && c != EOF && (to_newline || c != '\n'));
  ungetc(c, file);
}

static void
rc_token_read(FILE *f, GString *buf, gboolean to_newline) {
  int c;

  rc_space_skip(f, FALSE);

  /* get the next token */
  do {
    c = fgetc(f);
    buf = g_string_append_c(buf, c);
  } while (c != EOF && ((to_newline && c != '\n') || !isspace(c)));

  ungetc(c, f);
  buf = g_string_truncate(buf, (buf->len - 1));

  rc_space_skip(f, to_newline);
}

static gchar*
rc_path() {
  return g_strdup_printf("%s/%s", getenv("HOME"), RC_FILE);
}

static void
rc_option_write(FILE *f, RCOption *option) {
  fprintf(f, "%s %s\n", option->key, option->value);
}

static void
rc_option_read(FILE *f, RCOption *option) {
  GString *buf;

  buf = g_string_sized_new(128);

  rc_token_read(f, buf, FALSE);
  option->key = g_strdup(buf->str);
  buf = g_string_truncate(buf, 0);
  rc_token_read(f, buf, TRUE);
  option->value = g_strdup(buf->str);

  if (DEBUG) {
    fprintf(stderr, "rc_option_read: key '%s'; value '%s'\n", option->key,
	    option->value);
  }

  g_string_free(buf, TRUE);
}

static void
rc_config_default(Config *conf) {
  conf->user = g_strdup("ljuser");
  conf->password = g_strdup("ljpass");
  conf->use_fast = FALSE;

  conf->lj_server = g_strdup("www.livejournal.com");
  conf->lj_port = 80;

  conf->use_proxy = FALSE;
  conf->proxy_server = g_strdup("");
  conf->proxy_port = 3128;

  conf->interval = 300;

  conf->browser = g_strdup("netscape-remote -remote %s");
  conf->lj_client = g_strdup("logjam&");
}

void
rc_config_dump(Config *conf) {
  fprintf(stderr, "=== Config dump ===\n");
  fprintf(stderr, "User: '%s'\n", conf->user);
  fprintf(stderr, "Password: '%s'\n", conf->password);
  fprintf(stderr, "Use fast: '%d'\n", conf->use_fast);
  fprintf(stderr, "LJ server: '%s'\n", conf->lj_server);
  fprintf(stderr, "LJ port: '%d'\n", conf->lj_port);
  fprintf(stderr, "Use proxy: '%d'\n", conf->use_proxy);
  fprintf(stderr, "Proxy server: '%s'\n", conf->proxy_server);
  fprintf(stderr, "Proxy port: '%d'\n", conf->proxy_port);
  fprintf(stderr, "Interval: '%d'\n", conf->interval);
  fprintf(stderr, "Browser: '%s'\n", conf->browser);
  fprintf(stderr, "LJ client: '%s'\n", conf->lj_client);
  fprintf(stderr, "\n");
}

gboolean
rc_exists() {
  FILE *f;

  if ((f = fopen(rc_path(), "r")) == NULL) {
    return FALSE;
  }
  else {
    fclose(f);
    return TRUE;
  }
}

gboolean
rc_config_read(Config *conf) {
  FILE *f;
  RCOption option;

  /* first, fill the default values */
  rc_config_default(conf);

  if ((f = fopen(rc_path(), "r")) == NULL) {
    fprintf(stderr, "Can't open '%s' -- will create.\n", rc_path());
    rc_config_write(conf);
    if ((f = fopen(rc_path(), "r")) == NULL) {
      perror("rc_config_read: reading rc file");
      return FALSE;
    }
  }

  while(!feof(f)) {
    rc_option_read(f, &option);

    if (option.key == "")
      break;

    if (g_strcasecmp(option.key, "user") == 0) {
      conf->user = g_strdup(option.value);
    }
    else if (g_strcasecmp(option.key, "password") == 0) {
      conf->password = g_strdup(option.value);
    }
    else if (g_strcasecmp(option.key, "usefast") == 0) {
      conf->use_fast = atoi(option.value);
    }
    else if (g_strcasecmp(option.key, "ljserver") == 0) {
      conf->lj_server = g_strdup(option.value);
    }
    else if (g_strcasecmp(option.key, "ljport") == 0) {
      conf->lj_port = atoi(option.value);
    }
    else if (g_strcasecmp(option.key, "useproxy") == 0) {
      conf->use_proxy = atoi(option.value);
    }
    else if (g_strcasecmp(option.key, "proxyserver") == 0) {
      conf->proxy_server = g_strdup(option.value);
    }
    else if (g_strcasecmp(option.key, "proxyport") == 0) {
      conf->proxy_port = atoi(option.value);
    }
    else if (g_strcasecmp(option.key, "interval") == 0) {
      conf->interval = atoi(option.value);
    }
    else if (g_strcasecmp(option.key, "browser") == 0) {
      conf->browser = g_strdup(option.value);
    }
    else if (g_strcasecmp(option.key, "ljclient") == 0) {
      conf->lj_client = g_strdup(option.value);
    }

    /* otherwise, ignore the line */
  }

  fclose(f);

  return TRUE;
}

gboolean
rc_config_write(Config *conf) {
  FILE *f;
  RCOption option;

  if ((f = fopen(rc_path(), "w")) == NULL) {
    fprintf(stderr, "Can't open '%s' for writing", rc_path());
    perror("writing rcfile");
    return FALSE;
  }

  option.key = "User";
  option.value = g_strdup(conf->user);
  rc_option_write(f, &option);

  option.key = "Password";
  option.value = g_strdup(conf->password);
  rc_option_write(f, &option);

  option.key = "UseFast";
  option.value = g_strdup_printf("%d", conf->use_fast);
  rc_option_write(f, &option);

  option.key = "LJServer";
  option.value = g_strdup(conf->lj_server);
  rc_option_write(f, &option);

  option.key = "LJPort";
  option.value = g_strdup_printf("%d", conf->lj_port);
  rc_option_write(f, &option);

  option.key = "UseProxy";
  option.value = g_strdup_printf("%d",conf->use_proxy);
  rc_option_write(f, &option);

  option.key = "ProxyServer";
  option.value = g_strdup(conf->proxy_server);
  rc_option_write(f, &option);

  option.key = "ProxyPort";
  option.value = g_strdup_printf("%d", conf->proxy_port);
  rc_option_write(f, &option);

  option.key = "Interval";
  option.value = g_strdup_printf("%d", conf->interval);
  rc_option_write(f, &option);

  option.key = "Browser";
  option.value = g_strdup(conf->browser);
  rc_option_write(f, &option);

  option.key = "LJClient";
  option.value = g_strdup(conf->lj_client);
  rc_option_write(f, &option);

  fclose(f);

  /* make the file mode 0600, for security reasons */
  chmod(rc_path(), S_IRUSR|S_IWUSR);

  return TRUE;
}
