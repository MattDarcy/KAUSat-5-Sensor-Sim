%% Foreward

 % User must have following files and path referenced in code: .dae model and .anc articulation files
 % User must have additional 3 function files and 2 STK data textfiles included in path.
        
 % User must have correct COM port for RS232-ATMEGA128 USART referenced in this file.
 % User must hit the 'a' key with the figure as the active screen.

 % This simulator works as follows:
 
 % 1) A user must use J2Perturbation Propagator in STK, plug in all orbital
 % and desired time parameters, mass, area, etc. and create two textfiles
 % of attitude and position data. They must referenced by name and path below in
 % the code, if not in the path of this code's .m file. Even though the
 % position and attitude data from the text files will not be used directly
 % in further analyses, all data used in further analyses comes from STK as
 % a result of the data from the textfiles that are pumped into STK via
 % this code.
 
 % 2) This code uses RealTime Propagator to define Kausat5 and its sensors.
 % I added some graphics to help see what is going on. It inputs the text 
 % file data to STK, and the dependent data we want is given to MATLAB.
 % The data we want is:
 %     -Azimuth and Elevation angles of each sun sensor in +/- radians.
 %     -X,Y,Z body axes' magnetic intensity in units of +/- nT (nano-Teslas).
 %     -X,Y,Z body axes' gyro rotation in units of +/- radians per second.
 
 % 3) A separate function uses a timer to update the data from STK to
 % output to the MATLAB command window each second. This function also executes data 
 % feeding into STK. The data feeding itself calls upon two other functions
 % that also must be in the same path as this file, or added before 
 % execution. Those two functions designate the text file data into 
 % variables like x_pos, y_pos, q1, q2, etc. to be fed into STK. 
 
%% Bugs/WIP
 % 1) Want to make scenario time synced with computer clock instead of LCLG
 % 2) Sensor cones denote Earth obscuration instead of sun's. 
 % 3) If you zoom into the 3D satellite model in STK, time slows down...
 % 4) Confined to 10 minute run with MATLAB timer
 % 5) There are considerable multi-second chunks of dataless time during
 % some runs, while sometimes it runs smoothly. Consider running on a
 % workhorse of a pc.

%% Date Rev# Who? Changes
% 2014/10/22 Rev#1 Hongrae Add new data providers for real-time for a sunsensor
% 2014/10/22 Rev#2 Matt add code for all sun sensors
% 2014/10/27 Rev#3 Matt add code to change sun sensors --> AzEl!
% 2014/10/28 Rev#4 Matt add code to correct data format
% 2014/10/28 Rev#5 Matt add code to include gyro rate data
% 2014/11/21 Rev#6 Matt add variables modeldir and comport, clean comments
% 2015/01/20 Rev#7 Matt clean up code and comments
% 2015/01/23 Rev#8 Matt correct sun sensor ranges --> Azimuth (0 to +2Pi),
%                  Elevation (+Pi/4 to +Pi/2)
     %% 1) Initialization
        close all
        clear all
        clc
        global sundata
        global solint
        global timer_reset
        global it
        global s
        global i %this is a double, only for counting
        global j %this is an int, takes the double and makes it usable in array operations
        global comport
        global modeldir
        global packet
       
        modeldir = 'VO */Satellite/Kausat5 Model File "C:\Users\Administrator\Dropbox\! ADCS Simulator\ADCS Sensor Data Generation Project\STK-MATLAB Interface\STKfiles\kausat66.dae';
        comport = 'COM6';
        timer_reset = 1;
        i = 0;
        j = 0;
   
    % Use as-is for putting data packets to spreadsheet for duration of the simulation.
    % Pumps packets into newarray and oldarray each iteration
        global pop
        global oldarray
        global newarray
        pop = 1;                 
        for pop = 1:605;
            oldarray{pop,1} = 'string';
            newarray{pop,1} = 'string';
            pop = pop + 1;
        end
     
%% 2) Initialize Scenario 
    % Establish the connection
try
    % Grab an existing instance of STK
        uiapp = actxGetRunningServer('STK10.application');
catch
    % STK is not running, launch new instance
    % Launch a new instance of STK and grab it
        uiapp = actxserver('STK10.application');
end

    % get the root from the personality
    % it has two... get the second, its the newer STK Object Model Interface as
    % documented in the STK Help
        root = uiapp.Personality2;

    % set visible to true (show STK GUI)
        uiapp.visible = 1;

    % From the STK Object Root you can command every aspect of the STK GUI
    % close current scenario or open new one
try
        root.CloseScenario();
        root.NewScenario('KAUSat5_Sensor_Simulation');
