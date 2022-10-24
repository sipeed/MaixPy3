[basic]
type = awnn
param = ./model_int8.param
bin = ./model_int8.bin

[inputs]
input0 = 192,192,3,127.5, 127.5, 127.5,0.0078125, 0.0078125, 0.0078125

[outputs]
output0 = 48,48,17
output1 = 48,48,1
output2 = 48,48,34
output4 = 48,48,34


[extra]
outputs_scale =
inputs_scale=
