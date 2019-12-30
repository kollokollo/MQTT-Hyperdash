/* HYPERDASH.H (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#ifndef __HYPERDASH
#define __HYPERDASH

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
  STRING last_value;
} SUBSCRIPTION;

typedef struct {
  unsigned int opcode;
  const char *name;
  void (*init)();
  void (*draw)();
  void (*update)();
  int (*click)();
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
  int fontsize;
  char *topic;
  char *text;
  char *line;
  char *filename;
  char *format;
  double min,max;
  double amin,amax;
  STRING label[10];
  STRING data[10];
  unsigned long labelcolor[10];
  int subscription;
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

DASH *load_dash(const char *filename);
DASH *merge_dash(DASH *dash, const char *fname);
void free_dash(DASH *dash);
void init_dash(DASH *dash);
void close_dash(DASH *dash);
void draw_dash(DASH *dash, WINDOW *win);
int handle_dash(DASH *dash, WINDOW *win);


/* Here the definitions of the individual element functions: */

void i_meter(ELEMENT *el,char *pars);
void d_meter(ELEMENT *el,WINDOW *win);
void u_meter(ELEMENT *el,WINDOW *win, char *message);

void d_tvmeter(ELEMENT *el,WINDOW *win);
void u_tvmeter(ELEMENT *el,WINDOW *win, char *message);
void d_thmeter(ELEMENT *el,WINDOW *win);
void u_thmeter(ELEMENT *el,WINDOW *win, char *message);


#endif
