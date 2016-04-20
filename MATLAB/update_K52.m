%  update_K52.m
%  KAUSAT-5 Sensor Simulator
%
%  Copyright (c) 2016 Matt D'Arcy.
%  Shared under the MIT License.
%
%% Processing data for serial output

function update_K5(obj,event,fid,fid2,root,sunsensor1,sunsensor2,sunsensor3,sunsensor4,sunsensor5,Kausat5)
    %clc
    global sundata
    global solint
    global it
    global s
    global i
    global j
    global oldarray
    global newarray
    global packet
    curTime = datestr(event.Data.time, 'dd mmm yyyy HH:MM:SS.FFF');
      
% Two functions make variables out of a line of each text files.

    [epsec, x_pos, y_pos, z_pos, x_vel, y_vel, z_vel] = get_posvel_data(fid);
    [epsec, q1, q2, q3, q4] = get_attitude_data(fid2);
    curTime = datestr((now), 'dd mmm yyyy HH:MM:SS.FFF');
       
% The variables are fed into STK.

    Kausat5.Propagator.PointBuilder.ECI.Add(curTime,...
    str2double(x_pos), str2double(y_pos), str2double(z_pos),...
    str2double(x_vel), str2double(y_vel), str2double(z_vel));
    
    Kausat5.Attitude.AddQuaternion(curTime,...
    str2double(q1), str2double(q2), str2double(q3), str2double(q4));
   
%% Sun Sensor Data
 
% AzEl data is desired for each sun sensor. Regardless of placement or orientation,
% the +X axis of each sensor's body axis in STK was chosen as zero for
% azimuth. Azimuth increases in the direction toward the +Y axis all
% the way up to 360 (+X again). Elevation increases toward the +Z axis.
 
    sunsensor1azel = sunsensor1.DataProviders.Item('Vectors(Body)').Group.Item('Sun');
    Elems = {'Time';'x/Magnitude';'y/Magnitude';'z/Magnitude'};
    Results =  sunsensor1azel.ExecElements(curTime,curTime,1,Elems);
    sunx1 = cell2mat(Results.DataSets.GetDataSetByName('x/Magnitude').GetValues);
    suny1 = cell2mat(Results.DataSets.GetDataSetByName('y/Magnitude').GetValues);
    sunz1 = cell2mat(Results.DataSets.GetDataSetByName('z/Magnitude').GetValues);
        
    sunsensor2azel = sunsensor2.DataProviders.Item('Vectors(Body)').Group.Item('Sun');
    Elems = {'Time';'x/Magnitude';'y/Magnitude';'z/Magnitude'};
    Results =  sunsensor2azel.ExecElements(curTime,curTime,1,Elems);
    sunx2 = cell2mat(Results.DataSets.GetDataSetByName('x/Magnitude').GetValues);
    suny2 = cell2mat(Results.DataSets.GetDataSetByName('y/Magnitude').GetValues);
    sunz2 = cell2mat(Results.DataSets.GetDataSetByName('z/Magnitude').GetValues);
 
    sunsensor3azel = sunsensor3.DataProviders.Item('Vectors(Body)').Group.Item('Sun');
    Elems = {'Time';'x/Magnitude';'y/Magnitude';'z/Magnitude'};
    Results =  sunsensor3azel.ExecElements(curTime,curTime,1,Elems);
    sunx3 = cell2mat(Results.DataSets.GetDataSetByName('x/Magnitude').GetValues);
    suny3 = cell2mat(Results.DataSets.GetDataSetByName('y/Magnitude').GetValues);
    sunz3 = cell2mat(Results.DataSets.GetDataSetByName('z/Magnitude').GetValues);
        
    sunsensor4azel = sunsensor4.DataProviders.Item('Vectors(Body)').Group.Item('Sun');
    Elems = {'Time';'x/Magnitude';'y/Magnitude';'z/Magnitude'};
    Results =  sunsensor4azel.ExecElements(curTime,curTime,1,Elems);
    sunx4 = cell2mat(Results.DataSets.GetDataSetByName('x/Magnitude').GetValues);
    suny4 = cell2mat(Results.DataSets.GetDataSetByName('y/Magnitude').GetValues);
    sunz4 = cell2mat(Results.DataSets.GetDataSetByName('z/Magnitude').GetValues);
        
    sunsensor5azel = sunsensor5.DataProviders.Item('Vectors(Body)').Group.Item('Sun');
    Elems = {'Time';'x/Magnitude';'y/Magnitude';'z/Magnitude'};
    Results =  sunsensor5azel.ExecElements(curTime,curTime,1,Elems);
    sunx5 = cell2mat(Results.DataSets.GetDataSetByName('x/Magnitude').GetValues);
    suny5 = cell2mat(Results.DataSets.GetDataSetByName('y/Magnitude').GetValues);
    sunz5 = cell2mat(Results.DataSets.GetDataSetByName('z/Magnitude').GetValues);
     
