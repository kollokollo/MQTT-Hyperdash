# Makefile for MQTT-Hyperdash (c) Markus Hoffmann V.1.02

# This file is part of MQTT-Hyperdash, the universal MQTT Dashboard
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

LIBNO=1.01

RELEASE=4

# Register variables (-ffixed-reg) -Wall
REGS=-fno-omit-frame-pointer  

# Optimization and debugging options
OPT=-O3

# Additional header file paths
INC= 
WININC=-I./WINDOWS -I./WINDOWS/include -I./WINDOWS/include/mqtt -I./WINDOWS/include/GMP

# defines for special flavours

DEF= 

# Compiler
#CC=gcc  $(REGS)
CC=gcc -Wall -g

# Cross-Compiler fuer Windows-Excecutable

WINCC=i686-w64-mingw32-gcc

# Preprocessor
CPP=gcc -E

CFLAGS= -g -O2 $(INC) $(DEF) $(OPT) $(REGS) `pkg-config --cflags gtk+-2.0`

# these are objects which are compiled to binary

MAINOBJS= main.o hyperdash.o mqtt.o lodepng.o file.o util.o graphics.o \
          bitmap.o input.o elements.o subscribe.o

CSRC=    $(MAINOBJS:.o=.c)

CSRC_EXTRA= mqtt-list-topics.c rule-engine.c hddashgen.c

RULESRC=  commander.rule  demo.rule  syslogger.rule  sysmeasure.rule


DEPSRC=  $(CSRC)  mqtt-list-topics.c

# Headerfiles which should be added to the distribution

HSRC=  hyperdash.h lodepng.h mqtt.h file.h util.h graphics.h bitmap.h \
       input.h elements.h subscribe.h basics.h config.h hddashgen.h

# example dash files 

EXAMPLES= ../dashboards/demo.dash ../dashboards/sonnenschein.dash \
          ../dashboards/sysmeasure.dash ../dashboards/imagetest.dash  \
	  ../dashboards/main.dash ../dashboards/test.dash \
	  ../dashboards/metertest.dash

# bitmap files

BITMAPS= ../bitmaps/alarmRing ../bitmaps/bombe_gross  ../bitmaps/Hygrometer \
         ../bitmaps/SmallTriagUp ../bitmaps/battery   ../bitmaps/Bulb       \
	 ../bitmaps/P-meter      ../bitmaps/smiley    ../bitmaps/Battery0   \
	 ../bitmaps/Circle       ../bitmaps/PVModule  ../bitmaps/Snowflake  \
         ../bitmaps/Battery1     ../bitmaps/Disc1     ../bitmaps/RadiationSmall \
	 ../bitmaps/Thermometer  ../bitmaps/Battery2  ../bitmaps/Disc2     \
	 ../bitmaps/README       ../bitmaps/TrendNone ../bitmaps/Battery3  \
         ../bitmaps/Disc3        ../bitmaps/RightArrow  ../bitmaps/UpArrow \
         ../bitmaps/Battery4     ../bitmaps/Disc4       ../bitmaps/schlange  \
	 ../bitmaps/USB-Stick    ../bitmaps/batterySmall  ../bitmaps/DownArrow \
	 ../bitmaps/SmallCircle  ../bitmaps/warning     ../bitmaps/biene    \
	 ../bitmaps/GridPlug     ../bitmaps/SmallDonut  ../bitmaps/WarnTS   \
         ../bitmaps/bombe        ../bitmaps/helfer      \
	 ../bitmaps/SmallTriagDown


# Image files

ICONS=   ../icons/MQTT-Hyperdash-logo.png  ../icons/README


DIST= ../README.md ../CONTRIBUTING.md ../doc/MQTT-Hyperdash.txt ../COPYING *.1 \
	../doc/ACKNOWLEGEMENTS \
	../doc/AUTHORS  \
	../doc/MQTT-rule-engine-howto.md \
	../doc/MQTT-Hyperdash-file-format.md \
	../doc/MQTT-dashgen-naming-conventions.md \
      $(HSRC) $(CSRC) $(CSRC_EXTRA) $(RULESRC) MQTT-Hyperdash.iss \
        Makefile  \
        hyperdash.desktop  \
        $(BITMAPS) $(ICONS) $(EXAMPLES) 


WINDIST=WINDOWS/lib/SDL.dll \
        WINDOWS/lib/README-SDL.txt \
	hyperdash.exe  \
	mqtt-list-topics.exe  \
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

LIBS = -lm -lpaho-mqtt3c -lSDL -lSDL_gfx -lSDL_ttf -lX11 `pkg-config --libs gtk+-2.0`

MYWINLIBS = WINDOWS/lib/libgfx.lib 




all :    hyperdash mqtt-list-topics sysmeasure hddashgen

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

hyperdash.exe: $(CSRC) $(MYWINLIBS)
	$(WINCC) $(WININC)  $(WINLINKFLAGS) -DWINDOWS  $(CSRC) \
	-o $@ $(MYWINLIBS)  -lpaho-mqtt3c  -lSDL -lSDL_ttf

