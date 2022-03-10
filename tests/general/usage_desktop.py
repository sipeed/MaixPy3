from maix import camera, display

display.config((240, 240))
camera.config((320, 320), 2)

while True:
  display.show(camera.capture())
