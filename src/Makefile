# Makefile for MQTT-Hyperdash (c) Markus Hoffmann V.1.27

# This file is part of MQTT-Hyperdash, the universal MQTT Dashbord
# ======================================================================
# MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
# COPYING for details

# Insert the defs for your machine

SHELL=/bin/sh

# Directories
prefix=/usr
exec_prefix=${prefix}
datarootdir = ${prefix}/share

BINDIR=${exec_prefix}/bin
DATADIR=${datarootdir}
ICONDIR=$(DATADIR)/icons/hicolor/scalable/apps
DESKTOPDIR=$(DATADIR)/applications
MANDIR=${datarootdir}/man

LIBNO=1.27

RELEASE=1

# Register variables (-ffixed-reg) -Wall
REGS=-fno-omit-frame-pointer  

# Optimization and debugging options
OPT=-O3

# Additional header file paths
INC= 
WININC=-I./WINDOWS -I./WINDOWS/include -I./WINDOWS/include/GMP

# defines for special flavours

DEF= 

# Compiler
#CC=gcc  $(REGS)
CC=gcc -Wall -g

# Cross-Compiler fuer Windows-Excecutable

WINCC=i686-w64-mingw32-gcc

# Preprocessor
CPP=gcc -E

CFLAGS= -g -O2 $(INC) $(DEF) $(OPT) $(REGS)

# these are objects which are compiled to binary

MAINOBJS= main.o hyperdash.o mqtt.o lodepng.o file.o util.o

CSRC=    $(MAINOBJS:.o=.c)

DEPSRC=  $(CSRC) 

# Headerfiles which should be added to the distribution

HSRC=  hyperdash.h lodepng.h mqtt.h file.h util.h

# example dash files 

EXAMPLES= 

DIST= ../README ../doc/INSTALL ../COPYING ../doc/man-pages/*.1 \
	../doc/ACKNOWLEGEMENTS \
	../doc/AUTHORS ../doc/HISTORY ../RELEASE_NOTES \
      $(HSRC) $(CSRC) WINDOWS/MQTT-Hyperdash.iss \
      config.h.in  \
        Makefile.in configure config.guess configure.in \
        MQTT-Hyperdash.spec hyperdash.desktop  \
        ../bitmaps/* $(EXAMPLES) 


WINDIST=WINDOWS/lib/SDL.dll \
        WINDOWS/lib/README-SDL.txt \
	hyperdash.exe  \
        WINDOWS/readme-windows.txt \
	WINDOWS/hyperdash.ico  WINDOWS/demo.dash \
	WINDOWS/dash.ico ../COPYING ../RELEASE_NOTES INTRO

DIST2=$(DIST)

DIR=MQTT-Hyperdash-$(LIBNO)
DIR2=MQTT-Hyperdash-src-$(LIBNO)
DIR3=MQTT-Hyperdash-bin-$(LIBNO)
DIR33=$(DIR)-bin

LINKFLAGS = 
WINLINKFLAGS = -L./WINDOWS/lib 

LIBS =  -lm  -lasound -lpaho-mqtt3c 
WINLIBS = -lm  -lgdi32 -lkernel32 -luser32 -lole32 -luuid -lwsock32  -lSDL

all :    hyperdash

configure : configure.in
	autoconf
#Makefile : Makefile.in
#	./configure --prefix=/usr

# config.h: config.h.in
	./configure --prefix=/usr
 
# Make the excecutable

hyperdash :  $(MAINOBJS)
	$(CC) -L. $(LINKFLAGS) -o $@ $(MAINOBJS) $(LIBS)


# Make the exe for MS WINDOWS

hyperdash.exe: main.c $(MYWINLIBS)
	$(WINCC) -DWINDOWS $(OPT) $(WININC) $(WINLINKFLAGS) main.c -o $@ $(MYWINLIBS) \
	 $(WINLIBS)
	strip $@

install : hyperdash hyperdash.desktop hyperdash.svg
	install -m 644 hyperdash.svg $(ICONDIR)/
	install -m 644 hyperdash.desktop $(DESKTOPDIR)/
	install -s -m 755 hyperdash $(BINDIR)/
	install -d $(MANDIR)
	install -d $(MANDIR)/man1
	install -m 644 doc/man-pages/hyperdash.1 $(MANDIR)/man1/
uninstall :
	rm -f $(ICONDIR)/hyperdash.svg
	rm -f $(DESKTOPDIR)/hyperdash.desktop
	rm -f $(BINDIR)/hyperdash
	rm -f $(MANDIR)/man1/hyperdash.1
clean :
	rm -f *.o a.out Makefile.bak  \
	backup-*.tgz *.d
	rm -rf doc-pak
moreclean : clean
	rm -f hyperdash

distclean : moreclean
	rm -f Makefilex config.cache config.status config.log config.h \
	MQTT-Hyperdash-examples-$(LIBNO).zip \
	hyperdash.exe  \
	rm -rf autom4te.cache

$(DIR).tar.gz : dist
dist :	$(DIST2)
	rm -rf /tmp/$(DIR)
	mkdir /tmp/$(DIR)
	(tar cf - $(DIST))|(cd /tmp/$(DIR); tar xpf -)
	(cd /tmp; tar cf - $(DIR)|gzip -9 > $(DIR).tar.gz)
	mv /tmp/$(DIR).tar.gz .
bindist : $(DIR3).zip
rpm :	$(DIR).tar.gz hyperdash.spec
	cp $(DIR).tar.gz $(HOME)/rpmbuild/SOURCES/
	rpmbuild -ba  --clean --nodeps hyperdash.spec

DEBDOC=	../doc/ACKNOWLEGEMENTS ../doc/AUTHORS ../doc/HISTORY Debian/changelog.Debian Debian/copyright \
	../RELEASE_NOTES ../README 

doc-pak: $(DEBDOC)
	mkdir -p $@
	cp ../doc/ACKNOWLEGEMENTS ../doc/HISTORY $@/
	cp ../doc/AUTHORS ../RELEASE_NOTES ../README $@/
	cp Debian/changelog.Debian $@/
	gzip -9 $@/changelog.Debian
	cp Debian/copyright $@/

deb :	$(BINDIST) doc-pak
	sudo checkinstall -D --pkgname hyperdash --pkgversion $(LIBNO) \
	--pkgrelease $(RELEASE)  \
	--maintainer "kollo@users.sourceforge.net" \
        --requires libc6,libasound2, --backup  \
	--pkggroup interpreters   \
	--pkglicense GPL --strip=yes --stripso=yes --reset-uids 
	rm -f backup-*.tgz
	mv hyperdash_$(LIBNO)-$(RELEASE)_*.deb Debian/Outputs/


# Auto dependency stuff (from info make)
%.d: %.c
	$(CC) -MM -MT $(@:.d=.o) -MT $@ $(CPPFLAGS) $< -o $@
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include $(DEPSRC:.c=.d)
endif
endif
