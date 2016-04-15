


/****************************************************************************
//  Initialize UART
****************************************************************************/
void init_UART(void) //Set desired registers according to the ATMEGA128 Datasheet
{
    UCSR0A = 0x00;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);//0x98;
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); 
    UBRR0H = 0x00; //set baud rate hi
    UBRR0L = 0x03; //set baud rate lo, 1 Mbps 
}


/****************************************************************************
//  Initialize SPI
****************************************************************************/
void init_SPI(void) //function to initialize the SPI on the ATMEGA128
{ 
  //PORTD_Bit4 DIR (X) PORT ON (X)
  //PORTD_Bit6 DIR (X) PORT ON (X)
  //PORTD_Bit2 DIR (X) PORT ON (X)
  //PORTD_Bit3 DIR (X) PORT ON (X)
  //PORTG_Bit3 DIR (X) PORT ON (X)
  //PORTG_Bit4 DIR (X) PORT ON (X)
  //PORTB_Bit3 DIR (X) PORT ON (X)
  //PORTB_Bit4 DIR (X) PORT ON (X)
  //PORTE_Bit5 DIR (X) PORT ON (X)
  //PORTB_Bit0 DIR (X) PORT ON (X)
  //PORTE_Bit4 DIR (X) PORT ON (X)
  //PORTE_Bit3 DIR (X) PORT ON (X)
  //PORTE_Bit2 DIR (X) PORT ON (X)
  //SCK PB1    DIR (X) PORT ON (X)
  //SDI PB2    DIR (X) PORT ON (X)
  //Set data directions and activation of pins (1 is output from ATMega, 1 is ON)
  //Note that Pins D0 and D1 are SCL and SDA for TWI, respectively and if TWI errors happen later, alter these.
  DDRB = (1 << DDB7) | (1 << DDB6) | (1 << DDB5) | (1 << DDB4) | (1 << DDB3) | (1 << DDB2) | (1 << DDB1) | (1 << DDB0); 
  DDRD = (1 << DDD7) | (1 << DDD6) | (1 << DDD5) | (1 << DDD4) | (1 << DDD3) | (1 << DDD2) | (1 << DDD1) | (1 << DDD0);
  DDRE = (1 << DDE7) | (1 << DDE6) | (1 << DDE5) | (1 << DDE4) | (1 << DDE3) | (1 << DDE2) | (1 << DDE1) | (1 << DDE0);
  DDRG = (1 << DDG4) | (1 << DDG3) | (1 << DDG2) | (1 << DDG1) | (1 << DDG0);
  PORTB = (1 << PORTB7) | (1 << PORTB6) | (1 << PORTB5) | (1 << PORTB4) | (1 << PORTB3) | (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB0);
  PORTD = (1 << PORTD7) | (1 << PORTD6) | (1 << PORTD5) | (1 << PORTD4) | (1 << PORTD3) | (1 << PORTD2) | (1 << PORTD1) | (1 << PORTD0);
  PORTE = (1 << PORTE7) | (1 << PORTE6) | (1 << PORTE5) | (1 << PORTE4) | (1 << PORTE3) | (1 << PORTE2) | (1 << PORTE1) | (1 << PORTE0);
  PORTG = (1 << PORTG4) | (1 << PORTG3) | (1 << PORTG2) | (1 << PORTG1) | (1 << PORTG0);
  
  SPCR = (1 << SPIE) |(1 << SPE) | (1 << MSTR) | (1 << SPR1) | (0 << SPR0); //SPI CONTROL REGISTER SET, PG 166, SPE = 1 --> enable the SPI, MSTR = 1 --> master SPI mode, SPR1 = 1 and SPR0 = 0 --> frequency of the oscillator divided by 64
  SPSR = (0 << SPI2X); //SPI STATUS REGISTER PG 167, Double SPI Speed Bit SPI2x = 1 --> In master mode, the SPI speed (SCK frequency) is doubled. Minimum SCK period is now 2 CPU clock periods

}

/****************************************************************************
//  Initialize DAC Chip
****************************************************************************/
void init_DAC(void) //this bit of the code initializes the DAC
{ 
  //We need to initialize all 13 DAC chips. drive low, initialize, drive high, move on to the next one
  //#define SS_A1 PORTE_Bit3 --> PORTD_Bit4 --> Chip 1
  //#define SS_E1 PORTE_Bit2 --> PORTD_Bit6 --> Chip 2
  //#define SS_A2 PORTE_Bit5 --> PORTD_Bit2 --> Chip 3
  //#define SS_E2 PORTE_Bit4 --> PORTD_Bit3 --> Chip 4
  //#define SS_A3 PORTE_Bit7 --> PORTG_Bit3 --> Chip 5
  //#define SS_E3 PORTE_Bit6 --> PORTG_Bit4 --> Chip 6
  //#define SS_A4 PORTB_Bit3 --> PORTB_Bit3 --> Chip 7
  //#define SS_E4 PORTB_Bit0 --> PORTB_Bit4 --> Chip 8
  //#define SS_A5 PORTD_Bit4 --> PORTE_Bit5 --> Chip 9
  //#define SS_E5 PORTB_Bit4 --> PORTB_Bit0 --> Chip 10
  //#define SS_MX PORTD_Bit5 --> PORTE_Bit4 --> Chip 11
  //#define SS_MY PORTD_Bit6 --> PORTE_Bit3 --> Chip 12
  //#define SS_MZ PORTD_Bit7 --> PORTE_Bit2 --> Chip 13
 
  //For some reason, chip selecting needs to be done inside the same C function that sends the data (LTC2630_Send).
  //To overcome this obstacle, made LTC2630_send into a new format. 
 
  LTC2630_send(8,0x00,0x00,1); 
  LTC2630_send(8,0x00,0x00,2);          
  LTC2630_send(8,0x00,0x00,3); 
  LTC2630_send(8,0x00,0x00,4);
  LTC2630_send(8,0x00,0x00,5);
  LTC2630_send(8,0x00,0x00,6);
  LTC2630_send(8,0x00,0x00,7);
  LTC2630_send(8,0x00,0x00,8);
  LTC2630_send(8,0x00,0x00,9);
  LTC2630_send(8,0x00,0x00,10);
  LTC2630_send(8,0x00,0x00,11);
  LTC2630_send(8,0x00,0x00,12);
  LTC2630_send(8,0x00,0x00,13);
    
}
