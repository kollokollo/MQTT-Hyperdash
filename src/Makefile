# Makefile for MQTT-Hyperdash (c) Markus Hoffmann V.1.03

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
LIBDIR=${exec_prefix}/lib
MANDIR=${datarootdir}/man

NAME=		MQTT-Hyperdash
DNAME=		mqtt-hyperdash
LIBNO=		1.03
RELEASE=	16
YEAR=		2021

# Register variables (-ffixed-reg) -Wall
REGS=-fno-omit-frame-pointer  

# Optimization and debugging options
OPT=-O3

# Additional header file paths
INC= 
WININC= -I./WINDOWS -I./WINDOWS/include -I./WINDOWS/include/mqtt -I./WINDOWS/include/GMP \
	-I/usr/include/gtk-2.0 -I/usr/include/glib-2.0 \
	-I./WINDOWS/include/glib-2.0 \
	-I./WINDOWS/include/gtk-2.0 \
	-I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 \
	-I/usr/include/harfbuzz -I/usr/include/pixman-1 -I/usr/include/gdk-pixbuf-2.0

#    see:     `pkg-config --cflags gtk+-2.0`


# defines for special flavours

DEF= 

# Compiler
#CC=gcc  $(REGS)
CC=gcc -Wall
CCSHARED=gcc  -fPIC -shared -Wl,-Bsymbolic-functions 

# Cross-Compiler fuer Windows-Excecutable

WINCC=i686-w64-mingw32-gcc -std=c99

# Cross-Compiler fuer ATARI ST TOS
TOSCC=m68k-atari-mint-gcc -std=c99

# Cross-Compiler fuer ARM-Linux-Excecutable

ARMCC=arm-linux-gcc 

# Preprocessor
CPP=gcc -E

CFLAGS= -g -O2 $(INC) $(DEF) $(OPT) $(REGS) `pkg-config --cflags gtk+-2.0`

# these are objects which are compiled to binary


LIBOBJS=  hyperdash.o mqtt.o lodepng.o file.o util.o graphics.o \
          bitmap.o input.o input_value_dialog.o elements.o subscribe.o
LIBCSRC=  $(LIBOBJS:.o=.c)

MAINOBJS= main.o $(LIBOBJS)

CSRC=    $(MAINOBJS:.o=.c)

CSRC_EXTRA= mqtt-list-topics.c rule-engine.c hddashgen.c dashdesign.c \
            element_groups.c menu.c mqtt2serial.c serial.c

RULESRC=  commander.rule  demo.rule  syslogger.rule  sysmeasure.rule


DEPSRC=  $(CSRC) mqtt-list-topics.c hddashgen.c dashdesign.c menu.c \
                 element_groups.c 

# Headerfiles which should be added to the distribution

HSRC=  hyperdash.h lodepng.h mqtt.h file.h util.h graphics.h bitmap.h \
       input.h input_value_dialog.h elements.h subscribe.h basics.h config.h hddashgen.h \
       dashdesign.h menu.h element_groups.h serial.h

# example dash files 

EXAMPLES= ../dashboards/demo.dash ../dashboards/sonnenschein.dash \
          ../dashboards/sysmeasure.dash ../dashboards/imagetest.dash  \
	  ../dashboards/main.dash ../dashboards/test.dash \
	  ../dashboards/metertest.dash ../dashboards/remote_tree.dash \
	  ../dashboards/mosquitto.dash \
	  ../dashboards/_sys_broker_clients_hd.dash \
	  ../dashboards/_sys_broker_messages_hd.dash \
	  ../dashboards/_sys_broker_subscriptions_hd.dash \
	  ../dashboards/_sys_broker_heap_hd.dash \
	  ../dashboards/_sys_broker_publish_hd.dash \
	  ../dashboards/_sys_broker_bytes_hd.dash \
	  ../dashboards/_sys_broker_load_hd.dash \
	  ../dashboards/_sys_broker_publish_messages_hd.dash \
	  ../dashboards/_sys_broker_publish_bytes_hd.dash \
	  ../dashboards/_sys_broker_load_messages_hd.dash \
	  ../dashboards/_sys_broker_load_bytes_hd.dash \
	  ../dashboards/_sys_broker_load_sockets_hd.dash \
	  ../dashboards/_sys_broker_load_connections_hd.dash \
	  ../dashboards/_sys_broker_load_publish_hd.dash \
	  ../dashboards/_sys_broker_load_publish_received_hd.dash \
	  ../dashboards/_sys_broker_load_publish_sent_hd.dash \
	  ../dashboards/_sys_broker_load_bytes_received_hd.dash \
	  ../dashboards/_sys_broker_load_bytes_sent_hd.dash \
	  ../dashboards/_sys_broker_load_messages_received_hd.dash \
	  ../dashboards/_sys_broker_load_messages_sent_hd.dash
	  

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
	 ../bitmaps/SmallTriagDwn ../bitmaps/TickLeft ../bitmaps/TickRight \
	 ../bitmaps/Sun ../bitmaps/default

