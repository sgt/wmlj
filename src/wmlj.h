/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001,2002 Sergei Barbarash <sgt@livejournal.com>
 *
 * $Id: wmlj.h,v 1.5 2002/01/10 12:08:50 sgt Exp $
 */

#ifndef WMLJ_WMLJ_H
#define WMLJ_WMLJ_H

#include <pthread.h>
#include <glib.h>
#include <gtk/gtk.h>

#include "config.h"

#define RC_FILE ".wmljrc"

#define SERVER_PATH "/interface/flat"
#define USERAGENT "Gtk-WMLJ/" VERSION

#define DEBUG 1

typedef struct {
  gchar *lastupdate;
  gint new_messages;
} CheckFriends;

typedef struct {
  GtkWidget *win;

  GtkWidget *msettings;

  guint cf_timeout_id;
  guint anim_timeout_id;

  pthread_t network_thread;
  pthread_mutex_t network_mutex;

  CheckFriends cf;
} Wmlj;

extern Wmlj wmlj;

extern void wmlj_cf_timeout_add();
extern void wmlj_cf_timeout_remove();
extern void wmlj_anim_timeout_add();
extern void wmlj_anim_timeout_remove();

#endif /* WMLJ_WMLJ_H */
