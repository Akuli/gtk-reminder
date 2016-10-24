ifndef PREFIX
PREFIX = /usr/local
endif

CFLAGS = -Wall $(shell pkg-config --cflags gtk+-3.0)
LIBS = $(shell pkg-config --libs gtk+-3.0)

all:
	cc $(CFLAGS) reminder.c main.c -o reminder $(LIBS)

install:
	mkdir -p "$(PREFIX)/bin"
	install reminder "$(PREFIX)/bin/reminder"

uninstall:
	rm "$(PREFIX)/bin/reminder"

clean:
	rm -f reminder
