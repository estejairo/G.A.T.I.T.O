#include "msp430_version.h"// Depende del uC que Ud. esté ocupando.
#include <stdio.h>
#include "ds1307.h"
#include "i2c.h"

unsigned char data[8];
/**************************************************
Escribe segundos, miutos horas, etc...
**************************************************/
void ds1307_init(void){
    data[0]=0x00;    // Direccion de inicio
	data[1]=0x00;    // SEGUNDO
	data[2]=0x19;    // MINUTOS
	data[3]=0x17;   // HORAS
	data[4]=0x02;    // DAY
	data[5]=0x15;   // DATE
	data[6]=0x11;    // MONTH
	data[7]=0x00;    // YEAR

	i2c_start_w();
	
	for(int i=0;i<8;i++){
		i2c_write(data[i]);		
	}
}


