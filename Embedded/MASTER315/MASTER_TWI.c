/*****************************************************************************
*
* Atmel Corporation
*
* File              : TWI_Master.c
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
* Description       : This is a sample driver for the TWI hardware modules.
*                     It is interrupt driveren. All functionality is controlled through 
*                     passing information to and from functions. Se main.c for samples
*                     of how to use the driver.
*
*
****************************************************************************/


/****************************************************************************
MASTER_TWI.c
Refactored for KAUSAT-5 Sensor Simulator
Refactored by Matt D'Arcy. 
****************************************************************************/


#include "ioavr.h"                                                                                                                      // include the ioavr.h file. This is a file telling the controller to load its specific iom.h
                                                                                                                                        //includes iom128.h, iom128.inc, iomacro.h and sets up ATMEGA128 registers
#include "inavr.h"                                                                                                                      //includes inavr.h file, which includes intrinsics.h, which has many C language things such as
                                                                                                                                        //__enable_interrupt(),__disable_interrupt(), char, int, long, unsigned, etc.
#include "MASTER_TWI.h"                                                                                                                 //includes TWI_Master.h, TWBR, buffer size, and handy english declarations for TWI 

static unsigned char TWI_buf[ TWI_BUFFER_SIZE ];    // Transceiver buffer                                                               //Sets up a buffer for the transceiver Master. has size of 4. Remember the null char occupying the last character
                                                                                                                                        //So really we have spaces 0--1--2--3 and 3 is occupied by the \0
static unsigned char TWI_msgSize;                   // Number of bytes to be transmitted.                                               //Declaring it here to be static. Used in a function defined below
static unsigned char TWI_state = TWI_NO_STATE;      // State byte. Default set to TWI_NO_STATE.                                         //TWI_state is a variable we will extract the TWSR to for analysis. initialize this to 0xF8..

union TWI_statusReg TWI_statusReg = {0};            // TWI_statusReg is defined in TWI_Master.h                                         //Another union, this needs further examination.
unsigned int i = 0;


void ms_delay(unsigned int cnt) //Set a small delay so that clean packets are processed
{
  for(i=0;i<cnt;i++)
  {
  __delay_cycles(16000); 
  }
}


/****************************************************************************
Call this function to set up the TWI master to its initial standby state.
Remember to enable interrupts from the main application after initializing the TWI.
****************************************************************************/