catch
        root.NewScenario('KAUSat5_Sensor_Simulation');
end

    % get the scenario root, its of type IAgScenario 
        scenObj = root.CurrentScenario;

    % set the object model to expect all dates in Local Gregorian
        root.UnitPreferences.Item('DateFormat').SetCurrentUnit('LCLG');
    % set the object model to expect all distances in meters
        root.UnitPreferences.Item('Distance').SetCurrentUnit('m');

    % Get the system clock time and use that to set up the scenario's start and
    % stop time.
        tomorrow_date = datestr((now+10), 'dd mmm yyyy HH:MM:SS.FFF');
        current_date = datestr((now), 'dd mmm yyyy HH:MM:SS.FFF');               
        scenObj.Epoch = current_date;
        scenObj.StopTime = tomorrow_date;
        scenObj.StartTime = current_date;

    % set the scenario's animation properties to animate in realtime mode
        scAnimation = scenObj.Animation;
        scAnimation.AnimStepType = 'eScRealTime';
        %scAnimation.AnimStepValue = 0.016;  % this 0.016 makes a pseudo-realtime
        

    % reset the VO window and then begin playing the animation (in realtime)
        root.Rewind
        root.PlayForward
        
%% 3) Declare KAUSat5

    % Create KAUSat5 at sun synchronous 10:30 local time of descending node, 600 km orbit
        Kausat5 = scenObj.Children.New('eSatellite', 'KAUSat5');
   
    % set the satellite to expect realtime position and attitude data
        Kausat5.SetPropagatorType('ePropagatorRealtime');
        %Kausat5.Propagator.InitialState.Representation.AssignClassical('eCoordinateSystemICRF', 7059.14, 0, 98, 0, -2, 0);
        Kausat5.Propagator.LookAheadPropagator = 'eLookAheadTwoBody';
        Kausat5.Propagator.Duration.LookAhead = 86400.00;
        Kausat5.Propagator.Duration.LookBehind = 86400.00;
        Kausat5.Propagator.TimeStep = 60.0;
        Kausat5.Propagator.Propagate;
        
        Kausat5.SetAttitudeType('eAttitudeRealTime');
        Kausat5.Attitude.LookAheadMethod = 'eHold';
        Kausat5.Attitude.Duration.LookAhead = 86400.0;
        Kausat5.Attitude.Duration.LookBehind = 86400.0;

    % set mass of the satellite
        root.ExecuteCommand(['SetMass */Satellite/KAUSat5 Value 3.99']);
            

%% 4) Model and Articulations

    % Import model file and solar panel deployment
    % NOTE: Articulation starttime's are in sync with computer clock.
    % They are commented out here but may be executed by the user to extend
    % solar panels. Note: sun sensors will not move with the panels in STK.
        root.ExecuteCommand(modeldir);
        root.ExecuteCommand('VO */Satellite/Kausat5 Articulate "24 Mar 2015 09:06:00.00" 2 Spanel1 Roll -90 0');  
        root.ExecuteCommand('VO */Satellite/Kausat5 Articulate "24 Mar 2015 09:06:00.00" 2 Spanel2 Roll -90 0');                      
        root.ExecuteCommand('VO */Satellite/Kausat5 Articulate "24 Mar 2015 09:06:00.00" 2 Spanel3 Roll -90 0');                        
        root.ExecuteCommand('VO */Satellite/Kausat5 Articulate "24 Mar 2015 09:06:00.00" 2 Spanel4 Roll -90 0');   
        
% 5) Display Vectors, shrink model, disable lighting

%    Set up velocity vector, sun vector, nadir vector
%    Scales vectors (not needed now as the satellite model is scaled at
%    the end of this section).
        root.ExecuteCommand('Graphics */Satellite/Kausat5 Basic Show On Inherit Off Label Off Orbit Off');
%         root.ExecuteCommand('VO */Satellite/Kausat5 SetVectorGeometry Modify "Satellite/Kausat5 Velocity Vector" Show On ShowMagnitude On Color red MagnitudeUnits km/sec');
%         root.ExecuteCommand('VO */Satellite/Kausat5 SetVectorGeometry Modify "Satellite/Kausat5 Sun Vector" Show On');
%         root.ExecuteCommand('VO */Satellite/Kausat5 SetVectorGeometry Modify "Satellite/Kausat5 Nadir Vector" Show On');
%         root.ExecuteCommand('VO */Satellite/Kausat5 SetVectorGeometry Add "Satellite/Kausat5 Body.X Vector" Show On');
%         root.ExecuteCommand('VO */Satellite/Kausat5 SetVectorGeometry Add "Satellite/Kausat5 Body.Y Vector" Show On');
%         root.ExecuteCommand('VO */Satellite/Kausat5 SetVectorGeometry Add "Satellite/Kausat5 Body.Z Vector" Show On');

