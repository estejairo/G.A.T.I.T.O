/***************************************************
* Nombre del programa: main.c
*
* Escrito inicialmente el 20/11/2016 Por Jairo González
* Grupo 101 Proyecto Lab. Estructura de Computadores
* Ingeniería Civil Electrónica
* Universidad Técnica Federico Santa María
* Valparaíso, Chile.
*
* Descripción del módulo:
* Este programa implementa una central climatológica
* en la MSP430F1612.
* "Geo-Analizador Térmico Inteligente Totalmente Online"
* G.A.T.I.T.O
*
* Se utiliza módulo wiznet, Real-Time-Clock, sensor de luminosidad,
* sensor de temperatura/humedad, anemometro confeccionado
* por nuestro grupo y display LCD. Se hace uso de comunicación
* por SPI, I2C y mediciones mediante ADC.
*
* Partes del sitio web se hospedan en el servidor de alumnos
* de electrónica UTFSM.
***************************************************/


/*  Include section
*
***************************************************/
#include "msp430_version.h"
#include "stdio.h"
#include "string.h"
#include "osc.h"
#include "display.h"
#include "adc.h"
#include "spi.h"
#include "wiznet.h"
#include "ds1307.h" //rtc module
#include "i2c.h"
#include "sensorTH.h"


/*  Define section
*
***************************************************/
#define FACTOR_LUM 1/20 //Both factors calculated experimentaly
#define FACTOR_VEL 2.5 


/*  Useful Function
*   Used to find a word in a http response.
***************************************************/
int strindex(char *s,char *t)
{
  int i,n;

  n=strlen(t);
  for(i=0;*(s+i); i++) {
    if (strncmp(s+i,t,n) == 0)
      return i;
  }
  return -1;
}


/*  Wiznet global variables
*
***************************************************/
unsigned char sockreg;
unsigned char buf[MAX_BUF];


/*  Wiznet Index HTML page
*   (saved in flash memory as const values)
***************************************************/
const unsigned char html1[] =
    "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"
    "<!DOCTYPE html>"

    "<html lang='en'>"

    "<head>"

        "<meta charset='utf-8'>"
        "<meta http-equiv='X-UA-Compatible' content='IE=edge'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1'>"

        "<title>G.A.T.I.T.O. | Tiempo en Sala B360</title>"

        "<link href='http://aragorn.elo.utfsm.cl/~jairo.gonzalez.13/elo312/css/bootstrap.min.css' rel='stylesheet'>"

        "<link href='http://aragorn.elo.utfsm.cl/~jairo.gonzalez.13/elo312/css/custom.css' rel='stylesheet'>"

        "<link href='http://fonts.googleapis.com/css?family=Open+Sans:300italic,400italic,600italic,700italic,800italic,400,300,600,700,800"
        "rel='stylesheet' type='text/css'>"
        
    "</head>"

    "<body>"

        "<nav id='siteNav' class='navbar navbar-default navbar-fixed-top' role='navigation'>"
            "<div class='container'>"
                "<div class='navbar-header'>"
                    "<button type='button' class='navbar-toggle' data-toggle='collapse' data-target='#navbar'>"
                        "<span class='sr-only'>Toggle navigation</span>"
                        "<span class='icon-bar'></span>"
                        "<span class='icon-bar'></span>"
                        "<span class='icon-bar'></span>"
                    "</button>"
                    "<a class='navbar-brand' href='#'>"
                        "<span class='glyphicon glyphicon-cloud'></span> "
                        "G.A.T.I.T.O. | Tiempo en Sala B360"
                    "</a>"
                "</div>"
                "<div class='collapse navbar-collapse' id='navbar'>"
                    "<ul class='nav navbar-nav navbar-right'>"
                        "<li class='active'>"
                            "<a href='#'>Inicio</a>"
                        "</li>"
                        "<li>"
                            "<a href='#'>Acerca de</a>"
                        "</li>"
                        "<li class='dropdown'>";

