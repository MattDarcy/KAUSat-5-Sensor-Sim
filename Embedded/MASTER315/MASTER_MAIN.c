/*****************************************************************************
*
* Atmel Corporation
*
* File              : main.c
* Compiler          : IAR EWAAVR 2.28a/3.10c
* Revision          : $Revision: 1.13 $
* Date              : $Date: 24. mai 2004 11:31:20 $
* Updated by        : $Author: ltwa $
*
* Support mail      : avr@atmel.com
*
* Supported devices : All devices with a TWI module can be used.
*                     The example is written for the ATmega16
*
* AppNote           : AVR315 - TWI Master Implementation
*
* Description       : Example of how to use the driver for TWI master 
*                     communication.
*                     This code reads PORTD and sends the status to the TWI slave.
*                     Then it reads data from the slave and puts the data on PORTB.
*                     To run the example use STK500 and connect PORTB to the LEDS,
*                     and PORTD to the switches. .
*
****************************************************************************/

#include <ioavr.h>                                                                                              //Redundant, already in TWI_Master.c
#include <inavr.h>                                                                                              //Redundant, already in TWI_Master.c  
//#include "MASTER_TWI.h"   
#include <stdio.h>
#include "MASTER_TWI.c"                                                                                         //Include all the things



// ADCS BOARD CALLS
// ADCS Board will send over 2 bytes and depending on what it is, the slave (simulator) needs to send out the corresponding data byte (one half of a data reading)
// Via the data line on I2C
#define MPU6050_RA_GYRO_XOUT_H      0x43
#define MPU6050_RA_GYRO_XOUT_L      0x44
#define MPU6050_RA_GYRO_YOUT_H      0x45
#define MPU6050_RA_GYRO_YOUT_L      0x46
#define MPU6050_RA_GYRO_ZOUT_H      0x47
#define MPU6050_RA_GYRO_ZOUT_L      0x48


#define TWI_GEN_CALL         0x00  // The General Call address is 0                                             //General Call is 0 BUT TWAR must then by 0x01 to enable such a call

// Sample TWI transmission commands
#define TWI_CMD_MASTER_WRITE 0x10                                                                               //Seems to be random number 0001 0000
#define TWI_CMD_MASTER_READ  0x20                                                                               //Seems to be random number 0010 0000

// Sample TWI transmission states, used in the main application.
#define SEND_DATA             0x01                                                                              //Seems to be random number 0000 0001
#define REQUEST_DATA          0x02                                                                              //Seems to be random number 0000 0010
#define READ_DATA_FROM_BUFFER 0x03                                                                              //Seems to be random number 0000 0011


unsigned char j = 0x43;
unsigned int last_packet[48] = {0,0,0,0,0,0,0,0, //48 from 0 to 47...GyroxHi
			         0,0,0,0,0,0,0,0, //GyroxLo
			         0,0,0,0,0,0,0,0, //GyroyHi
			         0,0,0,0,0,0,0,0, //GyroyLo
			         0,0,0,0,0,0,0,0, //GyrozHi
			         0,0,0,0,0,0,0,0}; //GyrozLo
unsigned int new_packet[48] = {1,1,1,1,1,1,1,1, //48 from 0 to 47...GyroxHi
			        1,1,1,1,1,1,1,1, //GyroxLo
			        1,1,1,1,1,1,1,1, //GyroyHi
			        1,1,1,1,1,1,1,1, //GyroyLo
			        1,1,1,1,1,1,1,1, //GyrozHi
			        1,1,1,1,1,1,1,1}; //GyrozLo
unsigned int is_new_packet = 1;
unsigned int temp = 0;
unsigned int gyroxhi,gyroxlo,gyroyhi,gyroylo,gyrozhi,gyrozlo;
unsigned int last_byte_requested;

void init_UART(void) //Set desired registers according to the ATMEGA128 Datasheet
{
    DDRE = 0xFB; //1111 1000
    UCSR0A = 0x00;
    UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);//0x98;
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); 
    UBRR0H = 0x00; //set baud rate hi
    UBRR0L = 0x03; //set baud rate lo
    
 
  
  //DDRE=0xfe; //pe1-tx, pe0-rx
    //DDRD = 0xFB; //1111 1011
    //UCSR1A = 0x00;
    //UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1);//0x98;
   // UCSR1C = (1<<UCSZ11)|(1<<UCSZ10); 
    //UBRR1H = 0x00; //set baud rate hi
   // UBRR1L = 103; //set baud rate lo
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


