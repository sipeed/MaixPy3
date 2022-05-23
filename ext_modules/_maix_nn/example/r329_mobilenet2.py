class MobileNetv2:
    path = {
        #R329
        "bin": "./models/aipu_mobilenet2.bin"

        # V831
        # "bin": "./models/awnn_mobilenet2.bin"
        # "param": "./models/awnn_mobilenet2.param"
    }
    input_size = (224,224,3)
    output_size = (1, 1, 1000)

    options = {
        # R329
        "model_type":  "aipu",
        #V831
        # "model_type":  "awnn"

        "inputs": {
            "input0": input_size
        },
        "outputs": {
            "output0": output_size
        },
        "mean": [127.5, 127.5, 127.5],
        "norm": [0.0078125, 0.0078125, 0.0078125],
        "scale":[0.15196067],   # R329 有此选项，V831没有这个选项
    }

    def __init__(self) -> None:
        from maix import nn
        print(" - - load model :",self.path)
        self.model = nn.load(self.path , opt = self.options)
        print("- -load ok")

    def __del__(self):
        del self.model

#main
from classes_label import labels
from maix import  display, camera
import numpy as np
from maix import nn
global m
m = MobileNetv2()
while True:
    img = camera.capture().resize(size=(224,224))
    out = m.model.forward(img, quantize=True, layout="hwc")
    out2 = nn.F.softmax(out)
    msg = "{:.2f}: {}".format(out2.max(), labels[out.argmax()])
    img.draw_string(0,0,msg,scale = 0.5 , color = (183,127,221))
    display.show(img)