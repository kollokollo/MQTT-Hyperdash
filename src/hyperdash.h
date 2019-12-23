/* HYPERDASH.H (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#ifndef __HYPERDASH
#define __HYPERDASH

typedef struct {
  unsigned int len;
  char *pointer;
} STRING;

#define EL_IGNORE  0
#define EL_BROKER  1
#define EL_PANEL   0x800
#define EL_DYNAMIC 0x100
#define EL_INPUT   0x200
#define EL_VISIBLE 0x400
#define EL_TYPMASK 0xf00


typedef struct {
  char *topic;
  int anz;
} SUBSCRIPTION;

typedef struct {
  unsigned int opcode;
  const char *name;
  void (*init)();
  void (*draw)();
  void (*update)();
  void (*click)();
} ELDEF;
typedef struct {
  short type;
  short x,y,w,h;
  short x2,y2;
  short linewidth;
  short revert;
  unsigned long agc;
  unsigned long bgc;
  unsigned long fgc;
  char *font;
  char *topic;
  char *text;
  char *line;
  char *filename;
  char *format;
  double min,max;
  
  SUBSCRIPTION *sub;
} ELEMENT;


typedef struct {
  char *name;
  int anzelement;
  ELEMENT *tree;
  int panelelement;
  char *buffer;
  int bufferlen;
  int isconnected;
  int anzsubscriptions;
  SUBSCRIPTION *subscriptions;
} DASH;




extern int verbose;

DASH *load_dash(const char *filename);
DASH *merge_dash(DASH *dash, const char *fname);
void free_dash(DASH *dash);
void init_dash(DASH *dash);
void close_dash(DASH *dash);


#endif