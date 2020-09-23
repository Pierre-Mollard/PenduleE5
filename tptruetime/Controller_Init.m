function Controller_Init

% Control of Inverted Pendulum via TrueTime Real Time Kernel Model
%

% * Observer Controller task
% * name:    Obs_Cont_task
%   exec code:   Obs_Cont
%   type:        Periodic task
%   function:    Calculates the Pendulum states for each sampling period based on its linear model and measurement of pendulum angular position and cart horizontal position
%                each T_ech seconds
%
ttInitKernel('prioFP');                                                    % fixed priority

% Observer-Controller parameter 

Adc=[0.6300   -0.1206   -0.0008    0.0086
     -0.0953    0.6935    0.0107    0.0012
     -0.2896   -1.9184    1.1306    0.2351
    -3.9680   -1.7733   -0.1546    0.7222];

Bdc=[0.3658    0.1200
    0.0993    0.3070
    1.0887    2.0141
    3.1377    1.6599];

Cdc=[-80.3092   -9.6237  -14.1215  -23.6260];

Ddc=[0 0];

%Task attributes

Tech=0.01;
period = Tech;
deadline = period;                                                                                                                                                                                                                                                                                                                                              
offset = 0.0;                                                              % start of first task job
prio = 1;

% Create task data (local memory)

data.Adc=Adc;
data.Bdc=Bdc;
data.Cdc=-Cdc;
data.Ddc=Ddc;

data.x=[0 0 0 0]';                                                          % initial sate condition of pendulum
data.y=[0 0]';                                                              % initial positions/outputs of pendulum
data.u=0;                                                                   % initial control of the pendulum           

% Data relative to pendulum interfaced with Computer 

data.th = 0;
data.d=0;
data.thChanel=1;
data.dChanel=2;
data.uChanel=1;   

% IMPLEMENTATION : using the built-in support for periodic tasks

%ttCreatePeriodicTask('Obs_Cont_task', offset, period, 'Obs_Cont', data);
deadline = period;
ttCreateTask('Controller_task', deadline, 'Controller_code', data);
ttAttachNetworkHandler('Controller_task');

