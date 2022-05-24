
from PIL import Image

from _maix_display import Display

__show__ = Display()

canvas = Image.new("RGB", (240, 240), "#ff1b50")
__show__ = Display()
mk = canvas.tobytes()
__show__.draw(mk, 240, 240)

canvas = Image.new("RGB", (240, 240), "#501bff")
mk = canvas.tobytes()
__show__.draw(mk, 240, 240)

'''
import _maix_camera
from _maix_display import Display
cam = _maix_camera.Camera(240,240,0)
__show__ = Display(240, 240)

while True:
    img_bytes = cam.read()
    if img_bytes[0]:
        __show__.draw(img_bytes[1],240,240)

'''