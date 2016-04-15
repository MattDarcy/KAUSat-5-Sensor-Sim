#include <iom128.h>
#include <ina90.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

volatile float sunaz1_now = 2*__PI;
volatile float sunel1_now = __PI/2;
volatile float sunaz2_now = 999999;
volatile float sunel2_now = 999999;
volatile float sunaz3_now = 999999;
volatile float sunel3_now = 999999;
volatile float sunaz4_now = 999999;
volatile float sunel4_now = 999999;
volatile float sunaz5_now = 999999;
volatile float sunel5_now = 999999;

//(-2 +  4094   *(4/4095.0))*100000
volatile float magx_now =  (-2 +  2049.001   *(4/4095.0))*100000;
volatile double magy_now = (-2 +  2048   *(4/4095.0))*100000;
volatile double magz_now = (-2 +  2049.001   *(4/4095.0))*100000;

volatile float gyrox_now = (-250*__PI/180)+((   250    +250))*(__PI/180);
volatile float gyroy_now = (-250*__PI/180)+((   1    +250))*(__PI/180);;
volatile float gyroz_now = (-250*__PI/180)+((   249  +250))*(__PI/180);;

volatile double step1,step2,step3,step4,step5,step6,step7;

unsigned int sa1hex;
unsigned int se1hex;
unsigned int sa2hex;
unsigned int se2hex;
unsigned int sa3hex;
unsigned int se3hex;
unsigned int sa4hex;
unsigned int se4hex;
unsigned int sa5hex;
unsigned int se5hex;

unsigned int magxhex;
unsigned int magyhex;
unsigned int magzhex;
volatile float magxmask;
volatile float magymask;
volatile float magzmask;

unsigned int gyroxhex;
unsigned int gyroyhex;
unsigned int gyrozhex;

float voltagea1;
float voltagee1;
float voltagea2;
float voltagee2;
float voltagea3;
float voltagee3;
float voltagea4;
float voltagee4;
float voltagea5;
float voltagee5;
float voltagemx;
float voltagemy;
float voltagemz;

/****************************************************************************
//  Initialize UART
****************************************************************************/
void init_UART(void) //Set desired registers according to the ATMEGA128 Datasheet
{
  
  //DDRD = 0xFB; //1111 1011
    /*
    UCSR1A = 0x00; //sets USART Control and Status Register A to 0000 0000, default is 0010 0000. The special case is bit 5, UDREn, an indicator set equal to zero meaning the buffer (the mailbox) is NOT empty 
    UCSR1B = (1<<RXCIE1)|(1<<RXEN1)|(1<<TXEN1); //sets USART Control and Status Register B as follows: RX Complete Interrupt Enable(ON), Receiver Mode (ON), Transmitter Mode (ON)
    UCSR1C = (1<<UCSZ11)|(1<<UCSZ10);  //sets USART Control and Status Register C as follows: character size is 8-bit according to table 80 pg 191
    UBRR1H = 0x00; // higher 4 bits are reserved for future development, lower 4 bits are the most significant, set to zero
    UBRR1L = 103; //decimal 103 so lower 8 bits are set to 0110 0111 according to table 85 for 16MHz and 9600 baud rate (bits/second)
  */
    
    DDRE = 0xFB; //1111 1000
    UCSR0A = 0x00;
    UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);//0x98;
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); 
    UBRR0H = 0x00; //set baud rate hi
    UBRR0L = 103; //set baud rate lo  
  
}

/****************************************************************************
//  Putchar function - necessary for printf for UART-Serial
****************************************************************************/
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

