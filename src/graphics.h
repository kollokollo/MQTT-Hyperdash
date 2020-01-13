/* GRAPHICS.H (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
//#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

typedef struct {
  int x,y;
  unsigned int w,h,b,d;
  unsigned int fcolor,bcolor;
  char *title;
  char *info;
  unsigned long flags;
  unsigned short chw,chh,baseline;
  SDL_Surface *display;
} WINDOW;

typedef struct {
  char *name;
  int anz;
  int size;
  int height;
  TTF_Font *font;
} FONTDEF;

extern FONTDEF fonts[];
extern int anzfonts;

STRING pngtobmp(unsigned char *pngdata,size_t pngsize);
WINDOW *create_window(const char *title, const char* info,int x,int y,unsigned int w,unsigned int h, int);
void close_window(WINDOW *win);
int waitmouse(WINDOW *window);
int mouseevent(WINDOW *window, int *x, int *y, int *b, int *s);
void put_bitmap(WINDOW *window, const char *adr,int x,int y,unsigned int w, unsigned int h, unsigned long int fgc);
void put_graphics(WINDOW *window, STRING data,int x,int y,unsigned int w, unsigned int h, unsigned long int tgc);

int add_font(const char *name, int size);
void open_all_fonts();
void clear_all_fonts();
void put_font_text(SDL_Surface *display, int fidx, char *text, int x,int y,unsigned long int fgc, int h);
void put_font_long_text(SDL_Surface *display, int fidx, char *text, int x,int y,unsigned long int fgc, int w, int h);
STRING get_icon(const char *name, int *w, int *h);
STRING get_bitmap(const char *name, int *w, int *h);
