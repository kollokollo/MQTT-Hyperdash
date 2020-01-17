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

mosquitto_pub -h localhost -t MQTT_HYPERDASH/STATUS_SM -m "generating dashboards..." -r

mqtt-list-topics | sort | hddashgen






mosquitto_pub -h localhost -t MQTT_HYPERDASH/STATUS_SM -m "done." -r
