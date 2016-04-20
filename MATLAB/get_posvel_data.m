%  get_posvel_data.m
%  KAUSAT-5 Sensor Simulator
%
%  Copyright (c) 2016 Matt D'Arcy.
%  Shared under the MIT License.
%
%% Pos-Vel Feed

function [epsec, x_pos, y_pos, z_pos, x_vel, y_vel, z_vel] = get_posvel_data(fid)

data_line = fgetl(fid);

spc_cnt = regexp(data_line, '\s');

epsec = data_line(1:spc_cnt(1));
x_pos = data_line(spc_cnt(1):spc_cnt(2));
y_pos = data_line(spc_cnt(2):spc_cnt(3));
z_pos = data_line(spc_cnt(3):spc_cnt(4));
x_vel = data_line(spc_cnt(4):spc_cnt(5));
y_vel = data_line(spc_cnt(5):spc_cnt(6));
z_vel = data_line(spc_cnt(6):end);

