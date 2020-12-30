
#include "OLED.h"
SSD1306AsciiWire oled;


const byte Logo[128] PROGMEM = {
    0xff,0x81,0x81,0x81,0x81,0x81,0x81,0xff, 
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
    0x0C,0x0C,0x0C,0x1C,0x1C,0x18,0x18,0x38, 
    0x30,0x70,0xE0,0xC0,0x80,0x00,0x00,0x00, // Row 0
    
    0x00,0xE0,0xF8,0x3E,0x0F,0x03,0x01,0x00,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x80,0xC0,0xC0,0x40,0x60,0x20,0x20,
    0x20,0x20,0x20,0xE1,0x23,0x27,0xFE,0xF8, // Row 1
    
    0x00,0x1F,0x3F,0xF2,0xC2,0x8E,0x3F,0xE3,
    0x81,0x81,0x80,0x80,0x80,0xB8,0xD8,0x61,
    0x1F,0x01,0x03,0x0E,0x18,0x36,0x2E,0x20,
    0xA0,0xB0,0xD8,0xEF,0x70,0x38,0x1F,0x07, // Row 2
    
    0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x06,
    0x0E,0x0D,0x0D,0x1D,0x3D,0x78,0x70,0x30,
    0x3E,0x1E,0x18,0x18,0x1F,0x0F,0x0C,0x0C,
    0x07,0x07,0x07,0x00,0x00,0x00,0x00,0x00}; // Row 3

void show_logo() {
  byte r = 0; // Start row - Modify as needed
 byte c = 0; // Start col - Modify as needed
 byte a = 0; // Position in array - Don't change - an array larger than 256 will need to use "int = a"

  for (byte b = 0; b < 4; b++) {
    oled.setCursor (c,(r+b)); 
    for (byte i = 0; i < 32; i++) {
      oled.ssd1306WriteRam(pgm_read_byte(&Logo[a]));
      a++;
     }  
   } 
}

/* value is 0 to 1000*/

void bargraph(byte x, byte y, byte w,int val) {
  oled.setCursor (x,y);
  for (int i=0; i<w; i++) {
      if(i==0 || i==w-1) oled.ssd1306WriteRam(0xff);
      else if((long)i*1000/w<val) oled.ssd1306WriteRam(0xbd);
      else oled.ssd1306WriteRam(0x81);
  }  
}

void bargraph_s(byte x, byte y,byte w,int val) {
  oled.setCursor (x,y);
  for (int i = 0; i < w; i++) {
      if(i==0 || i==w-1 || i==w/2) oled.ssd1306WriteRam(0xff);
      else if(i>w/2 && (long int)(2*i-w)*1000<(long int)val*w) oled.ssd1306WriteRam(0xbd);
      else if(i<w/2 && (long int)(2*i-w)*1000>(long int)val*w) oled.ssd1306WriteRam(0xbd);
      else oled.ssd1306WriteRam(0x81);
  }  
}




const byte Stati[8*12] PROGMEM = {
  0x7f,0x41,0x41,0x41,0x41,0x41,0x7f,0x00,  /* 0 Frame */
  0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x00,  /* 1 Box */
  0x18,0x24,0x42,0x81,0x81,0x42,0x24,0x18,  /* 2 Raute */
  0x18,0x3c,0x7e,0xff,0xff,0x7e,0x3c,0x18,  /* 3 FRaute */
  0x3c,0x66,0xc3,0x81,0x81,0xc3,0x66,0x3c,  /* 4 Circle */
  0x3c,0x7e,0xff,0xff,0xff,0xff,0x7e,0x3c,  /* 5 PCircle */
//  0x40,0x70,0x4c,0x43,0x4c,0x70,0x40,0x00,  /* UP */
  0x40,0x70,0x7c,0x7f,0x7c,0x70,0x40,0x00,  /* 6 UP filled */
  0x01,0x07,0x1f,0x7f,0x1f,0x07,0x01,0x00,  /* 7 DWN filled */
  0x7f,0x3e,0x3e,0x1c,0x1c,0x08,0x08,0x00,  /* 8 RIGHT filled */
  0x08,0x08,0x1c,0x1c,0x3e,0x3e,0x7f,0x00,  /* 9 LEFT filled */
  0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,  /* 10 dot */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00   /* 11 nothing */
};
void status_indicator(byte x, byte y,byte typ) {
  oled.setCursor (x,y);
  for (byte i = 0; i < 8; i++) {
    oled.ssd1306WriteRam(pgm_read_byte(&Stati[typ*8+i]));
  }

}



void OLED_memdump(byte *p,unsigned char len) {
  unsigned char n,i;
  while(len>0) {
    if(len>16) {n=16;len-=16;}
    else {n=len;len=0;}
   // oled.print((unsigned int)p,HEX);
   // oled.print(F(": "));
    oled.print(F("ID:  "));
    for( i = 0; i < n; i++) {
      if (p[i] < 16) oled.print('0');
      oled.print(p[i], HEX);
    //  if (i < n-1) oled.print(" "); 
    }
    oled.println();
    p+=16; 
  }
}

/* full 0 bis 100 Prozent */
void bat_status(unsigned char full) {
  unsigned char i;
  oled.setCursor (128-16-1,0);
  oled.ssd1306WriteRam(0x3c);
  oled.ssd1306WriteRam(0x42);
  for(i=0;i<11;i++) {
    if(i*10<full) oled.ssd1306WriteRam(0x5a);
    else oled.ssd1306WriteRam(0x42);
  }
  oled.ssd1306WriteRam(0x42);
  oled.ssd1306WriteRam(0x3c);
  oled.ssd1306WriteRam(0x18);
}
void OLED_setup() {
  Wire.begin();
  Wire.setClock(400000L);
  #if RST_PIN >= 0
    oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
  #else // RST_PIN >= 0
    oled.begin(&Adafruit128x32, I2C_ADDRESS);
  #endif // RST_PIN >= 0
  oled.setFont(Adafruit5x7);
  uint32_t m = micros();
 // oled.setContrast(0); 
  oled.clear();
}
