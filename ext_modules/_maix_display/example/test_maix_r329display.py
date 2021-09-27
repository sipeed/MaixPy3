import _maix_display
from PIL import Image
from _maix_display import Display
canvas = Image.new("RGB", (240, 240), "#ff1b50")
__fastview__ = Display(240, 240)
mk = canvas.tobytes()
__fastview__.draw(mk,240,240)