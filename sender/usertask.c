#include "h8_69.h"
#include "rei.h"
#include "lib.h"
#include "lcd.h"

int recvpacket(char p[10])
{
	int i;
	if(p[0] == 0x01) {
		putsubs(p);
		for(i=0;i<3;i++) {
			puthex(p[i],2); puts(" ");
		}
		puts("\n");
	}
	return 0;
}

int setpacket(char p[10])
{
	int j;
	char io[4], ad[4];

	for(j=0;j<10;j++) p[j] = 0;

	//I/O入力
	P4DDR = 0x00;
	P4PCR = 0xFF;
	
	//A-D変換
	unsigned int data = 0;
	
	ADCSR = 0x20; //変換開始
	while(!(ADCSR | 0x80)){;} //終了まで待つ
	data = ADDRAH;
	
	for(j=1;j<25;j++) {
		if(data < (j*10)) {
			data = (j-1) * 10;
			break;
		}
	}
	
	//値の文字型数値化
	dec2str(P4DR, io);
	dec2str(data, ad);
	puts(ad); puts("\n");
	//ClearLCD();
	
	//LCD出力
	putLCD(0, 0, "*PACKET  STATUS*");
	putLCD(1, 0, "SW:");
	putLCD(1, 3, io);
	putLCD(1, 7, "Duty:");
	putLCD(1, 12, ad);
	
	p[0] = 0x01;
	p[1] = P4DR;
	p[2] = data;
	p[3] = 0x02;

	return 0;
}
