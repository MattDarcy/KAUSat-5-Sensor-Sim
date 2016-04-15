#include <iom128.h>                           
#include <ina90.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// We want to parse 16 different floating point numbers each packet. They are the az and el of all 5 sun sensors, and the xyz of magnetic, and xyz of gyromag
// 143-char SAMPLE PACKET:
// $,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#####.##,_#####.##,_#####.##,_###.####,_###.####,_###.####@
// 0th char is $, the last character is 141, null character is 143, and the total size is 144
char packet_data[144]; //declare string variable for the packet, 143 characters plus the null = 144
char data; //this is a 1 character string that will extract the packet from UDR1 character by character in a loop
char null[23]; //out of sensor view - 22 characters beginning at 0 (0 to 21) + null (character 22) is size of 23
int save_on, ii = 0; //define integer save_on which will tell the program that the packet_data array still has characters to fill from the received packet, and the ii is each incremented character space of packet_data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                                                                        //DECLARE STRINGS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SUN SENSOR ANGLES, _#.#### with null char = 10
char sunaz1[8];
char sunel1[8];
char sunaz2[8];
char sunel2[8];
char sunaz3[8];
char sunel3[8];
char sunaz4[8];
char sunel4[8];
char sunaz5[8];
char sunel5[8];
//MAGNETIC SENSOR VECTOR MAGNITUDES, _#####.## with null char =10
char magx[10];
char magy[10];
char magz[10];
//GYROMAG X,Y,Z, _###.#### with null char = 10
char gyrox[10];
char gyroy[10];
char gyroz[10];
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                                                                        //DECLARE FLOATS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SUN SENSOR ANGLES
volatile float sunaz1_before,sunaz1_now,sunel1_before,sunel1_now = 0;
volatile float sunaz2_before,sunaz2_now,sunel2_before,sunel2_now = 0;
volatile float sunaz3_before,sunaz3_now,sunel3_before,sunel3_now = 0;
volatile float sunaz4_before,sunaz4_now,sunel4_before,sunel4_now = 0;
volatile float sunaz5_before,sunaz5_now,sunel5_before,sunel5_now = 0;

//MAGNETIC SENSOR VECTOR MAGNITUDES
volatile float magx_before, magx_now = 0;
volatile float magy_before, magy_now = 0;
volatile float magz_before, magz_now = 0;

//GYROMAG X,Y,Z
volatile float gyrox_before, gyrox_now = 0;
volatile float gyroy_before, gyroy_now = 0;
volatile float gyroz_before, gyroz_now = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 void init_UART(void) //Set desired registers according to the ATMEGA128 Datasheet
{   
    DDRE = 0xFB; //1111 1000
    UCSR0A = 0x00;
    UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);//0x98;
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); 
    UBRR0H = 0x00; //set baud rate hi
    UBRR0L = 103; //set baud rate lo  
}

int putchar(int print_this)//Copy packet for processing, "clean the mailbox" for a new packet being delivered later
{
  while (((UCSR0A >> 5) & 0x01) == 0) // while UDRE data register is empty
            ;  //Do nothing
  UDR0 = print_this; //When not empty, set UDR1 to the data packet
  UCSR0A |= 0x20; //set UCSR1A (USART Control and Status Register 1 for Port A) to 0010 0000, default is 0010 0000, as in the data register is empty!
                  //In other words, imagine you have a mailbox where only 1 packet can go at a time. You copied the delivered packet and sent it to processing in the house, but you need to clean out the original packet
                  //from the data register
  return print_this; //send that copied packet to be processed. Returns to the interrupt.
}

