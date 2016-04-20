# STK-MATLAB Interface

Created with help from this 'STK DIY Series' video: https://vimeo.com/60602502
Also used STK Programming Interface: http://www.agi.com/resources/help/online/stkdevkit/10.1/

MATLAB files:
  
  KAUSAT5_Sensor_Sim2.m
  update_K52.m
  get_attitude_data.m
  get_posvel_data.m
  
STK generated data as text files:

  case1_v.a
  case1_v.e
  
KAUSAT-5 3D model and articulation files for STK:
  
  kausat66.dae
  kausat66.anc
  
#Notes:

    Created with MATLAB 2012B
    User must have following files and path referenced in code: kausat66.dae model and kausat66.anc articulation files
    User must have additional 3 function files and 2 STK data textfiles included in path.
        
    User must have correct COM port for RS232-ATMEGA128 USART referenced in this file.
    User must hit the 'a' key with the figure as the active screen.

#How it works:
 
    1) A user must use J2Perturbation Propagator in STK, plug in all orbital
    and desired time parameters, mass, area, etc. and create two textfiles
    of attitude and position data. They must referenced by name and path below in
    the code, if not in the path of this code's .m file. Even though the
    position and attitude data from the text files will not be used directly
    in further analyses, all data used in further analyses comes from STK as
    a result of the data from the textfiles that are pumped into STK via
    this code. In other words, STK makes data files, then feeds the data files to MATLAB
    in a separate operation. I have attached sample data files for KAUSAT-5's scenario.
 
    2) This code uses RealTime Propagator to define Kausat5 and its sensors.
    I added some graphics to help see what is going on. It inputs the text 
    file data to STK, and the desired data is given to MATLAB.
    The data desired is:
        -Azimuth and Elevation angles of each sun sensor in +/- radians.
        -X,Y,Z body axes' magnetic intensity in units of +/- nT (nano-Teslas).
        -X,Y,Z body axes' gyro rotation in units of +/- radians per second.
 
    3) A separate function uses a timer to update the data from STK to
    output to the MATLAB command window each second. This function also executes data 
    feeding into STK. The data-feed itself calls upon two other functions
    that also must be in the same path as this file, or added before 
    execution. Those two functions designate the text file data into 
    variables like x_pos, y_pos, q1, q2, etc. to be fed into STK. 
 
#Bugs/WIP 

    1) If you zoom into the 3D satellite model in STK, time slows down...
          Determined to be dependent on PC graphics hardware.
    2) Confined to 10 minute run with MATLAB timer
          Stack overflow couldn't help, if you can fix this please send me a msg :)
    3) There are considerable multi-second chunks of data-less time during
       some runs, while sometimes it runs smoothly. Consider running on a
       workhorse of a pc.
