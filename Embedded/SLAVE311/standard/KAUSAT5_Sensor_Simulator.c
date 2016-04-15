/****************************************************************************
//
// File              : KAUSAT5_Sensor_Simulator.c
// Compiler          : //IAR EWAAVR 2.28a/3.10c
// Revision          : $Revision: 1.0 $
// Date              : //$Date: Monday, May 24, 2004 09:32:18 UTC $
// Updated by        : $Author: Matthew D'Arcy $
//
// Support mail      : matthew.d'arcy@nasa-academy.org
//
// Supported devices : Simulator board with ATMega128, 13x LTC2630 DAC chips, KAUSAT5 ADCS Board.
//                     
// AppNote           : //AVR311 - TWI Slave Implementation
//
// Description       : //Example of how to use the driver for TWI slave 
//                     communication.
//
****************************************************************************/


/****************************************************************************
// We want to parse 16 different floating point numbers each packet. They are the az and el of all 5 sun sensors, and the xyz of magnetic, and xyz of gyromag
// 143-char SAMPLE PACKET:
// $,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#####.##,_#####.##,_#####.##,_###.####,_###.####,_###.####@
// 0th char is $, the last character is 141, null character is 143, and the total size is 144
****************************************************************************/


/****************************************************************************
//  Include system architecture, header and source files
****************************************************************************/
#include <iom128.h>
#include <ina90.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "declarations.h"
#include "TWI_slave.h"

