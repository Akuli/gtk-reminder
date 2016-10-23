#ifndef REMINDER_H
#define REMINDER_H

typedef struct {
	GtkWidget * window;
	GtkWidget * spinbutton;
	GtkWidget * button;
	GtkWidget * bottomlabel;
	time_t endtime;
} Reminder;

void reminder_convert_time(
	unsigned int seconds, int * minutetarget,
	int * secondtarget);
void reminder_format_time(int minutes, int seconds, char * target);
void reminder_make_gtk_button_big(GtkWidget * button);
Reminder * reminder_new(void);
gboolean reminder_on_click(GtkWidget * button, gpointer data);
void reminder_start(Reminder * reminder);
void reminder_stop(Reminder * reminder);
gboolean reminder_on_timeout(gpointer data);
gboolean reminder_on_close(
	GtkWidget * window, GdkEvent * event, gpointer data);

#endif   /* REMINDER_H */
