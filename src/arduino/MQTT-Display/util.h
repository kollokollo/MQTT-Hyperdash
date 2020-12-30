/* Pin Belegung der Buttons */


#define BUTTON_UP A3
#define BUTTON_OK A4
#define BUTTON_DWN A5

/* Bit Maske für Button state */

#define B_OK 1
#define B_UP 2
#define B_DWN 4




#define wait_any_button() {  while(!get_button_state()) ; }

void statistics(float *mean,float *variance,unsigned long int anz, unsigned int m,float value);


void memdump(byte *p,unsigned char len);
unsigned char simple_chk(unsigned char *p,unsigned char n);
void regression(float *x,float *y,int n, float *a, float *b);
long int read_voltage(int cycles,float *voltage);

unsigned char get_button_state();
void software_Reset();
