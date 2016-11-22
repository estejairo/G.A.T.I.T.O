/* Nombre del modulo: spi.c
*
* Escrito inicialmente el 1/11/2016 Por Jairo González
*
* Descripción del módulo:
* Modulo driver SPI
* Contiene las funciones que permiten manejar el
* módulo SPI.
***************************************************/

/*  Include section
*
***************************************************/
#include "msp430_version.h"// Depende del uC que Ud. esté ocupando.
#include "spi.h"



/**************************************************
* Nombre    		: void spi_init(void)
* returns			: void
* Creada por		: Jairo González
* Fecha creación	: 1/11/2016
* Descripción		: Inicializa el modulo SPI1
* The required USART initialization/re-configuration process is:
* 1) Set SWRST (BIS.B #SWRST,&UxCTL)
* 2) Initialize all USART registers with SWRST=1 (including UxCTL)
* 3) Enable USART module via the MEx SFRs (USPIEx)
* 4) Clear SWRST via software (BIC.B #SWRST,&UxCTL)
* 5) Enable interrupts (optional) via the IEx SFRs (URXIEx and/or UTXIEx)
* The SPI transmit/receive enable bit is USPIEx
**************************************************/
void spi_init(void)
{
	//SPI Pin configuration
	SPISEL |= MOSI+MISO+SCLK; //Mode Select: SIMO1, SOMI1, UCLK1
	SPIDIR |= MOSI+SCLK+SS; //Outputs
	SPIDIR &= ~MISO; //Inputs
	SPIOUT |= SS; //SS High, slave inactive

	//SPI Control configuration
	UCTL1 |= SWRST; //Enable Software Reset
	UCTL1 &= ~SPB; //Set SPI mode
	UCTL1 |= CHAR+SYNC+MM; //Set 8 bit lenght, enable synchronous mode and set as Master

	//SPI transmission configuration
	UTCTL1 |= CKPH; //Clock phase delayed by one half cycle
    UTCTL1 &= ~CKPL; //Set inactive level as low; data is output with the rising edge of UCLK;
					//input data is latched with the falling edge of UCLK
	UTCTL1 |= SSEL1+STC; // Select SMCLK, 3-bit SPI mode, STE Disabled. 
						//STE need to be enabled only when other masters try to control a common slave

	//SPI Baud Rate Configuration
	UBR01 = 0xFF;
	UBR11 = 0x00; // {U0BR1,U0BR0} = 2, min divider, max baud rate.
	UMCTL1 = 0x00; //No modulation bits needed

	//More configurations
	ME2 |= USPIE1; //Enable SPI
	UCTL1 &= ~SWRST; //Disable Software Reset

	//SPI Interruptions configuration
	IE1 &= ~UTXIE1;
	IE1 &= ~URXIE1; //TX & RX interrupt disabled
}

/**************************************************
* Nombre            : void spi_slave_enable(void)
* returns           : void
* Creada por        : Jairo González
* Fecha creación    : 1/11/2016
* Descripción       : Habilita el slave
**************************************************/
void spi_slave_enable(void)
{
	SPIOUT &= ~SS; //Enable Slave
}


/**************************************************
* Nombre            : void spi_1_slave_1_disable(void)
* returns           : void
* Creada por        : Jairo González
* Fecha creación    : 1/11/2016
* Descripción       : Deshabilita el slave
**************************************************/
void spi_slave_disable(void)
{
	SPIOUT |= SS; //SS High, slave inactive
}


/**************************************************
* Nombre            : void spi_send(unsigned char arg1)
* returns           : void
* arg 1             : Caracter a ser enviado
* Creada por        : Jairo González
* Fecha creación    : 1/11/2016
* Descripción       : Envia un caracter por SPI1
**************************************************/
void spi_send(unsigned char c)
{
	TXBUF1 = c; //Set next data to be transmitted
	while (!(IFG2 & UTXIFG1)); //Wait until the buffer is empty
}


/**************************************************
* Nombre            : void spi_send(void)
* returns           : void
* Creada por        : Jairo González
* Fecha creación    : 8/11/2016
* Descripción       : Espera hasta que la transmisión
* y recepción finalicen
**************************************************/
void spi_end_transmission(void)
{
	while (!(TXEPT & U1TCTL)); //Verify if all data was transmitted and received by master
}
