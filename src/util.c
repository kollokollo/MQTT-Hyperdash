/* UTIL.C (c) Markus Hoffmann  */

/* This file is part of MQTT-Hyperdash, the MQTT Dashboard 
 * ============================================================
 * MQTT-Hyperdash is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <locale.h> 
#include <math.h>
#include <time.h> 
#include "basics.h"
#include "util.h"





STRING create_string(const char *n) {
  STRING ergeb;
  if(n) {
    ergeb.len=strlen(n);
    ergeb.pointer=strdup(n);
  } else {
    ergeb.len=0;
    ergeb.pointer=malloc(1);
    ergeb.pointer[0]=0;
  }
  return(ergeb);
}
STRING double_string(const STRING *a) {
  STRING b;
  b.len=a->len;
  b.pointer=malloc(b.len+1);
  memcpy(b.pointer,a->pointer,b.len);
  (b.pointer)[b.len]=0;
  return(b);
}



/* Decode hexadecimal number to int  */
static int atohex(char *n) {
  int value=0;
  while(*n) {
    value<<=4;
    if(v_digit(*n)) value+=(int)(*n-'0');
    else if(*n>='a' && *n<='f') value+=(int)(*n-'a')+10;
    else if(*n>='A' && *n<='F') value+=(int)(*n-'A')+10;
    n++;
  }
  return(value);
}



static int atobin(char *n) {
  int value=0;
  while(*n) {
    value<<=1;
    if(*n!='0') value++;  
    n++;
  }
  return(value);
}


/* 
Wandelt einen String mit einer (floating-point) Zahl in einen double 
um.

Diese funktion muss stark Geschwindigkeitsoptimiert sein
TODO: Hier koennte man noch einen Flag zur체ckliefern, ob es ein real oder imaginaerteil ist.
*/
double myatof(char *n) {
  double sign=1.0;
  if(!n || *n==0) return(0);
  while (w_space(*n) ) n++;  /* Skip leading white space, if any. */
  if(*n=='-') { /*  Get sign, if any.  */
    sign=-1.0;
    n++;
  } else if(*n=='+') n++;
  /* try special codings  */
  if(*n=='$') return(sign*(double)atohex(++n));
  if(*n=='%') return(sign*(double)atobin(++n));
  if(*n=='0' && (n[1]&0xdf)=='X') return(sign*(double)atohex(n+2));
  if(*n=='N') return(NAN);
  /* Get digits before decimal point or exponent, if any. */
  double value=0.0;
  for(;v_digit(*n); n++) value=value*10.0+(*n-'0');
  /* Get digits after decimal point, if any. */
  if(*n=='.') {
    double pow10 = 10.0;
    n++;
    while(v_digit(*n)) {
      value+=(*n-'0')/pow10;
      pow10*=10.0;
      n++;
    }
  }
  /* Handle exponent, if any. */
  if((*n&0xdf)=='E') {
    int f=0;
    double scale=1.0;
    unsigned int ex=0; 
    n++;

    /* Get sign of exponent, if any. */
    if(*n=='-') {
      f=1;
      n++;
    } else if(*n=='+') n++;
    /* Get digits of exponent, if any. */
    for(;v_digit(*n); n++) ex=ex*10+(*n-'0');
    if(ex>308) ex=308;
    /* Calculate scaling factor. */
    while(ex>= 64) { scale *= 1E64; ex-=64; }
    while(ex>=  8) { scale *= 1E8;  ex-=8; }
    while(ex>   0) { scale *= 10.0; ex--; }

    /* Return signed and scaled floating point result. */
    return sign*(f?(value/scale):(value*scale));
  }
  /* Return signed floating point result. */
  return(sign*value);
}


/* Fuer den Teil nach dem Komma und fuer Exponenten*/
static void xfill(char *p,const char *q,char c, int n) {
  while(*p && n--) {
    if(*p==c) {
      if(*q) *p=*q++;
      else if(c=='#') *p='0';
    }
    p++;
  }
}
/*Fuer den Teil vor dem Komma*/
static void xfillx(char *p,const char *q,int n) {
  char c=0,c2;
  char f=' ';
  while(*p && n--) {
    c2=c;
    c=*q;
    switch(*p) {
    case '$':
      if(c==' ') {
        int i=1;
	while(*(p+i)==',') i++;
        if(*(p+i)=='$' && *(q+1)==' ') *p=f;
        q++;
      } else if(c) {*p=c;q++;}
      p++;
      break;
    case '*':
      if(c==' ') {*p=f='*';q++;}
      else if(c) {*p=c;q++;}
      p++;
      break;
    case '0':
    case '%':
      if(c==' ') {*p=c='0';q++;f=' ';}
      else if(c) {*p=c;q++;}
      p++;
      break;
    case '#':
      if(c) {*p=c;q++;f=' ';}
      p++;
      break;
    case ',':
      if(c2==' ') *p=f;
      p++;
      break;
    default:
      p++;
    }
  }
}

