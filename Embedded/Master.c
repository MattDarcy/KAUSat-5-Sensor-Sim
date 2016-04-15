#include <ina90.h>      //General header file needed for ATMEL MCU's
#include <iom128.h>     //General header file needed for ATMEL MCU's
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     //Needed for printf
#include <math.h>

/* TWBR - TWI Bit Rate Register */     
#define TWBR7   7         //TWI Bit Rate Register,                 CPU Clock Frequency (16 MHz)
#define TWBR6   6         //TWI Bit Rate Register  SCL Frequency = ----------------------------
#define TWBR5   5         //TWI Bit Rate Register                  16 + 2(TWBR) * 4^(TWPS) 
#define TWBR4   4         //TWI Bit Rate Register
#define TWBR3   3         //TWI Bit Rate Register      TWBR = Value of TWI Bit Rate Register
#define TWBR2   2         //TWI Bit Rate Register      TWPS = Value of prescaler bits in TWI Status Register
#define TWBR1   1         //TWI Bit Rate Register
#define TWBR0   0         //TWI Bit Rate Register

/* TWCR - TWI Control Register */
#define TWINT   7         //TWI Interrupt Flag - while set, SCL low period is stretched. Clearing is with written 1, and starts TWI!
#define TWEA    6         //TWI Enable Acknowledge - If written to 1, ACK pulse is generated if the conditions are met. Writing to 0 disconnects device
#define TWSTA   5         //TWI Start Condition Bit - Written 1 when desired to become a master. If bus is free, generates start. Otherwise waits for stop to generate start as master. 
                          //MUST BE CLEARED BY SOFTWARE WHEN THE START HAS BEEN TRANSMITTED
#define TWSTO   4         //TWI Stop Condition Bit - Written 1 in master mode generates a stop condition. In slave mode, does not generate stop but unaddresses and releases two wires to high-impedance state for error recovery
#define TWWC    3         //TWI Write Collision Flag - Set when attempting to write to the TWI data register - TWDR when TWINT is low. It is cleared by writing to TWDR when TWINT is high.
#define TWEN    2         //TWI Enable Bit - Enables this whole entire interface. When written to 1, TWI takes control over the I/O pins connected to the SCL and SDA pins. When 0, all operations are ceased.
//reserved
#define TWIE    0         //TWI Interrupt Enable - When this bit is set to 1, and the SREG I-bit (Global interrupt enable) is 1, the TWI interrupt request will be activated for as long as the TWINT flag is high

/* TWSR - TWI Status Register */
#define TWS7    7         //TWI Status. These bits reflect status of the TWI logic and I2C bus. See pg 215. 219. 221, 224 depending on mode in use
#define TWS6    6         //
#define TWS5    5         //
#define TWS4    4         //
#define TWS3    3         //
//reserved
#define TWPS1   1         //Prescaler bits. To check TWI status, make sure these are zero. 00 gives 1, 01 is 4, 10 is 16, 11 is 64.
#define TWPS0   0         //Prescaler bits. To check TWI status, make sure these are zero. 00 gives 1, 01 is 4, 10 is 16, 11 is 64.

/* TWDR - TWI Data Register */ 
#define TWD7   7         //TWI Data register
#define TWD6   6         //In transmit mode, TWDR contains the NEXT byte to be transmitted. 
#define TWD5   5         //In receiver mode, contains the LAST byte already received.
#define TWD4   4         //It is writable while the TWI is not in the process of byte-shifting
#define TWD3   3         //This occurs when the TWINT is set by hardware
#define TWD2   2         //CANNOT be initialized by the user before the first interrupt occurs.
#define TWD1   1         //Data inside remains stable so long as TWINT is set.
#define TWD0   0         //As data is shifted out, data on the bus is shifted in. 
                         //Always contains the last byte present on the bus, except after wake up from a sleep mode by the TWI interrupt
                         //In this case the contents of TWDR is undefined. 
                         //In the case of a lost bus arbitration, no data is lost in the transition from MASTER TO SLAVE. 
                         //Handling of ACK bit is controlled automatically by the TWI logic. CPU cannot access the ACK bit directly