const unsigned char html2[] =
                            "<a href='#' class='dropdown-toggle' data-toggle='dropdown' role='button' aria-haspopup='true' aria-expanded='false'>Relacionado <span class='caret'></span></a>"
                            "<ul class='dropdown-menu' aria-labelledby='about-us'>"
                                "<li><a href='http://www.staggeringbeauty.com/' target='_blank'>Sorpresa</a></li>"
                                "<li><a href='http://ducksarethebest.com/' target='_blank'>Sorpresa</a></li>"
                                "<li><a href='http://www.sanger.dk/' target='_blank'>Sorpresa</a></li>"
                                "<li><a href='http://www.koalastothemax.com/' target='_blank'>Sorpresa</a></li>"
                                "<li><a href='http://weirdorconfusing.com/' target='_blank'>Sorpresa</a></li>"
                            "</ul>"
                        "</li>"
                        "<li>"
                            "<a href='#'>Contacto</a>"
                        "</li>"
                    "</ul>"
                    
                "</div>"
            "</div>"
        "</nav>"

        "<header>"
            "<div class='header-content'>"
                "<div class='header-content-inner'>"
                    "<h1>Estado del Tiempo</h1>"
                    "<table style='font-size:30px' align='right'>"
                "<tr>"
                    "<td>"
                        "Temperatura:"
                    "</td>"
                    "<td align='right'>";

const unsigned char html3[] =
                    " &#176;C</td>"
                "</tr>"
                "<tr>"
                    "<td>"
                        "Luz:"
                    "</td>"
                    "<td align='right'>";

const unsigned char html4[] =
                    " cd</td>"
                "</tr>"
                "<tr>"
                    "<td>"
                        "Humedad:"
                    "</td>"           
                    "<td align='right'>";

const unsigned char html5[] =
                    " %</td>"
                "</tr>"
                "<tr>"
                    "<td>"
                        "Viento:"
                    "</td>"
                    "<td align='right'>";

const unsigned char html6[] =
                    " Km/h</td>"
                "</tr>"
                "<tr>"
                    "<td colspan='2' aling='center'>"
                    "------------------------------------------"
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td colspan='2' aling='center'>";

const unsigned char html7[] =
                    "</td>"
                "</tr>"
            "</table>"
                     "</div>"
            "</div>"
        "</header>"

        "<footer class='page-footer'>"
                
            "<div class='small-print'>"
                "<div class='container'>"
                    "<p>Sin Copyright &copy; gatitoprr.com 2016</p>"
                "</div>"
            "</div>"
            
        "</footer>"

        "<script src='http://aragorn.elo.utfsm.cl/~jairo.gonzalez.13/elo312/js/jquery-1.11.3.min.js'></script>"

        "<script src='http://aragorn.elo.utfsm.cl/~jairo.gonzalez.13/elo312/js/bootstrap.min.js'></script>"

        "<script src='http://aragorn.elo.utfsm.cl/~jairo.gonzalez.13/elo312/js/jquery.easing.min.js'></script>"

        "<script src='http://aragorn.elo.utfsm.cl/~jairo.gonzalez.13/elo312/js/custom.js'></script>"

    "</body>"

    "</html>";



/*  RTC Module Variables
*
***************************************************/
unsigned char tiempo[7];
char day[5] = "    ";


/*  Anemometer Variables
*
***************************************************/
float k = 0; //Contador. Cuenta numero de interrupciones por timer
int j = 0;  //Contador. Cuenta dos segundos, para que no cambie el valor de las mediciones en dos segundos
float muestra_v[3] = {0,0,0};
float x = 0; //Average value between last 3 measurements
float vel = 0; //Wind Velocity



/*  Luminosity Sensor Variables
*
***************************************************/
int vlum = 0; //Voltage input from luminosity sensor
float lum = 0; //Luminosity value in candela



/*  Temperature and Humidity Sensor Variables
*
***************************************************/
int toggle = 0; //Switch between temperature or humidity measuremente
float hum = 0; //Humidity value in percentage
float temp = 0; //Temperature value in celcius