/* print 0 using "+#.###^^^^"   */
STRING do_using(double num,STRING format) {
  STRING dest;
  int a=0,b=0,p,r=0,i,j,ex=0,v=0; 
  int neg;
  char des[32+format.len];
#ifndef WINDOWS
locale_t safe_locale = newlocale(LC_NUMERIC_MASK, "C", duplocale(LC_GLOBAL_LOCALE));
locale_t old = uselocale(safe_locale);
#endif
  if(*format.pointer=='%') { /* c-style format */
    char b[32];
    snprintf(b,sizeof(b),format.pointer,num);
    dest.len=strlen(b);
    dest.pointer=strdup(b);
  } else { /* basic-style format */
    dest=double_string(&format);
    
   /* Zaehle die Rauten vor dem Punkt */
   p=0;
   while((format.pointer)[p] && (format.pointer)[p]!='.') {
     if((format.pointer)[p]=='#' || 
        (format.pointer)[p]=='%' || 
        (format.pointer)[p]=='$' || 
        (format.pointer)[p]=='0' || 
	(format.pointer)[p]=='*') r++;
     p++;
   }
   /* Zaehle die Rauten nach dem Punkt */
   while((format.pointer)[p]) {
     if((format.pointer)[p++]=='#') a++;
   }
   /* Zaehle platzhalter f체r Exponentialdarstellung */
   p=0;
   while(p<format.len) {
     if((format.pointer)[p++]=='^') ex++;
   }
   /* Zaehle platzhalter f체r vorzeichen */
   p=0;
   while(p<format.len) {
     if((format.pointer)[p]=='+' || (format.pointer)[p]=='-') v++;
     p++;
   }
   neg=(num<0); 
   num=fabs(num);

   /* Vorzeichen als erstes: */
   for(i=0;i<dest.len;i++) {
     if(format.pointer[i]=='+') {dest.pointer[i]=(neg ? '-':'+');}
     else if(format.pointer[i]=='-') {dest.pointer[i]=(neg ? '-':' ');}
   }
   
   if(ex>2) {
     snprintf(des,sizeof(des),"%16.16e",num); 
     j=i=0;
     while(des[i] && des[i]!='e') i++;
     while(dest.pointer[j] && dest.pointer[j]!='^') j++;
     des[i++]=0;
     dest.pointer[j++]='e';   /*e */
     while(dest.pointer[j] && dest.pointer[j]!='^') j++;
     dest.pointer[j++]=des[i++];   /* +*/
     while(dest.pointer[j] && dest.pointer[j]!='^') j++;
     int l=strlen(&des[i])+2;
     while(l<ex) {
       dest.pointer[j++]='0'; 
       while(dest.pointer[j] && dest.pointer[j]!='^') j++;
       l++;
     }
     if(l>ex) {
       for(i=0;i<dest.len;i++) dest.pointer[i]='*';
       return(dest);    
     } else xfill(dest.pointer,&des[i],'^',dest.len);
   } else snprintf(des,sizeof(des),"%16.16f",num);
     /*Jetzt muss die Zahl gerundet werden.*/
     num=myatof(des)+pow(0.1,a)*0.5;
     snprintf(des,sizeof(des),"%16.16f",num);

    /*Hierzu brauchen wir die Anzahl der tatsaechlichen STellen vor dem Komma*/
    int count=0;
    i=0;
    while(des[i] && des[i]!='.') {
      if(des[i]>='0' && des[i]<='9') count++;
      i++;
    }
    i=0;
    while(des[i] && des[i]!='.') i++; 
    j=0;
    while(dest.pointer[j] && dest.pointer[j]!='.') j++;
    if(dest.pointer[j]) {
      if(des[i]) {
        des[i]=0;
        xfill(dest.pointer+j+1,&des[i+1],'#',dest.len-j-1);
      } else xfill(dest.pointer+j+1,"0000000000000000",'#',dest.len-j-1);
    }
    
    b=0;
    /*Jetzt noch Leerzeichen am Anfang entfernen und ggf minus einf체gen.*/
    
    char p[strlen(des)+1+1];
    char *p2=des;
    // printf("des=<%s>\n",des);
    if(neg && !v) p[b++]='-';
    neg=0;
    
    while(*p2 && *p2!='.') {
      if(*p2!=' ') p[b++]=*p2;
      p2++;
    }
    p[b]=0;
    if(b==r) xfillx(dest.pointer,p,dest.len);
    else if(b<r) {
      char buf[r+1];
      for(i=0;i<r-b;i++)  buf[i]=' ';
      for(i=r-b;i<r;i++)  buf[i]=p[i-(r-b)];
      buf[r]=0;  
      xfillx(dest.pointer,buf,dest.len);
    } else {
      for(i=0;i<dest.len;i++) dest.pointer[i]='*';    
    }
  } 
#ifndef WINDOWS
  uselocale(old);
  freelocale(safe_locale);
#endif
  return(dest);
}

