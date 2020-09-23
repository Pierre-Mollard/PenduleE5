function [exectime, data] = Sensor_code(seg, data)

switch seg
    case 1
        theta = ttAnalogIn(data.thChanel);
        d = ttAnalogIn(data.dChanel);
        data.y=[theta;d];
        exectime = 0.002;
    case 2
        ttSendMsg(2, data.y, length(data.y));
        exectime = 0.001;
    case 3
        exectime = -1;
end