/* TWAR - TWI SLA_Wress Register */
#define TWA6   7         //TWI Slave SLA_Wress Register. Determines how the slave will act, transmitting or receiving. 
#define TWA5   6         //Not needed in master modes.
#define TWA4   5         //TWAR must be set in multimaster systems in masters which can be addressed as slaves by other masters.
#define TWA3   4         //The LSB of TWAR is used to enable recognition of the general call address ($00). There is an associated address comparator that looks for the slave address
#define TWA2   3         //or general call address if enabled in the received serial address. If match is found, interrupt request is generated
#define TWA1   2         //
#define TWA0   1         //
#define TWGCE  0         //Enables recognition of a general call given over the I2C bus.

//ACK AND NACK
//During SLA+R/W ACK & NAK are used to acknowledge that a device is present/ready to be accessed. ACK indicates the device is present/ready, while NAK represents device is not present, or is currently busy and not ready.
//During the data transfer phase, ACK is used to indicate that the receiving device is ready to receive the next byte, and NAK means that the receiving device cannot receive any more data, and the transaction should end.

unsigned int START = 0x08;
unsigned int MT_SLA_ACK = 0x18;
unsigned int MT_DATA_ACK = 0x28;
unsigned int MR_DATA_ACK = 0x40;
unsigned int MR_REC_NACK = 0x58;
unsigned int i; 
unsigned int Sent_data = 0x31;     //ASCII for the number 1 in decimal format
unsigned char Received_data = 0;

void init_UART(void) //Set desired registers according to the ATMEGA128 Datasheet
{
    DDRE = 0xFB; //1111 1000
    UCSR0A = 0x00;
    UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);//0x98;
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); 
    UBRR0H = 0x00; //set baud rate hi
    UBRR0L = 103; //set baud rate lo  
  
  //DDRE=0xfe; //pe1-tx, pe0-rx
    //DDRD = 0xFB; //1111 1011
    //UCSR1A = 0x00;
    //UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1);//0x98;
   // UCSR1C = (1<<UCSZ11)|(1<<UCSZ10); 
    //UBRR1H = 0x00; //set baud rate hi
   // UBRR1L = 103; //set baud rate lo
}

void ms_delay(unsigned int cnt) //Set a small delay so that clean packets are processed
{
  for(i=0;i<cnt;i++)
  {
  __delay_cycles(16000); 
  }
}

int putchar(int data_zigbee)//Copy packet for processing, "clean the mailbox" for a new packet being delivered later
{
  while (((UCSR0A >> 5) & 0x01) == 0) // while UDRE data register is empty
            ;  //Do nothing
  UDR0 = data_zigbee; //When not empty, set UDR1 to the data packet
  UCSR0A |= 0x20; //set UCSR1A (USART Control and Status Register 1 for Port A) to 0010 0000, default is 0010 0000, as in the data register is empty!
                  //In other words, imagine you have a mailbox where only 1 packet can go at a time. You copied the delivered packet and sent it to processing in the house, but you need to clean out the original packet
                  //from the data register
  return data_zigbee; //send that copied packet to be processed. Returns to the interrupt.
}

void TWI_init(void)
{//Whatever is done with the bitrate, the CPU clock frequency in the slave must be AT LEAST
  //16 times higher than the SCL frequency
  TWBR = 12;                                       //      SCL frequency = CPU Clock Frequency (16000000 Hz)
 //                                                                      --------------------------------
 //                                                                        16 + 2(TWBR) * 4^(TWPS)
 // with TWBR at 12 and TWPS at 0, SCL becomes 142857 or 152.857kHz. CPU clock of slave/SCL is 112.
  TWSR = 0x00;           //Sets the status register TWSR to all zeros
  
}

