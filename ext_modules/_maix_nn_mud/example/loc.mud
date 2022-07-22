[basic]
type = awnn
param = ./slim_awnn.param
bin = ./slim_awnn.bin

[inputs]
input0 = 224,224,3,127.5, 127.5, 127.5,0.0078125, 0.0078125, 0.0078125

[outputs]
output0 = 1,4,2058
output1 = 1,2,2058
output2 = 1,8,2058

[extra]
outputs_scale =
inputs_scale=