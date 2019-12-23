/* BASICS.H (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
typedef struct {
  unsigned int len;
  char *pointer;
} STRING;
#define min(a,b) ((a<b)?a:b)
