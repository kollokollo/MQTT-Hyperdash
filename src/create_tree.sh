#!/bin/sh
# Shell script to create the initial local folder in 
# $(Home)/.hyperdash/dashboards
# copy the system files to the local folder and 
# make generic dashboards from availabe topics on the broker
#
# This file is part of MQTT-Hyperdash, the universal MQTT Dashboard
# ======================================================================
# MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
# COPYING for details
#                                 (c) 2020 by Markus Hoffmann
#
#
mosquitto_pub -h localhost -t MQTT_HYPERDASH/STATUS_SM -m "preparing directories..." -r
echo "preparing directories... " ~/.hyperdash/dashboards

mkdir -p ~/.hyperdash/dashboards
cp -n /usr/share/hyperdash/dashboards/* ~/.hyperdash/dashboards/
cp -u /usr/share/hyperdash/dashboards/main.dash ~/.hyperdash/dashboards/
cp -u /usr/share/hyperdash/dashboards/remote_tree.dash ~/.hyperdash/dashboards/
cd ~/.hyperdash/dashboards/



if [ "$#" -le 0 ]; then
 mosquitto_pub -h localhost -t MQTT_HYPERDASH/STATUS_SM -m "generating local dashboards..." -r
 echo "generating local dashboards..."
 mqtt-list-topics | sort | hddashgen
fi
if [ "$#" -eq 1 ]; then
 BROKER=`mosquitto_sub -h localhost -t "MQTT_HYPERDASH/BROKER_SC" -C 1`
 BASEBROKER="${BROKER##*/}"
 BASEBROKER="${BASEBROKER%%:*}"
 mosquitto_pub -h localhost -t MQTT_HYPERDASH/STATUS_SM -m "generating remote dashboards..." -r
 echo "generating remote dashboards for " $BROKER
 mqtt-list-topics --broker $BROKER | sort | hddashgen --broker $BROKER --prefix $BASEBROKER
fi
mosquitto_pub -h localhost -t MQTT_HYPERDASH/STATUS_SM -m "done." -r
echo "done."
