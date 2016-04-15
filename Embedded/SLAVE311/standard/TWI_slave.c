/*****************************************************************************
*
* Atmel Corporation
*
* File              : TWI_Slave.c
* Compiler          : IAR EWAAVR 2.28a/3.10c
* Revision          : $Revision: 1.7 $
* Date              : $Date: Thursday, August 05, 2004 09:22:50 UTC $
* Updated by        : $Author: lholsen $
*
* Support mail      : avr@atmel.com
*
* Supported devices : All devices with a TWI module can be used.
*                     The example is written for the ATmega16
*
* AppNote           : AVR311 - TWI Slave Implementation
*
* Description       : This is sample driver to AVRs TWI module. 
*                     It is interupt driveren. All functionality is controlled through 
*                     passing information to and from functions. Se main.c for samples
*                     of how to use the driver.
*
****************************************************************************/
 

static unsigned char TWI_buf[TWI_BUFFER_SIZE];                                                                                                  // Transceiver buffer. Set the size in the header file                                       
static unsigned char TWI_msgSize  = 0;                                                                                                          // Number of bytes to be transmitted.
static unsigned char TWI_state    = TWI_NO_STATE;                                                                                               // State byte. Default set to TWI_NO_STATE (0xF8)

static unsigned char TWI_busy = 0;                                                                                                              // From Giyeon's code

union TWI_statusReg TWI_statusReg = {0};                                                                                                        // TWI_statusReg is defined in TWI_Slave.h

