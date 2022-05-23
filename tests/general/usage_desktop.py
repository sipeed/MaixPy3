from maix import camera, display

display.config((416, 416))
# camera.config is remove.
camera.camera.config((640, 480), 0, 1, 0)

while True:
  t = camera.capture()
  print(t)
  print(type(t))
  display.show(t)
