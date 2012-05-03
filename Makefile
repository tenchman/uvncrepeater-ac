PACKAGE = uvncrepeater-ac
VERSION = 1.0.0
DESTDIR =
prefix  = /usr/local

CC      = gcc
LD      = gcc
CFLAGS  = -W -Wall # -ansi -D_XOPEN_SOURCE=500
LDFLAGS =
OPT     = -O2
OBJS    = repeater.o repeaterproc.o openbsd_stringfuncs.o iniparser.o readini.o repeaterevents.o in46_addr.o

ifeq ($(V), 1)
THECC = $(CC)
THELD = $(LD)
VERBOSE =
else
THECC = @echo " CC   $@"; $(CC)
THELD = @echo " LINK $@"; $(LD)
VERBOSE = @
endif

all: repeater

debug: CFLAGS += -DDEBUG -ggdb
debug: OPT = -O0
debug: all

release: CFLAGS += -DNDEBUG
release: all

CFLAGS += $(OPT) -DREPEATER_VERSION=\"$(VERSION)\"

repeater: $(OBJS)
	$(THELD) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(THECC) $(CFLAGS) -c $< -o $@

install: all
	$(VERBOSE)mkdir -p $(DESTDIR)$(prefix)/bin
	$(VERBOSE)mkdir -p $(DESTDIR)/etc
	$(VERBOSE)cp -f repeater $(DESTDIR)$(prefix)/bin/$(PACKAGE)
	$(VERBOSE)cp -f uvncrepeater.ini $(DESTDIR)/etc/$(PACKAGE).ini

clean:
	$(VERBOSE)rm -f *.o repeater $(PACKAGE)-$(VERSION).tar.bz2

tarball:
	tar --files-from=files.lst \
	  --transform='s,^,$(PACKAGE)-$(VERSION)/,S' \
	  -cjf $(PACKAGE)-$(VERSION).tar.bz2