unsigned char TWI_Act_On_Failure_In_Last_Transmission ( unsigned char TWIerrorMsg )
{                                                                                                               //TWIerrorMsg = TWI_Get_State_Info( ) which returns the variable TWI_state, which commonly refers
                                                                                                                //to TWSR except for when setting to 0xF8 RIGHT before a transmission from master to slave
                    // A failure has occurred, use TWIerrorMsg to determine the nature of the failure
                    // and take appropriate actions.
                    // Se header file for a list of possible failures messages.
                    
                    // Here is a simple sample, where if received a NACK on the slave address,
                    // then a retransmission will be initiated.
 
  if ( (TWIerrorMsg == TWI_MTX_ADR_NACK) | (TWIerrorMsg == TWI_MRX_ADR_NACK) )                                  //If the TWSR code is such that master received a NACK from slave after address
    TWI_Start_Transceiver();                                                                                    //Start transceiver (assumes data is already loaded in buffer and do repeated start)
    
  return TWIerrorMsg;                                                                                           //return the status code
}

void main(void)
{
  // This example code runs forever; sends a byte to the slave, then requests a byte
  // from the slave and stores it on PORTB, and starts over again. Since it is interupt
  // driven one can do other operations while waiting for the transceiver to complete.
  init_UART();
  printf("");
  unsigned char messageBuf[4];                                                                                  //messageBuf is 4 so 3 characters are useable 0, 1, and 2
  unsigned char TWI_targetSlaveAddress, temp, TWI_operation=0,                                                  //more unsigned chars for use later
                pressedButton, myCounter=0;
  //LED feedback port - connect port B to the STK500 LEDS                                                       //can use LEDS connect to port B
  DDRB  = 0xFF;
  PORTB = myCounter;
  //Switch port - connect portD to the STK500 switches
  DDRD  = 0x00;
  TWI_Master_Initialise();                                                                                      //initialize as per TWI_Master.c.....Bitrate, flood TWDR with 1's, enable TWI
  __enable_interrupt();                                                                                         //enable interrupts in general for the ATMEGA128
  TWI_targetSlaveAddress   = 0x10;     
  TWI_operation = REQUEST_DATA; // Set the next operation
  // Send initial data to slave. If not, output becomes eeeeeeeeeeeeeeeeeeeeeee
  messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
  messageBuf[1] = j;
  TWI_Start_Transceiver_With_Data( messageBuf, 2 );
  ms_delay(10); //NECESSARY OR THE SERIAL OUTPUT GETS MESSED UP
  printf("Sent 0x%X\n\r",messageBuf[1]);
  //In this time the slave receives the data from the buffer, operates it (+1) and puts in back in the buffer. 
  //Extracting this new data in buffer, printf'ing it, and then sending it back is the next step.
  
  for (;;)
  {
    
    j = 0x43;
    do 
    {
        //printf("j is 0x%X\n\r",j);
        // Check if the TWI Transceiver has completed an operation.
        if (!TWI_Transceiver_Busy()){
            // Check if the last operation was successful
            if (TWI_statusReg.lastTransOK){
                // Determine what action to take now
                if (TWI_operation == SEND_DATA){
                    // Send data to slave
                    messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
                    messageBuf[1] = j;
                    TWI_Start_Transceiver_With_Data( messageBuf, 2 );
                    ms_delay(10); //NECESSARY OR THE SERIAL OUTPUT GETS MESSED UP
                    //printf("Sent 0x%X\n\r",messageBuf[1]);
                    last_byte_requested = j;
                    TWI_operation = REQUEST_DATA; // Set next operation
                    }
                if (TWI_operation == REQUEST_DATA){
                    // Request data from slave
                    messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT);
                    TWI_Start_Transceiver_With_Data( messageBuf, 2 );
                    TWI_operation = READ_DATA_FROM_BUFFER; // Set next operation        
                    }
                if (TWI_operation == READ_DATA_FROM_BUFFER){
                    // Get the received data from the transceiver buffer
                    TWI_Get_Data_From_Transceiver( messageBuf, 2 );
                    PORTB = messageBuf[1];        // Store data on PORTB.
                    //printf("Received 0x%X\n\n\r",PORTB);
                    switch(j){
                        case MPU6050_RA_GYRO_XOUT_H:
                          gyroxhi = PORTB;
                          new_packet[0] = PORTB_Bit7;
                          new_packet[1] = PORTB_Bit6;
						  new_packet[2] = PORTB_Bit5;
						  new_packet[3] = PORTB_Bit4;
						  new_packet[4] = PORTB_Bit3;
						  new_packet[5] = PORTB_Bit2;
						  new_packet[6] = PORTB_Bit1;
						  new_packet[7] = PORTB_Bit0;
                        case MPU6050_RA_GYRO_XOUT_L:
                          gyroxlo = PORTB;
                          new_packet[8] = PORTB_Bit7;
                          new_packet[9] = PORTB_Bit6;
						  new_packet[10] = PORTB_Bit5;
						  new_packet[11] = PORTB_Bit4;
						  new_packet[12] = PORTB_Bit3;
						  new_packet[13] = PORTB_Bit2;
						  new_packet[14] = PORTB_Bit1;
						  new_packet[15] = PORTB_Bit0;
                        case MPU6050_RA_GYRO_YOUT_H:
                          gyroyhi = PORTB;
                          new_packet[16] = PORTB_Bit7;
                          new_packet[17] = PORTB_Bit6;
						  new_packet[18] = PORTB_Bit5;
						  new_packet[19] = PORTB_Bit4;
						  new_packet[20] = PORTB_Bit3;
						  new_packet[21] = PORTB_Bit2;
						  new_packet[22] = PORTB_Bit1;
						  new_packet[23] = PORTB_Bit0;
                        case MPU6050_RA_GYRO_YOUT_L:
                          gyroylo = PORTB;
                          new_packet[24] = PORTB_Bit7;
                          new_packet[25] = PORTB_Bit6;
						  new_packet[26] = PORTB_Bit5;
						  new_packet[27] = PORTB_Bit4;
						  new_packet[28] = PORTB_Bit3;
						  new_packet[29] = PORTB_Bit2;
						  new_packet[30] = PORTB_Bit1;
						  new_packet[31] = PORTB_Bit0;
                        case MPU6050_RA_GYRO_ZOUT_H:
                          gyrozhi = PORTB;
                          new_packet[32] = PORTB_Bit7;
                          new_packet[33] = PORTB_Bit6;
						  new_packet[34] = PORTB_Bit5;
						  new_packet[35] = PORTB_Bit4;
						  new_packet[36] = PORTB_Bit3;
						  new_packet[37] = PORTB_Bit2;
						  new_packet[38] = PORTB_Bit1;
						  new_packet[39] = PORTB_Bit0;
                        case MPU6050_RA_GYRO_ZOUT_L: 
                          gyrozlo = PORTB;
                          new_packet[40] = PORTB_Bit7;
                          new_packet[41] = PORTB_Bit6;
						  new_packet[42] = PORTB_Bit5;
						  new_packet[43] = PORTB_Bit4;
						  new_packet[44] = PORTB_Bit3;
						  new_packet[45] = PORTB_Bit2;
						  new_packet[46] = PORTB_Bit1;
						  new_packet[47] = PORTB_Bit0;
                        }
                    TWI_operation = SEND_DATA;    // Set next operation        
                    }
            }
            else{ // Got an error during the last transmission
                // Use TWI status information to detemine cause of failure and take appropriate actions. 
                TWI_Act_On_Failure_In_Last_Transmission( TWI_Get_State_Info( ) );
                } 
            }
        j++;
        } while(j<0x49);
		//printf("Before the if statement\n\r");
        //printf("The new bits are %d%d%d%d%d%d%d%d\n\r",new_packet[0],new_packet[1],new_packet[2],new_packet[3],new_packet[4],new_packet[5],new_packet[6],new_packet[7]);
		//printf("The old bits are %d%d%d%d%d%d%d%d\n\r",last_packet[0],last_packet[1],last_packet[2],last_packet[3],last_packet[4],last_packet[5],last_packet[6],last_packet[7]);
        if((memcmp(new_packet,last_packet,sizeof(new_packet)) != 0) && j == 0x49){
        //printf("In the if statement\n\r");
        //printf("The new bits are %d%d%d%d%d%d%d%d\n\r",new_packet[0],new_packet[1],new_packet[2],new_packet[3],new_packet[4],new_packet[5],new_packet[6],new_packet[7]);
		//printf("The old bits are %d%d%d%d%d%d%d%d\n\r",last_packet[0],last_packet[1],last_packet[2],last_packet[3],last_packet[4],last_packet[5],last_packet[6],last_packet[7]);
			is_new_packet = 1;
			memcpy(last_packet,new_packet, sizeof(last_packet));
        }
  		//Initialize 2 arrays. new_packet as all 00000's, last_packet as all 111111's.
  		//In the interrupt, gather each 6 byte transmission into new_packet. 
  		//if arrays are unequal, set boolean to 1. copy new_packet into last_packet.
  		//The packets are now equal but the flag was set to 1 and the printf should occur in the main loop and immediately set the boolean to 0.
  		//printf("The new bits are %d%d%d%d%d%d%d%d\n\r",new_packet[0],new_packet[1],new_packet[2],new_packet[3],new_packet[4],new_packet[5],new_packet[6],new_packet[7]);
		//printf("The old bits are %d%d%d%d%d%d%d%d\n\r",last_packet[0],last_packet[1],last_packet[2],last_packet[3],last_packet[4],last_packet[5],last_packet[6],last_packet[7]);
  		if (is_new_packet == 1){
  			printf("GYRO X = 0x%X%X\n\rGYRO Y = 0x%X%X\n\rGYRO Z = 0x%X%X\n\n\r",gyroxhi,gyroxlo,gyroyhi,gyroylo,gyrozhi,gyrozlo);
  			is_new_packet = 0;
  		}
  }
}
  
  



