/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001,2002 Sergei Barbarash <sgt@livejournal.com>
 *
 * $Id: rc.h,v 1.5 2002/01/06 13:45:08 sgt Exp $
 */

#ifndef WMLJ_RC_H
#define WMLJ_RC_H

#include <glib.h>

typedef struct {
  gchar *key;
  gchar *value;
} RCOption;

typedef struct {
  gchar *user;
  gchar *password;

  gchar *lj_server;
  gint lj_port;

  gboolean use_proxy;
  gchar *proxy_server;
  gint proxy_port;

  /* server check interval */
  gint interval;

  /* programs to spawn */
  gchar *browser;
  gchar *lj_client;
} Config;

/* The config file structure */
extern Config conf;
extern void rc_config_dump(Config *conf);
extern gboolean rc_exists();
extern gboolean rc_config_read(Config *conf);
extern gboolean rc_config_write(Config *conf);

#endif /* WMLJ_RC_H */
