function data = obscont(data,y)

data.x = data.Adc * data.x + data.Bdc * y;
data.u = data.Cdc * data.x;
end

