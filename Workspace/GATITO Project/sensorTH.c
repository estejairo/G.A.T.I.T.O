/* Nombre del modulo: sensorTH.c
*
* Escrito inicialmente el 17/11/2016 Por Hernán Oyanadel
* Modificado el 20/11/2016 por Jairo González
*
* Descripción del módulo:
* Implementa la configuración y comunicación con el 
* sensor de temperatura y humedad SHT15.
***************************************************/

#include "sensorTH.h"
#include "msp430_version.h"
#include "display.h"

#define CLK_UP {PSENOUT |= CLK_PIN;}
#define CLK_DW {PSENOUT &= ~CLK_PIN;}
#define DATA_UP {PSENOUT |= DATA_PIN;}
#define DATA_DW {PSENOUT &= ~DATA_PIN;}

/**************************************************
* Nombre    		: void sensor_init(void)
* returns			: void
* arg1				: void
* Creada por		: Grupo 101-ELO312
* Fecha creación	: 17/11/2016
* Descripción		: Inicializa el puerto del sensor
**************************************************/
void sensor_init(void){
	PSENDIR |= CLK_PIN + DATA_PIN;
}

/**************************************************
* Nombre    		: void port_reset(void)
* returns			: void
* arg1				: void
* Creada por		: Grupo 101-ELO312
* Fecha creación	: 17/11/2016
* Descripción		: Resetea la linea de clk y de
* data, dejandolo en su estado inicial.
**************************************************/
void port_reset(void){
  	DATA_UP;
  	CLK_DW
}

/**************************************************
* Nombre    		: void transmition_start(void)
* returns			: void
* arg1				: void
* Creada por		: Grupo 101-ELO312
* Fecha creación	: 17/11/2016
* Descripción		: Realiza la secuencia de inicio 
* de transmisión. 
*        _____         ________
*  DATA:      |_______|
*            ___     ___
*  SCK : ___|   |___|   |______
*
**************************************************/
void transmition_start(void){
	udelay(2);
	CLK_UP;
	udelay(2);
	DATA_DW;
	udelay(2);
	CLK_DW
	udelay(6);
	CLK_UP;
	udelay(2);
	DATA_UP;
	udelay(2);
	CLK_DW;
}


/**************************************************
* Nombre    		: int sensor_send_cmd(unsigned char arg1)
* returns			: int
* arg1				: Comando a enviar
* Creada por		: Grupo 101-ELO312
* Fecha creación	: 17/11/2016
* Descripción		: Envia el comando arg1 al sensor
**************************************************/
int sensor_send_cmd(unsigned char cmd)
{  
	unsigned char i = 0; //Variable to shift bit
	int error = 0;
	for (i = 0x80; i > 0; i/=2) //Shift bit for masking
	{
	  	CLK_DW;
	  	if(i & cmd) //Write bit
	  	{
            DATA_UP;
      	}
	  	else
	  	{
	    	DATA_DW;
       	}
       	udelay(2);
	  	CLK_UP;
	  	udelay(6);
	  	CLK_DW
	  	udelay(2);
	}
	DATA_UP;
	udelay(2);
	PSENDIR &= ~DATA_PIN; //Release data pin
	CLK_UP;
	udelay(6);
	error = PSENIN & DATA_PIN; //There is an intention to use error flag, but dont use it
	CLK_DW;
	return error; 
}


/**************************************************
* Nombre    		: unsigned int receive_data(void)
* returns			: Medición de Temperatura
* arg1				: void
* Creada por		: Grupo 101-ELO312
* Fecha creación	: 17/11/2016
* Descripción		: Recibe 16 bits del sensor(Sin checksum)
**************************************************/
unsigned int receive_data(void){
	unsigned int i = 0;
	unsigned int value = 0;

	//Most significant 8 bits
	PSENDIR &= ~DATA_PIN; //Release data pin
	for (i = 0x80; i > 0; i/=2) //Shift bit for masking
	{
		CLK_UP;
		if (PSENIN & DATA_PIN) 
			value = (value | (i<<8)); //Set bit
		CLK_DW;
	}
	PSENDIR |= DATA_PIN; //Take control of data bus
	DATA_DW;
	udelay(2);
	CLK_UP; //Extra clock pulse for ACK
	udelay(6);
	CLK_DW;
	udelay(2);
	PSENDIR &= ~DATA_PIN; //Release data pin

	//Less significant 8 bits
	for (i = 0x80; i > 0; i/=2)
	{
		CLK_UP;
		if (PSENIN & DATA_PIN)
			value = (value | i);
		CLK_DW;
	}
	PSENDIR |= DATA_PIN; //Take control of data bus
	DATA_UP;
	//Without extra clock pulse, no ACK => no checksum
	return value;
}

/**************************************************
* Nombre    		: void sensor_get_t(void)
* returns			: Medición de Temperatura
* arg1				: void
* Creada por		: Grupo 101-ELO312
* Fecha creación	: 17/11/2016
* Descripción		: Realiza todo lo necesario para
* medir la temperatura
**************************************************/
float sensor_get_t(void){
	unsigned int measure_t = 0;
	float temperature = 0;
	port_reset();
	transmition_start();
	sensor_send_cmd(0x03); //0x03 => temperature command
	while((PSENIN & DATA_PIN)!=0);//wait until sensor has finished the measurement
	measure_t = receive_data();
	temperature = -39.7 + 0.01*measure_t; //Math expression indicated by the manufacturer
	return temperature;
}
 
/**************************************************
* Nombre    		: void sensor_get_t(void)
* returns			: Medición de Humedad
* arg1				: void
* Creada por		: Grupo 101-ELO312
* Fecha creación	: 17/11/2016
* Descripción		: Realiza todo lo necesario para medir la humedad
**************************************************/
float sensor_get_h(void){
	unsigned int measure_h = 0;
	float humidity = 0;
	port_reset();
	transmition_start();
	sensor_send_cmd(0x05); //0x05 => Humidity command
	while((PSENIN & DATA_PIN)!=0); //wait until sensor has finished the measurement
	measure_h = receive_data();
	humidity = -2.0468 + 0.0367*measure_h + 0.0000015955*measure_h*measure_h; //Math expression indicated by the manufacturer
	return humidity;
}
