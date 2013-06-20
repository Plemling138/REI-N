#ifndef _REI_LCD_H_INCLUDED_
#define _REI_LCD_H_INCLUDED_

void ClearLCD(void);
void InitializeLCD(void);
void putLCD(int line, int cursor, char *str);

#endif
