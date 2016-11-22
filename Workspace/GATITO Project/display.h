#ifndef DISPLAY__H
#define DISPLAY__H
/*  Include section
*
***************************************************/
#include "msp430_version.h"

/*  Defines section
*
***************************************************/
// defines display layout
#define LCD_LINES			4
#define LCD_WIDTH			20

// defines for data
#define	LDOUT			P2OUT	// dataport
#define LDIN			P2IN
#define	LDSEL			P2SEL
#define	LDDIR			P2DIR

// defines for control
#define LCOUT			P1OUT
#define	LCDIR			P1DIR
#define LCSEL			P1SEL
#define	LCD_RS			BIT5	// bit meaning: 1 = Data , 0 = Control
#define LCD_RW			BIT6	// bit meanings: 0 = Write, 1 = Read
#define	LCD_E			BIT7	// Enable

/*  Function Prototype Section
*
***************************************************/

void display_clear(void);
void display_del_cursor(void);
void display_right_shift(void);
void display_left_shift(void);
void display_new_character(int, char*);
unsigned char display_get_pos(void);
void display_init(void);
void display_set_pos(unsigned char);
void display_lcd_clear(int, int);
void display_Write_CGRAM_animacion0();
void display_Write_CGRAM_animacion1();
void display_Write_CGRAM_animacion2();
void display_Write_CGRAM_animacion4();
void display_Write_CGRAM_animacion5();
void display_Write_CGRAM_animacion6();
void display_Write_CGRAM_animacion7();
void udelay(unsigned int);
void display_test_Write_CGRAM_MS(void);

#endif
