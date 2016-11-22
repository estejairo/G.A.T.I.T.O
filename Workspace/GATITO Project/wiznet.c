/* Nombre del modulo: wiznet.c
*
* Escrito inicialmente el 3/11/2016 Por Jairo González
*
* Descripción del módulo:
* Modulo driver Wiznet.
* Contiene las funciones que permiten manejar el
* modulo Wiznet WIZ811MJ
***************************************************/

/*  Include section
*
***************************************************/
#include "msp430_version.h"// Depende del uC que Ud. esté ocupando.
#include "wiznet.h"
#include "stdio.h"
#include "spi.h"
#include "display.h"

/*  Local Function Prototype Section
*
***************************************************/
void wiznet_init(void);
void wiznet_write(unsigned short int addr, unsigned char data);
unsigned char wiznet_read(unsigned short int addr);


/**************************************************
* Nombre    		: void wiznet_init(void)
* returns			: void
* Creada por		: Jairo González
* Fecha creación	: 3/11/2016
* Descripción		: Inicializa el modulo Wiznet
**************************************************/
void wiznet_init()
{
	// Ethernet Setup
	unsigned char mac_addr[] = {0x00,0x16,0x36,0xDE,0x58,0xF6};
	unsigned char ip_addr[] = {192,168,0,25};
	unsigned char sub_mask[] = {255,255,255,0};
	unsigned char gtw_addr[] = {192,168,0,1};
    //printf("wiznet_init();\n");
	//Setting the Wiznet W5100 Mode Register: 0x0000
	wiznet_write(MR,0x80);            // MR = 0b10000000;
	udelay(2000);
	//printf("Reading MR: %d\n",wiznet_read(MR));

	//Setting the Wiznet W5100 Gateway Address (GAR): 0x0001 to 0x0004
	//printf("Setting Gateway\nAddress\n%d.%d.%d.%d\n",gtw_addr[0],gtw_addr[1],gtw_addr[2],gtw_addr[3]);
	wiznet_write(GAR + 0,gtw_addr[0]);
	wiznet_write(GAR + 1,gtw_addr[1]);
	wiznet_write(GAR + 2,gtw_addr[2]);
	wiznet_write(GAR + 3,gtw_addr[3]);
	udelay(2000);
	//printf("Reading GAR:\n%d.%d.%d.%d\n",wiznet_read(GAR + 0),wiznet_read(GAR + 1),wiznet_read(GAR + 2),wiznet_read(GAR + 3));

	// Setting the Wiznet W5100 Source Address Register (SAR): 0x0009 to 0x000E
	//printf("Setting Source\nAddress\n%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",mac_addr[0],mac_addr[1],mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5]);
	wiznet_write(SAR + 0,mac_addr[0]);
	wiznet_write(SAR + 1,mac_addr[1]);
	wiznet_write(SAR + 2,mac_addr[2]);
	wiznet_write(SAR + 3,mac_addr[3]);
	wiznet_write(SAR + 4,mac_addr[4]);
	wiznet_write(SAR + 5,mac_addr[5]);
	udelay(2000);
	//printf("Reading SAR:\n%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",wiznet_read(SAR + 0),wiznet_read(SAR + 1),wiznet_read(SAR + 2),wiznet_read(SAR + 3),wiznet_read(SAR + 4),wiznet_read(SAR + 5));

	// Setting the Wiznet W5100 Sub Mask Address (SUBR): 0x0005 to 0x0008
	//printf("Setting Sub\nMask Address\n%d.%d.%d.%d\n",sub_mask[0],sub_mask[1],sub_mask[2],sub_mask[3]);
	wiznet_write(SUBR + 0,sub_mask[0]);
	wiznet_write(SUBR + 1,sub_mask[1]);
	wiznet_write(SUBR + 2,sub_mask[2]);
	wiznet_write(SUBR + 3,sub_mask[3]);
	udelay(2000);
	//printf("Reading SUBR:\n%d.%d.%d.%d\n",wiznet_read(SUBR + 0),wiznet_read(SUBR + 1),wiznet_read(SUBR + 2),wiznet_read(SUBR + 3));

	// Setting the Wiznet W5100 IP Address (SIPR): 0x000F to 0x0012
	//printf("Setting IP\nAddress\n%d.%d.%d.%d\n",ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3]);
	wiznet_write(SIPR + 0,ip_addr[0]);
	wiznet_write(SIPR + 1,ip_addr[1]);
	wiznet_write(SIPR + 2,ip_addr[2]);
	wiznet_write(SIPR + 3,ip_addr[3]);
	udelay(2000);
	//printf("Reading SIPR:\n%d.%d.%d.%d\n",wiznet_read(SIPR + 0),wiznet_read(SIPR + 1),wiznet_read(SIPR + 2),wiznet_read(SIPR + 3));

	// Setting the Wiznet W5100 RX and TX Memory Size, we use 2KB for Rx/Tx 4 channels
	//printf("Setting Wiznet\nRMSR and TMSR\n");
	wiznet_write(RMSR,0x55);
	wiznet_write(TMSR,0x55);
	//printf("Done Wiznet\nW5100 Initialized!");
}

