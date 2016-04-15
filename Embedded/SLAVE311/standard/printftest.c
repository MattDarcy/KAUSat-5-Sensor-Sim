#include <ioavr.h>

#include <stdio.h>

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



int main(void)
{
init_UART();
printf("hello");
}