%    shrink size of KAUSat5 to clearly see vectors of interest
       root.ExecuteCommand('VO */Satellite/Kausat5 Articulate "4 Oct 2014 07:27:25.00" 10 body Size 1 0.025');
        
%    disable lighting so the satellite is visible during eclipse
        root.ExecuteCommand('VO * Lighting Show Off');
        
%% 6) Define Sensors

    % Define first sun sensor, full clock angle, 45 degree half-cone width, at
    % the top of the satellite in the center.
        sunsensor1 = Kausat5.Children.New('eSensor', 'SunSensor1');
        root.ExecuteCommand('Define */Satellite/Kausat5/Sensor/SunSensor1 Conical 0.0 45 0.0 360.0');
        root.ExecuteCommand('Point */Satellite/Kausat5/Sensor/SunSensor1 Fixed AzEl 0 -90');
        root.ExecuteCommand('Location */Satellite/Kausat5/Sensor/SunSensor1 Fixed Cartesian 0 0 -2')
        root.ExecuteCommand('VO */Satellite/Kausat5/Sensor/SunSensor1 SpaceProjection Distance 5');
   
    % Define side panel sun sensors
        sunsensor2 = Kausat5.Children.New('eSensor', 'SunSensor2');
        root.ExecuteCommand('Define */Satellite/Kausat5/Sensor/SunSensor2 Conical 0.0 45 0.0 360.0');
        root.ExecuteCommand('Point */Satellite/Kausat5/Sensor/SunSensor2 Fixed AzEl 0 0');
        root.ExecuteCommand('Location */Satellite/Kausat5/Sensor/SunSensor2 Fixed Cartesian 1.3 0 2.75');
        root.ExecuteCommand('VO */Satellite/Kausat5/Sensor/SunSensor2 SpaceProjection Distance 5');
        
        sunsensor3 = Kausat5.Children.New('eSensor', 'SunSensor3');
        root.ExecuteCommand('Define */Satellite/Kausat5/Sensor/SunSensor3 Conical 0.0 45 0.0 360.0');
        root.ExecuteCommand('Point */Satellite/Kausat5/Sensor/SunSensor3 Fixed AzEl 90 0');
        root.ExecuteCommand('Location */Satellite/Kausat5/Sensor/SunSensor3 Fixed Cartesian 0 1.3 2.75');
        root.ExecuteCommand('VO */Satellite/Kausat5/Sensor/SunSensor3 SpaceProjection Distance 5');
        
        sunsensor4 = Kausat5.Children.New('eSensor', 'SunSensor4');
        root.ExecuteCommand('Define */Satellite/Kausat5/Sensor/SunSensor4 Conical 0.0 45 0.0 360.0');
        root.ExecuteCommand('Point */Satellite/Kausat5/Sensor/SunSensor4 Fixed AzEl -90 0');
        root.ExecuteCommand('Location */Satellite/Kausat5/Sensor/SunSensor4 Fixed Cartesian 0 -1.3 2.75');
        root.ExecuteCommand('VO */Satellite/Kausat5/Sensor/SunSensor4 SpaceProjection Distance 5');
        
        sunsensor5 = Kausat5.Children.New('eSensor', 'SunSensor5');
        root.ExecuteCommand('Define */Satellite/Kausat5/Sensor/SunSensor5 Conical 0.0 45 0.0 360.0');
        root.ExecuteCommand('Point */Satellite/Kausat5/Sensor/SunSensor5 Fixed AzEl -180 0');
        root.ExecuteCommand('Location */Satellite/Kausat5/Sensor/SunSensor5 Fixed Cartesian -1.3 0 2.75');
        root.ExecuteCommand('VO */Satellite/Kausat5/Sensor/SunSensor5 SpaceProjection Distance 5');
        