void TWI_MASTER_TRANSMIT(unsigned char Data, unsigned char SLA_W) //1010 1010 and 1010 0000
{

printf("MASTER TRANSMIT MODE\n\r");
ms_delay(1000);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);      //Send start condition
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
while (!(TWCR & (1<<TWINT)));  //Wait for TWINT flag set. This indicates start transmitted
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if((TWSR & 0xF8)!=START) //Check value of TWI status register. Mask prescaler bits. 
  {                       //If status different from start go to error
    printf("ERROR - MT START\n\r"); 
  } 
  else
  {
    printf("SUCCESS - MT START\n\r");
  }
  ms_delay(1000);
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  TWDR = SLA_W; //Load slave address into TWDR register. 
  TWCR = (1<<TWINT)|(1<<TWEN);//Clear Twint bit in TWCR to start transmission of address
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  while (!(TWCR & (1<<TWINT)));  //Wait for TWINT flag set. This indicates slave address transmitted and ACK/NACK received
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if((TWSR & 0xF8)!=MT_SLA_ACK)       //check value of TWI status register. Mask prescaler bits. 
                                      //Generally before and after data is sent we do a check. Hence the repeated.
                                      //For multiple data transfers, there will be a check in between each and one at the tail end.
  {                                   //If different from MT_SLA_ACK go to error
    printf("ERROR - MT SLAVE ACK\n\r");
  }
  else
  {
    printf("SUCCESS - MT SLAVE ADDRESS ACK\n\r");
  }  
  ms_delay(1000);
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  TWDR = Data;//Set data to the data that we want to send (0x31 = the number 1.)
  TWCR = (1<<TWINT)|(1<<TWEN);//Clear TWINT bit in TWCR to start transmission of data
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  while (!(TWCR & (1<<TWINT))); //Wait for TWINT flag to be set, this indicates data is transmitted and ack/nack received
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ms_delay(1000);
  if((TWSR & 0xF8)!=MT_DATA_ACK) //check value of TWI status register, mask prescaler bits. 
  {                              //If status different from MT_DATA_ACK go to error
    printf("ERROR - MT SLAVE DATA ACK\n\n\r");
  }
  else
  {
    printf("SUCCESS - MT SLAVE DATA ACK\n\r");
  }  
  ms_delay(1000);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);//Transmit stop condition
  ms_delay(100);
  printf("I gave the number %c\n\n\r",Data);
}

unsigned char TWI_MASTER_RECEIVE(unsigned char SLA_R)
{ 
  printf("MASTER RECEIVE MODE\n\r");
  unsigned char Data;
  //TWAR = 0x01;
  //DDRD = 0x01;
  TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  while (!(TWCR & (1<<TWINT)));
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if((TWSR & 0xF8)!=START)              
  {                                 
    printf("ERROR - MR START\n\r");                        
  }
  else                                  
  { 
    printf("SUCCESS - MR START\n\r");                      
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  TWDR = 0x00;   //need to write SLA_R to TWDR then twint and twen are reset in next step. then status codes can be had.
  //TWDR = 0x01;
  //DDRD = 0x01;
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
  TWCR = (1<<TWINT)|(1<<TWEN);
  ms_delay(2000);
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  while (!(TWCR & (1<<TWINT)));
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if((TWSR & 0xF8)!=MR_DATA_ACK)    
  {
    printf("ERROR - MR SLAVE ADDRESS ACK\n\r");                               
  }
  else                                         
  {
    printf("SUCCESS - MR SLAVE ADDRESS ACK\n\r");                  
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Data = TWDR;
  TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ms_delay(200);            //The following NACK check goes haywire without this delay.
  
  if((TWSR & 0xF8)!=MR_REC_NACK)                     
  {
    printf("ERROR - MR SLAVE DATA N-ACK\n\r");   
                                                                  
  }
  else                                        
  {
    printf("SUCCESS - MR SLAVE DATA N-ACK\n\r");
  }  
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN); 
  printf("I received the number %c\n\r",Data);
  return Data;
  
}

void main(void)
{       
	init_UART(); 
        TWI_init();
	//while(1){   
        TWI_MASTER_TRANSMIT(Sent_data,0x00); //0x31 is the number 1 in ASCII and general call address (all slaves)
        Received_data = TWI_MASTER_RECEIVE(0x00);
        Sent_data = Received_data;
        //}
}

  