MANPAGES= hddashgen.1  hyperdash.1  mqtt-list-topics.1 dashdesign.1 sysmeasure.1

# Image files

ICONS=   ../icons/MQTT-Hyperdash-logo.png  ../icons/README \
         ../icons/dashtree.png ../icons/default.png

EXE=	hyperdash mqtt-list-topics dashdesign sysmeasure hddashgen dashdesign mqtt2serial

DOC=	../doc/MQTT-Hyperdash.txt ../doc/MQTT-rule-engine-howto.md ../doc/MQTT-Hyperdash-file-format.md \
	../doc/MQTT-dashgen-naming-conventions.md ../doc/MQTT-dashdesign-user-manual.md \
	../doc/MQTT-Hyperdash-payload-conventions.md ../doc/ACKNOWLEGEMENTS ../doc/AUTHORS


DIST= ../README.md ../CONTRIBUTING.md ../COPYING \
	$(DOC) $(HSRC) $(CSRC) $(CSRC_EXTRA) $(RULESRC) MQTT-Hyperdash.iss.in \
        Makefile create_tree.sh \
        hyperdash.desktop  \
        $(BITMAPS) $(ICONS) $(EXAMPLES) $(MANPAGES)

WINEXE= hyperdash.exe mqtt-list-topics.exe hddashgen.exe mqtt2serial.exe dashdesign.exe

DLL=	WINDOWS/lib/SDL.dll WINDOWS/lib/libfreetype-6.dll \
	WINDOWS/lib/paho-mqtt3c.dll WINDOWS/lib/SDL_ttf.dll \
	WINDOWS/lib/zlib1.dll \
	WINDOWS/lib/libgdk-win32-2.0-0.dll \
	WINDOWS/lib/libgtk-win32-2.0-0.dll \
	WINDOWS/lib/libatk-1.0-0.dll \
	WINDOWS/lib/libgdk_pixbuf-2.0-0.dll \
	WINDOWS/lib/libgobject-2.0-0.dll \
	WINDOWS/lib/libpangowin32-1.0-0.dll \
	WINDOWS/lib/libcairo-2.dll \
	WINDOWS/lib/libpng16-16.dll \
	WINDOWS/lib/libffi-6.dll \
	WINDOWS/lib/libgio-2.0-0.dll \
	WINDOWS/lib/libiconv-2.dll \
	WINDOWS/lib/libglib-2.0-0.dll \
	WINDOWS/lib/libpango-1.0-0.dll

DLLTXT= WINDOWS/lib/README-SDL.txt \
	WINDOWS/lib/LICENSE.freetype.txt \
	WINDOWS/lib/LICENSE-SDL_ttf.txt \
	WINDOWS/lib/LICENSE.zlib.txt \
	WINDOWS/lib/README-paho.txt \
	WINDOWS/lib/README-SDL_gfx.txt

WINDOC= WINDOWS/readme-windows.txt INTRO ../COPYING ../RELEASE_NOTES 


WINDIST=$(DLL) $(DLLTXT) $(WINEXE) $(WINDOC) \
	WINDOWS/hyperdash.ico  WINDOWS/demo.dash ../dashboards/main.dash \
	WINDOWS/dash.ico 

