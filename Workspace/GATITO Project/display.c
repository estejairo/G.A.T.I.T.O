/***************************************************
* Nombre del modulo: display.c
*
* Modulo creado para la asignatura Elo312
* Laboratorio de Estructura de Computadores
* del departamento de Electrónica de la Universidad
* Técnica Federico Santa María. El uso o copia
* está permitido y se agracede mantener el nombre 
* de los creadores.
*
* Escrito inicialmente el 01/01/2004 Por Michael Kusch & Wolfgang Freund
* Modificado el 19/09/2014           Por Mauricio Solís.
*
* Descripción del módulo:
* Modulo driver display
* Contiene las funciones que permiten manejar un display
* LCD.
***************************************************/

/*  Include section
*
***************************************************/
#include "display.h"

/*  Defines section
*
***************************************************/
#define LCD_PORT_INIT   {LDSEL = 0x0; LCDIR |= (LCD_RS + LCD_RW + LCD_E); LCSEL &= ~(LCD_RS + LCD_RW + LCD_E);}
#define LCD_READ(x)     {SET_BIT(LCD_E); LCD_PDELAY; (x) = LDIN; RST_BIT(LCD_E); LCD_PDELAY;} 
#define LCD_WRITE(x)    {LDDIR = 0xff; LDOUT = x; LCD_ENABLE(); LDDIR = 0;}

#define LCD_PDELAY      {asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP");}

#define SET_BIT(x)      (LCOUT |= (x))   
#define RST_BIT(x)      (LCOUT &= ~(x))

#define LCD_ENABLE()    {SET_BIT(LCD_E); LCD_PDELAY; RST_BIT(LCD_E);}
#define LCD_WAIT()      udelay(120)


/*  Local Function Prototype Section
*
***************************************************/
void udelay(unsigned int);
void display_del_cursor(void);
unsigned char display_wait_BF(void);
char display_get_data(void);
unsigned char display_lcd_addr(int);
void display_send_cmd(unsigned char);
void display_send_data(char);
int putchar(int c);
int display_lcd_line(unsigned char);
int display_lcd_copy(unsigned char, unsigned char, int);
void display_lcd_scroll_up(void);

/*  Global Variables Section
*
***************************************************/


////////////////////////////////////////
//        RUTINA A CALIBRAR           //
//                                    //
/**************************************************
* Nombre    		: void udelay(unsigned int arg1)
* returns			: void
* arg1				: Cantidad de microsegundos 
* Creada por		: Michael Kusch
* Fecha creación	: 01/01/2004
* Descripción		: Hace un retardo de arg1 
* microsegundos
**************************************************/
void udelay(unsigned int x)
{
	unsigned int i;
	for (i = 0; i < x; i++)
	{
		LCD_PDELAY;
	}
}
//                                    //
////////////////////////////////////////


////////////////////////////////////////
//     RUTINAS NO IMPLEMENTADAS       //
//                                    //


/**************************************************
* Nombre    		: unsigned char display_wait_BF(void)
* returns			: Dirección actual del cursor
* Creada por		: Michael Kusch
* Fecha creación	: 01/01/2004
* Descripción		: Espera a que el display esté listo
* para recibir nuevos comandos o datos
**************************************************/
//#pragma inline = forced
unsigned char display_wait_BF(void)
{
	unsigned char add;
	unsigned char flag = 0x80;

	SET_BIT(LCD_RW);
	RST_BIT(LCD_RS);

	while(flag == 0x80)
	{
		LCD_READ(add)
		flag = 0x80&add;
	}

	return add;
}
/**************************************************
* Nombre    		: void diplay_del_cursor(void)
* returns			: none
* Creada por		: Herni
* Fecha creación	: 10/10/2016
* Descripción		: borra el cursor
**************************************************/
void display_del_cursor(void){
	display_send_cmd(0x0C);
}


/**************************************************
* Nombre    		: void display_clear(void)
* returns			: void
* Creada por		: Mauricio Solís
* Fecha creación	: 19/09/2014
* Descripción		: Limpia el contenido del display
**************************************************/
void display_clear(void)
{
	display_send_cmd(0x01);
}


/**************************************************
* Nombre    		: void display_right_shift(void)
* returns			: void
* Creada por		: Mauricio Solís
* Fecha creación	: 19/09/2014
* Descripción		: Corre el "visor" del display en un 
* caracter hacia la derecha 
**************************************************/
void display_right_shift(void)
{
	display_send_cmd(0x14); //comando para hacer shift a la derecha
}


