/* BASICS.H (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#ifdef WINDOWS
  #define MQTT_HYPERDASH_EXECUTABLE_NAME "hyperdash.exe"
  #define MQTT_DASHDESIGN_EXECUTABLE_NAME "dashdesign.exe"
#else
  #define MQTT_HYPERDASH_EXECUTABLE_NAME "hyperdash"
  #define MQTT_DASHDESIGN_EXECUTABLE_NAME "dashdesign"
#endif

#define AUTOSAVE_ENDING ".autosave"

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

#define SIZEOF_IFILENAME 128
#define SIZEOF_CALL_OPTIONS 256


#define DEFAULT_FONTSIZE 16
#define DEFAULT_FONT        "SMALL"
#define DEFAULT_ICON        "default.png"
#define DEFAULT_BITMAP      "default"
#define DEFAULT_DASH        "main"
#define DEFAULT_FORMAT      "####.###"
#define DEFAULT_TOPIC       "TOPIC_AD"
#define DEFAULT_SHELLCMD    "xload &"
#define DEFAULT_TITLE       "Dashboard"
#define DEFAULT_ALIGN       "TOP"
#define DEFAULT_FGC         "$FFFFFFFF"
#define DEFAULT_BGC         "$40FF"
