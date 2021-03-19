# Dockerfile for MQTT-Hyperdash based on the one by Bruno https://github.com/shakenfr/x11basic_docker/
# Start with Ubuntu 20.04
# The docker file generates an installable .deb package of MQTT-Hyperdash 
FROM ubuntu:20.04
VOLUME work
RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections

#
# Basic Packages
#


RUN apt-get update \
&& apt-get install -y apt-utils git tcsh nano vim curl wget make gcc build-essential
RUN apt-get install -y  checkinstall imagemagick transfig

RUN apt-get install -y libssl-dev libsdl-gfx1.2-dev libsdl1.2-dev libsdl-ttf2.0-dev libgtk2.0-dev 
RUN apt-get install -y ttf-mscorefonts-installer
#
# Crosscompiler for WINDOWS
#
RUN apt-get install -y gcc-mingw-w64 wine

#
# prepare ImageMagic (for Ubuntu)
#
RUN cd /etc/ImageMagick-6/;sed -i 's/"none"/"read | write"/g' policy.xml
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

# cleanup
RUN apt-get clean


#
# get the paho.c library
#

RUN git clone https://github.com/eclipse/paho.mqtt.c.git
RUN cd paho.mqtt.c;git checkout fbf9828200f46e212189d98eaedf8e11281e409a

# get MQTT-hyperdash
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

# Now make the debian package
RUN cd MQTT-Hyperdash/src;make deb


# copy the results in the root of the container
RUN cp -R /paho.mqtt.c/Debian/Outputs/*.deb /
RUN cp -R /MQTT-Hyperdash/src/Debian/Outputs/*.deb /



#
# Make Version for WINDOWS
#
# TODO:
RUN cd MQTT-Hyperdash/src;make windows
RUN cp -R MQTT-Hyperdash/src/Inno-Output/*.exe /


WORKDIR /work
CMD ["/bin/sh"]