/**************************************************
* Nombre    		: void display_left_shift(void)
* returns			: void
* Creada por		: Mauricio Solís
* Fecha creación	: 19/09/2014
* Descripción		: Corre el "visor" del display en un 
* caracter hacia la izquierda 
**************************************************/
void display_left_shift(void)
{
	display_send_cmd(0x10); //comando para hacer shift a la izquierda
}
/**************************************************
* Nombre    		: void display_new_character(int arg1, char* arg2)
* returns			: void
* arg1				: Dirección del display donde se debe empezar a escribir
* arg2				: Puntero al arreglo que contiene el nuevo caracter a escribir
* Creada por		: Mauricio Solís
* Fecha creación	: 19/09/2014
* Descripción		: Escribe en el display un nuevo caracter arg2, en la dirección
* arg1 
**************************************************/
void display_new_character(int pos, char* data)
{
	display_set_pos(pos);
	display_send_data(*data);
}

///////////////////////////////////////
//Rutinas Implementadas


/**************************************************
* Nombre    		: unsigned char display_get_pos(void)
* returns			: Dirección actual del cursor
* Creada por		: Michael Kusch
* Fecha creación	: 01/01/2004
* Descripción		: Retorna la posición actual del cursor
**************************************************/
unsigned char display_get_pos(void)
{
	unsigned char val;
    display_wait_BF();
	SET_BIT(LCD_RW);
	RST_BIT(LCD_RS);
	LCD_READ(val);
	return val&0x7F;
}

/**************************************************
* Nombre    		: char display_get_data(void)
* returns			: Caracter sobre el que está el cursor del display
* Creada por		: Michael Kusch
* Fecha creación	: 01/01/2004
* Descripción		: Envía el comando para leer el caracter sobre
* el que está el cursor del display y lo retorna
**************************************************/
char display_get_data(void)
{
	char value;
	display_wait_BF();
	SET_BIT(LCD_RS|LCD_RW);
	LCD_READ(value);
	RST_BIT(LCD_RS|LCD_RW);
	return value;
}

/**************************************************
* Nombre    		: void display_init(void)
* returns			: void
* Creada por		: Michael Kusch
* Fecha creación	: 01/01/2004
* Descripción		: Inicializa el display
**************************************************/
void display_init(void)
{
	udelay(15000);
	LCD_PORT_INIT;
	RST_BIT(LCD_RS + LCD_RW);

	LCD_WRITE(0x30);
	udelay(500);
	
	LCD_WRITE(0x30);
	udelay(120);

	LCD_WRITE(0x30);
	udelay(200);

	LCD_WRITE(0x38);
	udelay(200);
	LCD_WRITE(0x0F);

	udelay(200);
	LCD_WRITE(0x1);
	udelay(5000);
	LCD_WRITE(0x06);
	udelay(200);
	
	display_del_cursor();
}

/**************************************************
* Nombre    		: void display_set_pos(unsigned char arg1)
* returns			: void
* arg1				: Dirección a la que se debe enviar el cursor
* Creada por		: Michael Kusch
* Fecha creación	: 01/01/2004
* Descripción		: Setea la posición del cursor en el display

**************************************************/
void display_set_pos(unsigned char add)
{
	display_send_cmd(0x80 + add);
}


/**************************************************
* Nombre    		: unsigned char display_lcd_addr(int arg1)
* returns			: Dirección de la línea
* arg1				: Número de la línea
* Creada por		: Michael Kusch
* Fecha creación	: 01/01/2004
* Descripción		: Entrega la dirección de una determinada línea
* en el display.
* dado una línea (0, 1, etc) retorna la dirección de memoria
**************************************************/ 
unsigned char display_lcd_addr(int line)
{
	switch (LCD_LINES)
	{
	case 2:
		return (line > 0) ? 0x40 : 0x0;
	
	case 4:
		if (line == 0) return 0x0;
		else if (line == 1) return 0x40;
		else if (line == 2) return LCD_WIDTH;
		return (0x40 + LCD_WIDTH);
	
	default: return 0;
	}
}

/**************************************************
* Nombre    		: void display_send_cmd(unsigned char arg1)
* returns			: void
* arg1				: Comando a enviar
* Creada por		: Michael Kusch
* Fecha creación	: 01/01/2004
* Descripción		: Envía un comando al display
**************************************************/ 
void display_send_cmd(unsigned char value)
{
	display_wait_BF(); 
	RST_BIT(LCD_RS + LCD_RW);
	LCD_WRITE(value);
}

/**************************************************
* Nombre    		: void display_send_data(char arg1)
* returns			: void
* arg1				: Data a enviar
* Creada por		: Michael Kusch
* Fecha creación	: 01/01/2004
* Descripción		: Envía un dato al display
**************************************************/ 
void display_send_data(char value)
{
	display_wait_BF(); 
	SET_BIT(LCD_RS); RST_BIT(LCD_RW);
	LCD_WRITE(value);
}

