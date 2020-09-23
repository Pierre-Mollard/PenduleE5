function Sensor_Actuator_Init

% Control of Inverted Pendulum via TrueTime Real Time Kernel Model
%

% * Observer Controller task
% * name:    Obs_Cont_task
%   exec code:   Obs_Cont
%   type:        Periodic task
%   function:    Calculates the Pendulum states for each sampling period based on its linear model and measurement of pendulum angular position and cart horizontal position
%                each T_ech seconds
%
ttInitKernel('prioDM');                                                    % fixed priority

%Task attributes

Tech=0.01;
period = Tech;
deadline = period;                                                                                                                                                                                                                                                                                                                                              
starttime = 0.0;                                                              % start of first task job
prio = 1;

% Create task data (local memory)

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

ttCreatePeriodicTask('Sensor_task', starttime, period, 'Sensor_code', data);

deadline = 10.0;
ttCreateTask('Actuator_task', deadline, 'Actuator_code', data);
ttAttachNetworkHandler('Actuator_task');

