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

/* TWAR - TWI Address Register */
#define TWA6   7         //TWI Slave Address Register. Determines how the slave will act, transmitting or receiving. 
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

unsigned int START = 0x60;
unsigned int SR_DATA_ACK = 0x80;
unsigned int SR_STO_NACK = 0xA0;
unsigned int SW_ADD_ACK = 0xA8;
unsigned int SW_DATA_NACK = 0xC0;
unsigned int i; 

void init_UART(void) //Set desired registers according to the ATMEGA128 Datasheet
{
    DDRE = 0xFB; //1111 1011
    UCSR0A = 0x00;
    UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);//0x98;
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); 
    UBRR0H = 0x00; //set baud rate hi
    UBRR0L = 103; //set baud rate lo
  
  
  
    //DDRD = 0xFB; //1111 1011
    //UCSR1A = 0x00;
    //UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1);//0x98;
    //UCSR1C = (1<<UCSZ11)|(1<<UCSZ10); 
    //UBRR1H = 0x00; //set baud rate hi
    //UBRR1L = 103; //set baud rate lo
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

unsigned char TWI_SLAVE_RECEIVE(unsigned char MAS_R)        //Sent 0x02 which is 0000 0010 user-made
{
  
  unsigned char Data;
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  TWAR = MAS_R;                                                          
  TWCR = (1<<TWEA)|(1<<TWEN);           
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
  while (!(TWCR & (1<<TWINT)));   //Wait for TWINT flag set to 1. Indicates its own slave address and direction bit (read 1/write 0) has been received
                                  //If the direction bit is 0 (write) slave will receive.
  printf("SLAVE RECEIVE MODE\n\r");
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if((TWSR & 0xF8)!=START)               //0x60 indicates Master has addressed the slave (SLA+W) and acknowledge has been returned by slave
  { 
    printf("ERROR - SR SLA+W RECEIVE AND SEND ACK\n\r");                        
  }
  else                                  
  { 
    printf("SUCCESS - SR SLA+W RECEIVE AND SEND ACK\n\r");                      
  }
  ms_delay(1000);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
  TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN); 
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  while (!(TWCR & (1<<TWINT)));   
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if((TWSR & 0xF8)!=SR_DATA_ACK)     //ACK is something that microcontrollers can use with each other AND we can see it.  
  {
    printf("ERROR - SR DATA RECEIVE AND SEND ACK\n\r");                               
  }
  else                                         
  {
    printf("SUCCESS - SR DATA RECEIVE AND SEND ACK\n\r");                  
  }
  ms_delay(1000);
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Data = TWDR;   //Sent from the master code to slave during the previous step                             
  TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);    
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ms_delay(1200);            //The following NACK check goes haywire without this delay.
  if((TWSR & 0xF8)!=SR_STO_NACK)   //In response to the stop condition issued by the master code,                    
  {
    printf("ERROR - SR STOP OR REPEATED START RECEIVE\n\r");   //NACK is something only we can see. It means that 
                                      //at the current time no data is being received by the slave from the master.                             
  }
  else                                        
  {
    printf("SUCCESS - SR STOP OR REPEATED START RECEIVE\n\r");
  }  
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN); 
  printf("I received the number %c\n\n\r",Data);
  return Data;
  
}
void TWI_SLAVE_TRANSMIT(unsigned char MAS_W,unsigned char Data)
{ 

  printf("SLAVE TRANSMIT MODE\n\r");
  TWAR = MAS_W;
  TWCR = ((1<<TWEN)|(1<<TWEA));
  while(!(TWCR&(1<<TWINT)));
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if((TWSR & 0xF8)!=SW_ADD_ACK) 
  {                      
   printf("ERROR - ST SLA+R RECEIVE AND SEND ACK\n\r");                        
  }
  else                                  
  { 
    printf("SUCCESS - ST SLA+R RECEIVE AND SEND ACK\n\r");
  }
  ms_delay(1000);
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  TWDR = Data;
 
  ms_delay(1000);
  TWCR = ((1<<TWINT)|(1<<TWEN)|(1<<TWEN));
  while(!(TWCR&(1<<TWINT)));
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if((TWSR & 0xF8)!=SW_DATA_NACK) 
  {                      
    printf("ERROR - ST DATA SEND AND RECEIVE ACK\n\r"); 
  } 
  else
  {
    printf("SUCCESS - ST DATA SEND AND RECEIVE ACK\n\r");
  }
  ms_delay(1000);
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  TWCR = ((1<<TWINT)|(1<<TWSTO)|(TWEN));
  ms_delay(1000);
  printf("I sent the number %c\n\r",Data);

}

void main(void)
{   
    unsigned char Temp=0;         //creates an unsigned char called Temp and makes it 0.
    unsigned char Oper=0;
    init_UART();                //initiates serial
    //while(1){
      TWI_init();                   //initiates TWI bus
      Temp = TWI_SLAVE_RECEIVE(0x00);    //infinite loop of sending 0x02 to slave_r function. Sets Temp to the returned "Data" from slave_r
      Oper = (Temp + 0x01);
      ms_delay(1000);
      TWI_SLAVE_TRANSMIT(0x01,Oper);       //Adds 1 to the data (it is in hex so be careful, use ASCII chart) and send back. 
}