% Convert sunsensor unit vectors into azimuth, elevation, and range
        
    [a1, e1, r1] = cart2sph(sunx1,suny1,sunz1);
    [a2, e2, r2] = cart2sph(sunx2,suny2,sunz2);
    [a3, e3, r3] = cart2sph(sunx3,suny3,sunz3);
    [a4, e4, r4] = cart2sph(sunx4,suny4,sunz4);
    [a5, e5, r5] = cart2sph(sunx5,suny5,sunz5);
        
    AzEl1 = [a1, e1, r1];
    AzEl2 = [a2, e2, r2];
    AzEl3 = [a3, e3, r3];
    AzEl4 = [a4, e4, r4];
    AzEl5 = [a5, e5, r5];
     
% We have Az and El in radians for every sensor every second unformatted.
% We combine them into a single matrix to loop with later.
     
    AzElall = [a1 e1;a2 e2;a3 e3;a4 e4;a5 e5];
        
% Each matrix element is called out as its own variable for string operations.
        
    Az1 = AzElall(1,1);
    El1 = AzElall(1,2);
    Az2 = AzElall(2,1);
    El2 = AzElall(2,2);
    Az3 = AzElall(3,1);
    El3 = AzElall(3,2);
    Az4 = AzElall(4,1);
    El4 = AzElall(4,2);
    Az5 = AzElall(5,1);
    El5 = AzElall(5,2);
        
