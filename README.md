#KAUSAT-5 Sensor Simulator
MATLAB/Embedded C software for the ADCS sensor simulator for KAUSAT-5, a 3U CubeSat developed at Korea Aerospace University's Space Systems Research Laboratory. This was the main work of my 2014-2015 Fulbright grant to South Korea. The complete system was capable of delivering real-time simulated sensor data from the MATLAB-STK interface via ATMega128 to KAUSAT-5's ADCS board for on-the-ground debugging purposes.  Simulated sensors included 5x sun, x-y-x mag, and x-y-z gyro. Data transfer protocols included UART, SPI, I2C.

#Hardware
ATMega128 (1x as simulator unit, 1x as MASTER for simulator verification)  
LTC2630 Digital-Analog Converter (13x)  
Simulator PCB (1x) created in Altium Designer v13.2
See documentation folder for details

#Embedded
Embedded code was compiled with IAR Embedded Workbench IDE v6.7.  
Embedded code divided into MASTER and SLAVE according to TWI scheme.  
System design scheme implies the sensor simulator itself is SLAVE, and verification unit (in this case my pc, but generally KAUSAT-5) is MASTER.

#SLAVE
To open in IAREW, open Sensor_Simulator.eww in Embedded/SLAVE311  
To see individual .c and .h files, go to Embedded/SLAVE311/standard

#MASTER
To open in IAREW, open MASTER315.eww in Embedded/MASTER315  
To see individual .c and .h files, go to Embedded/MASTER315

#KAUSAT-5
[Lab's Facebook](https://www.facebook.com/ssrlmm/)  
© 2016 [Matt D'Arcy](https://www.linkedin.com/in/mmdarcy), shared under the [MIT License](http://www.opensource.org/licenses/MIT).
