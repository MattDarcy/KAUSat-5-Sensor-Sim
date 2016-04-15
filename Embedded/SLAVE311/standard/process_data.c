void parsing(char process_this[144])//Copy packet for processing, "clean the mailbox" for a new packet being delivered later
{ 
  ms_delay(100);
  timer(counter);
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
  
  if(process_this[0] == '$')
  {
    
    //SUN SENSOR 1 AZEl
    if(process_this[5]=='?')
    {
        //printf("Sun Sensor 1: sun out of sensor view\n\r");
        sunaz1_now = 999999; //Dummy value to indicate null. This will cause a set to SPI voltage 0x00 (true zero voltage)
        sunel1_now = 999999;
    }
    else if(process_this[5]!='?')
    {  
      for (i=0;i<7;i++)
        sunaz1[i]=process_this[2+i];
      for (i=0;i<7;i++)
        sunel1[i]=process_this[10+i];
      sunaz1_now = strtod(sunaz1,NULL);
      sunel1_now = strtod(sunel1,NULL);
      //printf("Sun Sensor 1 (Az,El): \(%5.3f,%5.3f)\n\r",sunaz1_now,sunel1_now);
    }
    
        //SUN SENSOR 2 AZEl
    if(process_this[21]=='?')
    {
       //printf("Sun Sensor 2: sun out of sensor view\n\r");
        sunaz2_now = 999999; 
        sunel2_now = 999999;
    }
    else if(process_this[21]!='?')
    {  
      for (i=0;i<7;i++)
        sunaz2[i]=process_this[18+i];
      for (i=0;i<7;i++)
        sunel2[i]=process_this[26+i];
      sunaz2_now = strtod(sunaz2,NULL);
      sunel2_now = strtod(sunel2,NULL);
      //printf("Sun Sensor 2 (Az,El): \(%5.3f,%5.3f\)\n\r",sunaz2_now,sunel2_now);
    }
    
          //SUN SENSOR 3 AZEl
    if(process_this[37]=='?')
    {
        //printf("Sun Sensor 3: sun out of sensor view\n\r");
        sunaz3_now = 999999; 
        sunel3_now = 999999;
    }
    else if(process_this[37]!='?')
    {  
      for (i=0;i<7;i++)
        sunaz3[i]=process_this[34+i];
      for (i=0;i<7;i++)
        sunel3[i]=process_this[42+i];
      sunaz3_now = strtod(sunaz3,NULL);
      sunel3_now = strtod(sunel3,NULL);
      //printf("Sun Sensor 3 (Az,El): \(%5.3f,%5.3f\)\n\r",sunaz3_now,sunel3_now);
    }
    
         //SUN SENSOR 4 AZEl
    if(process_this[53]=='?')
    {
        //printf("Sun Sensor 4: sun out of sensor view\n\r");
        sunaz4_now = 999999; 
        sunel4_now = 999999;
    }
    else if(process_this[53]!='?')
    {  
      for (i=0;i<7;i++)
        sunaz4[i]=process_this[50+i];
      for (i=0;i<7;i++)
        sunel4[i]=process_this[58+i];
      sunaz4_now = strtod(sunaz4,NULL);
      sunel4_now = strtod(sunel4,NULL);
      //printf("Sun Sensor 4 (Az,El): \(%5.3f,%5.3f\)\n\r",sunaz4_now,sunel4_now);
    }
    
    //SUN SENSOR 5 AZEl
    if(process_this[69]=='?')
    {
        //printf("Sun Sensor 5: sun out of sensor view\n\r");
        sunaz5_now = 999999; 
        sunel5_now = 999999;
    }
    else if(process_this[69]!='?')
    {  
      for (i=0;i<7;i++)
        sunaz5[i]=process_this[66+i];
      for (i=0;i<7;i++)
        sunel5[i]=process_this[74+i];
      sunaz5_now = strtod(sunaz5,NULL);
      sunel5_now = strtod(sunel5,NULL);
      //printf("Sun Sensor 5 (Az,El): \(%5.3f,%5.3f\)\n\r",sunaz5_now,sunel5_now);
    }
    
    
    //MAGNETIC SENSOR X,Y,Z 
    for(i=0;i<9;i++)
    {
      magx[i]=process_this[82+i];
      magy[i]=process_this[92+i];
      magz[i]=process_this[102+i];
      magx_now = strtod(magx,NULL);
      magy_now = strtod(magy,NULL);
      magz_now = strtod(magz,NULL);
    }
     //printf("Magnetic Field Sensor Components (X,Y,Z): \(%8.2f,%8.2f,%8.2f\)\n\r",magx_now,magy_now,magz_now);
    
    
    //GYRO MAG X,Y,Z
    for(i=0;i<9;i++)
    {
      gyrox[i]=process_this[112+i];
      gyroy[i]=process_this[122+i];
      gyroz[i]=process_this[132+i];
      gyrox_now = strtod(gyrox,NULL);
      gyroy_now = strtod(gyroy,NULL);
      gyroz_now = strtod(gyroz,NULL);
    }
     //printf("Gyro Magnitude (X,Y,Z): \(%8.4f,%8.4f,%8.4f\)\n\r",gyrox_now,gyroy_now,gyroz_now);
    
  
    
//printf("\n\n");
    
  }
  
  
  //We now have parsed float data. We need to send this over to LTC2630_send with variables dac_op 
  //according to how the DAC should treat each data type (0 for sun, 1 for mag) 
  //and the corresponding hex value of the conditioned integer out of 4095/4096 for dataspi for each data value
  //For 12-bit DAC, 4096 if it is cyclical (azimuth, 0 and 2PI are the same) and 4095 for all others.
  //Remember, it is the increments BETWEEN binary values that matter. See spreadsheet.
  
  
  
   /////////////////////////////////////////////////////////////////SUN AZIMUTH 1,2,3,4,5 RANGE 0 to (2*__PI)///////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////SUN ELEVATION 1,2,3,4,5 RANGE +PI/4 to +PI/2////////////////////////////////////////////////////////////
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

  SA1HI = sa1hex >> 4;
  SA1LO = (sa1hex & 0x0F) << 4;
  LTC2630_send(0,SA1HI,SA1LO,1);  
  voltagea1 = sa1hex * 4.096 / 4094;
 //printf("SA1Hex = %d, so %X and %f Volts was sent out via SPI\r\n",sa1hex,sa1hex,voltagea1); 
 //printf("SS1 Az = %5.3f, Hex = %d...%X, Voltage = %f\r\n",sunaz1_now,sa1hex,sa1hex,voltagea1); 

  if (sunel1_now == 999999)
  {
    se1hex = 0x00;
  }
  else
  {
  se1hex = floor(1 + (4094*(sunel1_now)/(__PI/4))-4094);
  }                                                     
  SE1HI = se1hex >> 4;
  SE1LO = (se1hex & 0x0F) << 4;
  LTC2630_send(0,SE1HI,SE1LO,2);    
  voltagee1 = se1hex * 4.096 / 4095;
  //printf("SE1Hex = %d, so %X and %f Volts was sent out via SPI\r\n",se1hex,se1hex,voltagee1);
   // printf("SS1 El = %5.3f, Hex = %d...%X, Voltage = %f\r\n",sunel1_now,se1hex,se1hex,voltagee1);

    if (sunaz2_now == 999999)
  {
    sa2hex = 0x00;
  }
  else if (sunaz2_now == 2 * __PI)
  {
    sa2hex = 0x01;
  }
  else
  {
    sa2hex = floor(1 + 4094*sunaz2_now/(2*__PI));
  }
  SA2HI = sa2hex >> 4;
  SA2LO = (sa2hex & 0x0F) << 4;
  LTC2630_send(0,SA2HI,SA2LO,3); 
  voltagea2 = sa2hex * 4.096 / 4094;
  //printf("SA2Hex = %d, so %X and %f Volts was sent out via SPI\r\n",sa2hex,sa2hex,voltagea2); 
   // printf("SS2 Az = %5.3f, Hex = %d...%X, Voltage = %f\r\n",sunaz2_now,sa2hex,sa2hex,voltagea2); 

  if (sunel2_now == 999999)
  {
    se2hex = 0x00;
  }
  else
  {
  se2hex = floor(1 + (4094*(sunel2_now)/(__PI/4))-4094);
  }                                                     
  SE2HI = se2hex >> 4;
  SE2LO = (se2hex & 0x0F) << 4;
  LTC2630_send(0,SE2HI,SE2LO,4); 
  voltagee2 = se2hex * 4.096 / 4095;
  //printf("SE2Hex = %d, so %X and %f Volts was sent out via SPI\r\n",se2hex,se2hex,voltagee2);
  // printf("SS2 El = %5.3f, Hex = %d...%X, Voltage = %f\r\n",sunel2_now,se2hex,se2hex,voltagee2);

  if (sunaz3_now == 999999)
  {
    sa3hex = 0x00;
  }
  else if (sunaz3_now == 2 * __PI)
  {
    sa3hex = 0x01;
  }
  else
  {
    sa3hex = floor(1 + 4094*sunaz3_now/(2*__PI));
  }
  SA3HI = sa3hex >> 4;
  SA3LO = (sa3hex & 0x0F) << 4;
  LTC2630_send(0,SA1HI,SA1LO,5); 
  voltagea3 = sa3hex * 4.096 / 4094;
  //printf("SA3Hex = %d, so %X and %f Volts was sent out via SPI\r\n",sa3hex,sa3hex,voltagea3); 
  //printf("SS3 Az = %5.3f, Hex = %d...%X, Voltage = %f\r\n",sunaz3_now,sa3hex,sa3hex,voltagea3); 

  if (sunel3_now == 999999)
  {
    se3hex = 0x00;
  }
  else
  {
  se3hex = floor(1 + (4094*(sunel3_now)/(__PI/4))-4094);
  }                                                     
  SE3HI = se3hex >> 4;
  SE3LO = (se3hex & 0x0F) << 4;
  LTC2630_send(0,SE3HI,SE3LO,6);      
  voltagee3 = se3hex * 4.096 / 4095;
  //printf("SE3Hex = %d, so %X and %f Volts was sent out via SPI\r\n",se3hex,se3hex,voltagee3);
 // printf("SS3 El = %5.3f, Hex = %d...%X, Voltage = %f\r\n",sunel3_now,se3hex,se3hex,voltagee3);

  if (sunaz4_now == 999999)
  {
    sa4hex = 0x00;
  }
  else if (sunaz4_now == 2 * __PI)
  {
    sa4hex = 0x01;
  }
  else
  {
    sa4hex = floor(1 + 4094*sunaz4_now/(2*__PI));
  }
  SA4HI = sa4hex >> 4;
  SA4LO = (sa4hex & 0x0F) << 4;
  LTC2630_send(0,SA1HI,SA1LO,7); 
  voltagea4 = sa4hex * 4.096 / 4094;
  //printf("SA4Hex = %d, so %X and %f Volts was sent out via SPI\r\n",sa4hex,sa4hex,voltagea4); 
   // printf("SS4 Az = %5.3f, Hex = %d...%X, Voltage = %f\r\n",sunaz4_now,sa4hex,sa4hex,voltagea4); 

  if (sunel4_now == 999999)
  {
    se4hex = 0x00;
  }
  else
  {
  se4hex = floor(1 + (4094*(sunel4_now)/(__PI/4))-4094);
  }                                                     
  SE4HI = se4hex >> 4;
  SE4LO = (se4hex & 0x0F) << 4;
  LTC2630_send(0,SE4HI,SE4LO,8);        
  voltagee4 = se4hex * 4.096 / 4095;
  //printf("SE4Hex = %d, so %X and %f Volts was sent out via SPI\r\n",se4hex,se4hex,voltagee4);
 // printf("SS4 El = %5.3f, Hex = %d...%X, Voltage = %f\r\n",sunel4_now,se4hex,se4hex,voltagee4);

  if (sunaz5_now == 999999)
  {
    sa5hex = 0x00;
  }
  else if (sunaz5_now == 2 * __PI)
  {
    sa5hex = 0x01;
  }
  else
  {
    sa5hex = floor(1 + 4094*sunaz5_now/(2*__PI));
  }
  SA5HI = sa5hex >> 4;
  SA5LO = (sa5hex & 0x0F) << 4;
  LTC2630_send(0,SA1HI,SA1LO,9); 
  voltagea5 = sa5hex * 4.096 / 4094;
  //printf("SA5Hex = %d, so %X and %f Volts was sent out via SPI\r\n",sa5hex,sa5hex,voltagea5); 
   // printf("SS5 Az = %5.3f, Hex = %d...%X, Voltage = %f\r\n",sunaz5_now,sa5hex,sa5hex,voltagea5); 

  if (sunel5_now == 999999)
  {
    se5hex = 0x00;
  }
  else
  {
  se5hex = floor(1 + (4094*(sunel5_now)/(__PI/4))-4094);
  }                                                     
  SE5HI = se5hex >> 4;
  SE5LO = (se5hex & 0x0F) << 4;
  LTC2630_send(0,SE5HI,SE5LO,10);      
  voltagee5 = se5hex * 4.096 / 4095;
  //printf("SE5Hex = %d, so %X and %f Volts was sent out via SPI\r\n",se5hex,se5hex,voltagee5);
  //printf("SS5 El = %5.3f, Hex = %d...%X, Voltage = %f\r\n",sunel5_now,se5hex,se5hex,voltagee5);







   //By this point, AzEl sensor data has been sent to DAC chips via SPI.
   //GO on to Magnetic Sensors
   //Magnetic sensor range -200000 nT to + 200000 nT. Offset all by +200000 nT for 0 to 400000.
   //Resolution of 1 bit over the 12-bit range
   //voltage is 0 at 0 (200000) increases to 2.048 on the way to 400000 and then anything over 
   //2.048 indicates count-down from 200000 to 0 (increasingly negative nT reading)
  
  //Mask all magnetic sensor values
      magxmask = magx_now + 200000;
      magymask = magy_now + 200000;
      magzmask = magz_now + 200000;
      
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
  MAGXHI = magxhex >> 4;
  MAGXLO = (magxhex & 0x0F) << 4;
  LTC2630_send(0,MAGXHI,MAGXLO,11); 
  voltagemx = magxhex * 4.096 / 4095;
  printf("Magx = %8.2f, Masked = %f,Hex = %u...%X, Voltage = %f\r\n",magx_now,magxmask,magxhex,magxhex,voltagemx); 

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
  MAGYHI = magyhex >> 4;
  MAGYLO = (magyhex & 0x0F) << 4;
  LTC2630_send(0,MAGYHI,MAGYLO,12);
  voltagemy = magyhex * 4.096 / 4095;
  //printf("Magy = %8.2f, Masked = %f,Hex = %u...%X, Voltage = %f\r\n",magy_now,magymask,magyhex,magyhex,voltagemy); 

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
  MAGZHI = magzhex >> 4;
  MAGZLO = (magzhex & 0x0F) << 4;
  LTC2630_send(0,MAGZHI,MAGZLO,13);                               
  voltagemz = magzhex * 4.096 / 4095;
  //printf("Magz = %8.2f, Masked = %f,Hex = %u...%X, Voltage = %f\r\n",magz_now,magzmask,magzhex,magzhex,voltagemz);   

 //By this point, magnetic sensor data has been sent to DAC chips via SPI.
 //GO on to Gyro Data. Gyro data will need to be stored until TWI is requested from the ADCS board master
      
 gyroxhex = (int)(65535*(gyrox_now+(2000*__PI/180))/(4000*__PI/180));
 GYROXHI = gyroxhex >> 8;
 GYROXLO = gyroxhex & 0xFF;
 printf("Gyrox = %8.4f, Hex = %u...%X\r\n",gyrox_now,gyroxhex,gyroxhex);
 
 //printf("Gyrox_now = %f\r\n",gyrox_now);
 //printf("GYROXHex = %u, so %X is saved for ADCS - TWI\r\n",gyroxhex,gyroxhex);
 // printf("Gyrox = %8.4f, Hex = %u...%X\r\n",gyrox_now,gyroxhex,gyroxhex);

 gyroyhex = (int)(65535*(gyroy_now+(2000*__PI/180))/(4000*__PI/180));
 GYROYHI = gyroyhex >> 8;
 GYROYLO = gyroyhex & 0xFF;
 printf("Gyroy = %8.4f, Hex = %u...%X\r\n",gyroy_now,gyroyhex,gyroyhex);

 gyrozhex = (int)(65535*(gyroz_now+(2000*__PI/180))/(4000*__PI/180));
 GYROZHI = gyrozhex >> 8;
 GYROZLO = gyrozhex & 0xFF;
 printf("Gyroz = %8.4f, Hex = %u...%X\r\n\n\n",gyroz_now,gyrozhex,gyrozhex);

 
 // UDR0 = data_zigbee; //When not empty, set UDR1 to the data packet (this transmits it via USART1 to the pc Token2Shell for display instantly)
  counter++;
  UCSR0A |= 0x20; //set UCSR1A (USART Control and Status Register 1 for Port A) to 0010 0000, default is 0010 0000, as in the data register is empty!
                  //In other words, imagine you have a mailbox where only 1 packet can go at a time. You copied the delivered packet and sent it to processing in the house, but you need to clean out the original packet
                  //from the data register
 // return data_zigbee; //send that copied packet to be processed. Returns to the interrupt.
}
