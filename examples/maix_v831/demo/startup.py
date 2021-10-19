#!/usr/bin/env python

from PIL import Image, ImageFont, ImageDraw
import asyncio
import time
import os
import socket

try:
  import shutil
  from PIL import ImageShow
  # use fbviewer on linux
  # os.system('ln -s /usr/sbin/fbviewer /usr/sbin/display')
  if shutil.which("fbviewer"):
    class fbViewer(ImageShow.UnixViewer):
      def get_command_ex(self, file, **options):
        command = executable = "fbviewer"
        return command, executable
    ImageShow.register(fbViewer, 0)
except ModuleNotFoundError as e:
  pass

def get_host_ip():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(('8.8.8.8', 80))
        ip = "IP: " + s.getsockname()[0]
    except Exception as e:
        ip = "IP does not exist"
    finally:
        s.close()
    return ip


font = ImageFont.truetype("./resc/baars.ttf", 27, encoding="unic")

canvas = Image.new("RGB", (240, 240), "#2c3e50")
with Image.open('./resc/logo.png') as logo:
    canvas.paste(logo, (50, 40, 50 + logo.size[0], 40 + logo.size[1]), logo)

draw = ImageDraw.Draw(canvas)
draw.text((25, 195), get_host_ip(), "#bdc3c7", font)
canvas.show()
