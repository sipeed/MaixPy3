#!/usr/bin/env python

from maix import display, camera
while True:
    display.show(camera.capture())

