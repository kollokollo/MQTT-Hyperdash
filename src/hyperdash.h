/* HYPERDASH.H (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#ifndef __HYPERDASH
#define __HYPERDASH

#define EL_IGNORE  0
#define EL_COMPOUND 0x2000
#define EL_BROKER   0x1000
#define EL_PANEL     0x800
#define EL_DYNAMIC   0x100
#define EL_INPUT     0x200
#define EL_VISIBLE   0x400
#define EL_TYPMASK  0x3f00

#include "subscribe.h"


typedef struct {
  unsigned int opcode;
  const char *name;
  void (*init)();
  void (*draw)();
  void (*update)();
  int (*click)();
  char * (*tostring)();
} ELDEF;
typedef struct {
  short type;
  short x,y,w,h;
  short x2,y2;
  short linewidth;
  short revert;
  unsigned long agc;
  unsigned long bgc;   /* Background color */
  unsigned long fgc;   /* Foreground color */
  char *font;
  int fontsize;
  char *topic;
  char *subtopic;
  char *text;
  char *line;
  char *filename;
  char *format;
  double min,max;
  double increment;
  double amin,amax;
  STRING label[10];
  STRING data[10];
  STRING data2[10];
  unsigned long labelcolor[10];
  int subscription;
  int fontnr;
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
extern char icondir[];
extern char bitmapdir[];
extern char fontdir[];
extern char dashboarddir[];
extern char hyperdashdir[];
extern int do_show_invisible;
extern const int anzeltyp;
extern const ELDEF eltyps[];



void hyperdash_set_defaults();
DASH *new_dash(const char *filename);
DASH *load_dash(const char *filename);
DASH *merge_dash(DASH *dash, const char *fname);
void save_dash(DASH *dash, const char *filename);
void free_dash(DASH *dash);
void init_dash(DASH *dash);
void close_dash(DASH *dash);
void draw_dash(DASH *dash, WINDOW *win);
int handle_dash(DASH *dash, WINDOW *win);
void update_topic_message(int sub,const char *, STRING message);
int find_element(DASH *dash,int st, int x, int y, unsigned int mask, unsigned int match);
char *element2a(ELEMENT *el);

void call_a_dash(char *filename);

void delete_element(DASH *dash, int idx);
void free_element(ELEMENT *el);
void add_element(DASH *dash, ELEMENT *el);
ELEMENT duplicate_element(ELEMENT *el);
void insert_element(DASH *dash, int idx, ELEMENT *el);
ELEMENT default_element(int type,unsigned long int fgc,unsigned long int bgc, const char *font);
void scale_element(ELEMENT *el,int w,int h);

int edit_element(ELEMENT *el);
void init_element(ELEMENT *el,const char *line);
void publish_element(ELEMENT *el,STRING a, int qos);
char *element_get_current_value(ELEMENT *el);

#endif
