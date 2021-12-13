


from maix import nn
from maix import display,camera
from classes_label import labels
import time

model = {
    # "param": "/root/models/resnet18_1000_awnn.param",
    # "bin": "/root/models/resnet18_1000_awnn.bin"
    "bin": "./resnet.bin"
}
options = {
    "model_type":  "normal",
    "inputs": {
        "input0": (224, 224, 3)
    },
    "outputs": {
        "output0": (1, 1, 1000)
    },
    "first_layer_conv_no_pad": False,
    "mean": [127.5, 127.5, 127.5],
    "norm": [0.0176, 0.0176, 0.0176],
}
print("-- load model:", model)
m = nn.load(model, opt=options)
print("-- load ok")
camera.config((224, 224))
while True:
    img = camera.capture()
    out = m.forward(img.tobytes(), quantize=True)
    out2 = nn.F.softmax(out)
    msg = "{:.2f}: {}".format(out2.max(), labels[out.argmax()])
    print(msg)
    img.draw_string(0,0,msg,scale = 0.5 , color = (183,127,221))
    display.show(img)