void parsing(void)//Copy packet for processing, "clean the mailbox" for a new packet being delivered later
{ 
  int i; //declares variable i for use in for loops
  while (((UCSR0A >> 5) & 0x01) == 0) // while UDRE data register is empty
            ;  //Do nothing. Once the UDRE data register is not empty, the parsing begins
  
  //SET "PREVIOUS" VARIABLES AS THE CURRENT VALUES FOR THE NEXT ITERATION
  sunaz1_before = sunaz1_now;
  sunel1_before = sunel1_now;
  sunaz2_before = sunaz2_now;
  sunel2_before = sunel2_now;
  sunaz3_before = sunaz3_now;
  sunel3_before = sunel3_now;
  sunaz4_before = sunaz4_now;
  sunel4_before = sunel4_now;
  sunaz5_before = sunaz5_now;
  sunel5_before = sunel5_now;
  magx_before = magx_now;
  magy_before = magy_now;
  magz_before = magz_now;
  gyrox_before = gyrox_now;
  gyroy_before = gyroy_now;
  gyroz_before = gyroz_now; 
  // $,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#####.##,_#####.##,_#####.##,_###.####,_###.####,_###.####@
  // $,+0.?000,+0.?000,+0.?000,-0.?000,+0.4720,+0.9295,-0.?000,-0.?000,-0.?000,+0.?000,+06735.21,-01398.24,+32506.76,+000.9377,+001.0970,+007.3590@
  // 0th char is $, the last character is 141, null character is 143, and the total size is 144
  
  //first teach the machine to recognize null values (the sensor has no reading). Question Marks in characters 5, 13, 21, 29, 36, 44, 52, 60, 68, 76 mean a null value: Out of sensor view
  //then convert the strings to floats and assign them as the _now values. This takes care of signs already???
  
  if(packet_data[0] == '$')
  {
    
    //SUN SENSOR 1 AZEl
    if(packet_data[5]=='?')
        printf("Sun Sensor 1: sun out of sensor view\n\r");
    else if(packet_data[5]!='?')
    {  
      for (i=0;i<7;i++)
        sunaz1[i]=packet_data[2+i];
      for (i=0;i<7;i++)
        sunel1[i]=packet_data[10+i];
      sunaz1_now = atof(sunaz1);
      sunel1_now = atof(sunel1);
      printf("Sun Sensor 1 (Az,El): \(%5.3f,%5.3f\)\n\r",sunaz1_now,sunel1_now);
    }
    
        //SUN SENSOR 2 AZEl
    if(packet_data[21]=='?')
        printf("Sun Sensor 2: sun out of sensor view\n\r");
    else if(packet_data[21]!='?')
    {  
      for (i=0;i<7;i++)
        sunaz2[i]=packet_data[18+i];
      for (i=0;i<7;i++)
        sunel2[i]=packet_data[26+i];
      sunaz2_now = atof(sunaz2);
      sunel2_now = atof(sunel2);
      printf("Sun Sensor 2 (Az,El): \(%5.3f,%5.3f\)\n\r",sunaz2_now,sunel2_now);
    }
    
          //SUN SENSOR 3 AZEl
    if(packet_data[37]=='?')
        printf("Sun Sensor 3: sun out of sensor view\n\r");
    else if(packet_data[37]!='?')
    {  
      for (i=0;i<7;i++)
        sunaz3[i]=packet_data[34+i];
      for (i=0;i<7;i++)
        sunel3[i]=packet_data[42+i];
      sunaz3_now = atof(sunaz3);
      sunel3_now = atof(sunel3);
      printf("Sun Sensor 3 (Az,El): \(%5.3f,%5.3f\)\n\r",sunaz3_now,sunel3_now);
    }
    
         //SUN SENSOR 4 AZEl
    if(packet_data[53]=='?')
        printf("Sun Sensor 4: sun out of sensor view\n\r");
    else if(packet_data[53]!='?')
    {  
      for (i=0;i<7;i++)
        sunaz4[i]=packet_data[50+i];
      for (i=0;i<7;i++)
        sunel4[i]=packet_data[58+i];
      sunaz4_now = atof(sunaz4);
      sunel4_now = atof(sunel4);
      printf("Sun Sensor 4 (Az,El): \(%5.3f,%5.3f\)\n\r",sunaz4_now,sunel4_now);
    }
    
    //SUN SENSOR 5 AZEl
    if(packet_data[69]=='?')
        printf("Sun Sensor 5: sun out of sensor view\n\r");
    else if(packet_data[69]!='?')
    {  
      for (i=0;i<7;i++)
        sunaz5[i]=packet_data[66+i];
      for (i=0;i<7;i++)
        sunel5[i]=packet_data[74+i];
      sunaz5_now = atof(sunaz5);
      sunel5_now = atof(sunel5);
      printf("Sun Sensor 5 (Az,El): \(%5.3f,%5.3f\)\n\r",sunaz5_now,sunel5_now);
    }
    
    
    //MAGNETIC SENSOR X,Y,Z 
    for(i=0;i<9;i++)
    {
      magx[i]=packet_data[82+i];
      magy[i]=packet_data[92+i];
      magz[i]=packet_data[102+i];
      magx_now = atof(magx);
      magy_now = atof(magy);
      magz_now = atof(magz);
    }
      printf("Magnetic Field Sensor Components (X,Y,Z): \(%8.2f,%8.2f,%8.2f\)\n\r",magx_now,magy_now,magz_now);
    
    
    //GYRO MAG X,Y,Z
    for(i=0;i<9;i++)
    {
      gyrox[i]=packet_data[112+i];
      gyroy[i]=packet_data[122+i];
      gyroz[i]=packet_data[132+i];
      gyrox_now = atof(gyrox);
      gyroy_now = atof(gyroy);
      gyroz_now = atof(gyroz);
    }
      printf("Gyro Magnitude (X,Y,Z): \(%8.4f,%8.4f,%8.4f\)\n\r",gyrox_now,gyroy_now,gyroz_now);
    
  
    
printf("\n\n");
    
  }

 // UDR0 = data_zigbee; //When not empty, set UDR1 to the data packet (this transmits it via USART1 to the pc Token2Shell for display instantly)
  UCSR0A |= 0x20; //set UCSR1A (USART Control and Status Register 1 for Port A) to 0010 0000, default is 0010 0000, as in the data register is empty!
                  //In other words, imagine you have a mailbox where only 1 packet can go at a time. You copied the delivered packet and sent it to processing in the house, but you need to clean out the original packet
                  //from the data register
 // return data_zigbee; //send that copied packet to be processed. Returns to the interrupt.
}
void main(void)//Set registers, put an "I'm alive" message, and run indefinitely
{  
  init_UART(); //run function to set desired registers
  printf("Setting OK\n\r");
  //put a message to the screen to show that this program is working up to this point
  __enable_interrupt(); //enable the interrupt to be ready to accept and process packets
  while(1)
    {  //start the program and keep going unless otherwise stopped by an outside factor
    }
}
#pragma vector = USART0_RXC_vect //Let the controller know that this interrupt protocol will be used in this
__interrupt void H5_UART0IT(void) // Interrupt accepts packet, processes, prints via the putchar function, and continues on
{
  __disable_interrupt(); //disable interrupt and continue with the processing of packet
  //while(!(UCSR0A & 0x80)); //while UCSR1A is not 1000 0000, do nothing (THIS IS USART RECEIVE COMPLETE FLAG! IT TURNS TO 1 WHEN THE PACKET RECEIVE IS COMPLETE)
                           //otherwise, once UCSR1A is 1000 0000, (PACKET IS RECEIVED AND SITTING IN UDR1 READY FOR USE)
  data = UDR0;  //set data equal to UDR1 - where the packet is sitting
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
  
 
  putchar(data); //run this packet character by character through the putchar function
  //printf("data : %c\n\r", data); //format the data left-sided in the token2shell
  __enable_interrupt(); //enable the interrupt again and wait for next packet
  
}