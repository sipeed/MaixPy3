from maix import nn
from PIL import Image, ImageDraw
import numpy as np
import time

test_jpg = "/root/input.jpg"
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

print("-- read image")
img = Image.open(test_jpg).resize(input_size[:2])
img.show()

print("-- read image ok")
print("-- forward model with image as input")
out = m.forward(img, quantize=True, layout="hwc")
# print("-- read image ok")
# out = out.reshape(222, 222, 3)
print("-- out:", out.shape, out.dtype)
out = out.astype(np.float32).reshape(output_size)
out = (np.abs(out) * 255 / out.max()).astype(np.uint8)
img2 = Image.fromarray(out, mode="RGB")

img2.show()
