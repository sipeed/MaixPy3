
class Edge:
    path = {
        # "param": "/root/models/sobel_int8.param",
        # "bin": "/root/models/sobel_int8.bin"
        "bin": "/root/models/aipu_sobel.bin"
    }

    input_size = (224,224,3)
    output_size = (224,224,3)

    options = {
        "model_type":  "aipu",
        "inputs": {
            "input0": input_size
        },
        "outputs": {
            "output0": output_size
        },
        "mean": [127.5, 127.5, 127.5],
        "norm": [0.0078125, 0.0078125, 0.0078125],
        "scale":[0.15196067], # R329 有此选项，V831没有这个选项
    }

    def __init__(self) -> None:
        from maix import nn
        print(" - - load model :",self.path)
        self.model = nn.load(self.path , opt = self.options)
        print("- -load ok")

    def __del__(self):
        del self.model


#main
from maix import  display, camera
import numpy as np
global m
m =Edge()

#loop
while True:
    img = camera.capture().resize(size=(224, 224))
    out = m.model.forward(img, quantize=True, layout="hwc")
    out = out.astype(np.float32).reshape(m.output_size)
    out = (np.ndarray.__abs__(out) * 255 / out.max()).astype(np.uint8)
    data = out.tobytes()
    img2 = img.load(data,(222, 222), mode="RGB")
    display.show(img2)