char *key_value(const char *a, const char *b, const char *def) {
  static char value[256]; 
  char par[256];
  char kv[256];
  char key[256];
  char val[256];
  strncpy(par,a,sizeof(par));
  int e=wort_sep(par,' ',1,kv,par);
  int e2;
  while(e>0) {
    e2=wort_sep(kv,'=',1,key,val);
    if(e2==2) {
       if(!strcmp(key,b)) {
         if(val[0]=='\"') {
	   if(strlen(val)>0) val[strlen(val)-1]=0;
	   strncpy(value,val+1,sizeof(value));
	 } else strncpy(value,val,sizeof(value));
	 return(value);
       }
    }  
    e=wort_sep(par,' ',1,kv,par);
  }
  strncpy(value,def,sizeof(value));
  return(value);
}







/* Diese Funktion gibt zurueck
   0 Wenn der String leer ist
     w1$=w2$=""
   1 wenn der String t$ nur aus einem Wort
     besteht. w2$="" w1$=t$
   2 wenn der String t$ aus mehreren Worten
     besteht. w1$=erstes Wort
     w2$= Rest von t$
  ##########################################
  der String in t wird nicht veraendert,
  w1 und w2 sind Stringbuffer, die mindestens so gross wie t sein muessen
  
   Befindet sich ein Teil des Strings in 
   Anf갿rungszeichen ("), so wird dieser Teil
   nicht ber갷ksichtigt.
   wird klam! mit True angegeben, wird
   auch der Inhalt von Klammern () nicht ber갷ksichtigt

   Die W봱ter werden als solche erkannt, wenn
   sie durch das Zeichen c getrennt sind.

   wort_sep2 hat einen string als separator
   wort_sepr und wort_sepr2 suchen rueckwaerts

   (c) Markus Hoffmann 1997

   2012: memories now may overlap and t==w2 or t==w1 is allowed.


  */


int wort_sep(const char *t,char c,int klamb ,char *w1, char *w2)    {
  int f=0, klam=0;

  /* klamb=0 : keine Klammern werden beruecksichtigt
     klamb=1 : normale Klammern werden beruecksichtigt
     klamb=2 : eckige Klammern werden beruecksichtigt
     klamb=4 : geschweifte Klammern werden beruecksichtigt
  */

  if(!(*t)) return(*w1=*w2=0);

  while(*t && (*t!=c || f || klam>0)) {
    if(*t=='"') f=!f;
    else if(!f && (((klamb&1) && *t=='(') || ((klamb&2) && *t=='[') || ((klamb&4) && *t=='{'))) klam++;
    else if(!f && (((klamb&1) && *t==')') || ((klamb&2) && *t==']') || ((klamb&4) && *t=='}'))) klam--;
    *w1++=*t++;
  }
  if(!(*t)) {
    *w2=*w1=0;
    return(1);
  } else {
    *w1=0;
    ++t;
    while(*t) *w2++=*t++;
    *w2=0; 
    return(2);
  }
}






/*  (c) Markus Hoffmann
  ' Teile in Anf갿rungszeichen werden nicht ver꼗dert
  ' Ersetzt Tabulator durch Leerzeichen
  ' mehrfache Leerzeichen zwischen W봱tern werden entfernt
  ' wenn fl&=1 ,  gib Gro얺uchstaben zur갷k
 */
 
void xtrim(const char *t,int f, char *w ) {
  register int a=0,u=0,b=0;

  while(*t) {
    while(*t && (!isspace(*t) || a)) {
      if(*t=='"') a=!a;
      u=1; if(b==1) {*w++=' '; b=0;}
      if(f && !a) *w++=toupper(*t++); else *w++=*t++;
    }
    if(u && !b) b=1;
    if(*t) t++;
  } *w=0;
}

char *date_string() {
  static char ergebnis[12];
  time_t timec;
  struct tm * loctim;
  timec = time(&timec);
  loctim=localtime(&timec);
  sprintf(ergebnis,"%02d.%02d.%04d",loctim->tm_mday,loctim->tm_mon+1,1900+loctim->tm_year);
  return(ergebnis);
}
char *time_string() {
  static char ergebnis[10];
  time_t timec;
  timec = time(&timec);
  strncpy(ergebnis,ctime(&timec)+11,8);
  ergebnis[8]=0;
  return(ergebnis);
}


