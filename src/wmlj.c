/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001,2002 Sergei Barbarash <sgt@livejournal.com>
 *
 * $Id: wmlj.c,v 1.15 2002/02/06 13:46:03 sgt Exp $
 */

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "foodock.h"
#include "rc.h"

#include "../pixmaps/logo.xpm"
#include "../pixmaps/logo_bw.xpm"
#include "../pixmaps/logo1.xpm"
#include "../pixmaps/logo2.xpm"

#include "menu.h"
#include "network.h"
#include "settings.h"
#include "spawn.h"
#include "wmlj.h"
#include "config.h"

static gint pixmap_state = 0;

Wmlj wmlj;

void
wmlj_pixmap_set(IconType icon) {
  gtk_pixmap_set((GtkPixmap*)wmlj.logo,
		 wmlj.datapix[icon],
		 wmlj.datamask[icon]);
}

static gint
wmlj_menu_popup(GtkWidget *widget, GdkEvent *event, GtkMenu *menu) {
  GdkEventButton *event_button;

  event_button = (GdkEventButton *) event;

  gtk_menu_popup (menu, NULL, NULL, NULL, NULL,
		  event_button->button, event_button->time);
  return FALSE;
}

static gint
wmlj_button_press(GtkWidget *widget, GdkEvent *event,
		       gpointer callback_data) {
  switch (event->type) {
  case GDK_BUTTON_PRESS:
    switch (event->button.button) {
    case 2:
      /* middle button click */
      if (DEBUG)
	rc_config_dump(&conf);
      return FALSE;
    case 3:
      /* right button click */
      return wmlj_menu_popup(widget, event, callback_data);
    default:
      return FALSE;
    }
  case GDK_2BUTTON_PRESS:
    switch (event->button.button) {
    case 1:
      /* left button double-click */
      if (wmlj.anim_timeout_id) {
	/* animation is active, run browser */
	spawn_url(conf.browser,
		  g_strdup_printf("http://%s:%d/users/%s/friends/",
				  conf.lj_server,
				  conf.lj_port,
				  conf.user));

	/* reset the monitor */
	wmlj.cf.lastupdate = g_strdup("");
	wmlj_anim_timeout_remove(); /* stop the animation */
	wmlj_cf_timeout_add();      /* resume polling */

	return FALSE;
      }
      else {
	/* animation is inactive, run LJ client */
	spawn_app(conf.lj_client);
      }
    default:
      return FALSE;
    }
  default: return FALSE;
  }
}

static gboolean
wmlj_animate_pixmap(GtkWidget* logo) {
  pixmap_state++;
  if (pixmap_state >= LOGO_ANIM_START+2)
    pixmap_state = LOGO_ANIM_START;

  wmlj_pixmap_set(pixmap_state);

  return TRUE;
}

void
wmlj_cf_timeout_add() {
  if (wmlj.cf_timeout_id)
    return;

  wmlj.cf_timeout_id = gtk_timeout_add(conf.interval*1000,
				       (GtkFunction)check_friends,
				       NULL);
}

void
wmlj_cf_timeout_remove() {
  if (wmlj.cf_timeout_id) {
    gtk_timeout_remove(wmlj.cf_timeout_id);
    wmlj.cf_timeout_id = 0;
  }
}

void
wmlj_anim_timeout_add() {
  if (wmlj.anim_timeout_id)
    return;

  wmlj.anim_timeout_id = gtk_timeout_add(200, (GtkFunction)wmlj_animate_pixmap,
					 wmlj.logo);
}

void
wmlj_anim_timeout_remove() {
  if (wmlj.anim_timeout_id) {
    gtk_timeout_remove(wmlj.anim_timeout_id);
    wmlj.anim_timeout_id = 0;
  }
}

static void
wmlj_pixmaps_create(GdkWindow *win) {
  wmlj.datapix[LOGO_MAIN] = \
    gdk_pixmap_create_from_xpm_d(win, &wmlj.datamask[LOGO_MAIN],
				 NULL, logo_xpm);
  wmlj.datapix[LOGO_BW] = \
    gdk_pixmap_create_from_xpm_d(win, &wmlj.datamask[LOGO_BW],
				 NULL, logo_bw_xpm);
  wmlj.datapix[LOGO_ANIM_START+0] = \
    gdk_pixmap_create_from_xpm_d(win, &wmlj.datamask[LOGO_ANIM_START+0],
				 NULL, logo1_xpm);
  wmlj.datapix[LOGO_ANIM_START+1] = \
    gdk_pixmap_create_from_xpm_d(win, &wmlj.datamask[LOGO_ANIM_START+1],
				 NULL, logo2_xpm);
}

/*
 * Creating the main window with logo pixmap
 */
static GtkWidget*
wmlj_main_create(int argc, char *argv[]) {
  GtkWidget *wmlj_main, *box;

  wmlj_main = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_realize(wmlj_main);

  box = foo_create_main_icon_window(wmlj_main, 56, argc, argv); {
    GtkWidget *menu;

    wmlj_pixmaps_create(wmlj_main->window);

    wmlj.logo = gtk_pixmap_new(wmlj.datapix[LOGO_MAIN],
			       wmlj.datamask[LOGO_MAIN]);

    gtk_container_add(GTK_CONTAINER(box), wmlj.logo);

    gtk_widget_show_all(box);

    menu = wmlj_menu_create();

    gtk_signal_connect(GTK_OBJECT(wmlj_main), "button_press_event",
		       GTK_SIGNAL_FUNC(wmlj_button_press),
		       GTK_OBJECT(menu));
  }

  return wmlj_main;
}

int
main( int argc, char *argv[] ) {

  g_thread_init(NULL);

  pthread_mutex_init(&wmlj.network_mutex, NULL);
  pthread_mutex_init(&wmlj.network_error_mutex, NULL);
  wmlj.anim_timeout_id = 0;
  wmlj.cf_timeout_id = 0;
  wmlj.pixmap_state = 0;

  gtk_init(&argc, &argv);

  wmlj.win = wmlj_main_create(argc, argv);

  gtk_widget_show(wmlj.win);

  /* if first time run, show settings dialog */
  if (!rc_exists()) {
    /* read the configuration */
    rc_config_read(&conf);
    /* show settings dialog */
    cmd_settings(wmlj.win, NULL);
  }
  else {
    /* read the configuration */
    rc_config_read(&conf);
    /* perform a client login */
    network_exec_thread();
  }

  gdk_threads_enter();
  gtk_main();
  gdk_threads_leave();

  /* save the configuration */
  rc_config_write(&conf);

  return 0;
}