/*
void main( void )                                                                                               
{
  unsigned char messageBuf[4];                                                                                  //messageBuf is 4 so 3 characters are useable 0, 1, and 2
  unsigned char TWI_targetSlaveAddress, temp, TWI_operation=0,                                                  //more unsigned chars for use later
                pressedButton, myCounter=0;

  //LED feedback port - connect port B to the STK500 LEDS                                                       //can use LEDS connect to port B
  DDRB  = 0xFF;
  PORTB = myCounter;
  
  //Switch port - connect portD to the STK500 switches
  DDRD  = 0x00;

  TWI_Master_Initialise();                                                                                      //initialize as per TWI_Master.c.....Bitrate, flood TWDR with 1's, enable TWI
  __enable_interrupt();                                                                                         //enable interrupts in general for the ATMEGA128
  
  TWI_targetSlaveAddress   = 0x10;                                                                              //Address of slave, this is changeable. 0001 0000 

  // This example is made to work together with the AVR311 TWI Slave application note and stk500.
  // In adition to connecting the TWI pins, also connect PORTB to the LEDS and PORTD to the switches.
  // The code reads the pins to trigger the action you request. There is an example sending a general call,
  // address call with Master Read and Master Write. The first byte in the transmission is used to send
  // commands to the TWI slave.

  // This is a stk500 demo example. The buttons on PORTD are used to control different TWI operations.
  for(;;)                                                                                                       //Infinite loop
  {  
    pressedButton = ~PIND;                                                                                      
    if (pressedButton)       // Check if any button is pressed                                                  
    {
      do{temp = ~PIND;}      // Wait until key released                                 
      while (temp);                                
      
      switch ( pressedButton ) 
      {
        // Send a Generall Call
        case (1<<PD0):                                                                                                                                                                  //PD0 is 1
          messageBuf[0] = TWI_GEN_CALL;     // The first byte must always consit of General Call code or the TWI slave address.                                                         //Send general call
          messageBuf[1] = 0xAA;             // The command or data to be included in the general call.                                                                                  //Send dummy Data
          TWI_Start_Transceiver_With_Data( messageBuf, 2 );                                                                                                                             //Start transceiver with data
                                                                                                                                                                                        //address, data, and size (2 bytes)
          break;                                                                                                                                                                        //break pressedButton

        // Send a Address Call, sending a command and data to the Slave          
        case (1<<PD1):                                                                                                                                                                  //PD1 is 1
          messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The first byte must always consit of General Call code or the TWI slave address.           //GOOD TO KNOW -.-
                                                                                                                                                                                        //Assign Bit 0 to value 0
          messageBuf[1] = TWI_CMD_MASTER_WRITE;             // The first byte is used for commands.                                                                                     //0x10 or 0001 0000                                                                                                                           
          messageBuf[2] = myCounter;                        // The second byte is used for the data.                                                                                    //myCounter is for PORTB set for LED's  
          TWI_Start_Transceiver_With_Data( messageBuf, 3 );
          break;

        // Send a Address Call, sending a request, followed by a resceive          
        case (1<<PD2):                                                                                                                                                                  //PD2 is 1
          // Send the request-for-data command to the Slave
          messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The first byte must always consit of General Call code or the TWI slave address.           //Address slave
          messageBuf[1] = TWI_CMD_MASTER_READ;             // The first byte is used for commands.                                                                                      //Read from slave
          TWI_Start_Transceiver_With_Data( messageBuf, 2 );                                                                                                                             //Transmit this 

          TWI_operation = REQUEST_DATA;         // To release resources to other operations while waiting for the TWI to complete,                                                      //Set to 0x02 for identity of command
                                                // we set a operation mode and continue this command sequence in a "followup" 
                                                // section further down in the code.
                    
        // Get status from Transceiver and put it on PORTB
        case (1<<PD5):                                                                                                                                                                  //PD5 is 1
          PORTB = TWI_Get_State_Info();                                                                                                                                                 //Set Port B to TWSR
          break;

        // Increment myCounter and put it on PORTB          
        case (1<<PD6):                                                                                                                                                                  //PD6 is 1
          PORTB = ++myCounter;                                                                                                                                                          //Set Port B to my counter
          break;
          
        // Reset myCounter and put it on PORTB 
        case (1<<PD7):                                                                                                                                                                  //PD7 is 1
          PORTB = myCounter = 0;                                                                                                                                                        //Set PORTB to this
          break;  
      }
    }    

    if ( ! TWI_Transceiver_Busy() )                                                                                                                                                     //If not transceiving
    {
    // Check if the last operation was successful
      if ( TWI_statusReg.lastTransOK )                                                                                                                                                  //Last transmission good?
      {
        if ( TWI_operation ) // Section for follow-up operations.                                                                                                                       //If so, what next?
        {                                                                                                                                                                               //Request Data?
                                                                                                                                                                                        //Read from already filled buffer?
        // Determine what action to take now
          if (TWI_operation == REQUEST_DATA)
          { // Request/collect the data from the Slave
            messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT); // The first byte must always consit of General Call code or the TWI slave address.
            TWI_Start_Transceiver_With_Data( messageBuf, 2 );       
            TWI_operation = READ_DATA_FROM_BUFFER; // Set next operation        
          }
          else if (TWI_operation == READ_DATA_FROM_BUFFER)
          { // Get the received data from the transceiver buffer
            TWI_Get_Data_From_Transceiver( messageBuf, 2 );
            PORTB = messageBuf[1];        // Store data on PORTB.
            TWI_operation = FALSE;        // Set next operation                                                                                                                         //Done, close out command identifier  
          }
        }
      }
      else // Got an error during the last transmission
      {
        // Use TWI status information to detemine cause of failure and take appropriate actions. 
        TWI_Act_On_Failure_In_Last_Transmission( TWI_Get_State_Info( ) );                                       //If failed, 
        
                                                                                                                //If the TWSR code is such that master received a NACK from slave after address
                                                                                                                //Start transceiver (assumes data is already loaded in buffer and do repeated start)
                                                                                                                //nonetheless return the status code   
      }
    }

    // Do something else while waiting for TWI operation to complete and/or a switch to be pressed
    __no_operation(); // Put own code here.

  }
}
*/