/****************************************************************************
Call this function to set up the TWI slave to its initial standby state.
Remember to enable interrupts from the main application after initializing the TWI.
Pass both the slave address and the requrements for triggering on a general call in the
same byte. Use e.g. this notation when calling this function:
TWI_Slave_Initialise( (TWI_slaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_GEN_BIT) );
The TWI module is configured to NACK on any requests. Use a TWI_Start_Transceiver function to 
start the TWI.
****************************************************************************/
void TWI_Slave_Initialise( unsigned char TWI_ownAddress )
{
  TWAR = TWI_ownAddress;                                                                                                                        // Set own TWI slave address. Accept TWI General Calls.
  TWDR = 0xFF;                                                                                                                                  // Default content = SDA released.
  TWCR = (1<<TWEN)|                                                                                                                             // Enable TWI-interface and release TWI pins.
         (0<<TWIE)|(0<<TWINT)|                                                                                                                  // Disable TWI Interupt.
         (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|                                                                                                       // Do not ACK on any requests, yet.
         (0<<TWWC);                                                                                                                             //THIS IS DIFFERENT FROM THE BOOK! TWEA IS SET TO 1 IN BOOK!!!!!!!!!!
  TWI_busy = 0;
}    
 

/****************************************************************************
Call this function to test if the TWI_ISR is busy transmitting.
****************************************************************************/
unsigned char TWI_Transceiver_Busy( void )                                                                                                      //Equivalent of wait for TWINT bit set. 
{
  return TWI_busy;                  // IF TWI interrupt is enabled then the Transceiver is busy
}


/****************************************************************************
Call this function to fetch the state information of the previous operation. The function will hold execution (loop)
until the TWI_ISR has completed with the previous operation. If there was an error, then the function 
will return the TWI State code. 
****************************************************************************/
unsigned char TWI_Get_State_Info( void )                                                                                                        //Go through a series of hoops to get TWSR and return it
{
  while ( TWI_Transceiver_Busy() );             // Wait until TWI has completed the transmission.
  return ( TWI_state );                         // Return error state. 
}


/****************************************************************************
Call this function to send a prepared message, or start the Transceiver for reception. Include
a pointer to the data to be sent if a SLA+W is received. The data will be copied to the TWI buffer. 
Also include how many bytes that should be sent. Note that unlike the similar Master function, the
Address byte is not included in the message buffers.
The function will hold execution (loop) until the TWI_ISR has completed with the previous operation,
then initialize the next operation and return.
****************************************************************************/
void TWI_Start_Transceiver_With_Data( unsigned char *msg, unsigned char msgSize )                                                               //inputs of pointer to memory where msg is, and msg size in bytes
{
  unsigned char temp;                                                                                           

  while ( TWI_Transceiver_Busy() );             // Wait until TWI is ready for next transmission.                                               //wait if it is busy

  TWI_msgSize = msgSize;                        // Number of data to transmit.                                                                  //how many bytes to send
  for ( temp = 0; temp < msgSize; temp++ )      // Copy data that may be transmitted if the TWI Master requests data.                           //Load buffer en route to master    
    TWI_buf[ temp ] = msg[ temp ];
  TWI_statusReg.all = 0;                                                                                                                        //Clear pseudostatus
  TWI_state         = TWI_NO_STATE ;                                                                                                            //set the state variable descriptor to 0xF8
  TWCR = (1<<TWEN)|                             // TWI Interface enabled.                                                                       //After transmitting ready slave for next time addressed
         (1<<TWIE)|(1<<TWINT)|                  // Enable TWI Interupt and clear the flag.
         (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|       // Prepare to ACK next time the Slave is addressed.
         (0<<TWWC);                             //
  TWI_busy = 1;
}


/****************************************************************************
Call this function to start the Transceiver without specifing new transmission data. Usefull for restarting
a transmission, or just starting the transceiver for reception. The driver will reuse the data previously put
in the transceiver buffers. The function will hold execution (loop) until the TWI_ISR has completed with the 
previous operation, then initialize the next operation and return.
****************************************************************************/
void TWI_Start_Transceiver( void )                                                                                                              //If data is already in buffer, just start the transceiver
{
  while ( TWI_Transceiver_Busy() );             // Wait until TWI is ready for next transmission.
  TWI_statusReg.all = 0;      
  TWI_state         = TWI_NO_STATE ;
  TWCR = (1<<TWEN)|                             // TWI Interface enabled.
         (1<<TWIE)|(1<<TWINT)|                  // Enable TWI Interupt and clear the flag.
         (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|       // Prepare to ACK next time the Slave is addressed.
         (0<<TWWC);                             //
  TWI_busy = 0;
}


/****************************************************************************
Call this function to read out the received data from the TWI transceiver buffer. I.e. first call
TWI_Start_Transceiver to get the TWI Transceiver to fetch data. Then Run this function to collect the
data when they have arrived. Include a pointer to where to place the data and the number of bytes
to fetch in the function call. The function will hold execution (loop) until the TWI_ISR has completed 
with the previous operation, before reading out the data and returning.
If there was an error in the previous transmission the function will return the TWI State code.
****************************************************************************/
unsigned char TWI_Get_Data_From_Transceiver( unsigned char *msg, unsigned char msgSize )                                                        //If not busy and last transmission ok, extract contents of buffer to variable msg
{
  unsigned char i;

  while ( TWI_Transceiver_Busy() );             // Wait until TWI is ready for next transmission.

  if( TWI_statusReg.lastTransOK )               // Last transmission competed successfully.              
  {                                             
    for ( i=0; i<msgSize; i++ )                 // Copy data from Transceiver buffer.
    {
      msg[ i ] = TWI_buf[ i ];
    }
    TWI_statusReg.RxDataInBuf = FALSE;          // Slave Receive data has been read from buffer.                                                //Get rid of the pseudostatus flag saying there was data in buffer. We got the data.
  }
  return( TWI_statusReg.lastTransOK );                                                                                                          //Return that the last transmission was ok.
}


unsigned char TWI_Act_On_Failure_In_Last_Transmission ( unsigned char TWIerrorMsg )                                                                                     
{
                                                                                                                                                                        // A failure has occurred, use TWIerrorMsg to determine the nature of the failure
                                                                                                                                                                        // and take appropriate actions.
                                                                                                                                                                        // Se header file for a list of possible failures messages.
  
                                                                                                                                                                        // This very simple example puts the error code on PORTB and restarts the transceiver with
                                                                                                                                                                        // all the same data in the transmission buffers.
  PORTB = TWIerrorMsg;
  TWI_Start_Transceiver();
                    
  return TWIerrorMsg;                                                                                                                                                   //Puts TWSR on PORTB and returns TWSR to wherever it was called
}


// ********** Interrupt Handlers ********** //
/****************************************************************************
This function is the Interrupt Service Routine (ISR), and called when the TWI interrupt is triggered;
that is whenever a TWI event has occurred. This function should not be called directly from the main
application.
****************************************************************************/
#pragma vector=TWI_vect
__interrupt void TWI_ISR( void )                                                                                                                //Switch-case - flows through each line until a break encountered.                                                                                                                                                //Code in between start/breaks/end is all one scenario
{
  static unsigned char TWI_bufPtr;
  
  switch (TWSR)
  {                                                                                                                                             // For SLAVE TRANSMIT (SLA+R)
    case TWI_STX_ADR_ACK:                                                                                                                       // Own SLA+R has been received; ACK has been returned
//    case TWI_STX_ADR_ACK_M_ARB_LOST:                                                                                                          // Arbitration lost in SLA+R/W as Master; own SLA+R has been received; ACK has been returned
      TWI_bufPtr   = 0;                                                                                                                         // Set buffer pointer to first data (address) location          
      
    case TWI_STX_DATA_ACK:                                                                                                                      // Data byte in TWDR has been transmitted; ACK has been received
      TWDR = TWI_buf[TWI_bufPtr++];                                                                                                             // Pump next byte (since it was an ACK and not a NACK) into TWDR
      TWCR = (1<<TWEN)|                                                                                                                         // TWI Interface enabled
             (1<<TWIE)|(1<<TWINT)|                                                                                                              // Enable TWI Interupt and clear the flag to send byte
             (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // 
             (0<<TWWC);                                 //  
      TWI_busy = 1;
      break;
      
      
      
      
      
    case TWI_STX_DATA_NACK:                                                                                                                     // Data byte in TWDR has been transmitted; NACK has been received.         
                                                                                                                                                // I.e. this could be the end of the transmission.
      if (TWI_bufPtr == TWI_msgSize)                                                                                                            // Have we transceived all expected data?
      {
        TWI_statusReg.lastTransOK = TRUE;                                                                                                       // Set status bits to completed successfully. 
      }
      else                                                                                                                                      // Master has sent a NACK before all data was sent to Master....
      {
        TWI_state = TWSR;                                                                                                                       // Store TWI State as errormessage.      
      }        
                                                                                                                                                // Put TWI Transceiver in passive mode.
      TWCR = (1<<TWEN)|                                                                                                                         // Enable TWI-interface and release TWI pins
             (0<<TWIE)|(0<<TWINT)|                                                                                                              // Disable Interupt
             (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|                                                                                                   // Do not acknowledge on any new requests.
             (0<<TWWC);                                 //
      TWI_busy = 0;
      if(current_command == 0x48){ //upon receipt of final command, assume transmissions went well and set the flag. 
	  		pure_transmissions_only = 0;
	  		current_command = 0;
	  }
      break; 
      
      
      
      
  
    case TWI_SRX_GEN_ACK:                                                                                                                       // General call address has been received; ACK has been returned
//    case TWI_SRX_GEN_ACK_M_ARB_LOST:                                                                                                          // Arbitration lost in SLA+R/W as Master; General call address has been received; ACK has been returned
      TWI_statusReg.genAddressCall = TRUE;                                                                                                      //Set pseudostatus to use of general call
      
    case TWI_SRX_ADR_ACK:                                                                                                                       // Own SLA+W has been received ACK has been returned
//    case TWI_SRX_ADR_ACK_M_ARB_LOST:                                                                                                          // Arbitration lost in SLA+R/W as Master; own SLA+W has been received; ACK has been returned    
                                                                                                                                                // Dont need to clear TWI_S_statusRegister.generalAddressCall due to that it is the default state.
      TWI_statusReg.RxDataInBuf = TRUE;      
      TWI_bufPtr   = 0;                                                                                                                         // Set buffer pointer to first data location
                                                                                                                                                // Reset the TWI Interupt to wait for a new event.
      TWCR = (1<<TWEN)|                                                                                                                         // TWI Interface enabled
             (1<<TWIE)|(1<<TWINT)|                                                                                                              // Enable TWI Interupt and clear the flag to send byte
             (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|                                                                                                   // Expect ACK on this transmission
             (0<<TWWC);                                 //    
      TWI_busy = 1;
      break;
      
      
      
      
      
      
    case TWI_SRX_ADR_DATA_ACK:                                                                                                                  // Previously addressed with own SLA+W; data has been received; ACK has been returned
      
      
      
    case TWI_SRX_GEN_DATA_ACK:                                                                                                                  // Previously addressed with general call; data has been received; ACK has been returned
      TWI_buf[TWI_bufPtr++]     = TWDR;                                                                                                         // Extract TWDR into data buffer
      TWI_statusReg.lastTransOK = TRUE;                                                                                                         // Set flag transmission successfull.       
                                                                                                                                                // Reset the TWI Interupt to wait for a new event.
      TWCR = (1<<TWEN)|                                                                                                                         // TWI Interface enabled
             (1<<TWIE)|(1<<TWINT)|                                                                                                              // Enable TWI Interupt and clear the flag to send byte
             (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|                                                                                                   // Send ACK after next reception
             (0<<TWWC);                                 //    
      TWI_busy = 1;
          //printf("Received 0x%X\n\r",TWI_buf[TWI_bufPtr-1]);         //Tell user received data
          if (TWI_buf[0] == MPU6050_RA_GYRO_XOUT_H){      //Make temp2 = gyro data according
                temp2 = GYROXHI;                          //to buf_temp
                }
          else if (TWI_buf[0] == MPU6050_RA_GYRO_XOUT_L){
                temp2 = GYROXLO;
                }
          else if (TWI_buf[0] == MPU6050_RA_GYRO_YOUT_H){
                temp2 = GYROYHI;
                }
          else if (TWI_buf[0] == MPU6050_RA_GYRO_YOUT_L){
                temp2 = GYROYLO;
                }
          else if (TWI_buf[0] == MPU6050_RA_GYRO_ZOUT_H){
                temp2 = GYROZHI;
                }
          else if (TWI_buf[0] == MPU6050_RA_GYRO_ZOUT_L){
                temp2 = GYROZLO;
                current_command = 0x48;
                }
      break;

    case TWI_SRX_STOP_RESTART:                                                                                                                  // A STOP condition or repeated START condition has been received while still addressed as Slave    
                                                                                                                                                // Put TWI Transceiver in passive mode.
      TWCR = (1<<TWEN)|                                                                                                                         // Enable TWI-interface and release TWI pins
             (0<<TWIE)|(0<<TWINT)|                                                                                                              // Disable Interupt
             (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|                                                                                                   // Do not acknowledge on any new requests.
             (0<<TWWC);                                 //
      TWI_busy = 0;
      break;           
      
      
      
      
      
    case TWI_SRX_ADR_DATA_NACK:                                                                                                                 // Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
      
    case TWI_SRX_GEN_DATA_NACK:                                                                                                                 // Previously addressed with general call; data has been received; NOT ACK has been returned
      
    case TWI_STX_DATA_ACK_LAST_BYTE:                                                                                                            // Last data byte in TWDR has been transmitted (TWEA = �0�); ACK has been received
      
//    case TWI_NO_STATE                                                                                                                         // No relevant state information available; TWINT = �0�
      
    case TWI_BUS_ERROR:                                                                                                                         // Bus error due to an illegal START or STOP condition
      
    default:                                                                                                                                    // If nothing else happened there was an error. Extract error and stop TWI Bus
      TWI_state = TWSR;                                                                                                                         // Store TWI State as errormessage, operation also clears the Success bit.      
      TWCR = (1<<TWEN)|                                                                                                                         // Enable TWI-interface and release TWI pins
             (0<<TWIE)|(0<<TWINT)|                                                                                                              // Disable Interupt
             (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|                                                                                                   // Do not acknowledge on any new requests.
             (0<<TWWC);                                                                                                                         //
      TWI_busy = 0;
  }
}