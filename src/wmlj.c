/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001, Sergei Barbarash <sgt@outline.ru>
 *
 * $Id: wmlj.c,v 1.2 2002/01/05 17:42:24 sgt Exp $
 */

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "foodock.h"
#include "rc.h"

#include "../pixmaps/logo1.xpm"
#include "../pixmaps/logo2.xpm"

#include "menu.h"
#include "network.h"
#include "spawn.h"
#include "wmlj.h"
#include "config.h"

CheckFriends cf = { 0 }; /* global */

static gint pixmap_state = 0;
static GtkWidget *logo; /* the animated pixmap widget */

/* global timeout id's */
guint cf_timeout_id;
guint anim_timeout_id;

static GdkPixmap *datapix[2] = { 0 };
static GdkBitmap *datamask[2] = { 0 };

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
      if (DEBUG) {
	rc_config_dump(&conf);
	return FALSE;
      }
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
      spawn_url(conf.browser,
		g_strdup_printf("http://%s:%d/users/%s/friends/",
				conf.lj_server,
				conf.lj_port,
				conf.user));

      /* reset the monitor */
      cf.lastupdate = g_strdup("");
      wmlj_anim_timeout_remove();

      return FALSE;
    default:
      return FALSE;
    }
  default: return FALSE;
  }
}

static gboolean
wmlj_animate_pixmap(GtkWidget* logo) {
  gdk_threads_enter();

  pixmap_state++;
  if (pixmap_state >= 2) pixmap_state = 0;

  gtk_pixmap_set(GTK_PIXMAP(logo),
		 datapix[pixmap_state], datamask[pixmap_state]);

  gdk_threads_leave();
  return TRUE;
}

void wmlj_cf_timeout_add() {
  if (cf_timeout_id)
    return;

  cf_timeout_id = gtk_timeout_add(conf.interval*1000,
				  (GtkFunction)check_friends,
				  NULL);
}

void wmlj_cf_timeout_remove() {
  if (cf_timeout_id) {
    gtk_timeout_remove(cf_timeout_id);
    cf_timeout_id = 0;
  }
}

void wmlj_anim_timeout_add() {
  if (anim_timeout_id)
    return;

  anim_timeout_id = gtk_timeout_add(200, (GtkFunction)wmlj_animate_pixmap,
				    logo);
}

void wmlj_anim_timeout_remove() {
  if (anim_timeout_id) {
    gtk_timeout_remove(anim_timeout_id);
    anim_timeout_id = 0;
  }
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

    datapix[0] = gdk_pixmap_create_from_xpm_d(wmlj_main->window, &datamask[0],
					      NULL, logo1_xpm);
    datapix[1] = gdk_pixmap_create_from_xpm_d(wmlj_main->window, &datamask[1],
					      NULL, logo2_xpm);

    logo = gtk_pixmap_new(datapix[0], datamask[0]);

    gtk_container_add(GTK_CONTAINER(box), logo);

    gtk_widget_show_all(box);

    menu = wmlj_menu_create();

    gtk_signal_connect(GTK_OBJECT(wmlj_main), "button_press_event",
		       GTK_SIGNAL_FUNC(wmlj_button_press),
		       GTK_OBJECT(menu));
  }

  return wmlj_main;
}

int main( int argc, char *argv[] ) {

  GtkWidget *wmlj_main;

  g_thread_init(NULL);

  anim_timeout_id = cf_timeout_id = 0;

  /* read the configuration */
  rc_config_read(&conf);

  gtk_init(&argc, &argv);

  wmlj_main = wmlj_main_create(argc, argv);

  gtk_widget_show(wmlj_main);

  gdk_threads_enter();

  /* Timed events */
  wmlj_cf_timeout_add();

  gtk_main();
  gdk_threads_leave();

  /* save the configuration */
  rc_config_write(&conf);

  return 0;
}