/*  Main Program
*
***************************************************/
void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;  

    //Modules initialization
    osc_init_xt2();
    display_init();

    //Just a distraction
    display_lcd_clear(0x14,20);
    printf("    G.A.T.I.T.O");
    udelay(0xFFFF);
    udelay(0xFFFF);
    udelay(0xFFFF);
    udelay(0xFFFF);
    udelay(0xFFFF);
    udelay(0xFFFF);
    udelay(0xFFFF);

    i2c_init();
	spi_init();
	wiznet_init();
    sensor_init();
    adc_init_single_channel_single_conv();


    //Anemometer initialization in P1.0
    P1DIR &= ~BIT0;
    P1IE |= BIT0;
    P1IES &= ~BIT0;
    P1IFG &= ~BIT0;

    //Wiznet
    unsigned char sockstat;
    unsigned int rsize;
    int getidx;

    char string_temperature[10]=" ";
    char string_humidity[10]=" ";
    char string_luminocity[10]=" ";
    char string_windvelocity[10]=" ";
    char string_time[30]=" ";

    sockreg = 0; //select socket


    //Wiznet re-initialization by interrupt detected in P1.1
    P1DIR &= ~BIT1;
    P1IE |= BIT1;
    P1IES &= ~BIT1;
    P1IFG &= ~BIT1;
     

    //Timers initialization
    TACTL = TASSEL_1 + MC_1;
    TACCTL0 = CCIE;
    TACCR0 = 32768-1;
  
    //Wiznet main state-machine | socket comunnication
    while(1)
    {
        _BIS_SR(GIE); 
        sockstat=wiznet_read(S0_SR); //Read socket status
        switch(sockstat)
        {
            case SOCK_CLOSED:
                if (open(sockreg,MR_TCP,TCP_PORT) > 0)
                {
                    if (listen(sockreg) <= 0) // Listen to Socket 0
                        udelay(1);
                }
                break;
            case SOCK_ESTABLISHED:
                rsize=recv_size(); // Get the client request size
                if (rsize > 0)
                {
                    // Read the client Request
                    if (recv(sockreg,buf,rsize) <= 0)
                        break;
                    // Check the Request Header
                    getidx=strindex((char *)buf,"GET /");
                    if (getidx >= 0) 
                    {
                        //Send first segment of the website
                        if (send(sockreg,html1,strlen((char *)html1)) <= 0)
                            break;
                        if (send(sockreg,html2,strlen((char *)html2)) <= 0)
                            break;

                        // Create the HTTP Temperature Response
                        sprintf(string_temperature,"%1.1f",temp); // Convert temperature value to string
                        if (send(sockreg,(unsigned char const *)string_temperature,strlen(string_temperature)) <= 0)
                            break;

                        //Send intermediate segment of the website
                        if (send(sockreg,html3,strlen((char *)html3)) <= 0)
                            break;

                        // Create the HTTP Luminocity Response
                        sprintf(string_luminocity,"%1.1f",lum); // Convert luminocity value to string
                        if (send(sockreg,(unsigned char const *)string_luminocity,strlen(string_luminocity)) <= 0)
                            break;

                        //Send another intermediate segment of the website
                        if (send(sockreg,html4,strlen((char *)html4)) <= 0)
                            break;

                        // Create the HTTP Humidity Response
                        sprintf(string_humidity,"%1.1f",hum); // Convert humidity value to string
                        if (send(sockreg,(unsigned char const *)string_humidity,strlen(string_humidity)) <= 0)
                            break;

                        //And here: another intermediate segment of the website
                        if (send(sockreg,html5,strlen((char *)html5)) <= 0)
                            break;

                        // Create the HTTP Wind Velocity Response
                        sprintf(string_windvelocity,"%1.1f",vel); // Convert wind velocity value to string
                        if (send(sockreg,(unsigned char const *)string_windvelocity,strlen(string_windvelocity)) <= 0)
                            break;

                        //Yep. Another intermediate segment of the website
                        if (send(sockreg,html6,strlen((char *)html6)) <= 0)
                            break;

                        // Create the HTTP Time Response
                        sprintf(string_time,"%s%x%x/%x%x/%d- %x%x:%x%x:%x%x",day,(tiempo[4]&0xF0)>>4,(tiempo[4]&0x0F),
                            (tiempo[5]&0xF0)>>4,(tiempo[5]&0x0F),tiempo[6]+2016,(tiempo[2]&0xF0)>>4,(tiempo[2]&0x0F),
                            (tiempo[1]&0xF0)>>4,(tiempo[1]&0x0F),(tiempo[0]&0xF0)>>4,(tiempo[0]&0x0F)); // Convert time value to string
                        if (send(sockreg,(unsigned char const *)string_time,strlen(string_time)) <= 0)
                            break;

                        //Finally! Last segment of the website
                        if (send(sockreg,html7,strlen((char *)html7)) <= 0)
                            break;
                    }
                    // Disconnect the socket
                    disconnect(sockreg);
                }
                else
                    udelay(10);    // Wait for request
                break;
            case SOCK_CLOSE_WAIT:
                close(sockreg); //Close socket
            break;
        }
    }
    
}