DIR=$(NAME)-$(LIBNO)
DIR2=$(NAME)-src-$(LIBNO)
DIR3=$(NAME)-bin-$(LIBNO)

LINKFLAGS = 
WINLINKFLAGS = -L./WINDOWS/lib 
WINLIBS=	-lm -lgdi32 -lkernel32 -luser32 -lole32 -luuid -lwsock32 \
		-lpaho-mqtt3c \
		-lSDL  -lSDL_ttf -lzlib1 -lfreetype-6 \
		-lgtk-win32-2.0-0 -lgdk-win32-2.0-0 \
		-lpangocairo-1.0-0 -latk-1.0-0 -lcairo-2 \
		-lgdk_pixbuf-2.0-0 \
		-lgio-2.0-0 -lpango-1.0-0 -lgobject-2.0-0 -lglib-2.0-0 
		
		
# -lfontconfig -lfreetype -lpangoft2-1.0 -lharfbuzz 

LIBS = -lm -lpthread -lpaho-mqtt3c -lSDL -lSDL_gfx -lSDL_ttf -lX11 `pkg-config --libs gtk+-2.0`

STATICWINLIBS = WINDOWS/lib/libSDL_gfx.a

BINDIST = ../COPYING $(EXE)

all :    $(EXE)

 
# Make the excecutable

hyperdash:  $(MAINOBJS)
	$(CC) -L. $(LINKFLAGS) -o $@ $+ $(LIBS)

# Make the exe for MS WINDOWS

hyperdash.exe: $(CSRC) WINDOWS/fnmatch.c $(STATICWINLIBS) WINDOWS/lib/SDL.dll WINDOWS/lib/SDL_ttf.dll
	$(WINCC) $(WININC) $(WINLINKFLAGS) -DWINDOWS -o $@ $(CSRC) WINDOWS/fnmatch.c $(STATICWINLIBS) $(WINLIBS)


dashdesign :  dashdesign.o menu.o element_groups.o $(LIBOBJS)
	$(CC) -L. $(LINKFLAGS) -o $@ $+ $(LIBS)

dashdesign.exe: dashdesign.c menu.c element_groups.c $(LIBCSRC) WINDOWS/fnmatch.c $(STATICWINLIBS) WINDOWS/lib/SDL.dll WINDOWS/lib/SDL_ttf.dll
	$(WINCC) $(WININC) $(WINLINKFLAGS) -DWINDOWS -o $@ dashdesign.c menu.c element_groups.c WINDOWS/fnmatch.c $(LIBCSRC) $(STATICWINLIBS) $(WINLIBS)

hddashgen : hddashgen.c file.o
	$(CC) -o $@ $+ -lm 

hddashgen.exe: hddashgen.c file.c  WINDOWS/fnmatch.c WINDOWS/getline.c
	$(WINCC) $(WININC) $(WINLINKFLAGS) -DWINDOWS -o $@ $+ -lm -lpaho-mqtt3c

mqtt2serial :  mqtt2serial.c mqtt.o subscribe.o util.o file.o serial.o
	$(CC) -o $@ $+ -lm -lpaho-mqtt3c 

mqtt2serial.exe: mqtt2serial.c mqtt.c subscribe.c util.c file.c serial.c
	$(WINCC) $(WININC) $(WINLINKFLAGS) -DWINDOWS -o $@ $+ -lm -lpaho-mqtt3c

mqtt-list-topics:  mqtt-list-topics.c mqtt.o subscribe.o util.o
	$(CC) -o $@ $+ -lm -lpaho-mqtt3c 

mqtt-list-topics.exe: mqtt-list-topics.c mqtt.c subscribe.c util.c
	$(WINCC) $(WININC)  $(WINLINKFLAGS) -DWINDOWS -o $@ $+ -lm -lpaho-mqtt3c

rules:	demo sysmeasure syslogger commander


# This defines the compile sequence for .rule files

% : %.rule rule-engine.c mqtt.o subscribe.o util.o
	$(CC) -DRULE=\"$<\" rule-engine.c mqtt.o subscribe.o  util.o \
	-o $@ -lm -lpaho-mqtt3c 

# This defines the compile sequence for .rule files for WINDOWS

