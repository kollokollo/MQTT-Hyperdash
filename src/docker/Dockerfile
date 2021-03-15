# Dockerfile for MQTT-Hyperdash based on the one by Bruno https://github.com/shakenfr/x11basic_docker/
# Start with basis jessie
# The docker file generates an installable .deb package of MQTT-Hyperdash 
FROM debian:jessie
VOLUME work
RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections
RUN apt-get update \
&& apt-get install -y apt-utils git tcsh nano vim curl wget make gcc build-essential

RUN apt-get install -y checkinstall libssl-dev \
libsdl-gfx1.2-dev libsdl1.2-dev libsdl-ttf2.0-dev \ 
libgtk2.0-dev 
#&& rm -rf /var/lib/apt/lists/*

RUN apt-get install -y git git 

# get the paho.c library

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

#RUN apt-get install ttf-mscorefonts-installer

RUN mkdir -p MQTT-Hyperdash/src/Debian/Outputs
RUN cd MQTT-Hyperdash/src;make;make install
RUN cd MQTT-Hyperdash/src/;sed -i 's/sudo\ checkinstall/checkinstall/g' Makefile
RUN cd MQTT-Hyperdash/src/;sed -i 's/sudo\ chown/chown/g' Makefile

# Now make the debian package
RUN cd MQTT-Hyperdash/src;make deb

# copy the results in the root of the container
RUN cp -R /paho.mqtt.c/Debian/Outputs/*.deb /
RUN cp -R /MQTT-Hyperdash/src/Debian/Outputs/*.deb /

WORKDIR /work
CMD ["/bin/sh"]
