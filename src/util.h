/* UTIL.H (c) Markus Hoffmann  */
/* Erweiterungen fuer die Datei Ein- und Ausgabe ....   */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */




void xtrim(const char *t,int f, char *w );
int wort_sep(const char *t,char c,int klamb ,char *w1, char *w2);
char *key_value(const char *a, const char *b, const char *def);
STRING do_using(double num,STRING format);
#define w_space(c) ((c) == ' ' || (c) == '\t')
#define v_digit(c) ((c) >= '0' && (c) <= '9')
double myatof(char *n);
STRING create_string(const char *n);
STRING double_string(const STRING *a);
char *date_string();
char *time_string();
char *tohex(unsigned i);