%.exe : %.rule rule-engine.c mqtt.c subscribe.c util.c
	$(WINCC) $(WININC) $(WINLINKFLAGS) -DWINDOWS -DRULE=\"$<\" \
	rule-engine.c mqtt.c subscribe.c util.c -o $@ -lm -lpaho-mqtt3c

#
# Installation sequences
#


install : $(EXE) hyperdash.desktop $(MANPAGES) create_tree.sh
	install -m 644 hyperdash.desktop $(DESKTOPDIR)/
	install -s -m 755 hyperdash $(BINDIR)/
	install -s -m 755 mqtt-list-topics $(BINDIR)/
	install -s -m 755 hddashgen $(BINDIR)/
	install -s -m 755 dashdesign $(BINDIR)/
	install -s -m 755 sysmeasure $(BINDIR)/
	install -s -m 755 mqtt2serial $(BINDIR)/
	install -d $(MANDIR)
	install -d $(MANDIR)/man1
	install -m 644 hyperdash.1 $(MANDIR)/man1/
	install -m 644 hddashgen.1 $(MANDIR)/man1/
	install -m 644 dashdesign.1 $(MANDIR)/man1/
	install -m 644 mqtt2serial.1 $(MANDIR)/man1/
	install -m 644 mqtt-list-topics.1 $(MANDIR)/man1/
	install -m 644 sysmeasure.1 $(MANDIR)/man1/
	install -d $(DATADIR)/hyperdash
	install -m 755 create_tree.sh $(DATADIR)/hyperdash/
	install -m 775 -d $(DATADIR)/hyperdash/bitmaps
	install -m 644 $(BITMAPS) $(DATADIR)/hyperdash/bitmaps/
	install -m 775 -d $(DATADIR)/hyperdash/icons
	install -m 644 $(ICONS) $(DATADIR)/hyperdash/icons/
	install -m 775 -d $(DATADIR)/hyperdash/dashboards
	install -m 644 $(EXAMPLES) $(DATADIR)/hyperdash/dashboards/

uninstall :
	rm -f $(DESKTOPDIR)/hyperdash.desktop
	rm -f $(BINDIR)/hyperdash
	rm -f $(BINDIR)/mqtt-list-topics
	rm -f $(BINDIR)/hddashgen
	rm -f $(BINDIR)/dashdesign
	rm -f $(BINDIR)/sysmeasure
	rm -f $(BINDIR)/mqtt2serial
	rm -f $(MANDIR)/man1/hyperdash.1
	rm -f $(MANDIR)/man1/hddashgen.1
	rm -f $(MANDIR)/man1/dashdesign.1
	rm -f $(MANDIR)/man1/mqtt2serial.1
	rm -f $(MANDIR)/man1/mqtt-list-topics.1
	rm -f $(MANDIR)/man1/sysmeasure.1
	rm -rf $(DATADIR)/hyperdash/bitmaps
	rm -rf $(DATADIR)/hyperdash/icons
	rm -rf $(DATADIR)/hyperdash/dashboards
	rm -f $(DATADIR)/hyperdash/create_tree.sh
clean :
	rm -f *.o a.out Makefile.bak demo commander \
	backup-*.tgz *.d
	rm -rf doc-pak
moreclean : clean
	rm -f $(EXE) $(WINEXE)

distclean : moreclean
	rm -f Makefilex config.cache config.status config.log config.h \
	rm -rf autom4te.cache

$(DIR).tar.gz : dist
dist :	$(DIST)
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
	 ../README.md \
	 ../doc/MQTT-Hyperdash-file-format.md \
	 ../doc/MQTT-Hyperdash-payload-conventions.md \
	 ../doc/MQTT-rule-engine-howto.md \
	 ../doc/MQTT-Hyperdash.txt \
	 ../doc/MQTT-dashgen-naming-conventions.md \
	 ../doc/MQTT-dashdesign-user-manual.md

doc-pak: $(DEBDOC)
	mkdir -p $@
	cp $(DEBDOC) $@/
	cp Debian/changelog.Debian $@/
	gzip -9 $@/changelog.Debian
	cp Debian/copyright $@/

