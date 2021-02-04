#!/usr/bin/env python

from PIL import Image, ImageFont, ImageDraw
from maix import display
import asyncio
import time
import os
import socket

os.system('replug_sensor.sh')


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


font = ImageFont.truetype("/home/res/baars.ttf", 27, encoding="unic")

canvas = Image.new("RGB", (240, 240), "#2c3e50")
with Image.open('/home/res/logo.png') as logo:
    canvas.paste(logo, (50, 40, 50 + logo.size[0], 40 + logo.size[1]), logo)

draw = ImageDraw.Draw(canvas)
draw.text((25, 195), get_host_ip(), "#bdc3c7", font)
display.show(canvas)