%     Display sun and boresight vectors for each sensor.
%        root.ExecuteCommand('VO */Satellite/Kausat5/Sensor/SunSensor1 SetVectorGeometry Modify "Satellite/Kausat5/Sensor/SunSensor1 Boresight Vector" Show On');
%        root.ExecuteCommand('VO */Satellite/Kausat5/Sensor/SunSensor1 SetVectorGeometry Modify "Satellite/Kausat5/Sensor/SunSensor1 Sun Vector" Show On');
%        root.ExecuteCommand('VO */Satellite/Kausat5/Sensor/SunSensor2 SetVectorGeometry Modify "Satellite/Kausat5/Sensor/SunSensor2 Boresight Vector" Show On');
%        root.ExecuteCommand('VO */Satellite/Kausat5/Sensor/SunSensor2 SetVectorGeometry Modify "Satellite/Kausat5/Sensor/SunSensor2 Sun Vector" Show On');
%        root.ExecuteCommand('VO */Satellite/Kausat5/Sensor/SunSensor3 SetVectorGeometry Modify "Satellite/Kausat5/Sensor/SunSensor3 Boresight Vector" Show On');
%        root.ExecuteCommand('VO */Satellite/Kausat5/Sensor/SunSensor3 SetVectorGeometry Modify "Satellite/Kausat5/Sensor/SunSensor3 Sun Vector" Show On');
%        root.ExecuteCommand('VO */Satellite/Kausat5/Sensor/SunSensor4 SetVectorGeometry Modify "Satellite/Kausat5/Sensor/SunSensor4 Boresight Vector" Show On');
%        root.ExecuteCommand('VO */Satellite/Kausat5/Sensor/SunSensor4 SetVectorGeometry Modify "Satellite/Kausat5/Sensor/SunSensor4 Sun Vector" Show On');
%        root.ExecuteCommand('VO */Satellite/Kausat5/Sensor/SunSensor5 SetVectorGeometry Modify "Satellite/Kausat5/Sensor/SunSensor5 Boresight Vector" Show On');
%        root.ExecuteCommand('VO */Satellite/Kausat5/Sensor/SunSensor5 SetVectorGeometry Modify "Satellite/Kausat5/Sensor/SunSensor5 Sun Vector" Show On');     
        
%% 7) Setup realtime position and velocity display in STK 3D graphics window.
    % This will work with sun sensors once the 'add' option is coded for
    % each sun sensor data display.
    % find the J2000 Position and Velocity data display 
    
% for i = 0:sunsensor1.VO.DataDisplay.Count-1
%     if (strcmp(sunsensor1.VO.DataDisplay.Item(i).Name, 'SunSensor1'))
%         posDD = sunsensor1.VO.DataDisplay.Item(i);
%         posDD.IsVisible = 1;
%         posDD.FontColor = '000255000';
%      elseif (strcmp(sunsensor1.VO.DataDisplay.Item(i).Name, 'Boresight Intersection'))
%          attDD = sunsensor1.VO.DataDisplay.Item(i);
%          attDD.IsVisible = 1;
%          attDD.Y = 180;
%          attDD.FontColor = '255255000';
%     end
% end

%% 8) Text File Reading

    % open the file with all positional data (this wouldn't be necessary in a
    % realworld application where you'd be getting the realtime data from an
    % outside application.
        fid = fopen('case1_v.e','r');
        fseek(fid, 0, 'eof');
        eof_byte = ftell(fid);
        fseek(fid, 0, 'bof');

    % do the same thing for the attitude data
        fid2 = fopen('case1_v.a','r');
        fseek(fid2, 0, 'eof');
        eof_byte2 = ftell(fid2);
        fseek(fid2, 0, 'bof');

%% 9) Realtime data output in MATLAB command window

    % Setup a timer in Matlab that goes each second. Each second, get the
    % time and sun sensor angles from STK and then display them in the
    % command window.
 


            it = 0;
            primID = 1;
            tic

            t = timer;
            t.ExecutionMode = 'fixedRate';
            t.BusyMode = 'drop';
            t.Period = 1;
            
            s = serial(comport,'BaudRate',250000);
            fopen(s);        
        
            
            t.TimerFcn = {@update_K52, fid, fid2, root,sunsensor1,sunsensor2,sunsensor3,sunsensor4,sunsensor5,Kausat5};
            start(t)

            
            
            
     % Develop a code to open a figure and upon execution of the 'a' key,
     % stop the timer operation, close the COM port, and display a
     % confirmation dialogue to the command window.
     
        f=figure;
            uicontrol('Style', 'text',...
            'String', 'With this figure window active, press the "a" key to stop the program. If that fails, close window and hit cntrl + c in the command window',... %replace something with the text you want
            'Units','normalized', 'FontSize', [20],...
            'Position', [0.1 0.1 0.8 0.8]);
        
            flag = 0;

            while flag == 0 
                pause(0.000001);
                if i ~= 0
                        newarray{int32(i),1} = {packet};
                        oldarray = newarray;
                end    
                        if get(f,'currentkey') == 'a'
                            flag = 1;
                            stop(t)
                            fclose(s);
                            disp('COM Port Closed. You may run the code again or quit')
                        end
            end
            
                
          

            if ishandle(f), delete(f); end           
     
           


