/****************************************************************************
putchar.c
KAUSAT-5 Sensor Simulator
Copyright (c) 2016 Matt D'Arcy. 
Shared under the MIT License.
****************************************************************************/


#include <iom128.h>
#include <ina90.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


/****************************************************************************
Putchar function - necessary for printf for UART-Serial
****************************************************************************/


int putchar(int print_this)//Copy packet for processing, "clean the mailbox" for a new packet being delivered later
{
  while (((UCSR0A >> 5) & 0x01) == 0) // while UDRE data register is empty
    ;  //Do nothing
  UDR0 = print_this; //When not empty, set UDR1 to the data packet
  UCSR0A |= 0x20; //Set UCSR1A (USART Control and Status Register 1 for Port A) to 0010 0000, default is 0010 0000, as in the data register is empty!
                  //In other words, imagine you have a mailbox where only 1 packet can go at a time. You copied the delivered packet and sent it to processing in the house, but you need to clean out the original packet
                  //from the data register
  return print_this; //send that copied packet to be processed. Returns to the interrupt.
}


/****************************************************************************
Interrupt Service Routine - Print packet data to UART-Serial
****************************************************************************/


#pragma vector = USART0_RXC_vect //Let the controller know that this interrupt protocol will be used in this
__interrupt void H5_UART0IT(void) // Interrupt accepts packet, processes, prints via the putchar function, and continues on
{
  __disable_interrupt(); //disable interrupt and continue with the processing of packet

//The following code gives the user the state of the 3 buffers

  //while(!(UCSR0A & 0x80)); //while UCSR1A is not 1000 0000, do nothing (THIS IS USART RECEIVE COMPLETE FLAG! IT TURNS TO 1 WHEN THE PACKET RECEIVE IS COMPLETE)
                           //otherwise, once UCSR1A is 1000 0000, (PACKET IS RECEIVED AND SITTING IN UDR1 READY FOR USE)
  data = UDR0;  //set data equal to UDR1 - where the packet is sitting
  putchar(data); //run this packet character by character through the putchar function
  if (data == '$'){ //first character of packet
    save_on = 1; //set save_on to 1, we will fill the c array with the packet, character by character
  }
  
  else if (data == '@'){ //otherwise if 'data' is @ symbol, last character of packet
    packet_data[ii] = NULL;//set that final character to null char
    ms_delay(100); //wait 100 milliseconds and then set the flag to allow TWI to commence.
    pure_transmissions_only = 1; //set the flag to allow the TWI transmissions to commence.
    save_on = 0; //set save_on to 0, we will not be filling any more characters of the array 'c'
    if(strcmp(packet_buff1,comparator) == 0){ //if buffer 1 is empty, fill it
      strcpy(packet_buff1,packet_data);
      //printf("I loaded buffer 1\n\r");
    }
    else if ((strcmp(packet_buff1,comparator) != 0) && (strcmp(packet_buff2,comparator) == 0)){ //otherwise if buffer 1 is full and 2 is empty, fill 2
      strcpy(packet_buff2,packet_data);
      //printf("\n\rI loaded buffer 2\n\r");
    }
    else if ((strcmp(packet_buff1,comparator) != 0) && (strcmp(packet_buff2,comparator) != 0) && (strcmp(packet_buff3,comparator) == 0)){ //otherwise if buffer 1 and 2 are full and 3 is empty, fill 3
      strcpy(packet_buff3,packet_data);
      //printf("I loaded buffer 3\n\r");
    }
    
    //while(strcmp(packet_buff1,comparator) == 0); //wait up while the first buffer is empty
    parsing(packet_buff1); 				//enter the parsing function with the next-in-line buffer
    //printf("I processed buffer 1\n\r");

    if(strcmp(packet_buff2,comparator) != 0){
      strcpy(packet_buff1,packet_buff2);	//move contents from buffer 2 into buffer 1
      //printf("I moved 2 into 1\n\r");
      strcpy(packet_buff2,comparator);	//re-initialize buffer 2
    }
    if(strcmp(packet_buff3,comparator) != 0){
      strcpy(packet_buff2,packet_buff3);	//move contents from buffer 3 into buffer 2
      //printf("I moved 3 into 2\n\r");
      strcpy(packet_buff3,comparator);	//re-initialize buffer 3
    }
    ii=0; //reset the counter
    return; //end and wait for next packet
  }
  
  if (save_on ==1){
    packet_data[ii++] = data; //since data (UDR1) is only getting 1 character at a time in serial, this sets each character of the c array to the incoming character in UDR1 before it receives the next character
  }

  __enable_interrupt(); //enable the interrupt again and wait for next packet
  
}
