/* ELEMENTS.H (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
 
 
 
 
/* Here the definitions of the individual element functions: */

void i_broker(ELEMENT *el,char *pars);
char *s_broker(ELEMENT *el);

void i_panel(ELEMENT *el,char *pars);
void d_panel(ELEMENT *el,WINDOW *win);
int  c_panel(ELEMENT *el,WINDOW *win,int x, int y, int b);
char *s_panel(ELEMENT *el);

void i_compound(ELEMENT *el,char *pars);
void d_compound(ELEMENT *el,WINDOW *win);
char *s_compound(ELEMENT *el);

void i_bar(ELEMENT *el,char *pars);
char *s_bar(ELEMENT *el);

void i_bitmap(ELEMENT *el,char *pars);
void d_bitmap(ELEMENT *el,WINDOW *win);
char *s_bitmap(ELEMENT *el);

void i_bitmaplabel(ELEMENT *el,char *pars);
void d_bitmaplabel(ELEMENT *el,WINDOW *win);
void u_bitmaplabel(ELEMENT *el,WINDOW *win, char *message, int);
char *s_bitmaplabel(ELEMENT *el);

void i_scmdlabel(ELEMENT *el,char *pars);
void u_scmdlabel(ELEMENT *el,WINDOW *win, char *message, int);
char *s_scmdlabel(ELEMENT *el);

void i_box(ELEMENT *el,char *pars);
void d_box(ELEMENT *el,WINDOW *win);
char *s_box(ELEMENT *el);

void d_circle(ELEMENT *el,WINDOW *win);

void i_frame(ELEMENT *el,char *pars);
void d_frame(ELEMENT *el,WINDOW *win);
int  c_frame(ELEMENT *el,WINDOW *win,int x, int y, int b);
char *s_frame(ELEMENT *el);

void i_framelabel(ELEMENT *el,char *pars);
void d_framelabel(ELEMENT *el,WINDOW *win);
void u_framelabel(ELEMENT *el,WINDOW *win, char *message, int);
char *s_framelabel(ELEMENT *el);

void d_hbar(ELEMENT *el,WINDOW *win);
void u_hbar(ELEMENT *el,WINDOW *win, char *message, int);

void d_vbar(ELEMENT *el,WINDOW *win);
void u_vbar(ELEMENT *el,WINDOW *win, char *message, int);

void i_icon(ELEMENT *el,char *pars);
void d_icon(ELEMENT *el,WINDOW *win);
char *s_icon(ELEMENT *el);

void i_line(ELEMENT *el,char *pars);
void d_line(ELEMENT *el,WINDOW *win);
char *s_line(ELEMENT *el);


void i_pbox(ELEMENT *el,char *pars);
void d_pbox(ELEMENT *el,WINDOW *win);
char *s_pbox(ELEMENT *el);

void d_pcircle(ELEMENT *el,WINDOW *win);

void i_string(ELEMENT *el,char *pars);
void d_string(ELEMENT *el,WINDOW *win);
char *s_string(ELEMENT *el);


void i_tstring(ELEMENT *el,char *pars);
void d_tstring(ELEMENT *el,WINDOW *win);
void u_tstring(ELEMENT *el,WINDOW *win,char *message, int);
char *s_tstring(ELEMENT *el);

void i_tnumber(ELEMENT *el,char *pars);
void d_tnumber(ELEMENT *el,WINDOW *win);
void u_tnumber(ELEMENT *el,WINDOW *win, char *message, int);
char *s_tnumber(ELEMENT *el);

void i_timage(ELEMENT *el,char *pars);
void u_timage(ELEMENT *el,WINDOW *win, char *message, int);
char *s_timage(ELEMENT *el);



void i_textarea(ELEMENT *el,char *pars);
void d_textarea(ELEMENT *el,WINDOW *win);
void u_textarea(ELEMENT *el,WINDOW *win, char *message, int);
char *s_textarea(ELEMENT *el);

void i_textlabel(ELEMENT *el,char *pars);
void d_textlabel(ELEMENT *el,WINDOW *win);
void u_textlabel(ELEMENT *el,WINDOW *win, char *message, int);
char *s_textlabel(ELEMENT *el);

void i_tinarea(ELEMENT *el,char *pars);
void i_tinstring(ELEMENT *el,char *pars);
void i_tinnumber(ELEMENT *el,char *pars);
void i_meter(ELEMENT *el,char *pars);
void d_meter(ELEMENT *el,WINDOW *win);
void u_meter(ELEMENT *el,WINDOW *win, char *message, int);
char *s_meter(ELEMENT *el);

void d_tvmeter(ELEMENT *el,WINDOW *win);
void u_tvmeter(ELEMENT *el,WINDOW *win, char *message, int);
void d_thmeter(ELEMENT *el,WINDOW *win);
void u_thmeter(ELEMENT *el,WINDOW *win, char *message, int);

void i_shellcmd(ELEMENT *el,char *pars);
int  c_shellcmd(ELEMENT *el,WINDOW *win,int x, int y, int b);
char *s_shellcmd(ELEMENT *el);

void i_subdash(ELEMENT *el,char *pars);
int  c_subdash(ELEMENT *el,WINDOW *win,int x, int y, int b);
char *s_subdash(ELEMENT *el);

void i_tticker(ELEMENT *el,char *pars);
int  c_tticker(ELEMENT *el,WINDOW *win,int x, int y, int b);
char *s_ticker(ELEMENT *el);

void d_subscribe(ELEMENT *el,WINDOW *win);
void i_scaler(ELEMENT *el,char *pars);
char *s_scaler(ELEMENT *el);
void d_hscaler(ELEMENT *el,WINDOW *win);
void d_vscaler(ELEMENT *el,WINDOW *win);
void u_hscaler(ELEMENT *el,WINDOW *win, char *message, int);
void u_vscaler(ELEMENT *el,WINDOW *win, char *message, int);
int  c_hscaler(ELEMENT *el,WINDOW *win,int x, int y, int b);
int  c_vscaler(ELEMENT *el,WINDOW *win,int x, int y, int b);
int  c_tinarea(ELEMENT *el,WINDOW *win,int x, int y, int b);
char *s_tinarea(ELEMENT *el);
int  c_tinnumber(ELEMENT *el,WINDOW *win,int x, int y, int b);
char *s_tinnumber(ELEMENT *el);
int  c_tinstring(ELEMENT *el,WINDOW *win,int x, int y, int b);
char *s_tinstring(ELEMENT *el);
void i_plot(ELEMENT *el,char *pars);
void d_plot(ELEMENT *el,WINDOW *win);
void u_plot(ELEMENT *el,WINDOW *win, char *message, int);
char *s_plot(ELEMENT *el);

void draw_invisible_element(ELEMENT *el,WINDOW *win);
