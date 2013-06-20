#include "defines.h"
#include "serial.h"
#include "lib.h"

/*Send 1 character*/
int putc(unsigned char c){
  if(c == '\n')  serial_send_byte(SERIAL_DEFAULT_DEVICE,'\r');
  return serial_send_byte(SERIAL_DEFAULT_DEVICE,c);
}

/*Send 1 string*/
int puts(unsigned char *str){
  while(*str) putc(*(str++));
  return 0;
}

int putsubc(unsigned char c){
  if(c == '\n')  serial_send_byte(SERIAL_SUB_DEVICE,'\r');
  return serial_send_byte(SERIAL_SUB_DEVICE,c);
}

/*Send 1 string*/
int putsubs(unsigned char *str){
  while(*str) putsubc(*(str++));
  return 0;
}

/*recv 1 character*/
unsigned char getc(void){
  unsigned char c = serial_recv_byte(SERIAL_DEFAULT_DEVICE);
  c = (c == '\r') ? '\n' : c;
  putc(c); //Echoback
  return c;
}

/*Recv 1 string*/
int gets(unsigned char *buf){
  int i = 0;
  unsigned char c;

  do{
    c = getc();
    if(c == '\n') c = '\0';
    buf[i++] = c;
  }
  while(c);
  return i - 1;
}

/*recv 1 character*/
unsigned char getsubc(void){
  unsigned char c = serial_recv_byte(SERIAL_SUB_DEVICE);
  c = (c == '\r') ? '\n' : c;
  putsubc(c); //Echoback
  return c;
}

/*Recv 1 string*/
int getsubs(unsigned char *buf){
  int i = 0;
  unsigned char c;

  do{
    c = getsubc();
    if(c == '\n') c = '\0';
    buf[i++] = c;
  }
  while(c);
  return i - 1;
}

/*define memset*/
void *memset(void *b, int c, long len){
  char *p;
  for(p = b;len > 0;len--)
    *(p++) = c;
  return b;
}

/*define memory copy*/
void *memcpy(void *dst, const void *src, long len){
  char *d = dst;
  const char *s = src;
  for(; len > 0; len--)
    *(d++) = *(s++);
  return dst;
}

/* define memory compair*/
int memcmp(const void *b1, const void *b2, long len){
  const char *p1 = b1, *p2 = b2;
  for(; len > 0; len--){
    if(*p1 != *p2)
      return (*p1 > *p2) ? 1 : -1;
    p1++;
    p2++;
  }
  return 0;
}

/*define strlen*/
int strlen(const char *s){
  int len;
  for(len = 0; *s; s++,len++);
  return len;
}

/*define strcpy*/
char *strcpy(char *dst, const char *src){
  char *d = dst;
  for(;; dst++, src++){
    *dst = *src;
    if(!*src) break;
  }
  return d;
}

/*define strcmp*/
int strcmp(const char *s1, const char *s2){
  while(*s1 || *s2){
    if(*s1 != *s2)
      return (*s1 > *s2) ? 1 : -1;
    s1++;
    s2++;
  }
  return 0;
}

/*define strncmp*/
int strncmp(const char *s1, const char *s2, int len){
  while((*s1 || *s2) && (len > 0)) {
    if(*s1 != *s2)
      return (*s1 > *s2) ? 1 : -1;
    s1++;
    s2++;
    len--;
  }
  return 0;
}

/* output hex for serial-term */
int puthex(unsigned long value, int column){
  char buf[9];
  char *p;

  p = buf + sizeof(buf) -1;
  *(p--) = '\0';

  if(!value && !column) column++;

  while(value || column){
    *(p--) = "0123456789abcdef"[value & 0xf]; //exchange hex -> buffer
    value >>= 4; //shift 4bits(next)
    if(column) column--;
  }

  puts(p + 1);

  return 0;
}

int dec2str(int value, char str[4]) {
	int i,cur=0;
	for(i=0;i<4;i++) str[i] = 0x20;
	
	if(value == 0) {str[0] = '0'; return 0;}
	else if(value<10)  {cur = 2; goto one;}
	else if(value<100) {cur = 1; goto ten;}
	
	for(i=1;i<=10;i++) {
		if(value<(i*100)) {
			value -= (i-1)*100;
			str[0] = 0x30 + (i-1);
			if(value == 0) {
				str[1] = '0';
				str[2] = '0';
				return 0;
			}
			break;
		}
	}
	
	ten:
	for(i=1;i<=10;i++) {
		if(value<(i*10)) {
			value -= (i-1)*10;
			str[1-cur] = 0x30+ (i-1);
			if(value == 0) {
				str[2-cur] = '0';
				return 0;
			}
			break;
		}
	}
	
	one:
	for(i=1;i<=9;i++) {
		if(value == i) {
			str[2-cur] = 0x30+ i;
			break;
		}
	}
	
	return 0;
}

