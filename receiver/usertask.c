#include "h8_69.h"
#include "rei.h"
#include "lib.h"

int recvpacket(char p[10])
{
	if(p[0] == 0x01) {
		puthex(p[1],2);
		putsubc(p[0]);
		putsubc(p[1]);
		putsubc(p[2]);
		putsubc(p[3]);
		puts("\n");
	}
	return 0;
}

int setpacket(char p[10])
{
	P5DDR = 0xF0;
	P5PCR = 0x0F;
	
	p[0] = 0x01;
	p[1] = P5DR;
	p[2] = 0x02;
	/*
	putsubc(0x01);
	putsubc(0xFF);
	putsubc(0x80);
	putsubc(0x02);
	*/
//	puts(p);

	return 0;
}