hddashgen : hddashgen.c
	$(CC) hddashgen.c -o $@ -lm 

mqtt-list-topics :  mqtt-list-topics.c mqtt.o subscribe.o util.o
	$(CC)  mqtt-list-topics.c mqtt.o subscribe.o util.o \
	-o $@ -lm -lpaho-mqtt3c 

mqtt-list-topics.exe: mqtt-list-topics.c mqtt.c subscribe.c util.c
	$(WINCC) $(WININC)  $(WINLINKFLAGS) -DWINDOWS  \
	-o $@ mqtt-list-topics.c mqtt.c subscribe.c util.c -lm -lpaho-mqtt3c

rules:	demo sysmeasure syslogger commander


# This defines the compile sequence for .rule files

% : %.rule rule-engine.c mqtt.o subscribe.o util.o
	$(CC) -DRULE=\"$<\" rule-engine.c mqtt.o subscribe.o  util.o \
	-o $@ -lm -lpaho-mqtt3c 

# This defines the compile sequence for .rule files for WINDOWS

%.exe : %.rule rule-engine.c mqtt.c subscribe.c util.c
	$(WINCC) $(WININC) $(WINLINKFLAGS) -DWINDOWS -DRULE=\"$<\" \
	rule-engine.c mqtt.c subscribe.c  util.c \
	-o $@ -lm -lpaho-mqtt3c

#
# Installation sequences
#


install : hyperdash hyperdash.desktop sysmeasure mqtt-list-topics
	install -m 644 hyperdash.desktop $(DESKTOPDIR)/
	install -s -m 755 hyperdash $(BINDIR)/
	install -s -m 755 mqtt-list-topics $(BINDIR)/
	install -s -m 755 hddashgen $(BINDIR)/
	install -s -m 755 sysmeasure $(BINDIR)/
	install -d $(MANDIR)
	install -d $(MANDIR)/man1
	install -m 644 hyperdash.1 $(MANDIR)/man1/
	install -m 644 mqtt-list-topics.1 $(MANDIR)/man1/
	install -d $(DATADIR)/hyperdash
	install -m 775  -d $(DATADIR)/hyperdash/bitmaps
	install -m 644 ../bitmaps/* $(DATADIR)/hyperdash/bitmaps/
	install -m 775  -d $(DATADIR)/hyperdash/icons
	install -m 644 ../icons/* $(DATADIR)/hyperdash/icons/
	install -m 775  -d $(DATADIR)/hyperdash/dashboards
	install -m 644 ../dashboards/* $(DATADIR)/hyperdash/dashboards/

uninstall :
	rm -f $(DESKTOPDIR)/hyperdash.desktop
	rm -f $(BINDIR)/hyperdash
	rm -f $(BINDIR)/mqtt-list-topics
	rm -f $(BINDIR)/hddashgen
	rm -f $(BINDIR)/sysmeasure
	rm -f $(MANDIR)/man1/hyperdash.1
	rm -f $(MANDIR)/man1/mqtt-list-topics.1
	rm -rf $(DATADIR)/hyperdash/bitmaps
	rm -rf $(DATADIR)/hyperdash/icons
	rm -rf $(DATADIR)/hyperdash/dashboards
clean :
	rm -f *.o a.out Makefile.bak demo commander \
	backup-*.tgz *.d
	rm -rf doc-pak
moreclean : clean
	rm -f hyperdash sysmeasure mqtt-list-topics

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

DEBDOC=	../doc/ACKNOWLEGEMENTS ../doc/AUTHORS \
	 ../README.md ../doc/MQTT-Hyperdash-file-format.md \
	 ../doc/MQTT-rule-engine-howto.md

doc-pak: $(DEBDOC)
	mkdir -p $@
	cp ../doc/ACKNOWLEGEMENTS  $@/
	cp ../doc/AUTHORS  ../README.md $@/
#	cp Debian/changelog.Debian $@/
#	gzip -9 $@/changelog.Debian
#	cp Debian/copyright $@/

DEBNAME= mqtt-hyperdash

deb :	$(BINDIST) doc-pak
	sudo checkinstall -D --pkgname $(DEBNAME) --pkgversion $(LIBNO) \
	--pkgrelease $(RELEASE)  \
	--maintainer "kollo@users.sourceforge.net" \
        --requires libc6,libsdl1.2debian,libsdl-gfx1.2-5,libsdl-ttf2.0-0 \
	--backup  \
	--pkggroup interpreters   \
	--pkglicense GPL --strip=yes --stripso=yes --reset-uids 
	rm -f backup-*.tgz
	sudo chown 1000 $(DEBNAME)_$(LIBNO)-$(RELEASE)_*.deb
	mv $(DEBNAME)_$(LIBNO)-$(RELEASE)_*.deb Debian/Outputs/


# Auto dependency stuff (from info make)
%.d: %.c
	$(CC) -MM -MT $(@:.d=.o) -MT $@ $(CPPFLAGS) $< -o $@
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include $(DEPSRC:.c=.d)
endif
endif
