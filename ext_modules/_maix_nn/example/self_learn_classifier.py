from maix import nn
from PIL import Image, ImageDraw
from maix import camera, display
import time
from maix.nn.app.classifier import Classifier, load

class_num = 3
sample_num = 15
feature_len = 512
input_w = 224
input_h = 224

model = {
    "param": "/root/models/resnet18_1000_awnn.param",
    "bin": "/root/models/resnet18_1000_awnn.bin"
}

options = {
    "model_type":  "awnn",
    "inputs": {
        "input0": (input_w, input_h, 3)
    },
    "outputs": {
        "190": (1, 1, feature_len)
    },
    "mean": [127.5, 127.5, 127.5],
    "norm": [0.0176, 0.0176, 0.0176],
}
print("-- load model:", model)
m = nn.load(model, opt=options)
print("-- load ok")

camera.config(size=(224, 224))

def key_pressed():
    # TODO: is button pressed
    return 1

classifier = Classifier(m, class_num, sample_num, feature_len, input_w, input_h)

curr_class = 0
curr_sample = 0
while 1:
    img = camera.capture()
    if not img:
        time.sleep(0.02)
        continue
    if key_pressed():
        if curr_class < class_num:
            print("add class")
            classifier.add_class_img(img)
            curr_class += 1
        elif curr_sample < sample_num:
            print("add sample")
            classifier.add_sample_img(img)
            curr_sample += 1
            if curr_sample == sample_num:
                print("train ...")
                classifier.train()
                print("train end ...")
                print("test save")
                classifier.save("m.classifier")
                print("test load")
                classifier = load(m, "m.classifier")
                print("test load ok")
    idx, distance = classifier.predict(img)
    print("predict class: {}, distance: {}".format(idx, distance))

