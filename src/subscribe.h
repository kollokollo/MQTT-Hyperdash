/* SUBSCRIBE.H (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

typedef struct {
  char *topic;
  int qos;
  int anz;
  STRING last_value;
} SUBSCRIPTION;

extern SUBSCRIPTION subscriptions[];
extern int anzsubscriptions;

int find_subscription(const char *topic);
void clear_subscription(int idx);
void clear_all_subscriptions();
int add_subscription(const char *topic, int qos);
