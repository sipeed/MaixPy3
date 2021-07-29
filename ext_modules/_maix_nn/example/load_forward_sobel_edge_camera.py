from maix import nn
from PIL import Image, ImageDraw
import numpy as np
import time
from maix import  display, camera

camera.config(size=(224, 224))

model = {
    "param": "/root/models/sobel_int8.param",
    "bin": "/root/models/sobel_int8.bin"
}

input_size = (224, 224, 3)
output_size = (222, 222, 3)

options = {
    "model_type":  "awnn",
    "inputs": {
        "input0": input_size
    },
    "outputs": {
        "output0": output_size
    },
    "mean": [127.5, 127.5, 127.5],
    "norm": [0.0078125, 0.0078125, 0.0078125],
}
print("-- load model:", model)
m = nn.load(model, opt=options)
print("-- load ok")

while 1:
    img = camera.capture()
    if not img:
        time.sleep(0.01)
        continue
    print("-- read image ok")
    print("-- forward model with image as input")
    out = m.forward(img, quantize=True, layout="hwc")
    # print("-- read image ok")
    # out = out.reshape(222, 222, 3)
    print("-- out:", out.shape, out.dtype)
    out = out.astype(np.float32).reshape(output_size)
    out = (np.abs(out) * 255 / out.max()).astype(np.uint8)
    img2 = Image.fromarray(out, mode="RGB")

    display.show(img2)
