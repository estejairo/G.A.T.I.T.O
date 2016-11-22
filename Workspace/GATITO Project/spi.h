#ifndef __SPI_H
#define __SPI_H

/*  Include section
*
***************************************************/
#include "msp430_version.h"


/*  Defines section
*
***************************************************/
#define SPISEL 		P5SEL
#define SPIDIR    	P5DIR
#define SPIOUT     	P5OUT
#define STE       	BIT0
#define MOSI      	BIT1
#define MISO      	BIT2
#define SCLK      	BIT3
#define SS       	BIT4


/*  Function Prototype Section
*
***************************************************/
void spi_init(void);
void spi_send(unsigned char);
void spi_slave_enable(void);
void spi_slave_disable(void);
void spi_end_transmission(void);

#endif