/**************************************************
* Nombre    		: int putchar(int arg1)
* returns			: El caracter ingresado como argumento
* arg1				: El caracter.
* Creada por		: Michael Kusch
* Fecha creación	: 01/01/2004
* Descripción		: Maneja los caracteres que "envía"
* la función printf de la biblioteca stdio
**************************************************/ 
int putchar(int c)
{
	unsigned char addr;
	addr = display_get_pos();
	if (c == '\n')
	{
		if(display_lcd_line(addr)==0)
        	display_set_pos(0x40);
        else
        	display_lcd_scroll_up();
		return c;
	}
    
	if (c == '\r')
	{
		//display_set_pos( (0x10 - addr&0x2F ) + addr&0x40 ); //vuelve al principio sin importar la linea
		if(display_lcd_line(addr)==1)
			display_set_pos(0x00);
		else
			display_set_pos(0x40);
		return c;
	}

	if (c == '\t')
	{
		if(((addr&0x0F)==0x00)|((addr&0x0F)==0x04)|((addr&0x0F)==0x08)|((addr&0x0F)==0x0C))
		{
			display_send_data(' ');	
			display_send_data(' ');
			display_send_data(' ');
			display_send_data(' ');
		}
		else
			if (((addr&0x0F)==0x01)|((addr&0x0F)==0x05)|((addr&0x0F)==0x09)|((addr&0x0F)==0x0D))
			{
				display_send_data(' ');
				display_send_data(' ');
				display_send_data(' ');
			}
			else
				if (((addr&0x0F)==0x02)|((addr&0x0F)==0x06)|((addr&0x0F)==0x0A)|((addr&0x0F)==0x0E))
				{
					display_send_data(' ');
					display_send_data(' ');
				}
				else
					display_send_data(' ');
		return c;
	}
	if( c== '\b')
	{
		display_left_shift();
		return c;
	}

	display_send_data((unsigned char) c);
	return c;
}

/**************************************************
* Nombre    		: int display_lcd_line(unsigned char addr)
* returns			: Número de la línea
* arg1				: Dirección del display
* Creada por		: Michael Kusch
* Fecha creación	: 01/01/2004
* Descripción		: Dada una dirección retorna el número de línea
**************************************************/ 
int display_lcd_line(unsigned char addr)
{
	int line;

	for (line = 0; line < LCD_LINES; line++)
		if (addr >= display_lcd_addr(line) && 
			addr < display_lcd_addr(line) + LCD_WIDTH)
			return line;
	return -1;
}

/**************************************************
* Nombre    		: void display_lcd_clear(int arg1, int arg2)
* returns			: void
* arg1				: Dirección de inicio
* arg2				: Cantidad de caracteres
* Creada por		: Michael Kusch
* Fecha creación	: 01/01/2004
* Descripción		: Limpia - arg2 - caracteres desde arg1
**************************************************/ 
void display_lcd_clear(int from, int len)
{
	int i;

	display_set_pos(from);
	for (i = 0; i < len; i++)
		display_send_data(' ');
	display_set_pos(from);
}

/**************************************************
* Nombre    		: int display_lcd_copy(unsigned char arg1, unsigned char arg2, int arg3)
* returns			: 0 si se hizo, -1 si hay un error de direcciones.
* arg1				: Dirección de inicio
* arg2				: Dirección destino
* arg3				: Cantidad de caracteres
* Creada por		: Michael Kusch
* Fecha creación	: 01/01/2004
* Descripción		: Copia - arg3 - caracteres desde - arg1 - a - arg2 -
**************************************************/
int display_lcd_copy(unsigned char from, unsigned char to, int len)
{
	char data[LCD_WIDTH];
	int i;

	if ((len > LCD_WIDTH) || (from > (0x7f - len))
		|| (to > (0x7f - len))) return -1;

	display_set_pos(from);
	for (i = 0; i < len; i++)
		data[i] = display_get_data();
	display_set_pos(to);
	for (i = 0; i < len; i++)
		display_send_data(data[i]);
	return 0;
}

/**************************************************
* Nombre    		: void display_lcd_scroll_up(void)
* returns			: void
* Creada por		: Michael Kusch
* Fecha creación	: 01/01/2004
* Descripción		: Copia línea i en línea i - 1 para todo el display
**************************************************/
void display_lcd_scroll_up(void)
{
	int line;
	for (line = 0; line < LCD_LINES - 1; line++)
		display_lcd_copy(display_lcd_addr(line + 1), display_lcd_addr(line), LCD_WIDTH);
	display_lcd_clear(display_lcd_addr(line), LCD_WIDTH);
}

//                                    //
////////////////////////////////////////