%% Constrain for solar intensity (is it in the Earth's shadow?), and cone width of sensor (what it can actually see)
 
% The sun sensors need sunlight to see anything! Thus, in the Earth's
% shadow, or total darkness (umbra), solar intensity is zero. Solar
% intensity is on a scale from 0.00 to 1.00. At the transition from
% Umbra to Penumbra, solar intensity rises from 0.00 as the shadow %
% lowers from 100%. The sum of both decimals is equal to 1. The
% criterion for solar sensor measurability is if the satellite is
% experiencing a solar intensity of anything above 0.00.
 
% The sun sensors have a cone width as displayed in 3D graphics of
% 45. As boresight (bisection of the cone) is the baseline for basing
% the angle to the sun vector for each sensor, any angles that deviate
% more than 45 degrees from the maximum elevation of 90 degrees will
% be treated as unreadable. It is undesirable to actually tell the system that the angle 
% is zero, since an angle of 50 would then lead the controller to try
% and correct for the 50 degree angle thinking it was zero.
    
% Since the scenario operates second-by-second, the
% system will trigger ignorance of sun angle if the offset from the
% boresight is greater than 45 degrees.  

    sunsensor1cone = sunsensor1.DataProviders.Item('Angles').Group.Item('Sun');
    Elems = {'Time';'Angle'};
    Results =  sunsensor1cone.ExecElements(curTime,curTime,1,Elems);
    angle1  = cell2mat(Results.DataSets.GetDataSetByName('Angle').GetValues);
        
    sunsensor2cone = sunsensor2.DataProviders.Item('Angles').Group.Item('Sun');
    Elems = {'Time';'Angle'};
    Results =  sunsensor2cone.ExecElements(curTime,curTime,1,Elems);
    angle2  = cell2mat(Results.DataSets.GetDataSetByName('Angle').GetValues);
        
    sunsensor3cone = sunsensor3.DataProviders.Item('Angles').Group.Item('Sun');
    Elems = {'Time';'Angle'};
    Results =  sunsensor3cone.ExecElements(curTime,curTime,1,Elems);
    angle3  = cell2mat(Results.DataSets.GetDataSetByName('Angle').GetValues);
        
    sunsensor4cone = sunsensor4.DataProviders.Item('Angles').Group.Item('Sun');
    Elems = {'Time';'Angle'};
    Results =  sunsensor4cone.ExecElements(curTime,curTime,1,Elems);
    angle4  = cell2mat(Results.DataSets.GetDataSetByName('Angle').GetValues);
        
    sunsensor5cone = sunsensor5.DataProviders.Item('Angles').Group.Item('Sun');
    Elems = {'Time';'Angle'};
    Results =  sunsensor5cone.ExecElements(curTime,curTime,1,Elems);
    angle5  = cell2mat(Results.DataSets.GetDataSetByName('Angle').GetValues);
        
    solarintensitystate = Kausat5.DataProviders.Item('Solar Intensity');
    Elems = {'Time';'Intensity'};
    Results = solarintensitystate.ExecElements(curTime,curTime,1,Elems);
        
    solint = cell2mat(Results.DataSets.GetDataSetByName('Intensity').GetValues);    
    sundata = [angle1;angle2;angle3;angle4;angle5];

%% Format the AzEl numbers into consistent strings
    
% STK-MATLAB spits out Azimuth values as values from  -Pi to Pi radians and deviance
% angle from boresight-vector to sun-vector as values from 0 to Pi/4
% radians. Yet, the real sensor's azimuth and elevation values will be
% 0 to 2Pi radians and Pi/4 to Pi/2 radians, respectively. For azimuth,
% always add Pi to all non-nulls. For elevation, if the sun IS seen,
% the actual elevation reading should be (Pi/2)-deviation.
    
    Az1abs = Az1+pi;
    Az2abs = Az2+pi;
    Az3abs = Az3+pi;
    Az4abs = Az4+pi;
    Az5abs = Az5+pi;
    El1abs = El1;
    El2abs = El2;
    El3abs = El3;
    El4abs = El4;
    El5abs = El5;
    
%   if Az1 < 0
%           Az1abs = -1.*Az1;
%       elseif Az1 >=0
%           Az1abs = Az1;
%       end

%   if El1 < 0
%           El1abs = -1.*El1;
%       elseif El1 >=0
%           El1abs = El1;
%       nd
%         
%   if Az2 < 0
%           Az2abs = -1.*Az2;
%       elseif Az2 >=0
%           Az2abs = Az2;
%       end 

%   if El2 < 0
%           El2abs = -1.*El2;
%       elseif El2 >=0
%           El2abs = El2;
%       end
%
%   if Az3 < 0
%           Az3abs = -1.*Az3;
%       elseif Az3 >=0
%           Az3abs = Az3;
%       end   

%   if El3 < 0
%           El3abs = -1.*El3;
%       elseif El3 >=0
%           El3abs = El3;
%       end
%         
%   if Az4 < 0
%           Az4abs = -1.*Az4;
%       elseif Az4 >=0
%           Az4abs = Az4;
%       end    

%   if El4 < 0
%           El4abs = -1.*El4;
%       elseif El4 >=0
%           El4abs = El4;
%       end
%
%   if Az5 < 0
%           Az5abs = -1.*Az5;
%       lseif Az5 >=0
%           Az5abs = Az5;
%       end 

%   if El5 < 0
%           El5abs = -1.*El5;
%       elseif El5 >=0
%           El5abs = El5;
%       end
   
% Always maintain 4 decimals and 1 integer.
        
    formatSpec1 = '%06.4f';
    az1f = num2str(Az1abs,formatSpec1);
    el1f = num2str(El1abs,formatSpec1);
    az2f = num2str(Az2abs,formatSpec1);
    el2f = num2str(El2abs,formatSpec1);
    az3f = num2str(Az3abs,formatSpec1);
    el3f = num2str(El3abs,formatSpec1);
    az4f = num2str(Az4abs,formatSpec1);
    el4f = num2str(El4abs,formatSpec1);
    az5f = num2str(Az5abs,formatSpec1);
    el5f = num2str(El5abs,formatSpec1);

% Run through checks for each angle. If solar intensity is ever zero,
% null. If angle is ever greater than 45, null. Solar intensity must
% be greater than zero and deviance angle must be less than or equal to 45 in order to not
% be null. A null set is going to be denoted with a ? sign in the tenths 
% decimal place of each value. For determination of eclipse status,
% signs of all AzEl in packets are made - if eclipse and + if in sun.
% This is possible because null values have the '?' and seen values are
% always positive.

        if (solint == 0) && (angle1 > 45)
            az11 = sprintf('%s','0.?000');
            el11 = sprintf('%s','0.?000');
        elseif (solint > 0) && (angle1 > 45)
            az11 = sprintf('%s','0.?000');
            el11 = sprintf('%s','0.?000');
        elseif (solint == 0) && (angle1 <= 45)
            az11 = sprintf('%s','0.?000');
            el11 = sprintf('%s','0.?000');
        elseif (solint > 0) && (angle1 <= 45)
            az11actual = az1f;
            el11actual = el1f;
        end
       
        if (solint == 0) && (angle2 > 45)
            az22 = sprintf('%s','0.?000');
            el22 = sprintf('%s','0.?000');
        elseif (solint > 0) && (angle2 > 45)
            az22 = sprintf('%s','0.?000');
            el22 = sprintf('%s','0.?000');
        elseif (solint == 0) && (angle2 <= 45)
            az22 = sprintf('%s','0.?000');
            el22 = sprintf('%s','0.?000');
        elseif (solint > 0) && (angle2 <= 45)
            az22actual = az2f;
            el22actual = el2f;
        end
       
        if (solint == 0) && (angle3 > 45)
            az33 = sprintf('%s','0.?000');
            el33 = sprintf('%s','0.?000');
        elseif (solint > 0) && (angle3 > 45)
            az33 = sprintf('%s','0.?000');
            el33 = sprintf('%s','0.?000');
        elseif (solint == 0) && (angle3 <= 45)
            az33 = sprintf('%s','0.?000');
            el33 = sprintf('%s','0.?000');
        elseif (solint > 0) && (angle3 <= 45)
            az33actual = az3f;
            el33actual = el3f;
        end
    
 
        if (solint == 0) && (angle4 > 45)
            az44 = sprintf('%s','0.?000');
            el44 = sprintf('%s','0.?000');
        elseif (solint > 0) && (angle4 > 45)
            az44 = sprintf('%s','0.?000');
            el44 = sprintf('%s','0.?000');
        elseif (solint == 0) && (angle4 <= 45)
            az44 = sprintf('%s','0.?000');
            el44 = sprintf('%s','0.?000');
        elseif (solint > 0) && (angle4 <= 45)
            az44actual = az4f;
            el44actual = el4f;
        end
        
        if (solint == 0) && (angle5 > 45)
            az55 = sprintf('%s','0.?000');
            el55 = sprintf('%s','0.?000');
        elseif (solint > 0) && (angle5 > 45)
            az55 = sprintf('%s','0.?000');
            el55 = sprintf('%s','0.?000');
        elseif (solint == 0) && (angle5 <= 45)
            az55 = sprintf('%s','0.?000');
            el55 = sprintf('%s','0.?000');
        elseif (solint > 0) && (angle5 <= 45)
            az55actual = az5f;
            el55actual = el5f;
        end

        
        
%% Setup format for sun sensor data.
 
% Need to display + sign if it is not present. Alternately, give the - 
% sign since they were made absolute earlier. Also add the tags. If the
% data snippet was nullified, callout az## and el##. If it was not
% nullified, callout az##actual and el##actual. 
    
            % SEEN?            SEEN?            SIGN?
        if (solint == 0) && (angle1 > 45) && (Az1 >= 0)      %null
            az111 = sprintf('%s',',-',az11);                 %null
        elseif (solint == 0) && (angle1 > 45) && (Az1 < 0)   %null
            az111 = sprintf('%s',',-',az11);                 %null
        elseif (solint == 0) && (angle1 <= 45) && (Az1 > 0)  %null
            az111 = sprintf('%s',',-',az11);                 %null
        elseif (solint == 0) && (angle1 <= 45) && (Az1 < 0)  %null
            az111 = sprintf('%s',',-',az11);                 %null
        elseif (solint > 0) && (angle1 > 45) && (Az1 >= 0)   %null
            az111 = sprintf('%s',',+',az11);                 %null
        elseif (solint > 0) && (angle1 > 45) && (Az1 < 0)    %null
            az111 = sprintf('%s',',+',az11);                 %null
        elseif (solint > 0) && (angle1 <= 45) && (Az1 >= 0)  %value
            az111 = sprintf('%s',',+',az11actual);           %value
        elseif (solint > 0) && (angle1 <= 45) && (Az1 < 0)   %value
            az111 = sprintf('%s',',+',az11actual);           %value
        end
        
           % SEEN?            SEEN?            SIGN?
        if (solint == 0) && (angle1 > 45) && (El1 >= 0)      %null
            el111 = sprintf('%s',',-',el11);                 %null
        elseif (solint == 0) && (angle1 > 45) && (El1 < 0)   %null
            el111 = sprintf('%s',',-',el11);                 %null
        elseif (solint == 0) && (angle1 <= 45) && (El1 > 0)  %null
            el111 = sprintf('%s',',-',el11);                 %null
        elseif (solint == 0) && (angle1 <= 45) && (El1 < 0)  %null
            el111 = sprintf('%s',',-',el11);                 %null
        elseif (solint > 0) && (angle1 > 45) && (El1 >= 0)   %null
            el111 = sprintf('%s',',+',el11);                 %null
        elseif (solint > 0) && (angle1 > 45) && (El1 < 0)    %null
            el111 = sprintf('%s',',+',el11);                 %null
        elseif (solint > 0) && (angle1 <= 45) && (El1 >= 0)  %value
            el111 = sprintf('%s',',+',el11actual);           %value
        elseif (solint > 0) && (angle1 <= 45) && (El1 < 0)   %value
            el111 = sprintf('%s',',+',el11actual);           %value
        end
         
 
          % SEEN?            SEEN?            SIGN?
        if (solint == 0) && (angle2 > 45) && (Az2 >= 0)      %null
            az222 = sprintf('%s',',-',az22);                 %null
        elseif (solint == 0) && (angle2 > 45) && (Az2 < 0)   %null
            az222 = sprintf('%s',',-',az22);                 %null
        elseif (solint == 0) && (angle2 <= 45) && (Az2 > 0)  %null
            az222 = sprintf('%s',',-',az22);                 %null
        elseif (solint == 0) && (angle2 <= 45) && (Az2 < 0)  %null
            az222 = sprintf('%s',',-',az22);                 %null
        elseif (solint > 0) && (angle2 > 45) && (Az2 >= 0)   %null
            az222 = sprintf('%s',',+',az22);                 %null
        elseif (solint > 0) && (angle2 > 45) && (Az2 < 0)    %null
            az222 = sprintf('%s',',+',az22);                 %null
        elseif (solint > 0) && (angle2 <= 45) && (Az2 >= 0)  %value
            az222 = sprintf('%s',',+',az22actual);           %value
        elseif (solint > 0) && (angle2 <= 45) && (Az2 < 0)   %value
            az222 = sprintf('%s',',+',az22actual);           %value
        end
        
           % SEEN?            SEEN?            SIGN?
        if (solint == 0) && (angle2 > 45) && (El2 >= 0)      %null
            el222 = sprintf('%s',',-',el22);                 %null
        elseif (solint == 0) && (angle2 > 45) && (El2 < 0)   %null
            el222 = sprintf('%s',',-',el22);                 %null
        elseif (solint == 0) && (angle2 <= 45) && (El2 > 0)  %null
            el222 = sprintf('%s',',-',el22);                 %null
        elseif (solint == 0) && (angle2 <= 45) && (El2 < 0)  %null
            el222 = sprintf('%s',',-',el22);                 %null
        elseif (solint > 0) && (angle2 > 45) && (El2 >= 0)   %null
            el222 = sprintf('%s',',+',el22);                 %null
        elseif (solint > 0) && (angle2 > 45) && (El2 < 0)    %null
            el222 = sprintf('%s',',+',el22);                 %null
        elseif (solint > 0) && (angle2 <= 45) && (El2 >= 0)  %value
            el222 = sprintf('%s',',+',el22actual);           %value
        elseif (solint > 0) && (angle2 <= 45) && (El2 < 0)   %value
            el222 = sprintf('%s',',+',el22actual);           %value
        end
 
 
 
            % SEEN?            SEEN?            SIGN?
        if (solint == 0) && (angle3 > 45) && (Az3 >= 0)      %null
            az333 = sprintf('%s',',-',az33);                 %null
        elseif (solint == 0) && (angle3 > 45) && (Az3 < 0)   %null
            az333 = sprintf('%s',',-',az33);                 %null
        elseif (solint == 0) && (angle3 <= 45) && (Az3 > 0)  %null
            az333 = sprintf('%s',',-',az33);                 %null
        elseif (solint == 0) && (angle3 <= 45) && (Az3 < 0)  %null
            az333 = sprintf('%s',',-',az33);                 %null
        elseif (solint > 0) && (angle3 > 45) && (Az3 >= 0)   %null
            az333 = sprintf('%s',',+',az33);                 %null
        elseif (solint > 0) && (angle3 > 45) && (Az3 < 0)    %null
            az333 = sprintf('%s',',+',az33);                 %null
        elseif (solint > 0) && (angle3 <= 45) && (Az3 >= 0)  %value
            az333 = sprintf('%s',',+',az33actual);           %value
        elseif (solint > 0) && (angle3 <= 45) && (Az3 < 0)   %value
            az333 = sprintf('%s',',+',az33actual);           %value
        end
        
           % SEEN?            SEEN?            SIGN?
        if (solint == 0) && (angle3 > 45) && (El3 >= 0)      %null
            el333 = sprintf('%s',',-',el33);                 %null
        elseif (solint == 0) && (angle3 > 45) && (El3 < 0)   %null
            el333 = sprintf('%s',',-',el33);                 %null
        elseif (solint == 0) && (angle3 <= 45) && (El3 > 0)  %null
            el333 = sprintf('%s',',-',el33);                 %null
        elseif (solint == 0) && (angle3 <= 45) && (El3 < 0)  %null
            el333 = sprintf('%s',',-',el33);                 %null
        elseif (solint > 0) && (angle3 > 45) && (El3 >= 0)   %null
            el333 = sprintf('%s',',+',el33);                 %null
        elseif (solint > 0) && (angle3 > 45) && (El3 < 0)    %null
            el333 = sprintf('%s',',+',el33);                 %null
        elseif (solint > 0) && (angle3 <= 45) && (El3 >= 0)  %value
            el333 = sprintf('%s',',+',el33actual);           %value
        elseif (solint > 0) && (angle3 <= 45) && (El3 < 0)   %value
            el333 = sprintf('%s',',+',el33actual);           %value
        end
 
 
 
             % SEEN?            SEEN?            SIGN?
        if (solint == 0) && (angle4 > 45) && (Az4 >= 0)      %null
            az444 = sprintf('%s',',-',az44);                 %null
        elseif (solint == 0) && (angle4 > 45) && (Az4 < 0)   %null
            az444 = sprintf('%s',',-',az44);                 %null
        elseif (solint == 0) && (angle4 <= 45) && (Az4 > 0)  %null
            az444 = sprintf('%s',',-',az44);                 %null
        elseif (solint == 0) && (angle4 <= 45) && (Az4 < 0)  %null
            az444 = sprintf('%s',',-',az44);                 %null
        elseif (solint > 0) && (angle4 > 45) && (Az4 >= 0)   %null
            az444 = sprintf('%s',',+',az44);                 %null
        elseif (solint > 0) && (angle4 > 45) && (Az4 < 0)    %null
            az444 = sprintf('%s',',+',az44);                 %null
        elseif (solint > 0) && (angle4 <= 45) && (Az4 >= 0)  %value
            az444 = sprintf('%s',',+',az44actual);           %value
        elseif (solint > 0) && (angle4 <= 45) && (Az4 < 0)   %value
            az444 = sprintf('%s',',+',az44actual);           %value
        end
        
           % SEEN?            SEEN?            SIGN?
        if (solint == 0) && (angle4 > 45) && (El4 >= 0)      %null
            el444 = sprintf('%s',',-',el44);                 %null
        elseif (solint == 0) && (angle4 > 45) && (El4 < 0)   %null
            el444 = sprintf('%s',',-',el44);                 %null
        elseif (solint == 0) && (angle4 <= 45) && (El4 > 0)  %null
            el444 = sprintf('%s',',-',el44);                 %null
        elseif (solint == 0) && (angle4 <= 45) && (El4 < 0)  %null
            el444 = sprintf('%s',',-',el44);                 %null
        elseif (solint > 0) && (angle4 > 45) && (El4 >= 0)   %null
            el444 = sprintf('%s',',+',el44);                 %null
        elseif (solint > 0) && (angle4 > 45) && (El4 < 0)    %null
            el444 = sprintf('%s',',+',el44);                 %null
        elseif (solint > 0) && (angle4 <= 45) && (El4 >= 0)  %value
            el444 = sprintf('%s',',+',el44actual);           %value
        elseif (solint > 0) && (angle4 <= 45) && (El4 < 0)   %value
            el444 = sprintf('%s',',+',el44actual);           %value
        end
 
 
            % SEEN?            SEEN?            SIGN?
        if (solint == 0) && (angle5 > 45) && (Az5 >= 0)      %null
            az555 = sprintf('%s',',-',az55);                 %null
        elseif (solint == 0) && (angle5 > 45) && (Az5 < 0)   %null
            az555 = sprintf('%s',',-',az55);                 %null
        elseif (solint == 0) && (angle5 <= 45) && (Az5 > 0)  %null
            az555 = sprintf('%s',',-',az55);                 %null
        elseif (solint == 0) && (angle5 <= 45) && (Az5 < 0)  %null
            az555 = sprintf('%s',',-',az55);                 %null
        elseif (solint > 0) && (angle5 > 45) && (Az5 >= 0)   %null
            az555 = sprintf('%s',',+',az55);                 %null
        elseif (solint > 0) && (angle5 > 45) && (Az5 < 0)    %null
            az555 = sprintf('%s',',+',az55);                 %null
        elseif (solint > 0) && (angle5 <= 45) && (Az5 >= 0)  %value
            az555 = sprintf('%s',',+',az55actual);           %value
        elseif (solint > 0) && (angle5 <= 45) && (Az5 < 0)   %value
            az555 = sprintf('%s',',+',az55actual);           %value
        end
        
           % SEEN?            SEEN?            SIGN?
        if (solint == 0) && (angle5 > 45) && (El5 >= 0)      %null
            el555 = sprintf('%s',',-',el55);                 %null
        elseif (solint == 0) && (angle5 > 45) && (El5 < 0)   %null
            el555 = sprintf('%s',',-',el55);                 %null
        elseif (solint == 0) && (angle5 <= 45) && (El5 > 0)  %null
            el555 = sprintf('%s',',-',el55);                 %null
        elseif (solint == 0) && (angle5 <= 45) && (El5 < 0)  %null
            el555 = sprintf('%s',',-',el55);                 %null
        elseif (solint > 0) && (angle5 > 45) && (El5 >= 0)   %null
            el555 = sprintf('%s',',+',el55);                 %null
        elseif (solint > 0) && (angle5 > 45) && (El5 < 0)    %null
            el555 = sprintf('%s',',+',el55);                 %null
        elseif (solint > 0) && (angle5 <= 45) && (El5 >= 0)  %value
            el555 = sprintf('%s',',+',el55actual);           %value
        elseif (solint > 0) && (angle5 <= 45) && (El5 < 0)   %value
            el555 = sprintf('%s',',+',el55actual);           %value
        end




      
%% Magnetic Sensor Data
 
        kaumagx = Kausat5.DataProviders.Item('Vectors(Body)').Group.Item('MagField(WMM)');
        Elems = {'Time';'x'};
        Results = kaumagx.ExecElements(curTime,curTime,1,Elems);
        magmagx = cell2mat(Results.DataSets.GetDataSetByName('x').GetValues);
        
        kaumagy = Kausat5.DataProviders.Item('Vectors(Body)').Group.Item('MagField(WMM)');
        Elems = {'Time';'y'};
        Results = kaumagy.ExecElements(curTime,curTime,1,Elems);
        magmagy = cell2mat(Results.DataSets.GetDataSetByName('y').GetValues);
        
        kaumagz = Kausat5.DataProviders.Item('Vectors(Body)').Group.Item('MagField(WMM)');
        Elems = {'Time';'z'};
        Results = kaumagz.ExecElements(curTime,curTime,1,Elems);
        magmagz = cell2mat(Results.DataSets.GetDataSetByName('z').GetValues);
    
%% Setup format for magnetic sensor data.    
    
% Negative signs mess with formatSpec2, and so the values are made as
% positive just to temporarily get rid of them. '-' signs are
% added in the if-elseif loops.
    
    if magmagx < 0
        magmagxabs = -1.*magmagx;
    elseif magmagx >=0
        magmagxabs = magmagx;
    end    
    if magmagy < 0
        magmagyabs = -1.*magmagy;
    elseif magmagy >=0
        magmagyabs = magmagy;
    end
    if magmagz < 0
        magmagzabs = -1.*magmagz;
    elseif magmagz >=0
        magmagzabs = magmagz;
    end
    
% Format to always have 5 integers and 2 decimal places
    
        formatSpec2 = '%08.2f';
        magmagx1 = num2str(magmagxabs,formatSpec2);
        magmagy1 = num2str(magmagyabs,formatSpec2);
        magmagz1 = num2str(magmagzabs,formatSpec2);
        
% Format to always have a sign displayed
    
        if magmagx >= 0
            magmagx11 = sprintf('%s',',+',magmagx1);
        elseif magmagx < 0
            magmagx11 = sprintf('%s',',-',magmagx1);
        end
        
        if magmagy >= 0
            magmagy11 = sprintf('%s',',+',magmagy1);
        elseif magmagy < 0
            magmagy11 = sprintf('%s',',-',magmagy1);
        end
        
        if magmagz >= 0
            magmagz11 = sprintf('%s',',+',magmagz1);
        elseif magmagz < 0
            magmagz11 = sprintf('%s',',-',magmagz1);
        end
        
        
%% Gyro data
    
        kaugyrox = Kausat5.DataProviders.Item('Body Axes Orientation').Group.Item('VVLH(Earth)');
        Elems = {'Time';'wx'};
        Results = kaugyrox.ExecElements(curTime,curTime,1,Elems);
        gyromagx = cell2mat(Results.DataSets.GetDataSetByName('wx').GetValues);
        
        kaugyroy = Kausat5.DataProviders.Item('Body Axes Orientation').Group.Item('VVLH(Earth)');
        Elems = {'Time';'wy'};
        Results = kaugyroy.ExecElements(curTime,curTime,1,Elems);
        gyromagy = cell2mat(Results.DataSets.GetDataSetByName('wy').GetValues);
        
        kaugyroz = Kausat5.DataProviders.Item('Body Axes Orientation').Group.Item('VVLH(Earth)');
        Elems = {'Time';'wz'};
        Results = kaugyroz.ExecElements(curTime,curTime,1,Elems);
        gyromagz = cell2mat(Results.DataSets.GetDataSetByName('wz').GetValues);
        
%% Setup format for magnetic sensor data.    
    
% Negative signs mess with formatSpec2, and so the values are made as
% positive just to temporarily get rid of them. '-' signs are
% added in the if-elseif loops.        
        
        if gyromagx < 0
            gyromagxabs = -1.*gyromagx;
        elseif gyromagx >=0
            gyromagxabs = gyromagx;
        end  
        if gyromagy < 0
            gyromagyabs = -1.*gyromagy;
        elseif gyromagy >=0
            gyromagyabs = gyromagy;
        end    
        if gyromagz < 0
            gyromagzabs = -1.*gyromagz;
        elseif gyromagz >=0
            gyromagzabs = gyromagz;
        end
        
% Format to always have 3 integers and 4 decimal places
    
        formatSpec2 = '%08.4f';
        gyromagxabs1 = num2str(gyromagxabs,formatSpec2);
        gyromagyabs1 = num2str(gyromagyabs,formatSpec2);
        gyromagzabs1 = num2str(gyromagzabs,formatSpec2); 
        
% Format to always have a sign displayed
    
        if gyromagx >= 0
            gyromagx11 = sprintf('%s',',+',gyromagxabs1);
        elseif gyromagx < 0
            gyromagx11 = sprintf('%s',',-',gyromagxabs1);
        end
        
        if gyromagy >= 0
            gyromagy11 = sprintf('%s',',+',gyromagyabs1);
        elseif gyromagy < 0
            gyromagy11 = sprintf('%s',',-',gyromagyabs1);
        end
        
        if gyromagz >= 0
            gyromagz11 = sprintf('%s',',+',gyromagzabs1);
        elseif gyromagz < 0
            gyromagz11 = sprintf('%s',',-',gyromagzabs1);
        end

        
%% Generate Packet
    
% Generate packet data to be send to COM port to microcontroller every 
% second as the following format:
    
% $,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#.####,_#####.##,_#####.##,_#####.##,_###.####,_###.####,_###.####@
        
% For checking data integrity, combined with the newarray/oldarray scheme
%    packet = sprintf('%s', '$',  num2str(sundata(1,1)),',', Az1abs,',', El1abs,',', az111, el111,...
%                                  ',',num2str(sundata(2,1)),',', Az2abs,',', El2abs,',', az222, el222,...
%                                  ',',num2str(sundata(3,1)),',', Az3abs,',', El3abs,',', az333, el333,...
%                                  ',',num2str(sundata(4,1)),',', Az4abs,',', El4abs,',', az444, el444,...
%                                  ',',num2str(sundata(5,1)),',', Az5abs,',', El5abs,',', az555, el555,...
%                                  magmagx11, magmagy11, magmagz11,...
%                                  gyromagx11, gyromagy11, gyromagz11,...
%                                      '@')  

        packet = sprintf('%s', '$', az111, el111,...
                                    az222, el222,...
                                    az333, el333,...
                                    az444, el444,...
                                    az555, el555,...
                                    magmagx11, magmagy11, magmagz11,...
                                    gyromagx11, gyromagy11, gyromagz11,...
                                    '@') 

      
        i = i + 1;
          

%% COM port Serial Packet Transfer
 
% Here we tell MATLAB that the COM port exits, to open it, and to output the 
% packet via serial to COM port. 
     
     fprintf(s, '%s', packet);
   
% The iteration is finished, reiterate.

     it = it +1;

