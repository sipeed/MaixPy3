from maix import camera, display

display.config((416, 416))
# camera.config is remove.
camera.camera.config((640, 480), 0, 1, 0)

while True:
  t = camera.capture()
  print(t)
  print(type(t))
  display.show(t)

# from maix import camera, display
# t = camera.capture()
# print(t)
# print(type(t))
# display.show(t)

# camera.camera.config((640, 480))
# t = camera.capture()
# print(t)
# print(type(t))
# display.show(t)

# from maix import camera, display
# t = camera.capture().draw_rectangle(30, 50, 130, 140, color=(255, 50, 50), thickness=-1)

# print(len(t.tobytes()))

# with open("t.bmp", "wb") as f:
#   f.write(t.tobytes("bmp"))

# with open("t.jpg", "wb") as f:
#   f.write(t.tobytes("jpg"))

# with open("t.png", "wb") as f:
#   f.write(t.tobytes("png", (16, 3)))
