/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001,2002 Sergei Barbarash <sgt@livejournal.com>
 *
 * $Id: network.h,v 1.7 2002/02/06 13:46:03 sgt Exp $
 */

#ifndef WMLJ_NETWORK_H
#define WMLJ_NETWORK_H

#include <glib.h>
#include "wmlj.h"

extern gboolean check_friends();
extern gboolean lj_login();
extern void* login_check_friends_thread();
extern void network_exec_thread();

#endif /* WMLJ_NETWORK_H */
