# Dockerfile for MQTT-Hyperdash 
# Start with Ubuntu 20.04
#
# The docker file generates an installable .deb package of MQTT-Hyperdash 
# and the paho.mqtt library 
# as well as a setup.exe for WINDOWS
#
FROM ubuntu:20.04
VOLUME work
RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections

#
# Basic Packages
#


RUN apt-get update \
&& apt-get install -y apt-utils git tcsh nano vim curl wget make gcc build-essential
RUN apt-get install -y  checkinstall imagemagick transfig zip unzip
RUN apt-get install -y libsdl-gfx1.2-dev libsdl1.2-dev libsdl-ttf2.0-dev

#
# For MQTT Version
#

RUN apt-get install -y libssl-dev

#
# Crosscompiler for WINDOWS
#
RUN apt-get install -y gcc-mingw-w64 wine

#
# Prepare for the WINDOWS version
#
RUN dpkg --add-architecture i386
RUN apt-get update
RUN apt-get install -y wine32

RUN wget https://files.jrsoftware.org/is/5/isetup-5.5.8.exe  
RUN mkdir -p /root
COPY wineblob.tar.gz /root/
RUN cd /root/; tar xzfv wineblob.tar.gz
RUN chown -R 0 /root/.wine
#RUN wine isetup-5.5.8.exe

# For some reason this command is needed to initialize wine and its directory
RUN wine cmd /c 'echo %PROGRAMFILES%'


#
# prepare ImageMagic (for Ubuntu)
#
RUN cd /etc/ImageMagick-6/;sed -i 's/"none"/"read | write"/g' policy.xml

#
# get the paho.c library
#

RUN git clone https://github.com/eclipse/paho.mqtt.c.git
RUN cd paho.mqtt.c;git checkout fbf9828200f46e212189d98eaedf8e11281e409a

#
# Special Packages for MQTT-Hyperdash
#


RUN apt-get install -y libgtk2.0-dev 
RUN apt-get install -y ttf-mscorefonts-installer


#
# Clean up some bit
#
RUN apt-get clean



# get MQTT-hyperdash
RUN echo "get MQTT-hyperdash"

RUN git clone https://codeberg.org/kollo/MQTT-Hyperdash.git

RUN cp MQTT-Hyperdash/etc/paho-description-pak paho.mqtt.c/description-pak
RUN cp MQTT-Hyperdash/etc/paho-Makefile paho.mqtt.c/Makefile
RUN cd paho.mqtt.c/;sed -i 's/sudo\ checkinstall/checkinstall/g' Makefile
RUN cd paho.mqtt.c/;sed -i 's/sudo\ chown/chown/g' Makefile
RUN cd paho.mqtt.c;make;make deb

# MQTT-hyperdash

RUN mkdir -p MQTT-Hyperdash/src/Debian/Outputs
RUN cd MQTT-Hyperdash/src;make;make install
RUN cd MQTT-Hyperdash/src/;sed -i 's/sudo\ checkinstall/checkinstall/g' Makefile
RUN cd MQTT-Hyperdash/src/;sed -i 's/sudo\ chown/chown/g' Makefile

# Now make and install the debian package
RUN cd MQTT-Hyperdash/src;make deb

#
# Prepare stuff for the WINDOWS Version
#

RUN cd MQTT-Hyperdash/src/;make WINDOWS/lib/SDL.dll
RUN cd MQTT-Hyperdash/src/;make WINDOWS/lib/libSDL_gfx.a WINDOWS/lib/README-SDL_gfx.txt
RUN cd MQTT-Hyperdash/src/;make WINDOWS/lib/SDL_ttf.dll WINDOWS/lib/libfreetype-6.dll
RUN cd MQTT-Hyperdash/src/;make clean

# copy the results in the root of the container
RUN cp -R /paho.mqtt.c/Debian/Outputs/*.deb /
RUN cp -R /MQTT-Hyperdash/src/Debian/Outputs/*.deb /

WORKDIR /work
CMD ["/usr/bin/dashdesign"]
