from maix import camera, display

display.config((640, 480))
camera.config((320, 240), 0)

while True:
  t = camera.capture()
  print(t)
  display.show(t)