deb :	$(BINDIST) doc-pak
	sudo checkinstall -D --pkgname $(DNAME) --pkgversion $(LIBNO) \
	--pkgrelease $(RELEASE)  \
	--maintainer "kollo@users.sourceforge.net" \
        --requires libsdl1.2debian,libsdl-gfx1.2-5,libsdl-ttf2.0-0 \
	--backup  \
	--pkggroup interpreters   \
	--pkglicense GPL --strip=yes --stripso=yes --reset-uids 
	rm -f backup-*.tgz
	sudo chown 1000 $(DNAME)_$(LIBNO)-$(RELEASE)_*.deb
	mkdir -p Debian/Outputs/
	mv $(DNAME)_$(LIBNO)-$(RELEASE)_*.deb Debian/Outputs/


####################################
# excecute the Dockerfile 
# and extract the debian package 
# and the WINDOWS Version
# of MQTT-Hyperdash
####################################
#
# build the docker image (basically including the build environment)
dockerimage-$(LIBNO)-$(RELEASE): docker/Dockerfile 
	cd docker;sudo docker build . -t kollo/$(DNAME):$(LIBNO)-$(RELEASE)
	touch $@

DOCKERRUNNER=cd docker;sudo docker run -ti -v`pwd`:/work/ kollo/$(DNAME):$(LIBNO)-$(RELEASE) 

docker/isetup-5.5.8.exe: dockerimage-$(LIBNO)-$(RELEASE)
	$(DOCKERRUNNER) cp /isetup-5.5.8.exe /work/
	sudo chown 1000 $@

# Get the debian package
docker/$(DNAME)_$(LIBNO)-$(RELEASE)_amd64.deb:	dockerimage-$(LIBNO)-$(RELEASE)
	$(DOCKERRUNNER) cp /$(DNAME)_$(LIBNO)-$(RELEASE)_amd64.deb /work/
	sudo chown 1000 $@

# Get the debian package of paho.c 
docker/paho.mqtt.c_1.0-1_amd64.deb:	dockerimage-$(LIBNO)-$(RELEASE)
	$(DOCKERRUNNER) cp /paho.mqtt.c_1.0-1_amd64.deb /work/
	sudo chown 1000 $@

# Make and get the WINDOWS Version
docker/$(NAME)-$(LIBNO)-$(RELEASE)-setup.exe:	dockerimage-$(LIBNO)-$(RELEASE)
	$(DOCKERRUNNER) tcsh -c "cd /MQTT-Hyperdash/src;make windows;cp Inno-Output/$(NAME)-$(LIBNO)-$(RELEASE)-setup.exe /work/"
	sudo chown 1000 $@


# excecute the Dockerfile and extract the debian packages and run it (this would start dashdesign)
docker: docker/isetup-5.5.8.exe \
	docker/$(DNAME)_$(LIBNO)-$(RELEASE)_amd64.deb \
	docker/paho.mqtt.c_1.0-1_amd64.deb \
	docker/$(NAME)-$(LIBNO)-$(RELEASE)-setup.exe
	XAUTHORITY=$(xauth info | grep "Authority file" | awk '{ print $3 }')
	echo $(XAUTHORITY)
	cd docker;sudo docker run -ti -v`pwd`:/work/ --net=host -e DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix:ro \
	-v $(XAUTHORITY):/root/.Xauthority:ro kollo/$(DNAME):$(LIBNO)-$(RELEASE)
	echo "done."

# Make the Windows distribution in form of a .zip file including an installer
# this is depreciated. Better use Inno-Setup to create a self installing
# .exe file

MQTT-Hyperdash.iss: MQTT-Hyperdash.iss.in
	cp $< $@
	sed -i "s/@winversion@/$(LIBNO)/g" $@ 
	sed -i "s/@winrelease@/$(RELEASE)/g" $@ 
	sed -i "s/@winyear@/$(YEAR)/g" $@ 