static char i2abuf[128];

static char *i2a(unsigned i, char *a, unsigned r) {
  if(i/r>0) a=i2a(i/r,a,r);
  *a = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz@$!?"[i % r];
  return a+1;
}

char *tohex(unsigned i) {
  int j;
  static int f=0;
  char *p=i2abuf+f*32;
  
  for(j=0;j<16;j++) p[j]='0';
  *i2a(i,p,16)=0;
  f++;
  f=f&3;
  return(p);
}

/* Remove leading and trailing "  */

int declose(char *c) {
  if(!c) return(0);
  int l=strlen(c);
  if(l<2) return(0);
  if(*c=='\"' && c[l-1]=='\"') {
    memmove(c,c+1,l-2);
    c[l-2]=0;
    return(1);
  }
  return(0);
}


/* Simple JSON parser to get the value of a first-level key */

STRING json_get_value(char *key, STRING pl) {
  STRING ret;
  ret.pointer=NULL;
  ret.len=0;
  if(!key || !pl.pointer || pl.len<=0) return(ret);
  int j;
  char a;
  int level=0;
  int flag=0;
  int k=0;
  char buffer[pl.len+1];

  /* better format the payload */
  for(j=0;j<pl.len;j++) {
    a=pl.pointer[j];
    if(a=='\"') {flag=!flag;buffer[k++]=a;}
    else if(!flag && a=='{') {level++;if(level>1) buffer[k++]=a;}
    else if(!flag && a=='}') {level--;if(level>0) buffer[k++]=a;}
    else if(level>=1 && k<pl.len && a!='\r' && a!='\n' && (flag || (a!='\t' && a!=' '))) buffer[k++]=a;
  }
  buffer[k]=0;

  /* Now evaluate key-value pairs */
  
  char aa[k+1];
  char b[k+1];
  char c[k+1];
  int e=wort_sep(buffer,',',2|4,aa,b);
  while(e) {
    wort_sep(aa,':',0,aa,c);
    xtrim(aa,0,aa);
    xtrim(c,0,c);
    declose(aa);
    declose(c);
    if(!strcmp(key,aa)) {
      ret.pointer=strdup(c);
      ret.len=strlen(c);
      return(ret);
    }
    e=wort_sep(b,',',2|4,aa,b);
  }
  return(ret);
}
/* Simple JSON parser to replace the value of a first-level key with another 
   the new value nv must be enclosed if necessary
 */

STRING json_replace_value(char *key, STRING pl, char *nv) {
  STRING ret;
  ret.pointer=NULL;
  ret.len=0;
  if(!pl.pointer || pl.len<=0) return(ret);
  if(!key || !*key || !nv) return(double_string(&pl));
  ret.pointer=malloc(pl.len+strlen(nv)+1);
  int j;
  char a;
  int level=0;
  int flag=0;
  int k=0;
  char buffer[pl.len+1];
  /* better format the payload */
  for(j=0;j<pl.len;j++) {
    a=pl.pointer[j];
    if(a=='\"') {flag=!flag;buffer[k++]=a;}
    else if(!flag && a=='{') {level++;if(level>1) buffer[k++]=a;}
    else if(!flag && a=='}') {level--;if(level>0) buffer[k++]=a;}
    else if(level>=1 && k<pl.len && a!='\r' && a!='\n' && (flag || (a!='\t' && a!=' '))) buffer[k++]=a;
  }
  buffer[k]=0;

  /* Now evaluate key-value pairs */
  sprintf(ret.pointer,"{");
  char aa[k+1];
  char b[k+1];
  char c[k+1];
  int e=wort_sep(buffer,',',2|4,aa,b);
  while(e) {
    wort_sep(aa,':',0,aa,c);
    xtrim(aa,0,aa);
    declose(aa);
    if(!strcmp(key,aa)) {
      /* Find out if the value need to be enclosed (stupid hack!)*/
      xtrim(c,0,c);
      int do_enclose=declose(c);
      if(do_enclose) sprintf(ret.pointer+strlen(ret.pointer),"\"%s\":\"%s\"",key,nv);
      else sprintf(ret.pointer+strlen(ret.pointer),"\"%s\":%s",key,nv);
    } else {
      sprintf(ret.pointer+strlen(ret.pointer),"\"%s\":%s",aa,c);
    }
    if(e==2) sprintf(ret.pointer+strlen(ret.pointer),", ");
    e=wort_sep(b,',',2|4,aa,b);
  }
  sprintf(ret.pointer+strlen(ret.pointer),"}");
  ret.len=strlen(ret.pointer);
  return(ret);
}
