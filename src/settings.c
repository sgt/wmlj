/*
 * wmlj - a LiveJournal dockapp
 *
 * (c) 2001,2002 Sergei Barbarash <sgt@livejournal.com>
 *
 * $Id: settings.c,v 1.5 2002/01/06 13:45:08 sgt Exp $
 */

#include <stdlib.h>

#include "config.h"
#include "dlg.h"
#include "rc.h"
#include "settings.h"
#include "wmlj.h"

static Config newconf;

static void
ok_cb(GtkWidget *widget, GtkWidget *dialog) {
  /* adjust the update interval */

  memcpy(&conf, &newconf, sizeof(Config));

  rc_config_write(&conf);

  /* adjust the update interval */
  wmlj_cf_timeout_remove();
  wmlj_cf_timeout_add();

  gtk_widget_destroy(dialog);
}

static void
proxy_cb(GtkWidget *widget, GtkWidget *box) {
  gtk_widget_set_sensitive(box, !GTK_WIDGET_IS_SENSITIVE(box));
}

static GtkWidget*
settings_account() {

  GtkWidget *vbox;

  vbox = gtk_vbox_new(FALSE, 10); {

    GtkWidget *frame;

    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);

    frame = gtk_frame_new("LJ user"); {

      GtkWidget *hbox;

      hbox = gtk_hbox_new(FALSE, 5); {

	GtkWidget *user, *password, *label;

	gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);

	label = gtk_label_new("User:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	user = dlg_entry_new_with_text(newconf.user);
	gtk_widget_set_usize(user,
			     gdk_string_width(user->style->font,
					      "abrakadabra"), -1);
	gtk_box_pack_start(GTK_BOX(hbox), user, TRUE, TRUE, 0);

	label = gtk_label_new("Password:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	password = dlg_entry_new_with_text(newconf.password);
	/* conceal the password, show the Mysterious Asterisks! yay! */
	gtk_entry_set_visibility(GTK_ENTRY(password), FALSE);
	gtk_widget_set_usize(password,
			     gdk_string_width(password->style->font,
					      "abrakadabra"), -1);
	gtk_box_pack_start(GTK_BOX(hbox), password, TRUE, TRUE, 0);

	gtk_signal_connect(GTK_OBJECT(user), "changed",
			   GTK_SIGNAL_FUNC(dlg_entry_str_cb),
			   &newconf.user);
	gtk_signal_connect(GTK_OBJECT(password), "changed",
			   GTK_SIGNAL_FUNC(dlg_entry_str_cb),
			   &newconf.password);
      }

      gtk_container_add(GTK_CONTAINER(frame), hbox);
    }

    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

    frame = gtk_frame_new("Update interval"); {

      GtkWidget *interval_box;

      interval_box = gtk_vbox_new(FALSE, 5); {
	GtkWidget *desc_l, *hbox;

	gtk_container_set_border_width(GTK_CONTAINER(interval_box), 10);

	hbox = gtk_hbox_new(FALSE, 5); {

	  GtkWidget *pre_l, *interval_e, *post_l;

	  gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);

	  pre_l = gtk_label_new("Check for new entries every ");
	  gtk_box_pack_start(GTK_BOX(hbox), pre_l, FALSE, FALSE, 0);

	  interval_e = dlg_entry_new_with_text(g_strdup_printf("%d", newconf.interval));
	  gtk_widget_set_usize(interval_e,
			       gdk_string_width(interval_e->style->font,
						"3000"), -1);
	  gtk_box_pack_start(GTK_BOX(hbox), interval_e, FALSE, FALSE, 0);

	  post_l = gtk_label_new(" seconds.");
	  gtk_box_pack_start(GTK_BOX(hbox), post_l, FALSE, FALSE, 0);

	  gtk_signal_connect(GTK_OBJECT(interval_e), "changed",
			   GTK_SIGNAL_FUNC(dlg_entry_num_cb),
			   &newconf.interval);
	}
	gtk_box_pack_start(GTK_BOX(interval_box), hbox, FALSE, FALSE, 0);

	desc_l = gtk_label_new("Note: the server specifies its own minimum "
			       "update interval. If you enter a smaller "
			       "value, it will be automatically adjusted "
			       "to meet the server requirements");
	gtk_misc_set_alignment(GTK_MISC(desc_l), 0, 0.5);
	gtk_label_set_line_wrap(GTK_LABEL(desc_l), TRUE);
	gtk_label_set_justify(GTK_LABEL(desc_l), GTK_JUSTIFY_LEFT);
	gtk_box_pack_start(GTK_BOX(interval_box), desc_l, FALSE, FALSE, 0);
      }
      gtk_container_add(GTK_CONTAINER(frame), interval_box);
    }

    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
  }

  return vbox;
}

