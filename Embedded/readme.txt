Mon 5/4
Due to timing and the transceiver, master/slave can get into a loop where
they both miss each other's messages. Perhaps making an interrupt can make this happen
Sit around and interrupt when received something on the TWI, depending on the number 
act and send that out, keep the main function clear so that timing doesn't mess with anything


Fri 4/30
able to receive and send hex data according to what the master wants and when
Need to bundle the 6 bytes together into a printf to the screen to mimic exactly what
the gyro readings are in the master


4-16/4-17
DAC would not produce good reading.
Giyeon found bad solder on test DAC. 
Works well with CS as PB4 

Upon fixing that, left USB ports were knocked out
RESET
worked well.
Plugging in IPTIME led to service crashes leading to BSOD
RESET
worked well.

Upon going through the code portion for each DAC using only
the test DAC, sometimes the scale switches unreliably from 0-4.096V
to 0-5V. While wired to a 0-5V scale pin/code combo, resetting
reinitializes the DACs at 0x60 (0110) internal reference and it 
goes back to 0-4.096. It should be tested with all pins wired to each
DAC simultaneously. Need to solder the header pins and put the 
controller in there.

Lessons:
add solder the way giyeon did to each pin
check code/wiring
check solder job
check another breadboard
