from maix import nn
from PIL import Image, ImageDraw
from maix import camera, display

test_jpg = "/root/test_input/input.jpg"
model = {
    "param": "/root/models/resnet_awnn.param",
    "bin": "/root/models/resnet_awnn.bin"
}

camera.config(size=(224, 224))

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
    "norm": [0.00784313725490196, 0.00784313725490196, 0.00784313725490196],
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

from classes_label import labels
while 1:
    img = camera.capture()
    if not img:
        time.sleep(0.02)
        continue
    out = m.forward(img, quantize=True)
    out = nn.F.softmax(out)
    msg = "{:.2f}: {}".format(out.max(), labels[out.argmax()])
    print(msg)
    draw = ImageDraw.Draw(img)
    draw.text((0, 0), msg, fill=(255, 0, 0))
    display.show(img)