int main(void)
{
init_UART();

printf("\n\n\r");

//(-2 +  2049   *(4/4095.0))*1000;
// if less than    magyhex = 4095-4095*(magymask/400000);
// if greater than    magyhex = 4095 * (magymask / 400000) - 2048;
step1 = 4/4.095;
step2 = 2049 * step1;
step3 = step2 - 2;
step4 = step3 * 100; //get around 199.9k
step5 = step4/400000; //if less than 200k
step6 = step5*4095;
step7 = 4095-step6;

 // else if (magxmask >= 200000)
 // {
  //  magxhex = 4095 * (magxmask / 400000) - 2048;
 // }
 // else if (magxmask <= 200000)
 // {
 //   magxhex = 4095-4095*(magxmask/400000);
 // }


//step1 = -250 * __PI/180;
//step2 = 1 + 250;
//step3 = step2 * __PI / 180;
//step4 = step1 + step3;
//step5 = step4 + (250*__PI/180);
//step6 = step5 * 65535;
//step7 = step6 / (500*__PI/180);

printf("Step 1 = %15.13f\n\r",step1);
printf("Step 2 = %15.13f\n\r",step2);
printf("Step 3 = %15.13f\n\r",step3);
printf("Step 4 = %15.12f\n\r",step4);
printf("Step 5 = %15.12f\n\r",step5);
printf("Step 6 = %15.12f\n\r",step6);
printf("Step 7 = %15.12f\n\r",step7);

  //mask necessary quantities (magnetic sensor only in this case)
  magxmask = magx_now + 200000;
  magymask = magy_now + 200000;
  magzmask = magz_now + 200000;

  if (sunaz1_now == 999999)
  {
    sa1hex = 0x00;
  }
  else if (sunaz1_now == 2 * __PI)
  {
    sa1hex = 0x01;
  }
  else
  {
    sa1hex = floor(1 + 4094*sunaz1_now/(2*__PI));
  }
  //LTC2630_send(0,sa1hex);
  voltagea1 = sa1hex * 4.096 / 4094;
  //printf("SA1Hex = %d, so %X and %f Volts was sent out via SPI\r\n",sa1hex,sa1hex,voltagea1); 
  
  
 
  if (sunel1_now == 999999)
  {
    se1hex = 0x00;
  }
  else
  {
  se1hex = floor(1 + (4094*(sunel1_now)/(__PI/4))-4094);
  }                                                     
  //LTC2630_send(0, se1hex);                                
  voltagee1 = se1hex * 4.096 / 4095;
  //printf("SE1Hex = %d, so %X and %f Volts was sent out via SPI\r\n",se1hex,se1hex,voltagee1); 
  
  
  
  if (magxmask == 200000)
  {
    magxhex = 0x00;
  }
  else if (magxmask >= 200000)
  {
    magxhex = 4095 * (magxmask / 400000) - 2048;
  }
  else if (magxmask <= 200000)
  {
    magxhex = 4095-4095*(magxmask/400000);
  }
  //LTC2630_send(0, magxhex);                                
  voltagemx = magxhex * 4.096 / 4095;
 printf("Magx_now = %f\r\n",magx_now);
 printf("Magxmask = %f\r\n", magxmask);
 printf("MAGXHex = %u, so %X and %f Volts was sent out via SPI\r\n",magxhex,magxhex,voltagemx); 
 
 
 
   if (magymask == 200000)
  {
    magyhex = 0x00;
  }
  else if (magymask >= 200000)
  {
    magyhex = 4095 * (magymask / 400000) - 2048;
  }
  else if (magymask <= 200000)
  {
    magyhex = 4095-4095*(magymask/400000);
  }
  //LTC2630_send(0, magyhex);                                
  voltagemy = magyhex * 4.096 / 4095;
  printf("Magy_now = %f\r\n",magy_now);
  printf("Magymask = %f\r\n",magymask);
  printf("MAGYHex = %d, so %X and %f Volts was sent out via SPI\r\n",magyhex,magyhex,voltagemy); 
 
  
  
  if (magzmask == 200000)
  {
    magzhex = 0x00;
  }
  else if (magzmask >= 200000)
  {
    magzhex = 4095 * (magzmask / 400000) - 2048;
  }
  else if (magzmask <= 200000)
  {
    magzhex = 4095-4095*(magzmask/400000);
  }
  //LTC2630_send(0, magyhex);                                
  voltagemz = magzhex * 4.096 / 4095;
 printf("Magz_now = %f\r\n",magz_now);
 printf("Magzmask = %f\r\n", magzmask);
 printf("MAGZHex = %d, so %X and %f Volts was sent out via SPI\r\n",magzhex,magzhex,voltagemz); 
 
  
  
 gyroxhex = (int)(65535*(gyrox_now+(250*__PI/180))/(500*__PI/180));
 //printf("Gyrox_now = %f\r\n",gyrox_now);
 //printf("GYROXHex = %u, so %X is saved for ADCS - TWI\r\n",gyroxhex,gyroxhex);
 
 gyroyhex = (int)(65535*(gyroy_now+(250*__PI/180))/(500*__PI/180));
 //printf("Gyroy_now = %f\r\n",gyroy_now);
 //printf("GYROYHex = %u, so %X is saved for ADCS - TWI\r\n",gyroyhex,gyroyhex);
  
 gyrozhex = (int)(65535*(gyroz_now+(250*__PI/180))/(500*__PI/180));
 //printf("Gyroz_now = %f\r\n",gyroz_now);
 //printf("GYROZHex = %u, so %X is saved for ADCS - TWI\r\n",gyrozhex,gyrozhex);
 

  
 printf("\n\n\r"); 
}