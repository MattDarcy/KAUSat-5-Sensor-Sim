/*****************************************************************************
*
* Atmel Corporation
*
* File              : TWI_Master.h
* Compiler          : IAR EWAAVR 2.28a/3.10c
* Revision          : $Revision: 1.13 $
* Date              : $Date: 24. mai 2004 11:31:22 $
* Updated by        : $Author: ltwa $
*
* Support mail      : avr@atmel.com
*
* Supported devices : All devices with a TWI module can be used.
*                     The example is written for the ATmega16
*
* AppNote           : AVR315 - TWI Master Implementation
*
* Description       : Header file for TWI_Master.c                                                                                      //This is a header file. It can just be included in TWI_Master.c
                                                                                                                                        //Note TWI_Master.c is a driver. TWI_Master.c defines all the relevant functions, 
                                                                                                                                        //interrupts, etc. and names them for easy English understanding.
                                                                                                                                        
*                     Include this file in the application.
*
****************************************************************************/


/****************************************************************************
  TWI Status/Control register definitions
****************************************************************************/
#define TWI_BUFFER_SIZE 4   // Set this to the largest message size that will be sent including address byte.                            //Set buffer size to 4, this will need to be changed for larger applications (ADCS simulator)

#define TWI_TWBR            0x0C        // TWI Bit rate Register setting.                                                                //TWI_TWBR is defined as 0x0C or 0000 1100, 12 as before. 
                                                                                                                                         //Whatever is done with the bitrate, the CPU clock frequency in the slave must be AT LEAST
                                                                                                                                         //16 times higher than the SCL frequency
                                                                                                                                         //      SCL frequency = CPU Clock Frequency (16000000 Hz)
                                                                                                                                         //                      --------------------------------
                                                                                                                                         //                       16 + 2(TWBR) * 4^(TWPS)
                                                                                                                                         // with TWBR at 12 and TWPS at 0, SCL becomes 142857 or 152.857kHz. CPU clock of slave/SCL is 112.
// Not used defines!
//#define TWI_TWPS          0x00        // This driver presumes prescaler = 00

/****************************************************************************
  Global definitions
****************************************************************************/

union TWI_statusReg                                                                               
// Status byte holding flags.                                                                                                            //Need to find out what this does. 
{                                                                                                                                        //
    unsigned char all;                                                                                                                   //
    struct                                                                                                                               //             
    {                                                                                                                                    //
        unsigned char lastTransOK:1;                                                                                                     //   The colons are bitfields, assign 1 bit to the char lastTransOK
        unsigned char unusedBits:7;                                                                                                      //   assign 7 bits to the char unusedBits
    };                                                                                                                                   //
};

extern union TWI_statusReg TWI_statusReg;                                                                                                // Declare external union for the whole program. It is declaring a union with 
                                                                                                                                     // the name TWI_statusReg and the properties of the union declared above.
                                                                                                                                         //This seems wholly redundant.

/****************************************************************************
  Function definitions
****************************************************************************/
void TWI_Master_Initialise( void );                                                                                                      //Define function that will be worked through in TWI_Master.c
unsigned char TWI_Transceiver_Busy( void );                                                                                              //Define function that will be worked through in TWI_Master.c
unsigned char TWI_Get_State_Info( void );                                                                                                //Define function that will be worked through in TWI_Master.c
void TWI_Start_Transceiver_With_Data( unsigned char * , unsigned char );                                                                 //Define function that will be worked through in TWI_Master.c
void TWI_Start_Transceiver( void );                                                                                                      //Define function that will be worked through in TWI_Master.c
unsigned char TWI_Get_Data_From_Transceiver( unsigned char *, unsigned char );                                                           //Define function that will be worked through in TWI_Master.c

/****************************************************************************
  Bit and byte definitions
****************************************************************************/
#define TWI_READ_BIT  0       // Bit position for R/W bit in "address byte".                                                             //In address byte, we want to know read/write direction. This is the 0th bit, so define this here
#define TWI_ADR_BITS  1       // Bit position for LSB of the slave address bits in the init byte.                                        //Shortcut for defining the address bits by only saying the least-significant bit is Bit 1.

#define TRUE          1                                                                                                                  //Define True as 1 
#define FALSE         0                                                                                                                  //Define False as 0

/****************************************************************************
  TWI State codes                                                                                                                        //These are the status codes of TWSR from the book. 
****************************************************************************/
// General TWI Master status codes                      
#define TWI_START                  0x08  // START has been transmitted  
#define TWI_REP_START              0x10  // Repeated START has been transmitted
#define TWI_ARB_LOST               0x38  // Arbitration lost

// TWI Master Transmitter status codes                      
#define TWI_MTX_ADR_ACK            0x18  // SLA+W has been tramsmitted and ACK received
#define TWI_MTX_ADR_NACK           0x20  // SLA+W has been tramsmitted and NACK received 
#define TWI_MTX_DATA_ACK           0x28  // Data byte has been tramsmitted and ACK received
#define TWI_MTX_DATA_NACK          0x30  // Data byte has been tramsmitted and NACK received 

// TWI Master Receiver status codes  
#define TWI_MRX_ADR_ACK            0x40  // SLA+R has been tramsmitted and ACK received
#define TWI_MRX_ADR_NACK           0x48  // SLA+R has been tramsmitted and NACK received
#define TWI_MRX_DATA_ACK           0x50  // Data byte has been received and ACK tramsmitted
#define TWI_MRX_DATA_NACK          0x58  // Data byte has been received and NACK tramsmitted

// TWI Slave Transmitter status codes
#define TWI_STX_ADR_ACK            0xA8  // Own SLA+R has been received; ACK has been returned
#define TWI_STX_ADR_ACK_M_ARB_LOST 0xB0  // Arbitration lost in SLA+R/W as Master; own SLA+R has been received; ACK has been returned
#define TWI_STX_DATA_ACK           0xB8  // Data byte in TWDR has been transmitted; ACK has been received
#define TWI_STX_DATA_NACK          0xC0  // Data byte in TWDR has been transmitted; NOT ACK has been received
#define TWI_STX_DATA_ACK_LAST_BYTE 0xC8  // Last data byte in TWDR has been transmitted (TWEA = “0”); ACK has been received

// TWI Slave Receiver status codes
#define TWI_SRX_ADR_ACK            0x60  // Own SLA+W has been received ACK has been returned
#define TWI_SRX_ADR_ACK_M_ARB_LOST 0x68  // Arbitration lost in SLA+R/W as Master; own SLA+W has been received; ACK has been returned
#define TWI_SRX_GEN_ACK            0x70  // General call address has been received; ACK has been returned
#define TWI_SRX_GEN_ACK_M_ARB_LOST 0x78  // Arbitration lost in SLA+R/W as Master; General call address has been received; ACK has been returned
#define TWI_SRX_ADR_DATA_ACK       0x80  // Previously addressed with own SLA+W; data has been received; ACK has been returned
#define TWI_SRX_ADR_DATA_NACK      0x88  // Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
#define TWI_SRX_GEN_DATA_ACK       0x90  // Previously addressed with general call; data has been received; ACK has been returned
#define TWI_SRX_GEN_DATA_NACK      0x98  // Previously addressed with general call; data has been received; NOT ACK has been returned
#define TWI_SRX_STOP_RESTART       0xA0  // A STOP condition or repeated START condition has been received while still addressed as Slave

// TWI Miscellaneous status codes
#define TWI_NO_STATE               0xF8  // No relevant state information available; TWINT = “0”
#define TWI_BUS_ERROR              0x00  // Bus error due to an illegal START or STOP condition