/**************************************************
* Nombre    		: void display_test_Write_CGRAM_MS(void)
* returns			: void
* Creada por		: Mauricio Solís
* Fecha creación	: 20/09/2014
* Descripción		: Escribe en la memoria CGRAM
* un caracter nuevo ... un robot.
**************************************************/
const char new_char0[]={BIT2,BIT3+BIT1,BIT2,BIT4+BIT3+BIT2+BIT1+BIT0,BIT2,BIT3+BIT1,BIT3+BIT1,BIT4+BIT3+BIT1+BIT0};
void display_test_Write_CGRAM_MS(void)
{
	int i=0;
	int x=1; 
	display_send_cmd(0x40+(x<<3));//set CG RAM Address
	for(i=0; i<8; i++)
	{
		display_send_data(new_char0[i]);
	}
}


/**************************************************
* Nombre    		: void display_Write_CGRAM_animacionN(void)
* returns			: void
* Creada por		: Mauricio Solís, Jairo González, Hernán Oyanadel
* Fecha creación	: 20/09/2014
* Descripción		: Escribe en la memoria CGRAM
* un caracter nuevo ... animación musical
**************************************************/
/*const char  llave_fa[] = {0x00,0x00,BIT3,BIT4+BIT2+BIT0,BIT4+BIT2,BIT2+BIT0,BIT3,BIT4};
void display_Write_CGRAM_animacion0(void)
{
	int i=0;
	int x=0; 
	display_send_cmd(0x40+(x<<3));//set CG RAM Address
	for(i=0; i<8; i++)
	{
		display_send_data(llave_fa[i]);
	}
}

const char  bis_start[] = {BIT4+BIT2,BIT4+BIT2,BIT4+BIT2+BIT0,BIT4+BIT2,BIT4+BIT2,BIT4+BIT2+BIT0,BIT4+BIT2,BIT4+BIT2};
void display_Write_CGRAM_animacion1(void)
{
	int i=0;
	int x=1; 
	display_send_cmd(0x40+(x<<3));//set CG RAM Address
	for(i=0; i<8; i++)
	{
		display_send_data(bis_start[i]);
	}
}

const char  corchea_up[] = {BIT0,BIT0+BIT1,BIT0+BIT2,BIT0+BIT3,BIT0+BIT3,BIT0+BIT1+BIT3,BIT0+BIT1+BIT3+BIT4,BIT4+BIT3};
void display_Write_CGRAM_animacion2(void)
{
	int i=0;
	int x=2; 
	display_send_cmd(0x40+(x<<3));//set CG RAM Address
	for(i=0; i<8; i++)
	{
		display_send_data(corchea_up[i]);
	}
}

const char  corchea_down[] = {BIT3,BIT3+BIT2,BIT3+BIT1,BIT3+BIT0,BIT3+BIT0,BIT4+BIT3+BIT0,BIT4+BIT3+BIT1+BIT0,BIT1+BIT0};
void display_Write_CGRAM_animacion3(void)
{
	int i=0;
	int x=3; 
	display_send_cmd(0x40+(x<<3));//set CG RAM Address
	for(i=0; i<8; i++)
	{
		display_send_data(corchea_down[i]);
	}
}

const char  scorchea_down[] = {0x00,0x00,BIT3,BIT3+BIT2,BIT3+BIT1,BIT3+BIT1,BIT4+BIT3,BIT4+BIT3};
void display_Write_CGRAM_animacion4(void)
{
	int i=0;
	int x=4; 
	display_send_cmd(0x40+(x<<3));//set CG RAM Address
	for(i=0; i<8; i++)
	{
		display_send_data(scorchea_down[i]);
	}
}

const char  scorchea_up[] = {BIT2,BIT2+BIT1,BIT2+BIT0,BIT2+BIT0,BIT3+BIT2,BIT3+BIT2,0x00,0x00};
void display_Write_CGRAM_animacion5(void)
{
	int i=0;
	int x=5; 
	display_send_cmd(0x40+(x<<3));//set CG RAM Address
	for(i=0; i<8; i++)
	{
		display_send_data(scorchea_up[i]);
	}
}

const char  h_silence[] = {0x00,0x00,0x00,0x00,BIT3+BIT2+BIT1,BIT4+BIT3+BIT2+BIT1+BIT0,0x00,0x00};
void display_Write_CGRAM_animacion6(void)
{
	int i=0;
	int x=6; 
	display_send_cmd(0x40+(x<<3));//set CG RAM Address
	for(i=0; i<8; i++)
	{
		display_send_data(h_silence[i]);
	}
}

const char  bis_end[] = {BIT2+BIT0,BIT2+BIT0,BIT4+BIT2+BIT0,BIT2+BIT0,BIT2+BIT0,BIT4+BIT2+BIT0,BIT2+BIT0,BIT2+BIT0};
void display_Write_CGRAM_animacion7(void)
{
	int i=0;
	int x=7; 
	display_send_cmd(0x40+(x<<3));//set CG RAM Address
	for(i=0; i<8; i++)
	{
		display_send_data(bis_end[i]);
	}
}
*/