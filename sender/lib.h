#ifndef _LIB_H_INCLUDED_
#define _LIB_H_INCLUDED_

int putc(unsigned char c);
unsigned char getc(void);
int putsubc(unsigned char c);
unsigned char getsubc(void);
int puts(unsigned char *str);
int gets(unsigned char *buf);
int putsubs(unsigned char *str);
int getsubs(unsigned char *buf);

void *memset(void *b, int c, long len);
void *memcpy(void *dst, const void *src, long len);
int memcmp(const void *b1, const void *b2, long len);
int strlen(const char *s);
char *strcpy(char *dst, const char *src);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, int len);

int puthex(unsigned long val, int column);
int dec2str(int value, char *str);

#endif
