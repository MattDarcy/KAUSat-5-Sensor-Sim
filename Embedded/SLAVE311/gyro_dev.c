#include <iom128.h>                           
#include <ina90.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Need to respond to the following 
#define MPU6050_RA_GYRO_XOUT_H      0x43
#define MPU6050_RA_GYRO_XOUT_L      0x44
#define MPU6050_RA_GYRO_YOUT_H      0x45
#define MPU6050_RA_GYRO_YOUT_L      0x46
#define MPU6050_RA_GYRO_ZOUT_H      0x47
#define MPU6050_RA_GYRO_ZOUT_L      0x48
*/

// We want to parse 16 different floating point numbers each packet. They are the az and el of all 5 sun sensors, and the xyz of magnetic, and xyz of gyromag
// 143-char SAMPLE PACKET:
// $,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#####.##,_#####.##,_#####.##,_###.####,_###.####,_###.####@
// 0th char is $, the last character is 141, null character is 143, and the total size is 144

int i;                    //declares variable i for use in for loops
int counter = 1;          //declare the integer counter that will keep track of the iteration
int save_on, ii = 0;      //define integer save_on which will tell the program that the packet_data array still has characters to fill from the received packet, and the ii is each incremented character space of packet_data
char packet_data[144];    //declare string variable for the packet, 143 characters plus the null = 144
char data;                //this is a 1 character string that will extract the packet from UDR1 character by character in a loop
char gyrox[10];           //declare the string that will hold gyrox data
volatile float gyrox_before, gyrox_now = 0; //declare floating values that will be the data values in radians of the current and previous iterations. Before moving onto another iteration, the _before is set to the current
                                            //reading.

int min_gyro = -250;
int max_gyro = 250;
int range_gyro = 500;
float gyrox_now_deg = 0;
//float gyrox_now_shifted = 0;
//float gyrox_now_500 = 0;
//float gyrox_now_product = 0;
//volatile unsigned long int gyrox_now_int = 1;
volatile unsigned long int gyrox_now_check = 0;

void timer(int num)
{ 
  int denom, ans, ans2, rem, rem2 = 0;
  denom = 60; //seconds in 1 minute
  ans = num/denom;
    rem = num%denom;   
    if (rem > 9)
    {  ans2 = rem/10;
       rem2 = rem%10;
    }
  else if (rem < 10)
    rem2 = rem;
  printf("Elapsed Time (MM:SS) = %d%d:%d%d\r\n",0,ans,ans2,rem2);
}

void parsing(void)//Copy packet for processing, "clean the mailbox" for a new packet being delivered later
{ 
  timer(counter);
  while (((UCSR1A >> 5) & 0x01) == 0); // while UDRE data register is empty do nothing. Once the UDRE data register is not empty, the parsing begins
  gyrox_before = gyrox_now;  //SET "PREVIOUS" VARIABLE AS THE CURRENT VALUE FOR THE NEXT ITERATION
  // $,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#####.##,_#####.##,_#####.##,_###.####,_###.####,_###.####@
  // $,+0.?000,+0.?000,+0.?000,-0.?000,+0.4720,+0.9295,-0.?000,-0.?000,-0.?000,+0.?000,+06735.21,-01398.24,+32506.76,+000.9377,+001.0970,+007.3590@
  // 0th char is $, the @ character is 141, null character is 143, and the total size is 144
    
    //GYRO MAG X
    for(i=0;i<9;i++)
    {
      gyrox[i]=packet_data[112+i];
      gyrox_now = atof(gyrox);
    }
   
    
   printf("          GX (rad/s) = \(%6.4f\)\n\r",gyrox_now);
   gyrox_now_deg = gyrox_now*180/__PI;
   printf("          GX (deg/s) = \(%8.4f\)\n\r",gyrox_now_deg);
   
   /* Saved for development purposes - step-by-step calculations and debugging
   gyrox_now_shifted = gyrox_now_deg + 250;
   printf("       + 250 (deg/s) = \(%8.4f\)\n\r",gyrox_now_shifted);
   gyrox_now_500 = gyrox_now_shifted/500;
   printf("          out of 500 = \(%f\)\n\r",gyrox_now_500);
   gyrox_now_product = 65535 * gyrox_now_500;
   printf("  GX ( bit - float ) = \(%f\)\n\r",gyrox_now_product);
   gyrox_now_int = (unsigned int)(gyrox_now_product);
   printf("    GX ( bit - int ) = \(%u\)\n\r",gyrox_now_int);          //This was plaguing me for days. Use %u because it is an unsigned long int. Otherwise it spits out stupid negative numbers.    
   */
   
   gyrox_now_check = ((gyrox_now*180/__PI)+250)*65535/500;
   gyrox_now_check = (unsigned int) (gyrox_now_check);
   printf("    GX (  checked  ) = \(%u\)\n\r",gyrox_now_check);   //gyrox_now_check is the one we want to send to I2C
   printf("\n");
   counter++;
   UCSR1A |= 0x20; //set UCSR1A (USART Control and Status Register 1 for Port A) to 0010 0000, default is 0010 0000, as in the data register is empty!
                  //In other words, imagine you have a mailbox where only 1 packet can go at a time. You copied the delivered packet and sent it to processing in the house, but you need to clean out the original packet
                  //from the data register
}

#pragma vector = USART1_RXC_vect //Let the controller know that this interrupt protocol will be used in this
__interrupt void H5_UART1IT(void) // Interrupt accepts packet, processes, prints via the putchar function, and continues on
{
  __disable_interrupt(); //disable interrupt and continue with the processing of packet
  //while(!(UCSR1A & 0x80)); //while UCSR1A is not 1000 0000, do nothing (THIS IS USART RECEIVE COMPLETE FLAG! IT TURNS TO 1 WHEN THE PACKET RECEIVE IS COMPLETE)
                           //otherwise, once UCSR1A is 1000 0000, (PACKET IS RECEIVED AND SITTING IN UDR1 READY FOR USE)
  data = UDR1;  //set data equal to UDR1 - where the packet is sitting
  if (data == '$') //first character of packet
  {
    save_on = 1; //set save_on to 1, we will fill the c array with the packet, character by character
  }
  else if (data == '@') //otherwise if 'data' is @ symbol, last character of packet
  {
    packet_data[ii] = NULL;//set that final character to null char
    save_on = 0; //set save_on to 0, we will not be filling any more characters of the array 'c'
    parsing(); //enter the parsing function with the newly extracted array
    ii=0; //reset the counter
    return; //end and wait for next packet
  }
  if (save_on ==1)
  {
    packet_data[ii++] = data; //since data (UDR1) is only getting 1 character at a time in serial, this sets each character of the c array to the incoming character in UDR1 before it receives the next character
  }
  __enable_interrupt(); //enable the interrupt again and wait for next packet
}