#include "util.h"
/* Version von Memdump: */


void memdump(byte *p,unsigned char len) {
  unsigned char n,i;
  while(len>0) {
    if(len>16) {n=16;len-=16;}
    else {n=len;len=0;}
    Serial.print((unsigned int)p,HEX);
    Serial.print(F(": "));
    for( i = 0; i < n; i++) {
      if (p[i] < 16) Serial.print('0');
      Serial.print(p[i], HEX);
      if (i < n-1) Serial.print(" "); 
    }
    Serial.println();
    p+=16; 
  }
}

unsigned char simple_chk(unsigned char *p,unsigned char n) {
  unsigned char ret=0;
  while(n-->0) {
    ret^=*p++;
  }
  return(ret);
}

void statistics(float *mean,float *variance,unsigned long int anz,unsigned int m,float value) {
  float d;
  if(anz>m) anz=m;
  *mean=(*mean*anz+value)/(anz+1);
  d=(value-*mean);
  *variance=(*variance*anz+d*d)/(anz+1);
}


void regression(float *x,float *y,int n, float *a, float *b) {
  /* Für die Regression und bestimmung der Dosisleistung....*/
  float sxy=0;
  float sxx=0;
  float sy=0;
  float sx=0;
  int i;
  for(i=0;i<n;i++) {
    sxy+=x[i]*y[i];
    sxx+=x[i]*x[i];
    sx+=x[i];
    sy+=y[i];
  }
  *b=(n*sxy-sx*sy)/(n*sxx-sx*sx);
  if(a) *a=sy/n-*b*sx/n;
}


long int read_voltage(int cycles,float *voltage) {
  int i;
  long int a6=0;
  for(i=0; i<cycles;i++) { 
    delay(2);
    a6+=analogRead(A6);
  }
  /*Diese Kalibration wurde mit der (eigentlich unpasssenden) Zehnerdiode vorgenommen...*/
  *voltage=4.97-((float)a6-448.00*(float)cycles)/((499.00-448.00)*(float)cycles)*(4.97-4.34);
  return(a6);
}
unsigned char get_button_state() {
  delay(10);
  return((~((digitalRead(BUTTON_DWN)<<2)|(digitalRead(BUTTON_UP)<<1)|digitalRead(BUTTON_OK)))&7);
}


void software_Reset() { // Restarts program from beginning but does not reset the peripherals and registers
  asm volatile ("  jmp 0"); 
} 
