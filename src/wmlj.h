/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001, Sergei Barbarash <sgt@outline.ru>
 *
 * $Id: wmlj.h,v 1.1 2002/01/05 15:31:18 sgt Exp $
 */

#ifndef WMLJ_WMLJ_H
#define WMLJ_WMLJ_H

#include <glib.h>

#include "config.h"

#define RC_FILE ".wmljrc"

#define SERVER_PATH "/interface/flat"
#define USERAGENT PACKAGE "-" VERSION

#define DEBUG 1

typedef struct {
  gchar *lastupdate;
  gint new_messages;
} CheckFriends;

extern CheckFriends cf;

extern guint cf_timeout_id;
extern guint anim_timeout_id;

extern void wmlj_cf_timeout_add();
extern void wmlj_cf_timeout_remove();
extern void wmlj_anim_timeout_add();
extern void wmlj_anim_timeout_remove();

#endif /* WMLJ_WMLJ_H */
