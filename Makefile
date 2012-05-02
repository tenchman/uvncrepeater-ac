DESTDIR =
PREFIX  = /usr/local

THECC   = gcc
THELD   = gcc
CFLAGS  = -W -Wall # -ansi -D_XOPEN_SOURCE=500
LDFLAGS =
OPT     = -O2
OBJS    = repeater.o repeaterproc.o openbsd_stringfuncs.o iniparser.o readini.o repeaterevents.o in46_addr.o

ifeq ($(V), 1)
CC = $(THECC)
LD = $(THELD)
VERBOSE =
else
CC = @echo " CC   $@"; $(THECC)
LD = @echo " LINK $@"; $(THELD)
VERBOSE = @
endif

all: repeater

debug: CFLAGS += -DDEBUG -ggdb
debug: OPT = -O0
debug: all

release: CFLAGS += -DNDEBUG
release: all

CFLAGS += $(OPT)

repeater: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install: all
	$(VERBOSE)mkdir -p $(DESTDIR)$(PREFIX)/bin
	$(VERBOSE)mkdir -p $(DESTDIR)/etc
	$(VERBOSE)cp -f repeater $(DESTDIR)$(PREFIX)/bin/$(PACKAGE)
	$(VERBOSE)cp -f uvncrepeater.ini $(DESTDIR)/etc/$(PACKAGE).ini

clean:
	$(VERBOSE)rm -f *.o repeater
