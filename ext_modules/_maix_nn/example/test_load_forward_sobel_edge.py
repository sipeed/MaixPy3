from maix import nn
from PIL import Image, ImageDraw
from maix import camera, display
import numpy as np
import time

model = {
    "param": "/models/sobel_int8.param",
    "bin": "/models/sobel_int8.bin"
}

input_size = (224, 224, 3)
output_size = (222, 222, 3)
camera.config(size=input_size[:2])

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

from PIL import Image

m = nn.load(model, opt=options)

while 1:
    # tmp = camera.read()
    # if len(tmp):
    #   img = Image.frombytes("RGB", (224, 224), tmp)
    #   img.save("img.jpg")
    img = camera.capture()
    if img:
      # img.save("img.jpg")
      out = m.forward(img, quantize=True)
      out = out.astype(np.float32).reshape(output_size)
      out = (np.abs(out) * 255 / out.max()).astype(np.uint8)
      img2 = Image.fromarray(out, mode="RGB")
      # img2.save("img2.jpg")
      display.show(img2)
