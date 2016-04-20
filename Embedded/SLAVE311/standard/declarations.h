/****************************************************************************
declarations.h
KAUSAT-5 Sensor Simulator
Copyright (c) 2016 Matt D'Arcy. 
Shared under the MIT License.
****************************************************************************/


#ifndef DECLARATIONS_H
#define DECLARATIONS_H


/****************************************************************************
Variable definitions
****************************************************************************/


// Sample TWI transmission commands

#define TWI_CMD_MASTER_WRITE 0x10                                                                                                                                       //internally stored hex values for a write command
#define TWI_CMD_MASTER_READ  0x20                                                                                                                                       //internally stored hex values for a read command

unsigned int i = 0;
unsigned int indicate_m = 1;
volatile unsigned char buf_temp = 0;
float gyrox_now_deg = 0;
volatile unsigned long int gyrox_now_check = 0;
float gyroy_now_deg = 0;
volatile unsigned long int gyroy_now_check = 0;
float gyroz_now_deg = 0;
volatile unsigned long int gyroz_now_check = 0;
unsigned char messageBuf[4];                                                                                                                                          //message buffer 0---1---2---3 with 3 as null char. Each space is 1 byte (1 char)
unsigned char temp, temp2;                                                                                                                                            //more chars

// Own TWI slave address

unsigned int TWI_slaveAddress = 0x10;                                                                                                                                 //arbitration of slave address 0001 0000


/****************************************************************************
Declare variables for SPI
****************************************************************************/


unsigned int mm,mmh,mml;

#define SS_A1 PORTD_Bit4
#define SS_E1 PORTD_Bit6
#define SS_A2 PORTD_Bit2
#define SS_E2 PORTD_Bit3
#define SS_A3 PORTG_Bit3
#define SS_E3 PORTG_Bit4
#define SS_A4 PORTB_Bit5
#define SS_E4 PORTB_Bit4
#define SS_A5 PORTE_Bit5
#define SS_E5 PORTB_Bit0
#define SS_MX PORTE_Bit4
#define SS_MY PORTE_Bit3
#define SS_MZ PORTE_Bit2

int counter = 0;
volatile float voltagea1 = 0;
volatile float voltagea2 = 0;
volatile float voltagea3 = 0;
volatile float voltagea4 = 0;
volatile float voltagea5 = 0;
volatile float voltagee1 = 0;
volatile float voltagee2 = 0;
volatile float voltagee3 = 0;
volatile float voltagee4 = 0;
volatile float voltagee5 = 0;
volatile float voltagemx = 0;
volatile float voltagemy = 0;
volatile float voltagemz = 0;

unsigned int sa1hex,SA1HI,SA1LO;
unsigned int se1hex,SE1HI,SE1LO;
unsigned int sa2hex,SA2HI,SA2LO;
unsigned int se2hex,SE2HI,SE2LO;
unsigned int sa3hex,SA3HI,SA3LO;
unsigned int se3hex,SE3HI,SE3LO;
unsigned int sa4hex,SA4HI,SA4LO;
unsigned int se4hex,SE4HI,SE4LO;
unsigned int sa5hex,SA5HI,SA5LO;
unsigned int se5hex,SE5HI,SE5LO;

unsigned int magxhex,MAGXHI,MAGXLO;
unsigned int magyhex,MAGYHI,MAGYLO;
unsigned int magzhex,MAGZHI,MAGZLO;
volatile float magxmask;
volatile float magymask;
volatile float magzmask;

unsigned int gyroxhex,GYROXHI,GYROXLO;
unsigned int gyroyhex,GYROYHI,GYROYLO;
unsigned int gyrozhex,GYROZHI,GYROZLO;


/****************************************************************************
Declare variables for function: Parsing()
****************************************************************************/


char packet_data[144]; //declare string variable for the packet, 143 characters plus the null = 144
char data; //this is a 1 character string that will extract the packet from UDR1 character by character in a loop
char null[23]; //out of sensor view - 22 characters beginning at 0 (0 to 21) + null (character 22) is size of 23
int save_on, ii = 0; //define integer save_on which will tell the program that the packet_data array still has characters to fill from the received packet, and the ii is each incremented character space of packet_data


/****************************************************************************
//  Declare variables used in maintaining pure TWI transmissions
****************************************************************************/


char packet_buff1[144] = "unused"; //buffer variable for storing packets. Without it, the data packets switch during transmission to external hardware, creating botched data.
char packet_buff2[144] = "unused"; //buffer variable for storing packets. Without it, the data packets switch during transmission to external hardware, creating botched data.
char packet_buff3[144] = "unused"; //buffer variable for storing packets. Without it, the data packets switch during transmission to external hardware, creating botched data.
char comparator[144] = "unused";   //identical string as the initialized buffers to easily revert buffers back to their initial states

int pure_transmissions_only = 0; //TWI data transmission of the 6 bytes can only take place if this equals 1. Since there are packets coming in each second, this can be 
								 //set to 1 a certain delay (try 200 milliseconds) from the instant the new packet has been extracted into the packet_data variable. After the 0x48 (GYROZLO) has
								 //been transmitted, the flag can be reset to 0
int current_command = 0;


/****************************************************************************
//  Declare strings for sensor data for function: Parsing()
****************************************************************************/


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


/****************************************************************************
//  Declare floats for sensor data _before and _now for previous and current 
//  iteration, respectively for function: Parsing()
****************************************************************************/


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


/****************************************************************************
  NON-TWI Function definitions
****************************************************************************/


void init_UART(void);
void init_SPI(void);
void init_DAC(void);

int putchar(int print_this);
void ms_delay(unsigned int cnt);
void timer (int num);

void parsing(char process_this[144]);

void SPI_SPDR(unsigned char data);
void LTC2630_send(int dac_op, unsigned int dataspih, unsigned int dataspil, unsigned int dac_num);
void TWI_Slave_Initialise( unsigned char TWI_ownAddress );
unsigned char TWI_Transceiver_Busy( void );  
unsigned char TWI_Get_State_Info( void ); 
void TWI_Start_Transceiver_With_Data( unsigned char *msg, unsigned char msgSize );
void TWI_Start_Transceiver( void ); 
unsigned char TWI_Get_Data_From_Transceiver( unsigned char *msg, unsigned char msgSize );  
unsigned char TWI_Act_On_Failure_In_Last_Transmission ( unsigned char TWIerrorMsg ); 

#endif
