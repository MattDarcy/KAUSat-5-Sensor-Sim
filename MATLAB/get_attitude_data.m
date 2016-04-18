%% Metadata  
	% get_attitude_data.m
	%  KAUSAT-5 Sensor Simulator
    %
    %  Copyright (c) 2016 Matt D'Arcy.
    %  Shared under the MIT License.
    %

function [epsec, q1, q2, q3, q4] = get_attitude_data(fid)

att_line = fgetl(fid);

spLine = regexp(att_line, '\s', 'split');

epsec = spLine{1};
q1 = spLine{2};
q2 = spLine{3};
q3 = spLine{4};
q4 = spLine{5};