#include "DAC.c"
#include "initializations.c"
#include "process_data.c"
#include "putchar.c"
#include "time.c"
#include "TWI_slave.c"
/****************************************************************************
//  Main Routine
****************************************************************************/
int main(void)
{ 
  init_UART();
  printf("\nUART Initialized\n\r");
  init_SPI();
  printf("SPI Initialized\n\r");
  init_DAC();
  printf("DAC Initialized\n\r");               //Initialize all 13 DAC chips and then leave all SS pins HI
  printf("DAC Count: 13\n\r");
  TWI_Slave_Initialise( (TWI_slaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_GEN_BIT) );
  TWI_Start_Transceiver();
  printf("TWI Initialized\n\r");
  printf("Awaiting Serial Data\n\n\r");
  __enable_interrupt();
  // Initialise TWI module for slave operation. Include address and/or enable General Call.
  TWI_Slave_Initialise( (TWI_slaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_GEN_BIT) );      //Initialize TWI for slave operation.
  TWI_Start_Transceiver(); 
  
 
  for(;;){ 
          while(pure_transmissions_only == 0); //wait for the flag to be set to 1 by the UART interrupt, allowing TWI to commence for a given data packet.
          if((!TWI_Transceiver_Busy()) && TWI_statusReg.lastTransOK){
          //TWI_statusReg.all = 0; 
          TWI_statusReg.all = 0;                      //Clear status register
          TWI_Start_Transceiver_With_Data(&temp2, 1); //Put temp2 in the transceiver (send it)
          //printf("Sent 0x%X\n\n\r",temp2);
          }     
   }
}


  /*
  gyrox_now = 1.4357;
  gyroxhex = (int)(65535*(gyrox_now+(2000*__PI/180))/(4000*__PI/180));
  GYROXHI = gyroxhex >> 8;
  GYROXLO = gyroxhex & 0xFF;
  printf("Gyrox = %8.4f, Hex = %u...%X\r\n",gyrox_now,gyroxhex,gyroxhex);
  gyroy_now = 4.2851;
  gyroyhex = (int)(65535*(gyroy_now+(2000*__PI/180))/(4000*__PI/180));
  GYROYHI = gyroyhex >> 8;
  GYROYLO = gyroyhex & 0xFF;
  printf("Gyroy = %8.4f, Hex = %u...%X\r\n",gyroy_now,gyroyhex,gyroyhex);
  gyroz_now = 5.8752;
  gyrozhex = (int)(65535*(gyroz_now+(2000*__PI/180))/(4000*__PI/180));
  GYROZHI = gyrozhex >> 8;
  GYROZLO = gyrozhex & 0xFF;
  printf("Gyroz = %8.4f, Hex = %u...%X\r\n\n\n",gyroz_now,gyrozhex,gyrozhex);
 
  
  // Start the TWI transceiver to enable reseption of the first command from the TWI Master.                                                                                                                                      
  // A simplified example.
  // This will store data received on PORTB, and increment it before sending it back.
  
    for(;;)
  {
   if((!TWI_Transceiver_Busy())){                         //If the transceiver is NOT busy
          if (TWI_statusReg.RxDataInBuf){                 //If there is data in buffer
            TWI_Get_Data_From_Transceiver(&temp, 1);      //Read data to var temp
            buf_temp = temp;                              //Make buf_temp hold the data
            printf("Received 0x%X\n\r",buf_temp);         //Tell user received data
            }
          if (buf_temp == MPU6050_RA_GYRO_XOUT_H){      //Make temp2 = gyro data according
                temp2 = GYROXHI;                          //to buf_temp
                }
          else if (buf_temp == MPU6050_RA_GYRO_XOUT_L){
                temp2 = GYROXLO;
                }
          else if (buf_temp == MPU6050_RA_GYRO_YOUT_H){
                temp2 = GYROYHI;
                }
          else if (buf_temp == MPU6050_RA_GYRO_YOUT_L){
                temp2 = GYROYLO;
                }
          else if (buf_temp == MPU6050_RA_GYRO_ZOUT_H){
                temp2 = GYROZHI;
                }
          else if (buf_temp == MPU6050_RA_GYRO_ZOUT_L){
                temp2 = GYROZLO;
                }
          //TWI_statusReg.all = 0; 
          if((!TWI_Transceiver_Busy())){                //If transceiver is NOT busy
            TWI_statusReg.all = 0;                      //Clear status register
            TWI_Start_Transceiver_With_Data(&temp2, 1); //Put temp2 in the transceiver (send it)
            //ms_delay(10);
            printf("Sent 0x%X\n\n\r",temp2);             //the slave can now be interrupted by any incoming things from master
            TWI_Start_Transceiver();                   //This is necessary at the end. ATMEL left this out and I wasted 2 days trying to 
            }
          }
        }
        //figure out the issue.       
  //#define SS_A1 --> PORTD_Bit4 (GOOD)  0-5
  //#define SS_E1 --> PORTD_Bit6 (GOOD)  0-5
  //#define SS_A2 --> PORTD_Bit2 (GOOD)  0-5
  //#define SS_E2 --> PORTD_Bit3 (GOOD)  0-5
  //#define SS_A3 --> PORTG_Bit3 (GOOD)  0-5
  //#define SS_E3 --> PORTG_Bit4 (GOOD)  0-5
  //#define SS_A4 --> PORTB_Bit3 (wont work) MADE PB5 PORTB_Bit5
  //#define SS_E4 --> PORTB_Bit4 (GOOD)  0-5
  //#define SS_A5 --> PORTE_Bit5 (GOOD)  0-5
  //#define SS_E5 --> PORTB_Bit0 (GOOD)  0-5
  //#define SS_MX --> PORTE_Bit4 (GOOD)  0-5
  //#define SS_MY --> PORTE_Bit3 (GOOD)  0-5
  //#define SS_MZ --> PORTE_Bit2 (GOOD)  0-4.096

    ms_delay(1000);
   
    for(mm=0;mm<0xFFF;mm++)
    { 
      mmh = mm>>4;
      mml = (mm&0x0F)<<4;
      LTC2630_send(0,mmh,mml,1);
      LTC2630_send(0,mmh,mml,2);
      LTC2630_send(0,mmh,mml,3);
      LTC2630_send(0,mmh,mml,4);
	  LTC2630_send(0,mmh,mml,5);
	  LTC2630_send(0,mmh,mml,6);
	  LTC2630_send(0,mmh,mml,7);
	  LTC2630_send(0,mmh,mml,8);
	  LTC2630_send(0,mmh,mml,9);
	  LTC2630_send(0,mmh,mml,10);
	  LTC2630_send(0,mmh,mml,11);
	  LTC2630_send(0,mmh,mml,12);
      LTC2630_send(0,mmh,mml,13);
    }

    printf("Max V\n\r");
    ms_delay(1000);
    
     for(mm=0xFFF;mm>0;mm--)
    {
      mmh = mm>>4;
      mml = (mm&0x0F)<<4;
      LTC2630_send(0,mmh,mml,1);
      LTC2630_send(0,mmh,mml,2);
      LTC2630_send(0,mmh,mml,3);
      LTC2630_send(0,mmh,mml,4);
	  LTC2630_send(0,mmh,mml,5);
	  LTC2630_send(0,mmh,mml,6);
	  LTC2630_send(0,mmh,mml,7);
	  LTC2630_send(0,mmh,mml,8);
	  LTC2630_send(0,mmh,mml,9);
	  LTC2630_send(0,mmh,mml,10);
	  LTC2630_send(0,mmh,mml,11);
	  LTC2630_send(0,mmh,mml,12);
      LTC2630_send(0,mmh,mml,13);
      
    }
    
    printf("0V\n\r");
    
    
    
    //Make a pseudo case for data. 1.4357 4.2851 5.8752 respectively
    //Comes from the sim in radians/s.
    //Convert from rad/s to the -2000 to 2000 deg/s and increment to 65536 bits
    
    
    gyrox_now = 1.4357;
    gyroxhex = (int)(65535*(gyrox_now+(2000*__PI/180))/(4000*__PI/180));
    GYROXHI = gyroxhex >> 8;
    GYROXLO = gyroxhex & 0xFF;
    printf("Gyrox = %8.4f, Hex = %u...%X\r\n",gyrox_now,gyroxhex,gyroxhex);

    gyroy_now = 4.2851;
    gyroyhex = (int)(65535*(gyroy_now+(2000*__PI/180))/(4000*__PI/180));
    GYROYHI = gyroyhex >> 8;
    GYROYLO = gyroyhex & 0xFF;
    printf("Gyroy = %8.4f, Hex = %u...%X\r\n",gyroy_now,gyroyhex,gyroyhex);
    
    gyroz_now = 5.8752;
    gyrozhex = (int)(65535*(gyroz_now+(2000*__PI/180))/(4000*__PI/180));
    GYROZHI = gyrozhex >> 8;
    GYROZLO = gyrozhex & 0xFF;
    printf("Gyroz = %8.4f, Hex = %u...%X\r\n\n\n",gyroz_now,gyrozhex,gyrozhex);
    
	if( (!TWI_Transceiver_Busy()))                              
    {
      if ( TWI_statusReg.RxDataInBuf)
      { 
        TWI_Get_Data_From_Transceiver(&temp, 1);  
        buf_temp = temp;
        if (buf_temp == MPU6050_RA_GYRO_XOUT_H)
        {
        	//printf("I just received 0x%X and will send GYROXHI\n\r",buf_temp);
        	temp2 = GYROXHI;
        }
        
        else if (buf_temp == MPU6050_RA_GYRO_XOUT_L)
        {
       		temp2 = GYROXLO;
        }
        
        else if (buf_temp == MPU6050_RA_GYRO_YOUT_H)
        {
        	temp2 = GYROYHI;
        }
        
        else if (buf_temp == MPU6050_RA_GYRO_YOUT_L)
        {
        	temp2 = GYROYLO;
        }
        
        else if (buf_temp == MPU6050_RA_GYRO_ZOUT_H)
        {
        	temp2 = GYROZHI;
        }
        
        else if (buf_temp == MPU6050_RA_GYRO_ZOUT_L)
        {
        	temp2 = GYROZLO;
        }
        
      }
      
     TWI_statusReg.all = 0; 
     TWI_Start_Transceiver_With_Data(&temp2, 1);
     //ms_delay(10);
     printf("I just sent 0x%X\n\n\r",temp2);                              //the slave can now be interrupted by any incoming things from master
     TWI_Start_Transceiver();                                           //This is necessary at the end. ATMEL left this out and I wasted 2 days trying to 
                                                                        //figure out the issue.    
     } 

	



    //Revision Contents;
    //1. I use 32 bits format according to the datasheet. 
    //0x@@ 0xC@ 0x$$ 0x$@ @- don't care bit, C is command, $ is data.
    
    //2. The function was changed as followings;
    // LTC2630_send(X,0xXX) --> LTC2620_send(X,0xXX,0xXX)
    // for sending High bits and low bits because the bit operation cann't be adapated in the function. I don't know the reasons. I think it is the memory problem.
    // I declare the local variable in the function.
    // It works well. you can see the video which I sent in Kakaotalk

    //3. I change the sequence for initialization of SPI
    // You have to declare the direction bits, and then PORT or PIN, and subfuction such as UART, SPI, etc.
    // During the debugging, your code is stop in SPI_SPDR function in DAC.c file. So, I changed the code as I mentioned over.

    // Command for making triangular waves
    // 0x@@ 0xC@ 0x$$ 0x$@ @- don't care bit, C is command, $ is data.
    // starts at 0, adds a bit each clock tick until it hits the 12-bit 0xFFF. Need to be represented as 2 8-bit chars, HI and LO
    // HI is the 12-bit thing shifted to the right 4, cutting off the 4 least-significant bits.
    // LO is the 12-bit thing with only the 4 least-significant bits remaining, and then shifting that left 4 to produce 4 dont-care bits and completing the byte
    // Thus the 12-bit data takes place with 8 of those bits in the 3rd byte transmitted, and 4 in the first half of the final byte transmitted.
    // First command is a dont-care byte.
    // Second command is a 4bit command and then 4 dont-care bits.
   
    // When you stop the code, the Vout of each DAC will be the last Vout before it was ended....
    // Resetting the controller does nothing. You MUST cycle power for the whole system to re-run.

    
    */