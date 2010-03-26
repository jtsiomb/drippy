PREFIX = /usr/local
src = $(wildcard src/*.c)
obj = $(src:.c=.o)
bin = drippy

CC = gcc
CFLAGS = -pedantic -Wall -g

$(bin): $(obj)
	$(CC) -o $@ $(obj) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) $(bin)

.PHONY: install
install:
	install -d $(PREFIX)/bin
	install -m 755 $(bin) $(PREFIX)/bin/$(bin)

.PHONY: uninstall
uninstall:
	rm -f $(PREFIX)/bin/$(bin)
