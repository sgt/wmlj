/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001, Sergei Barbarash <sgt@outline.ru>
 *
 * $Id: spawn.h,v 1.1 2002/01/05 17:42:24 sgt Exp $
 */

#ifndef WMLJ_SPAWN_H
#define WMLJ_SPAWN_H

#include <glib.h>

extern int spawn_cmd(gchar *cmd);
extern void spawn_url(gchar *command, gchar *url);

#endif /* WMLJ_SPAWN_H */
