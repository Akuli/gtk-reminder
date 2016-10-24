#ifndef REMINDER_H
#define REMINDER_H

struct Reminder {
	GtkWidget * window;
	GtkWidget * spinbutton;
	GtkWidget * button;
	GtkWidget * bottomlabel;
	time_t endtime;
};

void reminder_convert_time(int seconds, int * minutetarget, int * secondtarget);
void reminder_format_time(int minutes, int seconds, char * target);
void reminder_make_gtk_button_big(GtkWidget * button);
struct Reminder * reminder_new(void);
gboolean reminder_on_click(GtkWidget * button, gpointer data);
void reminder_start(struct Reminder * reminder);
void reminder_stop(struct Reminder * reminder);
gboolean reminder_on_timeout(gpointer data);
gboolean reminder_on_close(GtkWidget * window, GdkEvent * event, gpointer data);

#endif   /* REMINDER_H */
