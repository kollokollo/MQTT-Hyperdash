/* MQTT.h (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
void mqtt_init();
void mqtt_exit();
int mqtt_broker(char *url,char *user, char *passwd);
void mqtt_subscribe(const char *topic,int qos);
void mqtt_publish(char *topic, STRING payload, int qos, int retain);
void mqtt_disconnect();
void mqtt_unsubscribe_all();

extern volatile int mqtt_isconnected;

int add_subscription(const char *topic);
int find_subscription(const char *topic);
extern SUBSCRIPTION subscriptions[];
extern int anzsubscriptions;
void mqtt_subscribe_all();
