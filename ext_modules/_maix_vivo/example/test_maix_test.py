
import time
from maix import display, camera

from PIL import ImageFont
font = ImageFont.truetype("/home/res/baars.ttf", 20, encoding="unic")

camera.config((224, 224))

print("input ai img", len(camera.read()) == 224 * 224 * 3) # camera.capture()
draw = display.get_draw()
draw.text((10, 10), u'time:%s' % time.time(), (255, 0, 0), font) # rgb
display.show()
# display.clear()

camera.config((240, 240))

print("input ui img", len(camera.read()) == 240 * 240 * 3) # camera.capture()
draw = display.get_draw()
draw.text((10, 10), u'time:%s' % time.time(), (255, 0, 0), font) # rgb
display.show()
# display.clear()

camera.config((224, 224))

print("input ai img", len(camera.read()) == 224 * 224 * 3) # camera.capture()
draw = display.get_draw()
draw.text((10, 10), u'time:%s' % time.time(), (255, 0, 0), font) # rgb
display.show()
# display.clear()
