
from maix import camera

print(camera.camera.cam.cfg(240, 240, 0))
img = camera.camera.cam.get()
print(len(img[0]))

print(camera.camera.cam.cfg(320, 240, 0))
img = camera.camera.cam.get()
print(len(img[0]))

# camera.config(size=(240, 240))

# camera.config(size=(320, 240))

from _maix_vivo import _v83x_vivo
cam = _v83x_vivo(320, 240)

print(cam.cfg(240, 240, 0))
frame = cam.get()
print(len(frame))
if len(frame) > 0:
  print(len(frame[0]))

print(cam.cfg(320, 240, 0))
frame = cam.get()
print(len(frame))
if len(frame) > 0:
  print(len(frame[0]))

