/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001,2002 Sergei Barbarash <sgt@livejournal.com>
 *
 * $Id: network.h,v 1.6 2002/01/10 12:08:50 sgt Exp $
 */

#ifndef WMLJ_NETWORK_H
#define WMLJ_NETWORK_H

#include <glib.h>
#include "wmlj.h"

extern gboolean check_friends();
extern gboolean lj_login();
extern void* login_check_friends_thread();

#endif /* WMLJ_NETWORK_H */