/*
  // This example code runs forever; sends a byte to the slave, then requests a byte
  // from the slave and stores it on PORTB, and starts over again. Since it is interupt
  // driven one can do other operations while waiting for the transceiver to complete.
  
  // Send initial data to slave
  messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
  messageBuf[1] = 0x00;
  TWI_Start_Transceiver_With_Data( messageBuf, 2 );

  TWI_operation = REQUEST_DATA; // Set the next operation

  for (;;)
  {
    // Check if the TWI Transceiver has completed an operation.
    if ( ! TWI_Transceiver_Busy() )                              
    {
    // Check if the last operation was successful
      if ( TWI_statusReg.lastTransOK )
      {
      // Determine what action to take now
        if (TWI_operation == SEND_DATA)
        { // Send data to slave
          messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
          TWI_Start_Transceiver_With_Data( messageBuf, 2 );
          
          TWI_operation = REQUEST_DATA; // Set next operation
        }
        else if (TWI_operation == REQUEST_DATA)
        { // Request data from slave
          messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT);
          TWI_Start_Transceiver_With_Data( messageBuf, 2 );
          
          TWI_operation = READ_DATA_FROM_BUFFER; // Set next operation        
        }
        else if (TWI_operation == READ_DATA_FROM_BUFFER)
        { // Get the received data from the transceiver buffer
          TWI_Get_Data_From_Transceiver( messageBuf, 2 );
          PORTB = messageBuf[1];        // Store data on PORTB.
          
          TWI_operation = SEND_DATA;    // Set next operation        
        }
      }
      else // Got an error during the last transmission
      {
        // Use TWI status information to detemine cause of failure and take appropriate actions. 
        TWI_Act_On_Failure_In_Last_Transmission( TWI_Get_State_Info( ) );
      }
    }
    // Do something else while waiting for the TWI Transceiver to complete the current operation
    __no_operation(); // Put own code here.
  }
}
*/
