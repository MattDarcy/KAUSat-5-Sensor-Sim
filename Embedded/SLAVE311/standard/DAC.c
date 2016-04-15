
/****************************************************************************
  DAC Data Transfer Protocol
****************************************************************************/
void SPI_SPDR(unsigned char data) //defines a data transmission and bit-shifting
{ 
  SPDR = data;   //defines SPDR (SPI DATA REGISTER pg 168) to the data that was inputted to SPI_SPDR. This 'data' is written to the SPDR and initiates data transmisson
  while(!(SPSR & (1<<SPIF))); //while bit 7 (8th bit) SPIF is not 1 (is 0), do nothing. When it is 1, continue on to putchar(SPIF)
}  


/****************************************************************************
  DAC Specific Data Transfers 
****************************************************************************/
void LTC2630_send(int dac_op, unsigned int dataspih, unsigned int dataspil, unsigned int dac_num) //decide what to send and how to behave with a switch case. Inputs are the command case and the data
{ 
  // DAC Operation Selection
  // Every transmission must be 4 bytes (32-bits) 
  // 0x@@ 0xC@ 0x$$ 0x$@ @- don't care bit, C is command, $ is data.

  // My PCB config is such that Pin1 chip select of the dac chips and corresponding ATMEGA128 Pins are:
  //#define SS_A1 PORTE_Bit3 --> PORTD_Bit4
  //#define SS_E1 PORTE_Bit2 --> PORTD_Bit6
  //#define SS_A2 PORTE_Bit5 --> PORTD_Bit2
  //#define SS_E2 PORTE_Bit4 --> PORTD_Bit3
  //#define SS_A3 PORTE_Bit7 --> PORTG_Bit3
  //#define SS_E3 PORTE_Bit6 --> PORTG_Bit4
  //#define SS_A4 PORTB_Bit3 --> PORTB_Bit3
  //#define SS_E4 PORTB_Bit0 --> PORTB_Bit4
  //#define SS_A5 PORTD_Bit4 --> PORTE_Bit5
  //#define SS_E5 PORTB_Bit4 --> PORTB_Bit0
  //#define SS_MX PORTD_Bit5 --> PORTE_Bit4
  //#define SS_MY PORTD_Bit6 --> PORTE_Bit3
  //#define SS_MZ PORTD_Bit7 --> PORTE_Bit2
  //Before and after each DAC chip init, the respective pin is driven LO. After the send, driven HI. Same case for each respective pin every parsing function iteration.

  if(dac_num == 1){
  switch(dac_op)
  {    
    case 0:                                           //For data values to be sent through 
    SS_A1 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x30);                                   //Send the command 0011 and 0000 as dont-care bits
    SPI_SPDR(dataspih);                               //Send the upper data byte
    SPI_SPDR(dataspil);                               //Send the lower data byte (last 4 bits of data and then 4 dont-care bits)
    SS_A1 = 1;
    break;
    
    case 8:                                           //CMD8(C3 = 0, C2 = 1, C1 = 1, C0 = 1) Select Supply as Reference (turn it on)
    SS_A1 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x60);                                   //Sends an 8 bit-byte with the first 4 bits being the command codes from pg 13, this case is 0000
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits. 
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits.
    SS_A1 = 1;
    break;                                                          
  }
  }
  
  else if(dac_num == 2){
  switch(dac_op)
  {    
    case 0:                                           //For data values to be sent through 
    SS_E1 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x30);                                   //Send the command 0011 and 0000 as dont-care bits
    SPI_SPDR(dataspih);                               //Send the upper data byte
    SPI_SPDR(dataspil);                               //Send the lower data byte (last 4 bits of data and then 4 dont-care bits)
    SS_E1 = 1;
    break;
    
    case 8:                                           //CMD8(C3 = 0, C2 = 1, C1 = 1, C0 = 1) Select Supply as Reference (turn it on)
    SS_E1 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x60);                                   //Sends an 8 bit-byte with the first 4 bits being the command codes from pg 13, this case is 0000
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits. 
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits.
    SS_E1 = 1;
    break;                                                          
  }
  }
  
  else if(dac_num == 3){
  switch(dac_op)
  {    
    case 0:                                           //For data values to be sent through 
    SS_A2 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x30);                                   //Send the command 0011 and 0000 as dont-care bits
    SPI_SPDR(dataspih);                               //Send the upper data byte
    SPI_SPDR(dataspil);                               //Send the lower data byte (last 4 bits of data and then 4 dont-care bits)
    SS_A2 = 1;
    break;
    
    case 8:                                           //CMD8(C3 = 0, C2 = 1, C1 = 1, C0 = 1) Select Supply as Reference (turn it on)
    SS_A2 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x60);                                   //Sends an 8 bit-byte with the first 4 bits being the command codes from pg 13, this case is 0000
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits. 
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits.
    SS_A2 = 1;
    break;                                                          
  }
  }
  
  else if(dac_num == 4){
  switch(dac_op)
  {    
    case 0:                                           //For data values to be sent through 
    SS_E2 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x30);                                   //Send the command 0011 and 0000 as dont-care bits
    SPI_SPDR(dataspih);                               //Send the upper data byte
    SPI_SPDR(dataspil);                               //Send the lower data byte (last 4 bits of data and then 4 dont-care bits)
    SS_E2 = 1;
    break;
    
    case 8:                                           //CMD8(C3 = 0, C2 = 1, C1 = 1, C0 = 1) Select Supply as Reference (turn it on)
    SS_E2 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x60);                                   //Sends an 8 bit-byte with the first 4 bits being the command codes from pg 13, this case is 0000
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits. 
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits.
    SS_E2 = 1;
    break;                                                          
  }
  }
  
  else if(dac_num == 5){
  switch(dac_op)
  {    
    case 0:                                           //For data values to be sent through 
    SS_A3 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x30);                                   //Send the command 0011 and 0000 as dont-care bits
    SPI_SPDR(dataspih);                               //Send the upper data byte
    SPI_SPDR(dataspil);                               //Send the lower data byte (last 4 bits of data and then 4 dont-care bits)
    SS_A3 = 1;
    break;
    
    case 8:                                           //CMD8(C3 = 0, C2 = 1, C1 = 1, C0 = 1) Select Supply as Reference (turn it on)
    SS_A3 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x60);                                   //Sends an 8 bit-byte with the first 4 bits being the command codes from pg 13, this case is 0000
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits. 
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits.
    SS_A3 = 1;
    break;                                                          
  }
  }
  
  else if(dac_num == 6){
  switch(dac_op)
  {    
    case 0:                                           //For data values to be sent through 
    SS_E3 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x30);                                   //Send the command 0011 and 0000 as dont-care bits
    SPI_SPDR(dataspih);                               //Send the upper data byte
    SPI_SPDR(dataspil);                               //Send the lower data byte (last 4 bits of data and then 4 dont-care bits)
    SS_E3 = 1;
    break;
    
    case 8:                                           //CMD8(C3 = 0, C2 = 1, C1 = 1, C0 = 1) Select Supply as Reference (turn it on)
    SS_E3 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x60);                                   //Sends an 8 bit-byte with the first 4 bits being the command codes from pg 13, this case is 0000
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits. 
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits.
    SS_E3 = 1;
    break;                                                          
  }
  }
  
  else if(dac_num == 7){
  switch(dac_op)
  { 
    case 0:                                           //For data values to be sent through 
    SS_A4 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x30);                                   //Send the command 0011 and 0000 as dont-care bits
    SPI_SPDR(dataspih);                               //Send the upper data byte
    SPI_SPDR(dataspil);                               //Send the lower data byte (last 4 bits of data and then 4 dont-care bits)
    SS_A4 = 1;
    break;
    
    case 8:                                           //CMD8(C3 = 0, C2 = 1, C1 = 1, C0 = 1) Select Supply as Reference (turn it on)
    SS_A4 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x60);                                   //Sends an 8 bit-byte with the first 4 bits being the command codes from pg 13, this case is 0000
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits. 
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits.
    SS_A4 = 1;
    break;                                                          
  }
  }
  
  else if(dac_num == 8){
  switch(dac_op)
  {    
    case 0:                                           //For data values to be sent through 
    SS_E4 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x30);                                   //Send the command 0011 and 0000 as dont-care bits
    SPI_SPDR(dataspih);                               //Send the upper data byte
    SPI_SPDR(dataspil);                               //Send the lower data byte (last 4 bits of data and then 4 dont-care bits)
    SS_E4 = 1;
    break;
    
    case 8:                                           //CMD8(C3 = 0, C2 = 1, C1 = 1, C0 = 1) Select Supply as Reference (turn it on)
    SS_E4 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x60);                                   //Sends an 8 bit-byte with the first 4 bits being the command codes from pg 13, this case is 0000
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits. 
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits.
    SS_E4 = 1;
    break;                                                          
  }
  }
  
  else if(dac_num == 9){
  switch(dac_op)
  {    
    case 0:                                           //For data values to be sent through 
    SS_A5 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x30);                                   //Send the command 0011 and 0000 as dont-care bits
    SPI_SPDR(dataspih);                               //Send the upper data byte
    SPI_SPDR(dataspil);                               //Send the lower data byte (last 4 bits of data and then 4 dont-care bits)
    SS_A5 = 1;
    break;
    
    case 8:                                           //CMD8(C3 = 0, C2 = 1, C1 = 1, C0 = 1) Select Supply as Reference (turn it on)
    SS_A5 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x60);                                   //Sends an 8 bit-byte with the first 4 bits being the command codes from pg 13, this case is 0000
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits. 
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits.
    SS_A5 = 1;
    break;                                                          
  }
  }
  
  else if(dac_num == 10){
  switch(dac_op)
  {    
    case 0:                                           //For data values to be sent through 
    SS_E5 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x30);                                   //Send the command 0011 and 0000 as dont-care bits
    SPI_SPDR(dataspih);                               //Send the upper data byte
    SPI_SPDR(dataspil);                               //Send the lower data byte (last 4 bits of data and then 4 dont-care bits)
    SS_E5 = 1;
    break;
    
    case 8:                                           //CMD8(C3 = 0, C2 = 1, C1 = 1, C0 = 1) Select Supply as Reference (turn it on)
    SS_E5 = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x60);                                   //Sends an 8 bit-byte with the first 4 bits being the command codes from pg 13, this case is 0000
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits. 
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits.
    SS_E5 = 1;
    break;                                                          
  }
  }
  
  else if(dac_num == 11){
  switch(dac_op)
  {    
    case 0:                                           //For data values to be sent through 
    SS_MX = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x30);                                   //Send the command 0011 and 0000 as dont-care bits
    SPI_SPDR(dataspih);                               //Send the upper data byte
    SPI_SPDR(dataspil);                               //Send the lower data byte (last 4 bits of data and then 4 dont-care bits)
    SS_MX = 1;
    break;
    
    case 8:                                           //CMD8(C3 = 0, C2 = 1, C1 = 1, C0 = 1) Select Supply as Reference (turn it on)
    SS_MX = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x60);                                   //Sends an 8 bit-byte with the first 4 bits being the command codes from pg 13, this case is 0000
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits. 
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits.
    SS_MX = 1;
    break;                                                          
  }
  }
  
  else if(dac_num == 12){
  switch(dac_op)
  {    
    case 0:                                           //For data values to be sent through 
    SS_MY = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x30);                                   //Send the command 0011 and 0000 as dont-care bits
    SPI_SPDR(dataspih);                               //Send the upper data byte
    SPI_SPDR(dataspil);                               //Send the lower data byte (last 4 bits of data and then 4 dont-care bits)
    SS_MY = 1;
    break;
    
    case 8:                                           //CMD8(C3 = 0, C2 = 1, C1 = 1, C0 = 1) Select Supply as Reference (turn it on)
    SS_MY = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x60);                                   //Sends an 8 bit-byte with the first 4 bits being the command codes from pg 13, this case is 0000
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits. 
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits.
    SS_MY = 1;
    break;                                                          
  }
  }
  
  else if(dac_num == 13){
  switch(dac_op)
  {    
    case 0:                                           //For data values to be sent through 
    SS_MZ = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x30);                                   //Send the command 0011 and 0000 as dont-care bits
    SPI_SPDR(dataspih);                               //Send the upper data byte
    SPI_SPDR(dataspil);                               //Send the lower data byte (last 4 bits of data and then 4 dont-care bits)
    SS_MZ = 1;
    break;
    
    case 8:                                           //CMD8(C3 = 0, C2 = 1, C1 = 1, C0 = 1) Select Supply as Reference (turn it on)
    SS_MZ = 0;
    SPI_SPDR(0xFF);                                   //Send a byte of dont-care bits.
    SPI_SPDR(0x60);                                   //Sends an 8 bit-byte with the first 4 bits being the command codes from pg 13, this case is 0000
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits. 
    SPI_SPDR(0x00);                                   //Send a data byte of dont-care bits.
    SS_MZ = 1;
    break;                                                          
  }
  }
  
  
  
  
  
  
}