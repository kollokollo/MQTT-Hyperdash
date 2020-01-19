#!/bin/sh
# Shell script to create the initial local folder in 
# $(Home)/.hyperdash/dashboards
# copy the system files to the local folder and 
# make generic dashboards from availabe topics on the broker
#
# this file is part of MQTT-Hyperdash (c) by Markus Hoffmann
#
#
mosquitto_pub -h localhost -t MQTT_HYPERDASH/STATUS_SM -m "preparing directories..." -r
mkdir -p ~/.hyperdash/dashboards
cp /usr/share/hyperdash/dashboards/* ~/.hyperdash/dashboards/
cd ~/.hyperdash/dashboards/



if [ "$#" -le 0 ]; then
mosquitto_pub -h localhost -t MQTT_HYPERDASH/STATUS_SM -m "generating local dashboards..." -r
mqtt-list-topics | sort | hddashgen
fi
if [ "$#" -eq 1 ]; then
BROKER=`mosquitto_sub -h localhost -t "MQTT_HYPERDASH/BROKER_SC" -C 1`
BASEBROKER="${BROKER##*/}"
BASEBROKER="${BASEBROKER%%:*}"
mosquitto_pub -h localhost -t MQTT_HYPERDASH/STATUS_SM -m "generating remote dashboards..." -r
 mqtt-list-topics --broker $BROKER | sort | hddashgen --broker $BROKER --prefix $BASEBROKER

fi


mosquitto_pub -h localhost -t MQTT_HYPERDASH/STATUS_SM -m "done." -r
