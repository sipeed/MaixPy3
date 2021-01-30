import py
import pytest

def test_import():
  from maix import display, video, rpycs, camera

def test_display():
  from maix import display
  display.clear((255, 0, 0))
  display.fill((20, 20, 220, 220), (0, 0, 0))
  display.fill((40, 40, 200, 200), (0, 0, 255))

def test_image():
  from PIL import Image, ImageDraw
  image = Image.new("RGB", (100, 60), "#FFFFFF")
  draw = ImageDraw.Draw(image)
  draw.text((10, 10), u'hello world', (0, 0, 0))
  image = image.rotate(90)
  # image.show()

  from maix import display
  display.show(image, (0, 120))

def test_camera():
  from maix import camera, display
  display.show(camera.capture())
