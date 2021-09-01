#!/usr/bin/env python

from PIL import Image

from maix import display

display.show(Image.new("RGB", (320, 240), (0, 0xFF, 0)))

from maix import camera

for i in range(30):
    display.show(camera.read())

print(display.__draw__)

display.show(Image.new("RGB", (22, 22), (0xFF, 0, 0)))

import time

time.sleep(2)

display.clear()

display.show(Image.new("RGB", (22, 22), (0, 0, 0xFF)))

time.sleep(2)

# bytes