void TWI_Master_Initialise(void)                                                                                                        //Initialize TWI for Master device.
{
  TWBR = TWI_TWBR;                                  // Set bit rate register (Baudrate). Defined in header file.                        //Actually set TWBR to the value we defined in TWI_Master.h (redundant)
// TWSR = TWI_TWPS;                                  // Not used. Driver presumes prescaler to be 00.                                   //
  TWDR = 0xFF;                                      // Default content = SDA released.                                                  //Floods the data register with all 1's
  TWCR = (1<<TWEN)|                                 // Enable TWI-interface and release TWI pins.                                       //Enable TWI via TWEN
         (0<<TWIE)|(0<<TWINT)|                      // Disable Interupt.                                                                //
         (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // No Signal requests.                                                              //
         (0<<TWWC);                                 //                                                                                  //
}    
    

/****************************************************************************
Call this function to test if the TWI_ISR is busy transmitting.
****************************************************************************/


unsigned char TWI_Transceiver_Busy( void )
{
  return ( TWCR & (1<<TWIE) );                  // IF TWI Interrupt is enabled then the Transceiver is busy                             //This is like how we waited for TWINT to be set to 1. 
                                                                                                                                        //returns TWCR as-is with TWIE enabled. Along with interrupt-bit of SREG, TWI interrupt request is activated
                                                                                                                                        //for as long as TWINT flag is high (1). Upon interrupt (TWINT --> 0) status code is obtainable.
}


/****************************************************************************
Call this function to fetch the state information of the previous operation. The function will hold execution (loop)
until the TWI_ISR has completed with the previous operation. If there was an error, then the function 
will return the TWI State code. 
****************************************************************************/


unsigned char TWI_Get_State_Info( void )                                                                                                //Some calls in the main.c folder
{
  while ( TWI_Transceiver_Busy() );                                                                                                     // Wait until TWI has completed the transmission.                             
  return ( TWI_state );                                                                                                                 // TWI_state is sometimes set to 0xF8 and by default set to TWSR (if all is working and 
                                                                                                                                        // a status code was obtainable)
}


/****************************************************************************
Call this function to send a prepared message. The first byte must contain the slave address and the
read/write bit. Consecutive bytes contain the data to be sent, or empty locations for data to be read
from the slave. Also include how many bytes that should be sent/read including the address byte.
The function will hold execution (loop) until the TWI_ISR has completed with the previous operation,
then initialize the next operation and return.
****************************************************************************/


void TWI_Start_Transceiver_With_Data( unsigned char *msg, unsigned char msgSize )                                                       //the function to begin transceiver with data takes pointer and size
{
  unsigned char temp;                                                                                                                   // 

  while ( TWI_Transceiver_Busy() );             // Wait until TWI is ready for next transmission.                                       //wait for transmission to end

  TWI_msgSize = msgSize;                        // Number of data to transmit.                                                          //msgSize is just the locally inputted size
  TWI_buf[0]  = msg[0];                         // Store slave address with R/W setting.                                                //TWI_msgSize is the extern static size in bytes to be transmitted
                                                                                                                                        //Set the inputted msgSize to the external value
                                                                                                                                        //Set the first character of the TWI_buf to the first of the msg
  if (!( msg[0] & (TRUE<<TWI_READ_BIT) ))       // If it is a write operation, then also copy data.                                     //This code captures the data character by character into TWI_buf
  {
    for ( temp = 1; temp < msgSize; temp++ )                                                                                            //
      TWI_buf[ temp ] = msg[ temp ];                                                                                                    //
  }
  TWI_statusReg.all = 0;                                                                                                                //Set the variable to 0
  TWI_state         = TWI_NO_STATE ;                                                                                                    //Set TWI_state to TWI_NO_STATE (0xF8 or 1111 1000)
  TWCR = (1<<TWEN)|                             // TWI Interface enabled.                                                               //
         (1<<TWIE)|(1<<TWINT)|                  // Enable TWI Interupt and clear the flag.                                              //Set relevant TWCR bits for start condition to the slave
         (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|       // Initiate a START condition.                                                          //
         (0<<TWWC);                             //                                                                                      //
}


/****************************************************************************
Call this function to resend the last message. The driver will reuse the data previously put in the transceiver buffers.
The function will hold execution (loop) until the TWI_ISR has completed with the previous operation,
then initialize the next operation and return.
****************************************************************************/


void TWI_Start_Transceiver( void )                                                                                                      //Resend the data that was put into buffer (it is still there) 
{
  while ( TWI_Transceiver_Busy() );                                                                                                     // Wait until TWI is ready for next transmission.
  TWI_statusReg.all = 0;                                                                                                                //Set the variable to 0
  TWI_state         = TWI_NO_STATE ;                                                                                                    //
  TWCR = (1<<TWEN)|                             // TWI Interface enabled.                                                               //Basically do everything except load the buffere because
         (1<<TWIE)|(1<<TWINT)|                  // Enable TWI Interupt and clear the flag.                                              //it is already full with the last buffer-loading
         (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|       // Initiate a START condition.                                                          //
         (0<<TWWC);                             //                                                                                      //
}


/****************************************************************************
Call this function to read out the requested data from the TWI transceiver buffer. I.e. first call
TWI_Start_Transceiver to send a request for data to the slave. Then Run this function to collect the
data when they have arrived. Include a pointer to where to place the data and the number of bytes
requested (including the address field) in the function call. The function will hold execution (loop)
until the TWI_ISR has completed with the previous operation, before reading out the data and returning.
If there was an error in the previous transmission the function will return the TWI error code.
****************************************************************************/


unsigned char TWI_Get_Data_From_Transceiver( unsigned char *msg, unsigned char msgSize )                                                //pointer to where to place the data and the number of bytes requested
{                                                                                                                                       //(including the address field in the msgSize)
  unsigned char i;                                                                                                                      //used for unloading buffer

  while ( TWI_Transceiver_Busy() );                                                                                                     //Wait until TWI is ready for next transmission.                                      

  if( TWI_statusReg.lastTransOK )               // Last transmission competed successfully.                                             //When the last transmission was successful
  {                                             
    for ( i=0; i<msgSize; i++ )                 // Copy data from Transceiver buffer.                                                   //unload the buffer TWI_buf into msg char by char
    {   
      msg[ i ] = TWI_buf[ i ];                                                                                                          //
    }
  }
  return( TWI_statusReg.lastTransOK );                                                                                                  //return the TWI_statusReg.lastTransOK value                            
}


// ********** Interrupt Handlers ********** //
/****************************************************************************
This function is the Interrupt Service Routine (ISR), and called when the TWI interrupt is triggered;
that is whenever a TWI event has occurred. This function should not be called directly from the main
application.
****************************************************************************/


#pragma vector=TWI_vect                                                                                                                 //TWI_vect is defined in iom128.h as 0x84
__interrupt void TWI_ISR(void)                                                                                                          //this is an interrupt routine called TWI_ISR
{
  static unsigned char TWI_bufPtr;                                                                                                      //declare buffer pointer
  
  switch (TWSR)                                                                                                                         //Switch case called TWSR
  {
    case TWI_START:             // START has been transmitted                                                                           //As defined in TWI_Master.h = 0x08
      
    case TWI_REP_START:         // Repeated START has been transmitted                                                                  //As defined in TWI_Master.h = 0x10
      TWI_bufPtr = 0;                                                                                                                   // Set buffer pointer to the TWI Address location  
      
    case TWI_MTX_ADR_ACK:       // SLA+W has been tramsmitted and ACK received                                                          //As defined in TWI_Master.h = 0x18
      
    case TWI_MTX_DATA_ACK:      // Data byte has been tramsmitted and ACK received                                                      //As defined in TWI_Master.h = 0x28       
      if (TWI_bufPtr < TWI_msgSize)                                                                                                     //With use of pointers and stored memory,
      {
        TWDR = TWI_buf[TWI_bufPtr++];                                                                                                   //If the buffer pointer is less than the 
        TWCR = (1<<TWEN)|                                 // TWI Interface enabled                                                      //message size pointer, 
               (1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interupt and clear the flag to send byte                        //Load the next memory sector's data (byte)
               (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           //                                                                            //into TWDR
               (0<<TWWC);                                 //                                                                            //Transmit
      }else                    // Send STOP after last byte                                                                             //
      {
        TWI_statusReg.lastTransOK = TRUE;                 // Set status bits to completed successfully.                                 //If it is the last byte
        TWCR = (1<<TWEN)|                                 // TWI Interface enabled                                                      //Issue a stop condition
               (0<<TWIE)|(1<<TWINT)|                      // Disable TWI Interrupt and clear the flag                                   //
               (0<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|           // Initiate a STOP condition.                                                 //
               (0<<TWWC);                                 //
        ms_delay(10);
      }
      break;                                                                                                                            //Break out because Master-Transmit is done (ACK)
      
      
      
      
      
      
    case TWI_MRX_DATA_ACK:      // Data byte has been received and ACK tramsmitted                                                      //As defined in TWI_Master.h = 0x50
      TWI_buf[TWI_bufPtr++] = TWDR;                                                                                                     //There is more TWDR coming in so store the next TWDR into
                                                                                                                                        //memory at TWI_buf's next address
      
    case TWI_MRX_ADR_ACK:       // SLA+R has been tramsmitted and ACK received                                                          //As defined in TWI_Master.h = 0x40
      if (TWI_bufPtr < (TWI_msgSize-1) )                  // Detect the last byte to NACK it.                                           //
      {                                                                                                                                 //This group of statements sends an ACK
        TWCR = (1<<TWEN)|                                 // TWI Interface enabled                                                      //to the slave if the byte received is not the last data
               (1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interupt and clear the flag to read next byte                   //byte on TWDR and if it is, send a NACK
               (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // Send ACK after reception                                                   //
               (0<<TWWC);                                 //                                                                            //
      }else                    // Send NACK after next reception                                                                        //
      {
        TWCR = (1<<TWEN)|                                 // TWI Interface enabled                                                      //
               (1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interupt and clear the flag to read next byte                   //
               (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // Send NACK after reception                                                  //
               (0<<TWWC);                                 //                                                                            //
      }    
      break;                                                                                                                            //Break out because Master-Receive is done (ACK)
      
      
      
      
      
      
      
    case TWI_MRX_DATA_NACK:     // Data byte has been received and NACK tramsmitted                                                     //As defined in TWI_Master.h = 0x58
      TWI_buf[TWI_bufPtr] = TWDR;                                                                                                       //When Master sends NACK because last data byte
      TWI_statusReg.lastTransOK = TRUE;                 //                                                                              //No more coming in so store this TWDR at TWI_buf's current address 
      TWCR = (1<<TWEN)|                                 // TWI Interface enabled                                                        //Set status bits to completed successfully. 
             (0<<TWIE)|(1<<TWINT)|                      // Disable TWI Interrupt and clear the flag                                     //Enable TWI Bus
             (0<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|           // Initiate a STOP condition.                                                   //Disable TWIE, Clear Flag (TWINT = 1)
             (0<<TWWC);                                 //                                                                              //Stop Condition
      ms_delay(10);
      break;                                                                                                                            //break out because Master-Receive is done (NACK)
 
      
      
      
      
      
    case TWI_ARB_LOST:          // Arbitration lost                                                                                     //As defined in TWI_Master.h = 0x38
      TWCR = (1<<TWEN)|                                                                                                                 // TWI Interface enabled                                                        
             (1<<TWIE)|(1<<TWINT)|                                                                                                      // Enable TWI Interupt and clear the flag                                       
             (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|                                                                                           // Initiate a (RE)START condition.                                              
             (0<<TWWC);                                                                                                          
      break;                                                                                                                            //break out because Restarting

      
      
      
      
      
    case TWI_MTX_ADR_NACK:      // SLA+W has been tramsmitted and NACK received                                                         //As defined in TWI_Master.h = 0x20
      
    case TWI_MRX_ADR_NACK:      // SLA+R has been tramsmitted and NACK received                                                         //As defined in TWI_Master.h = 0x48
      
    case TWI_MTX_DATA_NACK:     // Data byte has been tramsmitted and NACK received                                                     //As defined in TWI_Master.h = 0x30
    //case TWI_NO_STATE              // No relevant state information available; TWINT = “0”                                            //As defined in TWI_Master.h = 0xF8
      
    case TWI_BUS_ERROR:         // Bus error due to an illegal START or STOP condition                                                  //As defined in TWI_Master.h = 0x00
      
    default:                                                                                                                            //When TWSR does not match any prescribed codes
      TWI_state = TWSR;                                 // Store TWSR and automatically sets clears noErrors bit.                       //record the TWSR into TWI_state
                                                        // Reset TWI Interface                                                          //reinitializes TWI
      TWCR = (1<<TWEN)|                                 // Enable TWI-interface and release TWI pins                                    //TWINT stays 0 so not waiting on anything
             (0<<TWIE)|(0<<TWINT)|                      // Disable Interupt                                                             //No signal requests sent
             (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // No Signal requests                                                           //Now is the time to analyze any errors in peace
             (0<<TWWC);                                 //                                                                              //
  }
}
