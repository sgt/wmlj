/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001, Sergei Barbarash <sgt@outline.ru>
 *
 * $Id: rc.h,v 1.1 2002/01/05 15:31:18 sgt Exp $
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
} Config;

/* The config file structure */
extern Config conf;
extern void rc_config_dump(Config *conf);
extern gboolean rc_config_read(Config *conf);
extern gboolean rc_config_write(Config *conf);

#endif /* WMLJ_RC_H */
