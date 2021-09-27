import _maix_display

from PIL import Image

from _maix_display import R329Display

canvas = Image.new("RGB", (240, 240), "#ff1b50")

__fastview__ = R329Display(240, 240)

mk = canvas.tobytes()

__fastview__.draw(mk,240,240)