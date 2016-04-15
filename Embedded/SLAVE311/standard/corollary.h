
/*
void main( void )
{
  unsigned char messageBuf[4];                                                                                                                                          //message buffer 0---1---2---3 with 3 as null char. Each space is 1 byte (1 char)
  unsigned char TWI_slaveAddress, temp;                                                                                                                                 //more chars

  // LED feedback port - connect port B to the STK500 LEDS
  DDRB  = 0xFF; // Set to ouput                                                                                                                                         //I don't have this but if I did...
  PORTB = 0x55; // Startup pattern

  // Own TWI slave address
  TWI_slaveAddress = 0x10;                                                                                                                                              //arbitration of slave address 0001 0000

  // Initialise TWI module for slave operation. Include address and/or enable General Call.
  TWI_Slave_Initialise( (TWI_slaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_GEN_BIT) );                                                                                       //Initialize TWI for slave operation.
                                                                                                                                                                        //Turns out the command writes the address AS WELL AS sets general call.
                                                                                                                                                                        //General call enable can always be on, removes issue and all addresses will work
                                                                                  
  __enable_interrupt();                                                                                                                                                 //the slave can now be interrupted by any incoming things from master

  
  TWI_Start_Transceiver();                                                                                                                                              // Start the TWI transceiver to enable reseption of the first command from the TWI Master.                                                                                                                                      

  // This example is made to work together with the AVR315 TWI Master application note. In adition to connecting the TWI
  // pins, also connect PORTB to the LEDS. The code reads a message as a TWI slave and acts according to if it is a                                                     //We will not use general call here but for future reference
  // general call, or an address call. If it is an address call, then the first byte is considered a command byte and
  // it then responds differently according to the commands.

  // This loop runs forever. If the TWI is busy the execution will just continue doing other operations.
  for(;;)
  {    
    
    if ( ! TWI_Transceiver_Busy() )                                                                                                                                     // Check if the TWI Transceiver has completed an operation.                             
    {                                                                                                                                                                   // If it is complete
    
      if ( TWI_statusReg.lastTransOK )                                                                                                                                  // Check if the last operation was successful
      {
    
        if ( TWI_statusReg.RxDataInBuf )                                                                                                                                // Check if the last operation was a reception
        {
          TWI_Get_Data_From_Transceiver(messageBuf, 2);                                                                                                                 // Get data from TWDR         
          
          if ( TWI_statusReg.genAddressCall )                                                                                                                           // Check if the last operation was a reception as General Call  
          {
          // Put data received out to PORTB as an example.        
            PORTB = messageBuf[0];                              
          }
    // Ends up here if the last operation was a reception as Slave Address Match                  
          else
          {
    // Example of how to interpret a command and respond.
          // TWI_CMD_MASTER_WRITE stores the data to PORTB
            if (messageBuf[0] == TWI_CMD_MASTER_WRITE)                                                                                                                  //Since these were defined in the master, can use to make decision of read/write behavior in slave
              PORTB = messageBuf[1];                            
          // TWI_CMD_MASTER_READ prepares the data from PINB in the transceiver buffer for the TWI master to fetch.
            if (messageBuf[0] == TWI_CMD_MASTER_READ)
            {
              messageBuf[0] = PINB;                                                                                                                                     //Connected to LED's to view what is going on. Also stored in PINB for use
              TWI_Start_Transceiver_With_Data( messageBuf, 1 );                                                                                                         //Send a data byte to the master          
            }
          }
        }
    // Ends up here if the last operation was a transmission                  
        else
        {
            __no_operation(); // Put own code here. This is just a place holder. It then moves on to the next if statement
        }
        
    // Check if the TWI Transceiver has already been started.
    // If not then restart it to prepare it for new receptions.             
        if ( ! TWI_Transceiver_Busy() )
        {
          TWI_Start_Transceiver();
        }      
      }
    // Ends up here if the last operation completed unsuccessfully
      
      
      
      else
      {
        TWI_Act_On_Failure_In_Last_Transmission( TWI_Get_State_Info() );                                                                                                //Get TWSR
      }
    }
    // Do something else while waiting for the TWI transceiver to complete.    
    __no_operation(); // Put own code here. This is just a place holder. It then reloops
  }
}

*/
/*  
  // A simplified example.
  // This will store data received on PORTB, and increment it before sending it back.

  TWI_Start_Transceiver( );    
         
  for(;;)
  {
    if ( ! TWI_Transceiver_Busy() )                              
    {
      if ( TWI_statusReg.RxDataInBuf )
      {
        TWI_Get_Data_From_Transceiver(&temp, 1);  
        PORTB = temp;
      }
      temp = PORTB + 1;
      TWI_Start_Transceiver_With_Data(&temp, 1); 
    }
    __no_operation();   // Do something else while waiting
  }
}

*/
