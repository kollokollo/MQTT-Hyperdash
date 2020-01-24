/* BASICS.H (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#ifdef WINDOWS
  #define MQTT_HYPERDASH_EXECUTABLE_NAME "hyperdash.exe"
#else
  #define MQTT_HYPERDASH_EXECUTABLE_NAME "hyperdash"
#endif
typedef struct {
  unsigned int len;
  char *pointer;
} STRING;


#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#define min(a,b) ((a<b)?a:b)
#define max(a,b) ((a>b)?a:b)
#define PI       3.141592653589793


#ifdef WINDOWS
  #define bzero(p, l) memset(p, 0, l)
#endif
