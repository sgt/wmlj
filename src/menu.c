/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001,2002 Sergei Barbarash <sgt@livejournal.com>
 *
 * $Id: menu.c,v 1.3 2002/01/10 12:08:50 sgt Exp $
 */

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "menu.h"
#include "settings.h"
#include "about.h"
#include "wmlj.h"

static void
cmd_quit(GtkWidget *widget, GdkEvent *event) {
  gtk_main_quit();
}

/*
 * Creating popup menu
 */
GtkWidget*
wmlj_menu_create() {
  GtkWidget *wmlj_menu;

  wmlj_menu = gtk_menu_new(); {
    GtkItemFactory *item_factory;

    GtkItemFactoryEntry menu_items[] = {
      { "/Settings", NULL, cmd_settings, 0, NULL},
      { "/About", NULL, cmd_about, 0, NULL},
      { "/sep1", NULL, NULL, 0, "<Separator>" },
      { "/Quit", NULL, cmd_quit, 0, NULL }
    };

    gint nmenu_items = sizeof (menu_items) / sizeof (menu_items[0]);

    item_factory = gtk_item_factory_new(GTK_TYPE_MENU, "<main>", NULL);
    gtk_item_factory_create_items(item_factory, nmenu_items, menu_items,
				  NULL);

    wmlj_menu = gtk_item_factory_get_widget (item_factory, "<main>");

    wmlj.msettings = gtk_item_factory_get_widget(item_factory, "/Settings");

    gtk_widget_show(wmlj_menu);
  }

  return wmlj_menu;
}