/*  Interrupt rutine
*
***************************************************/

//Anemometer
#pragma vector= PORT1_VECTOR                
__interrupt void port1_interrupt (void){
    if (P1IFG & BIT1)
    {
        wiznet_init(); //Wiznet reconfiguration
        P1IFG&=~BIT1;
    }
    else
    {
        k++; // 1 interruption, k+=1
        P1IFG&=~BIT0;                 
    }
    
}

//RTC Module
#pragma vector=USART0TX_VECTOR
__interrupt void I2C_ISR(void)
{
    static int i=0;
    switch(I2CIV)
    {
        case  0: break;             // No interrupt
        case  2: break;             // Arbitration lost
        case  4: break;             // No Acknowledge
        case  6: break;             // Own Address
        case  8: break;             // Register Access Ready
        case 10:                    // Receive Ready

            tiempo[i] = I2CDRB;
            i++;
            if(i==7)  i=0;  
            break;
             
        case 12: break;             // Transmit Ready                  
        case 14: break;             // General Call
        case 16: break;             // Start Condition
    }
  
}

//Measurement
#pragma vector = TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
    //Anemometer measurement:

    //We record interruptions to estimate frecuency
    muestra_v[2] = muestra_v[1]; //Save last 3 values
    muestra_v[1] = muestra_v[0];
    muestra_v[0] = k;

    if( j == 2) {
        k = 0;
        j = 0;
    }
    else
        j++;

    x = (muestra_v[0]+muestra_v[1]+muestra_v[2])/3; //Average value:

    vel = x*FACTOR_VEL; //Wind velocity value


    //Luminosity measurement
    vlum = adc_single_channel_single_conv(0);
    lum = vlum*FACTOR_LUM;

    //Temperature and humidity measurement
    //We toggle between measuremets to avoid sensor overheatings
    if (toggle){
        temp = sensor_get_t();
        toggle = 0;
    }
    else{
        hum = sensor_get_h();
        toggle = 1;
    }

    //Time measurement
    i2c_start_r(0x00);
    i2c_read(0x07);

    display_lcd_clear(0x00,20);
    switch(tiempo[3])
    {
    case 1:
        strcpy(day,"Lun ");
        break;
    case 2:
        strcpy(day,"Mar ");
        break;
    case 3:
        strcpy(day,"Mie ");
        break;
    case 4:
        strcpy(day,"Jue ");
        break;
    case 5:
        strcpy(day,"Vie ");
        break;
    case 6:
        strcpy(day,"Sab ");
        break;
    case 7:
        strcpy(day,"Dom ");
        break;
    }

    //Display values
    printf("%s%x%x/%x%x/%d",day,(tiempo[4]&0xF0)>>4,(tiempo[4]&0x0F),(tiempo[5]&0xF0)>>4,(tiempo[5]&0x0F),tiempo[6]+2016);
    printf("\n%x%x:%x%x:%x%x",(tiempo[2]&0xF0)>>4,(tiempo[2]&0x0F),(tiempo[1]&0xF0)>>4,(tiempo[1]&0x0F),(tiempo[0]&0xF0)>>4,(tiempo[0]&0x0F));
    display_lcd_clear(0x14,20);
    printf("V=%1.1fKm/H L=%1.1fcd",vel,lum);
    display_lcd_clear(0x54,20);
    printf("T=%1.1f C H=%1.1f %%",temp,hum);
}


