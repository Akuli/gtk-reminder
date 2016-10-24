#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "reminder.h"


void reminder_convert_time(int seconds, int * minutetarget, int * secondtarget) {
	* minutetarget = seconds / 60;
	* secondtarget = seconds % 60;
}


void reminder_format_time(int minutes, int seconds, char * target) {
	char minuteformat[50];
	char secondformat[50];

	sprintf(minuteformat, minutes == 1 ? "%d minute" : "%d minutes", minutes);
	sprintf(secondformat, seconds == 1 ? "%d second" : "%d seconds", seconds);

	if (minutes != 0 && seconds != 0) {
		sprintf(target, "%s and %s", minuteformat, secondformat);
	} else if (minutes != 0 && seconds == 0) {
		strcpy(target, minuteformat);
	} else {
		strcpy(target, secondformat);
	}
}


void reminder_make_gtk_button_big(GtkWidget * button) {
	/* I have no idea why this needs to be so difficult. */
	GtkCssProvider * provider;
	GtkStyleContext * context;

	provider = gtk_css_provider_new();
	gtk_css_provider_load_from_data(
		provider, "* { font-size: 2em; }", -1, NULL);
	context = gtk_widget_get_style_context(button);
	gtk_style_context_add_provider(
		context, GTK_STYLE_PROVIDER(provider),
		GTK_STYLE_PROVIDER_PRIORITY_USER);
}


struct Reminder * reminder_new(void) {
	struct Reminder * reminder = (struct Reminder *) malloc(sizeof(struct Reminder));
	GtkWidget * bigbox;
	GtkWidget * topbox;
	GtkWidget * middlebox;
	GtkWidget * toplabel;

	reminder->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(reminder->window), 5);

	/* The big box. */
	bigbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(reminder->window), bigbox);

	/* Top box. */
	topbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
	gtk_box_pack_start(GTK_BOX(bigbox), topbox, FALSE, FALSE, 0);

	toplabel = gtk_label_new("Waiting time in minutes:");
	gtk_box_pack_start(GTK_BOX(topbox), toplabel, FALSE, FALSE, 0);

	reminder->spinbutton = gtk_spin_button_new_with_range(1, 999, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(reminder->spinbutton), 60);
	gtk_box_pack_start(
		GTK_BOX(topbox), reminder->spinbutton, TRUE, TRUE, 0);

	/* Button in the middle. */
	middlebox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_button_box_set_layout(
		GTK_BUTTON_BOX(middlebox), GTK_BUTTONBOX_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(middlebox), 50);
	gtk_box_pack_start(GTK_BOX(bigbox), middlebox, TRUE, TRUE, 0);

	reminder->button = gtk_button_new_with_label("Start");
	g_signal_connect(reminder->button, "clicked",
				G_CALLBACK(reminder_on_click), reminder);
	gtk_box_pack_start(GTK_BOX(middlebox), reminder->button, TRUE, TRUE, 0);
	reminder_make_gtk_button_big(reminder->button);

	/* Bottom label. */
	reminder->bottomlabel = gtk_label_new("");
	gtk_box_pack_start(
		GTK_BOX(bigbox), reminder->bottomlabel, FALSE, FALSE, 0);

	/* Other things. */
	reminder_stop(reminder);  /* Set up more things. */

	gtk_window_set_title(GTK_WINDOW(reminder->window), "Reminding program");
	g_signal_connect(reminder->window, "delete-event",
				G_CALLBACK(reminder_on_close), reminder);
	gtk_widget_show_all(reminder->window);
	return reminder;
}


gboolean reminder_on_click(GtkWidget * button, gpointer data) {
	struct Reminder * reminder = (struct Reminder *) data;
	if (reminder->endtime < 0) {
		reminder_start(reminder);
	} else {
		reminder_stop(reminder);
	}
	return FALSE;
}


void reminder_start(struct Reminder * reminder) {
	int minutes;

	gtk_button_set_label(GTK_BUTTON(reminder->button), "Stop");
	minutes = gtk_spin_button_get_value_as_int(
		GTK_SPIN_BUTTON(reminder->spinbutton));
	reminder->endtime = minutes * 60 + time(NULL);
	g_timeout_add(1000, reminder_on_timeout, reminder);
	reminder_on_timeout(reminder);    /* Run it now. */
}


void reminder_stop(struct Reminder * reminder) {
	reminder->endtime = -1;
	gtk_window_set_title(GTK_WINDOW(reminder->window), "Reminding program");
	gtk_button_set_label(GTK_BUTTON(reminder->button), "Start");
	gtk_label_set_text(GTK_LABEL(reminder->bottomlabel), "");
}


gboolean reminder_on_timeout(gpointer data) {
	struct Reminder * reminder = (struct Reminder *) data;
	time_t time_left;
	int minutes;
	int seconds;
	char formatted_time[100];
	char label[100];
	char title[100];
	GtkWidget * dialog;

	if (reminder->endtime < 0) {
		/* The reminding is not running. */
		return FALSE;
	}

	time_left = reminder->endtime - time(NULL);
	if (time_left > 0) {
		/* Keep going. */
		reminder_convert_time(time_left, &minutes, &seconds);
		reminder_format_time(minutes, seconds, formatted_time);
		sprintf(label, "Reminding after %s.", formatted_time);
		sprintf(title, "[%d:%d] Reminding program", minutes, seconds);
		gtk_window_set_title(GTK_WINDOW(reminder->window), title);
		gtk_label_set_text(GTK_LABEL(reminder->bottomlabel), label);
		return TRUE;  /* Keep running this. */
	} else {
		/* Stop, show a message and restart. */
		reminder_stop(reminder);
		dialog = gtk_message_dialog_new(
			GTK_WINDOW(reminder->window), GTK_DIALOG_MODAL,
			GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "The time ended.");
		gtk_window_set_title(GTK_WINDOW(dialog), "Time's up!");
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		reminder_start(reminder);
		return FALSE;  /* reminder_start() scheduled this to run again. */
	}
}


gboolean reminder_on_close(GtkWidget * window, GdkEvent * event, gpointer data) {
	struct Reminder * reminder = (struct Reminder *) data;
	GtkWidget * dialog;
	int response;

	if (reminder->endtime >= 0) {
		/* A reminding is running. */
		dialog = gtk_message_dialog_new(
			GTK_WINDOW(reminder->window), GTK_DIALOG_MODAL,
			GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
			"Do you want to stop the reminding?");
		gtk_window_set_title(GTK_WINDOW(dialog), "Reminding program");
		response = gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		if (response != GTK_RESPONSE_YES) {
			return TRUE;  /* Don't destroy the window. */
		}
	}
	gtk_main_quit();
	return FALSE;
}
