#include "h8_69.h"

static void udelay(int usec)
{
  volatile int i;
  for (i = 0; i < 20; i++) /* 20MHz */
    ;
}

static void mdelay(int msec)
{
  volatile int i;
  for (i = 0; i < msec; i++) {
    udelay(1000);
  }
}

void outlcd(char data, char RS) {
	PADDR = 0xFF;
	PADR = data & 0xF0;
	
	if(RS == 1) { //文字コード
		PADR |= 0x02; //RSに1を立てる
	}
	else PADR &= 0xF0; //RSを0にする
	
	udelay(1);
	PADR |= 0x01; //書き込み許可
	udelay(1);
	PADR &= 0xFE;
}

void OutLCD_Text(char code) {
	outlcd(code, 1);
	outlcd(code<<4, 1);
	udelay(50);
}

void OutLCD_Command(char code) {
	outlcd(code, 0);
	outlcd(code<<4, 0);
	udelay(50);
}

void ClearLCD(void) {
	outlcd(1, 0);
	outlcd(1<<4, 0);
	mdelay(2);
}

void InitializeLCD(void) {
	mdelay(20);
	outlcd(0x30, 0); //8ビットモード設定
	mdelay(5);
	outlcd(0x30, 0);
	udelay(150);
	outlcd(0x30, 0);
	mdelay(5);
	outlcd(0x20, 0);
	mdelay(5);
	OutLCD_Command(0x28);
	OutLCD_Command(0x08);
	OutLCD_Command(0x0C);
	OutLCD_Command(0x06);
}

void putLCD(int line, int cursor, char *str) {
	switch(line) {
		case 0:
			OutLCD_Command(0x80 + cursor);
			break;
		case 1:
			OutLCD_Command(0xC0 + cursor);
			break;
		case 2:
			OutLCD_Command(0x94 + cursor);
			break;
		case 3:
			OutLCD_Command(0xD4 + cursor);
			break;
		default:
			break;
	}
		
	while(*str != 0x00) { //NULLが出るまで
		OutLCD_Text(*str);
		str++;
	}
}