static GtkWidget*
settings_connection() {

  GtkWidget *vbox, *frame, *hbox;

  vbox = gtk_vbox_new(FALSE, 10); {
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);

    /* Server settings */
    frame = gtk_frame_new("LiveJournal Server"); {
      /* server connection parameters */
      hbox = dlg_address_port(&newconf.lj_server, &newconf.lj_port);

      gtk_container_add(GTK_CONTAINER(frame), hbox);
    }

    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

    frame = gtk_frame_new("HTTP Proxy"); {
      GtkWidget *proxy_box;

      proxy_box = gtk_vbox_new(FALSE, 5); {
	GtkWidget *cb;

	gtk_container_set_border_width(GTK_CONTAINER(proxy_box), 5);

	cb = gtk_check_button_new_with_label("Use proxy server");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb), newconf.use_proxy);

	hbox = dlg_address_port(&newconf.proxy_server, &newconf.proxy_port);
	gtk_widget_set_sensitive(hbox, newconf.use_proxy);

	gtk_signal_connect(GTK_OBJECT(cb), "toggled",
			   GTK_SIGNAL_FUNC(dlg_toggle_cb), &newconf.use_proxy);
	gtk_signal_connect(GTK_OBJECT(cb), "toggled",
			   GTK_SIGNAL_FUNC(proxy_cb), hbox);

	gtk_box_pack_start(GTK_BOX(proxy_box), cb, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(proxy_box), hbox, FALSE, FALSE, 0);
      }
      gtk_container_add(GTK_CONTAINER(frame), proxy_box);
    }
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
  }

  return vbox;
}

static GtkWidget*
settings_programs() {
  /* XXX should check if command has one and only %s! */
  GtkWidget *vbox;

  vbox = gtk_vbox_new(FALSE, 10); {
    GtkWidget *frame, *desc_l;

    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);

    desc_l = gtk_label_new("These commands will be evaluated by /bin/sh.");
    gtk_misc_set_alignment(GTK_MISC(desc_l), 0, 0.5);
    gtk_label_set_justify(GTK_LABEL(desc_l), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(vbox), desc_l, FALSE, FALSE, 0);

    frame = gtk_frame_new("Web browser"); {
      GtkWidget *frame_box;

      frame_box = gtk_vbox_new(FALSE, 5); {
	GtkWidget *desc_l, *cmd_l, *cmd_e;

	gtk_container_set_border_width(GTK_CONTAINER(frame_box), 5);

	desc_l = gtk_label_new("Use %s in the command line in the place of "
			       "the URL.");
	gtk_misc_set_alignment(GTK_MISC(desc_l), 0, 0.5);
	gtk_label_set_justify(GTK_LABEL(desc_l), GTK_JUSTIFY_LEFT);
	gtk_box_pack_start(GTK_BOX(frame_box), desc_l, FALSE, FALSE, 0);

	cmd_l = gtk_label_new("Command:");
	gtk_misc_set_alignment(GTK_MISC(cmd_l), 0, 0.5);
	gtk_label_set_justify(GTK_LABEL(cmd_l), GTK_JUSTIFY_LEFT);
	gtk_box_pack_start(GTK_BOX(frame_box), cmd_l, FALSE, FALSE, 0);

	cmd_e = dlg_entry_new_with_text(g_strdup(newconf.browser));
	gtk_box_pack_start(GTK_BOX(frame_box), cmd_e, FALSE, FALSE, 0);

	gtk_signal_connect(GTK_OBJECT(cmd_e), "changed",
			   GTK_SIGNAL_FUNC(dlg_entry_str_cb),
			   &newconf.browser);
      }
      gtk_container_add(GTK_CONTAINER(frame), frame_box);
    }
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

    frame = gtk_frame_new("LJ client"); {
      GtkWidget *frame_box;

      frame_box = gtk_vbox_new(FALSE, 5); {
	GtkWidget *cmd_l, *cmd_e;

	gtk_container_set_border_width(GTK_CONTAINER(frame_box), 5);

	cmd_l = gtk_label_new("Command:");
	gtk_misc_set_alignment(GTK_MISC(cmd_l), 0, 0.5);
	gtk_label_set_justify(GTK_LABEL(cmd_l), GTK_JUSTIFY_LEFT);
	gtk_box_pack_start(GTK_BOX(frame_box), cmd_l, FALSE, FALSE, 0);

	cmd_e = dlg_entry_new_with_text(g_strdup(newconf.lj_client));
	gtk_box_pack_start(GTK_BOX(frame_box), cmd_e, FALSE, FALSE, 0);

	gtk_signal_connect(GTK_OBJECT(cmd_e), "changed",
			   GTK_SIGNAL_FUNC(dlg_entry_str_cb),
			   &newconf.lj_client);
      }
      gtk_container_add(GTK_CONTAINER(frame), frame_box);
    }
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
  }

  return vbox;
}

void
cmd_settings(GtkWidget *parent, GdkEvent *event) {
  GtkWidget *dialog, *nb;

  /* make a copy of existing config */
  memcpy(&newconf, &conf, sizeof(Config));

  dialog = dlg_new(parent, PACKAGE "settings");

  gtk_signal_connect(GTK_OBJECT(dialog), "destroy",
		     GTK_SIGNAL_FUNC(gtk_main_quit), NULL);

  /* non-resizable */
  gtk_window_set_policy(GTK_WINDOW(dialog), FALSE, FALSE, TRUE);

  nb = gtk_notebook_new(); {
    gtk_notebook_append_page(GTK_NOTEBOOK(nb), 
			     settings_account(),
			     gtk_label_new("Account"));
    gtk_notebook_append_page(GTK_NOTEBOOK(nb), 
			     settings_connection(),
			     gtk_label_new("Connection"));
    gtk_notebook_append_page(GTK_NOTEBOOK(nb),
			     settings_programs(),
			     gtk_label_new("Programs"));

    dlg_set_contents(dialog, nb);
  }

  dlg_add_ok_cancel(dialog, NULL, GTK_SIGNAL_FUNC(ok_cb), dialog);
  gtk_widget_show(dialog);
  gtk_main();
}
