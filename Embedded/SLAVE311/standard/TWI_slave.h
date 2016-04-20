/*****************************************************************************
*
* Atmel Corporation
*
* File              : TWI_Slave.h
* Compiler          : IAR EWAAVR 2.28a/3.10c
* Revision          : $Revision: 1.6 $
* Date              : $Date: Monday, May 24, 2004 09:32:18 UTC $
* Updated by        : $Author: ltwa $
*
* Support mail      : avr@atmel.com
*
* Supported devices : All devices with a TWI module can be used.
*                     The example is written for the ATmega16
*
* AppNote           : AVR311 - TWI Slave Implementation
*
* Description       : Header file for TWI_slave.c
*                     Include this file in the application.
*
****************************************************************************/


/****************************************************************************
TWI_slave.h
Refactored for KAUSAT-5 Sensor Simulator
Refactored by Matt D'Arcy. 
****************************************************************************/


#ifndef TWI_SLAVE_H
#define TWI_SLAVE_H


/****************************************************************************
TWI Status/Control register definitions
****************************************************************************/

#define TWI_BUFFER_SIZE 4      // Reserves memory for the drivers transceiver buffer.                                                   //Define buffer size of 4, indicating 3 characters + null character \0 (3 bytes total), 0,1,2
                               // Set this to the largest message size that will be sent including address byte.


/****************************************************************************
Global definitions
****************************************************************************/


union TWI_statusReg                       // Status byte holding flags.                                                                 //This is not well understood
{
    unsigned char all;
    struct
    {
        unsigned char lastTransOK:1;                                                                                                    //Last transmission went well
        unsigned char RxDataInBuf:1;                                                                                                    //There is data to be received in the buffer
        unsigned char genAddressCall:1;                        // TRUE = General call, FALSE = TWI Address;                             //General call enable
        unsigned char unusedBits:5;                                                                                                     //UnusedBits
    };                                                                                                                                  //All in all this is 8 bits or 1 byte. This is a custom "status register" made by the coder
                                                                                                                                        //for further coding purposes. Let's call it a pseudostatus
};

extern union TWI_statusReg TWI_statusReg;                                                                                               //Declare external union for the whole program. It is declaring a union with 
                                                                                                                                        //the name TWI_statusReg and the properties of the union declared above.
                                                                                                                                        //This seems wholly redundant.                                                                                           
/****************************************************************************
Function definitions
****************************************************************************/


void TWI_Slave_Initialise( unsigned char );                                                                                             //Functions described in more detail in TWI_slave.c
unsigned char TWI_Transceiver_Busy( void );
unsigned char TWI_Get_State_Info( void );
void TWI_Start_Transceiver_With_Data( unsigned char * , unsigned char );
void TWI_Start_Transceiver( void );
unsigned char TWI_Get_Data_From_Transceiver( unsigned char *, unsigned char );


/****************************************************************************
Bit and byte definitions
****************************************************************************/


#define TWI_READ_BIT  0   // Bit position for R/W bit in "address byte".                                                                //SLA+R/SLA+W? This bit in the address byte will say so
#define TWI_ADR_BITS  1   // Bit position for LSB of the slave address bits in the init byte.                                           //This and the 6 more significant bits constitute the slave address
#define TWI_GEN_BIT   0   // Bit position for LSB of the general call bit in the init byte.                                             //General call is the 0th bit in the init byte. 0 means none, 1 means general call is useable

#define TRUE          1
#define FALSE         0


/****************************************************************************
TWI State codes
****************************************************************************/


// General TWI Master staus codes           

#define TWI_START                  0x08  // START has been transmitted                                                                  //Status codes as in ATMEGA128 datasheet
#define TWI_REP_START              0x10  // Repeated START has been transmitted
#define TWI_ARB_LOST               0x38  // Arbitration lost

// TWI Master Transmitter staus codes  

#define TWI_MTX_ADR_ACK            0x18  // SLA+W has been tramsmitted and ACK received
#define TWI_MTX_ADR_NACK           0x20  // SLA+W has been tramsmitted and NACK received 
#define TWI_MTX_DATA_ACK           0x28  // Data byte has been tramsmitted and ACK received
#define TWI_MTX_DATA_NACK          0x30  // Data byte has been tramsmitted and NACK received 

// TWI Master Receiver staus codes  

#define TWI_MRX_ADR_ACK            0x40  // SLA+R has been tramsmitted and ACK received
#define TWI_MRX_ADR_NACK           0x48  // SLA+R has been tramsmitted and NACK received
#define TWI_MRX_DATA_ACK           0x50  // Data byte has been received and ACK tramsmitted
#define TWI_MRX_DATA_NACK          0x58  // Data byte has been received and NACK tramsmitted

// TWI Slave Transmitter staus codes

#define TWI_STX_ADR_ACK            0xA8  // Own SLA+R has been received; ACK has been returned
#define TWI_STX_ADR_ACK_M_ARB_LOST 0xB0  // Arbitration lost in SLA+R/W as Master; own SLA+R has been received; ACK has been returned
#define TWI_STX_DATA_ACK           0xB8  // Data byte in TWDR has been transmitted; ACK has been received
#define TWI_STX_DATA_NACK          0xC0  // Data byte in TWDR has been transmitted; NOT ACK has been received
#define TWI_STX_DATA_ACK_LAST_BYTE 0xC8  // Last data byte in TWDR has been transmitted (TWEA = “0”); ACK has been received

// TWI Slave Receiver staus codes

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

// ADCS BOARD CALLS
// ADCS Board will send over 2 bytes and depending on what it is, the slave (simulator) needs to send out the corresponding data byte (one half of a data reading)
// Via the data line on I2C

#define MPU6050_RA_GYRO_XOUT_H      0x43
#define MPU6050_RA_GYRO_XOUT_L      0x44
#define MPU6050_RA_GYRO_YOUT_H      0x45
#define MPU6050_RA_GYRO_YOUT_L      0x46
#define MPU6050_RA_GYRO_ZOUT_H      0x47
#define MPU6050_RA_GYRO_ZOUT_L      0x48

#endif