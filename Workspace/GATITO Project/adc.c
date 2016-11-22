/***************************************************
* Nombre del modulo: adc.c
*
* Modulo creado para la asignatura Elo312
* Laboratorio de Estructura de Computadores
* del departamento de Electrónica de la Universidad
* Técnica Federico Santa María. El uso o copia
* está permitido y se agracede mantener el nombre 
* de los creadores.
*
* Escrito inicialmente el 01/01/2004 Por Michael Kusch & Wolfgang Freund
* Modificado el 24/09/2014           Por Andrés Llico.
*
* Descripción del módulo:
* Modulo driver adc
* Contiene las funciones que permiten manejar el ADC
***************************************************/

/*  Include section
*
***************************************************/
#include "adc.h"


////////////////////////////////////////
//       RUTINAS IMPLEMENTADAS        //
//                                    //

/**************************************************
* Nombre    		: int adc_single_channel_single_conv(int arg1)
* returns			: Resultado de la conversión
* arg1				: Canal a convertir
* Creada por		: Michael Kusch
* Fecha creación	: 01/01/2004
* Descripción		: Realiza una conversión simple
de un solo canal.
**************************************************/
int adc_single_channel_single_conv(int ch)
{
	ADC12CTL0 &= ~ENC;
	ADC12MCTL0 &= 0xF0; 		// Se limpia canal anterior
	ADC12MCTL0 |= (ch & 0x0F);  // Selecciona canal a convertir
	ADC12CTL0 |= ENC;           // habilita conversión
	ADC12CTL0 |= ADC12SC;
        
	while (ADC12CTL0 & ADC12SC);
	
	return ADC12MEM0;    		// Los resultados de la conversión
}

/**************************************************
* Nombre    		: void adc_init_single_channel_single_conv(void)
* returns			: void
* Creada por		: Michael Kusch
* Fecha creación	: 01/01/2004
* Descripción		: Inicializa el ADC en modo de
conversión simple de un solo canal.
**************************************************/
void adc_init_single_channel_single_conv()
{
    P6SEL |= 0xFF;
    P6DIR &= 0x00;
    ADC12CTL0 &= ~ENC;
    ADC12CTL0 = SHT1_15 | SHT0_15 | ADC12ON;
    ADC12CTL1 = CSTARTADD_0 | SHS_0 | SHP | ADC12DIV_7 | ADC12SSEL_2 | CONSEQ_0;
    //CONSEQx = 1 => MODO DEL ADC12: Secuencia de canales.
    ADC12MCTL0 = SREF_0;
}
//                                    //
////////////////////////////////////////


////////////////////////////////////////
//     RUTINAS NO IMPLEMENTADAS       //
//                                    //

/**************************************************
* Nombre    		: void adc_seq_channel_single_conv(int *arg1, int *arg2)
* returns			: void
* arg1				: Arreglo con resultados de la conversión
* arg2				: Arreglo de canal a convertir
* Creada por		: Andrés Llico
* Fecha creación	: 24/09/2014
* Descripción		: Realiza una conversión simple
de una secuencia de canales.
**************************************************/
void adc_seq_channel_single_conv(int *results, int *ch)
{
  ADC12CTL0 &= ~ENC;

    ADC12MCTL0 &= 0xF0;          // Se limpia canal anterior
    ADC12MCTL0 |= (*ch & 0x0F);  // Selecciona canal a convertir

    ADC12MCTL1 &= 0xF0;         // Se limpia canal anterior
    ADC12MCTL1 |= (*(ch+1) & 0x0F);  // Selecciona canal a convertir

    ADC12MCTL2 &= 0xF0;         // Se limpia canal anterior
    ADC12MCTL2 |= (*(ch+2) & 0x0F);  // Selecciona canal a convertir

    ADC12MCTL3 &= 0xF0;         // Se limpia canal anterior
    ADC12MCTL3 |= (*(ch+3) & 0x0F);  // Selecciona canal a convertir

    ADC12MCTL4 &= 0xF0;         // Se limpia canal anterior
    ADC12MCTL4 |= (*(ch+4) & 0x0F);  // Selecciona canal a convertir

    ADC12MCTL5 &= 0xF0;         // Se limpia canal anterior
    ADC12MCTL5 |= (*(ch+5) & 0x0F);  // Selecciona canal a convertir

    ADC12MCTL6 &= 0xF0;         // Se limpia canal anterior
    ADC12MCTL6 |= (*(ch+6) & 0x0F);  // Selecciona canal a convertir

    ADC12MCTL7 &= 0xF0;         // Se limpia canal anterior
    ADC12MCTL7 |= (*(ch+7) & 0x0F);  // Selecciona canal a convertir

    ADC12CTL0 |= ENC;           // habilita conversión
    ADC12CTL0 |= ADC12SC;

    while ((ADC12CTL0 & ADC12SC));

    *results = ADC12MEM0;
    *(results+1) = ADC12MEM1;
    *(results+2) = ADC12MEM2;
    *(results+3) = ADC12MEM3;
    *(results+4) = ADC12MEM4;
    *(results+5) = ADC12MEM5;
    *(results+6) = ADC12MEM6;
    *(results+7) = ADC12MEM7;
}

/**************************************************
* Nombre    		: void adc_init_seq_channel_single_conv(void)
* returns			: void
* Creada por		: Andrés Llico
* Fecha creación	: 24/09/2014
* Descripción		: Inicializa el ADC en modo de
conversión simple de una secuencia de canales.
**************************************************/
void adc_init_seq_channel_single_conv()
{
    P6SEL |= 0xFF;
    P6DIR &= 0x00;
    ADC12CTL0 &= ~ENC;
    ADC12CTL0 = SHT1_15 | SHT0_15 | ADC12ON |MSC;
    ADC12CTL1 = CSTARTADD_0 | SHS_0 | SHP | ADC12DIV_7 | ADC12SSEL_2 | CONSEQ_1; //CONSEQx = 1 => MODO DEL ADC12: Secuencia de canales.
    ADC12MCTL0 = SREF_0;
    ADC12MCTL1 = SREF_0;
    ADC12MCTL2 = SREF_0;
    ADC12MCTL3 = SREF_0;
    ADC12MCTL4 = SREF_0;
    ADC12MCTL5 = SREF_0;
    ADC12MCTL6 = SREF_0;
    ADC12MCTL7 = SREF_0;
    ADC12IE = 0x00; // deshabilitar interrupciones


}

/**************************************************
* Nombre    		: __interrupt void adc_ADC12_interrupt(void)
* returns			: void
* Creada por		: Mauricio Solis
* Fecha creación	: 01/01/2013
* Descripción		: Vector de interrupciones del
ADC.
**************************************************/
/*#pragma vector = ADC12_VECTOR
__interrupt void adc_ADC12_interrupt(void)
{
	
}
*/
//                                    //
////////////////////////////////////////

