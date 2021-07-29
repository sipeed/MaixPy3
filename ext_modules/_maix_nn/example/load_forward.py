from maix import nn
from PIL import Image, ImageDraw
from maix import display
from classes_label import labels
import time

test_jpg = "/root/input.jpg"
model = {
    "param": "/root/models/resnet18_1000_awnn.param",
    "bin": "/root/models/resnet18_1000_awnn.bin"
}

options = {
    "model_type":  "awnn",
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

print("-- read image")
img = Image.open(test_jpg)
print("-- read image ok")
print("-- forward model with image as input")
out = m.forward(img, quantize=True)
print("-- read image ok")
print("-- out:", out.shape)
out = nn.F.softmax(out)
print(out.max(), out.argmax())

while 1:
    t = time.time()
    out = m.forward(img, quantize=True)
    t = time.time() - t
    print("-- forward time: {}s".format(t))
    t = time.time()
    out2 = nn.F.softmax(out)
    t = time.time() - t
    print("-- softmax time: {}s".format(t))
    msg = "{:.2f}: {}".format(out2.max(), labels[out.argmax()])
    print(msg)
    draw = ImageDraw.Draw(img)
    draw.text((0, 0), msg, fill=(255, 0, 0))

