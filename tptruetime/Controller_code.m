function [exectime, data] = Controller_code(seg, data)

switch seg
    case 1
        %theta = ttAnalogIn(data.thChanel);
        %d = ttAnalogIn(data.dChanel);
        %y=[theta;d];
        data.y = ttGetMsg;
        data = obscont(data, data.y);
        exectime = 0.002;
    case 2
        ttSendMsg(1, data.u, length(data.u));
        exectime = -1;
end