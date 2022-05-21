#!/usr/bin/env python

from maix import display, camera
while True:
    display.show(
      display.as_image()
      .draw_rectangle(0, 0, display.width(), display.height(), color=(0, 0, 0), thickness=-1)
      .draw_image(
        camera.capture()
        .resize(size=(224, 224)), alpha=0.9
      )
    )