/**************************************************
* Nombre    		: void wiznet_write(unsigned short int arg1, unsigned char arg2)
* returns			: void
* arg1				: Dirección del registro a escribir
* arg2				: Dato a ser escrito
* Creada por		: Jairo González
* Fecha creación	: 3/11/2016
* Descripción		: Escribe un registro en el
* modulo Wiznet.
**************************************************/
void wiznet_write(unsigned short int addr, unsigned char data)
{
	spi_slave_enable();
	spi_send(WIZNET_WRITE_OPCODE);
	spi_send((addr & 0xFF00)>>8);
	spi_send(addr & 0x00FF);
	spi_send(data);
	spi_end_transmission();
	spi_slave_disable();
}

/**************************************************
* Nombre    		: unsigned char wiznet_read(unsigned int arg1)
* returns			: unsigned char
* arg1				: Dirección del registro a leer
* Creada por		: Jairo González
* Fecha creación	: 3/11/2016
* Descripción		: Lee un registro del
* modulo Wiznet.
**************************************************/
unsigned char wiznet_read(unsigned short int addr)
{
	spi_slave_enable();
	spi_send(WIZNET_READ_OPCODE);
	spi_send((addr & 0xFF00)>>8);
	spi_send(addr & 0x00FF);
	spi_send(0x00); //Send nothing
	spi_end_transmission();
	spi_slave_disable();
	return RXBUF1;
}

/**************************************************
* Nombre    		: unsigned char open(unsigned char arg1, unsigned char arg2, unsigned char arg3)
* returns			: unsigned char
* arg1				: Numero del socket
* arg2				: Protocolo Ethernet
* arg3				: Puerto
* Creada por		: Jairo González
* Fecha creación	: 13/11/2016
* Descripción		: Abre socket 0
**************************************************/
unsigned char open(unsigned char sock, unsigned char eth_protocol, unsigned char tcp_port)
{
	unsigned char retval = 0;
	if (sock != 0)
		return retval;

	// Make sure we close the socket first
	if (wiznet_read(S0_SR) == SOCK_CLOSED) {
     	close(sock);
    }

     // Assign Socket 0 Mode Register
    wiznet_write(S0_MR,eth_protocol);

    // Now open the Socket 0
    wiznet_write(S0_PORT,((tcp_port & 0xFF00) >> 8 ));
    wiznet_write(S0_PORT + 1,(tcp_port & 0x00FF));
    wiznet_write(S0_CR,CR_OPEN); // Open Socket

    // Wait for Opening Process
    while(wiznet_read(S0_CR));

    // Check for Init Status
    if (wiznet_read(S0_SR) == SOCK_INIT)
     	retval=1;
    else
     	close(sock);

  	return retval;
}

/**************************************************
* Nombre    		: unsigned char listen(unsigned char arg1)
* returns			: unsigned char
* arg1				: Numero del socket
* Creada por		: Jairo González
* Fecha creación	: 13/11/2016
* Descripción		: Mantiene en escucha el socket
**************************************************/
unsigned char listen(unsigned char sock)
{
	unsigned char retval = 0;
	if (sock != 0)
		return retval;

	if (wiznet_read(S0_SR) == SOCK_INIT) 
	{
		// Send the LISTEN Command
		wiznet_write(S0_CR,CR_LISTEN);

		// Wait for Listening Process
		while(wiznet_read(S0_CR));

		// Check for Listen Status
		if (wiznet_read(S0_SR) == SOCK_LISTEN)
			retval=1;
		else
			close(sock);
    }
    return retval;
}

