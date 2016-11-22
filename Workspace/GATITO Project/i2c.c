/***************************************************
* Nombre del programa: i2.c
*
* Liberia basada en documentos de grupos de años
* anteriores.
*
* Creado el 11/11/2016 por Hernán Oyanadel
* Grupo 101 Proyecto Lab. Estructura de Computadores
*
* Descripción del módulo:
* Permite comunicacion mediante i2c, para el modulo
* rtc.
***************************************************/

#include "msp430_version.h"
#include <stdio.h>
#include "ds1307.h"
#include "i2c.h"

/**************************************************
	  Inicializa transmision I2C
**************************************************/

void i2c_init(void)
{    
    I2CSEL=0x0A;     				      //P3SEL = BIT1+BIT3, Dependiendo del estado seran entradas o salidas...?       
    U0CTL |= SYNC + I2C; 			      //ACLK + MASTER + Sincronismo	  
	U0CTL &= ~I2CEN;                                      //Recommended init procedure
	I2CTCTL = I2CSSEL_2;		          
    I2CNDAT = 0x01;									  // Se escribira una cantidad de n bytes entre un start y stop
    I2CSA = 0x68;                                          // Slave Addres del DS1307	
	I2CIE = RXRDYIE;
 	I2CPSC = 0x0A;                                        //Prescaler divide por 10 +1 smclk ????               
	U0CTL |= I2CEN;                                       // Enable I2C, 7 bit addr,
}



/****************************************************
	      Inicializa escritura I2C
****************************************************/
void i2c_start_w(void){
	I2CNDAT = 0x08;     // Siempre se transmitiran 8 datos para iniciar el ds1307. (: 
	U0CTL |= MST;
    I2CTCTL |= I2CTRX + I2CSTT + I2CSTP;     //Modo transmicion y envia bit de start, el de stop se genera luego de 8 bytes.
}


/****************************************************
	      I2C WRITE
****************************************************/

void i2c_write(unsigned char dato){
	while((I2CIFG & TXRDYIFG)==0);		//Espera que el transmisor este listo
	I2CDRB = dato;					//Envia dadta[i].                            // Carga dato.
}



/****************************************************
Inicializa direccion a leer del DS1307.
****************************************************/

void i2c_start_r(unsigned char addr){
	
	I2CNDAT = 0x01;                              // Solo es necesario escribir 1 byte.
	U0CTL |= MST;                                 // Master mode 
	I2CTCTL |= I2CSTP+I2CSTT + I2CTRX;                  // Genera bit de start y se deja en write mode
	while((I2CIFG & TXRDYIFG)==0);		//Espera que el transmisor este listo
	I2CDRB=addr;
		
}



/****************************************************
	        I2C READ
Esta funcion leera datos (n bytes) del DS1307 desde la 
direccion de memoria que fue especificada por la
funcion i2c_start_r(unsigned char addr).
*****************************************************/

void i2c_read(unsigned char n){
	while((I2CTCTL & I2CSTP)!=0);
	I2CNDAT = n;						          // n bytes a leer
	U0CTL |= MST;
	I2CTCTL &= ~I2CTRX;        					 // Modo Lecutra
	I2CTCTL |= I2CSTT + I2CSTP;					 // Genera bit de start y bit de stop
	while ((I2CIFG & RXRDYIFG) == 1);                               // Llego informacion? 
	
	// El dato se almacena en I2CDRB	
	
}