# Make prerequisites for the WINDOWS build
# get the official WINDOWS binary for libSDL
WINDOWS/lib/SDL.dll:
	mkdir -p WINDOWS/tmp
	cd WINDOWS/tmp;wget https://www.libsdl.org/release/SDL-1.2.15-win32.zip
	cd WINDOWS/tmp;unzip SDL-1.2.15-win32.zip
	mkdir -p WINDOWS/lib
	mv WINDOWS/tmp/README-SDL.txt WINDOWS/lib/
	mv WINDOWS/tmp/SDL.dll WINDOWS/lib/

# get the DSL_gfx sources and compile a static library for mingw

WINDOWS/lib/libSDL_gfx.a WINDOWS/lib/README-SDL_gfx.txt:
	mkdir -p WINDOWS/tmp
	cd WINDOWS/tmp;wget www.ferzkopp.net/Software/SDL_gfx-2.0/SDL_gfx-2.0.26.tar.gz
	cd WINDOWS/tmp;tar xzfv SDL_gfx-2.0.26.tar.gz
	mv WINDOWS/tmp/SDL_gfx-2.0.26.tar.gz WINDOWS/contrib/
	cd WINDOWS/tmp/SDL_gfx-2.0.26/;./configure  --host=i686-w64-mingw32
	sed -i "s/CC = gcc/CC = i686-w64-mingw32-gcc -std=c99/g" WINDOWS/tmp/SDL_gfx-2.0.26/Makefile.mingw 
	sed -i "s/prefix=c:\/dev/prefix=..\/..\//g" WINDOWS/tmp/SDL_gfx-2.0.26/Makefile.mingw 
	sed -i "s/-march=athlon-xp -mmmx -msse -m3dnow//g" WINDOWS/tmp/SDL_gfx-2.0.26/Makefile.mingw 
	sed -i "s/=SDL_gfx.dll/=/g" WINDOWS/tmp/SDL_gfx-2.0.26/Makefile.mingw 
	sed -i "s/SDL_imageFilter.o SDL_rotozoom.o/SDL_imageFilter.o SDL_rotozoom.o SDL_gfxBlitFunc.o/g" WINDOWS/tmp/SDL_gfx-2.0.26/Makefile.mingw 
	cd WINDOWS/tmp/SDL_gfx-2.0.26/;make -f Makefile.mingw
	mv WINDOWS/tmp/SDL_gfx-2.0.26/README WINDOWS/lib/README-SDL_gfx.txt
	mv WINDOWS/tmp/SDL_gfx-2.0.26/libSDL_gfx.a WINDOWS/lib/

WINDOWS/lib/SDL_ttf.dll WINDOWS/lib/libfreetype-6.dll:
	mkdir -p WINDOWS/tmp
	cd WINDOWS/tmp;wget https://www.libsdl.org/projects/SDL_ttf/release/SDL_ttf-2.0.11-win32.zip 
	cd WINDOWS/tmp;unzip SDL_ttf-2.0.11-win32.zip
	mkdir -p WINDOWS/lib
	mv WINDOWS/tmp/LICENSE.zlib.txt     WINDOWS/lib/
	mv WINDOWS/tmp/LICENSE.freetype.txt WINDOWS/lib/
	mv WINDOWS/tmp/SDL_ttf.dll          WINDOWS/lib/
	mv WINDOWS/tmp/zlib1.dll            WINDOWS/lib/
	mv WINDOWS/tmp/libfreetype-6.dll    WINDOWS/lib/
	mv WINDOWS/tmp/README.txt           WINDOWS/lib/LICENSE-SDL_ttf.txt

# Make a simple zip file distribution for WINDOWS

winzip: $(NAME)-$(LIBNO)-$(RELEASE)-win.zip
$(NAME)-$(LIBNO)-$(RELEASE)-win.zip: $(WINDIST)
	zip -j -D  -o $@ $(WINDIST)

# Make the setup.exe installation program for WINDOWS

windows: $(WINDIST) MQTT-Hyperdash.iss
	WINDOWS/contrib/iscc MQTT-Hyperdash.iss

# Auto dependency stuff (from info make)
%.d: %.c
	$(CC) -DDEPEND -MM -MT $(@:.d=.o) -MT $@ $(CPPFLAGS) $< -o $@
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include $(DEPSRC:.c=.d)
endif
endif