/**************************************************
* Nombre    		: unsigned int send(unsigned char arg1, const unsigned char agr2, unsigned int arg3)
* returns			: unsigned int
* arg1				: Numero del socket
* arg2				: Puntero al primer dato a enviar
* arg3				: Largo del mensaje a enviar
* Creada por		: Jairo González
* Fecha creación	: 13/11/2016
* Descripción		: Envia datos al cliente
**************************************************/
unsigned int send(unsigned char sock, const unsigned char *buf, unsigned int buflen)
{
    unsigned int ptr,offaddr,realaddr,txsize,timeout;   

    if (buflen <= 0 || sock != 0)
    	return 0;

    // Make sure the TX Free Size Register is available
    txsize=wiznet_read(SO_TX_FSR);
    txsize=(((txsize & 0x00FF) << 8 ) + wiznet_read(SO_TX_FSR + 1));

    timeout=0;
    while (txsize < buflen)
    {
		udelay(1);
		txsize=wiznet_read(SO_TX_FSR);
		txsize=(((txsize & 0x00FF) << 8 ) + wiznet_read(SO_TX_FSR + 1));

		// Timeout for approx 1000 ms
		if (timeout++ > 1000)
		{
			// Disconnect the connection
			disconnect(sock);
			return 0;
		}
   }	

   // Read the Tx Write Pointer
   ptr = wiznet_read(S0_TX_WR);
   offaddr = (((ptr & 0x00FF) << 8 ) + wiznet_read(S0_TX_WR + 1));

   while(buflen)
   {
      buflen--;

      // Calculate the real W5100 physical Tx Buffer Address
      realaddr = TXBUFADDR + (offaddr & TX_BUF_MASK);

      // Copy the application data to the W5100 Tx Buffer
      wiznet_write(realaddr,*buf);
      offaddr++;
      buf++;
    }

    // Increase the S0_TX_WR value, so it point to the next transmit
    wiznet_write(S0_TX_WR,(offaddr & 0xFF00) >> 8 );
    wiznet_write(S0_TX_WR + 1,(offaddr & 0x00FF));	

    // Now Send the SEND command
    wiznet_write(S0_CR,CR_SEND);

    // Wait for Sending Process
    while(wiznet_read(S0_CR));	

    return 1;
}


/**************************************************
* Nombre    		: unsigned int recv(unsigned char arg1, const unsigned char arg2, unsigned int arg3)
* returns			: unsigned int
* arg1				: Numero del socket
* arg2				: Puntero al primer dato a enviar
* arg3				: Largo del mensaje a enviar
* Creada por		: Jairo González
* Fecha creación	: 13/11/2016
* Descripción		: Recibe datos del cliente
**************************************************/
unsigned int recv(unsigned char sock,unsigned char *buf,unsigned int buflen)
{
    unsigned int ptr,offaddr,realaddr;   	

    if (buflen <= 0 || sock != 0) return 1;   

    // If the request size > MAX_BUF,just truncate it
    if (buflen > MAX_BUF)
    	buflen=MAX_BUF - 2;
    
    // Read the Rx Read Pointer
    ptr = wiznet_read(S0_RX_RD);
    offaddr = (((ptr & 0x00FF) << 8 ) + wiznet_read(S0_RX_RD + 1));

    while(buflen)
    {
    	buflen--;
      	realaddr=RXBUFADDR + (offaddr & RX_BUF_MASK);
      	*buf = wiznet_read(realaddr);
      	offaddr++;
      	buf++;
    }

    *buf='\0'; // String terminated character

    // Increase the S0_RX_RD value, so it point to the next receive
    wiznet_write(S0_RX_RD,(offaddr & 0xFF00) >> 8 );
    wiznet_write(S0_RX_RD + 1,(offaddr & 0x00FF));	

    // Now Send the RECV command
    wiznet_write(S0_CR,CR_RECV);
    udelay(5);    // Wait for Receive Process

    return 1;
}

/**************************************************
* Nombre    		: unsigned int recv_size(void)
* returns			: unsigned int
* Creada por		: Jairo González
* Fecha creación	: 13/11/2016
* Descripción		: Recibe tamaño del dato a recibir
**************************************************/
unsigned int recv_size(void)
{
  return ((wiznet_read(S0_RX_RSR) & 0x00FF) << 8 ) + wiznet_read(S0_RX_RSR + 1);
}

/**************************************************
* Nombre    		: void close(unsigned char arg1)
* returns			: void
* arg1				: Numero del socket
* Creada por		: Jairo González
* Fecha creación	: 13/11/2016
* Descripción		: Cierra el socket indicado
**************************************************/
void close(unsigned char sock)
{
   if (sock != 0) return;

   // Send Close Command
   wiznet_write(S0_CR,CR_CLOSE);
   // Waiting until the S0_CR is clear
   while(wiznet_read(S0_CR));
}


/**************************************************
* Nombre    		: void disconnect(unsigned char arg1)
* returns			: void
* arg1				: Numero del socket
* Creada por		: Jairo González
* Fecha creación	: 13/11/2016
* Descripción		: Termina la comunicación en el socket
**************************************************/
void disconnect(unsigned char sock)
{
   if (sock != 0) return;
   // Send Disconnect Command
   wiznet_write(S0_CR,CR_DISCON);
   // Wait for Disconecting Process
   while(wiznet_read(S0_CR));
}