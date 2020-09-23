function [exectime, data] = Actuator_code(seg, data)

switch seg
    case 1
        data.u = ttGetMsg;
        exectime = 0.0005;
    case 2
        ttAnalogOut(1, data.u);
        exectime = -1;
end