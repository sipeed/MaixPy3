
#
# self learning classifier
# download model from https://maixhub.com/model/zoo/86 first
#

from maix import nn
from PIL import Image, ImageDraw
from maix import camera, display
import time
from maix.nn.app.classifier import Classifier, load
from evdev import InputDevice
from select import select

def get_keys(device):
    '''
        @return is_left_pressed, is_right_pressed
    '''
    left = False
    right = False
    r,w,x = select([device], [], [], 0)
    if r:
        for event in device.read():
            if event.value == 1 and event.code == 0x02:
                right = True
            elif event.value == 1 and event.code == 0x03:
                left = True
    return left, right


class_num = 3
sample_num = 15
feature_len = 512
input_w = 224
input_h = 224

model = "./resnet18_1000_awnn.mud"
print("-- load model:", model)
m = nn.load(model)
print("-- load ok")

# model = {
#     "param": "./resnet18_1000_awnn.param",
#     "bin": "./resnet18_1000_awnn.bin"
# }

# options = {
#     "model_type":  "awnn",
#     "inputs": {
#         "input0": (input_w, input_h, 3)
#     },
#     "outputs": {
#         "190": (1, 1, feature_len)
#     },
#     "mean": [127.5, 127.5, 127.5],
#     "norm": [0.0176, 0.0176, 0.0176],
# }
# print("-- load model:", model)
# m = nn.load(model, opt=options)
# print("-- load ok")


keys_device = InputDevice('/dev/input/event0')
def key_pressed():
    l, r = get_keys(keys_device)
    return l, r

key_l = False
key_r = False

classifier = Classifier(m, class_num, sample_num, feature_len, input_w, input_h)

curr_class = 0
curr_sample = 0
while 1:
    img = camera.capture().resize(size=(input_w, input_h))
    if not img:
        time.sleep(0.02)
        continue
    l, r = key_pressed()
    key_l = l | key_l
    key_r = r | key_r
    if key_l:
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
        key_l = False
    if curr_class < class_num:
        img.draw_string(0, 0, f"press to record class {curr_class}", color=(255, 0, 0))
    elif curr_sample < sample_num:
        img.draw_string(0, 0, f"press to record sample {curr_sample}", color=(255, 0, 0))
    else:
        idx, distance = classifier.predict(img)
        img.draw_string(0, 0, f"class {idx}, distance: {distance}", color=(255, 0, 0))
        print("predict class: {}, distance: {}".format(idx, distance))
    display.show(img)

