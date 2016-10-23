#include <stdlib.h>
#include <gtk/gtk.h>
#include "reminder.h"


int main(int argc, char ** argv) {
	Reminder * reminder;

	gtk_init(&argc, &argv);
	reminder = reminder_new();
	gtk_main();
	free(reminder);
	return 0;
}
