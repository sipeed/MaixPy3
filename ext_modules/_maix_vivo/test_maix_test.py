
import time
from maix import display, camera
from PIL import Image, ImageDraw, ImageFont
font = ImageFont.truetype("/home/res/baars.ttf", 20, encoding="unic")

while True:
  print("input ai img", len(camera.read()) == 224 * 224 * 3) # camera.capture()
  image = Image.new("RGBA", (240, 240), "#00000000")
  draw = ImageDraw.Draw(image)
  draw.text((10, 10), u'time:%s' % time.time(), (255, 0, 0), font) # bgr
  display.show(image)
