#!/usr/bin/env python

from maix import display, camera
import zbarlight
while True:
    image = camera.capture()
    codes = zbarlight.scan_codes(['qrcode'], image)
    print(codes)