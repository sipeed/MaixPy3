from _maix_vivo import _v83x_vivo

from PIL import Image
vivo = _v83x_vivo(240, 240, 192, 128, vo_dir = 0, ai_dir = 0)
import time

from PIL import Image, ImageDraw, ImageFont
font = ImageFont.truetype("/home/res/baars.ttf", 18, encoding="unic")

bak = b'\x00\x00\x00\x00' * (240 * 240)

for i in range(600):
    time.sleep(0.02)
    # tmp = vivo.get_vi()
    # print(len(tmp))
    tmp = vivo.get_ai()
    print(len(tmp))
    image = Image.frombytes("RGBA", (240, 240), bak)
    draw = ImageDraw.Draw(image)
    draw.text((10, 10), u'time:%s' % time.time(), (255, 255, 255), font)
    # print(image)
    vivo.set_ui(image.tobytes())

# tmp = vivo.get_vi()
# print(len(tmp))
# vi = Image.frombytes("RGB", (240, 240), tmp)
# vi.save('vi.jpg', quality=95)
# tmp = vivo.get_ai()
# print(len(tmp))
# ai = Image.frombytes("RGB", (192, 128), tmp)
# ai.save('ai.jpg', quality=95)

# from PIL import Image
# vivo = _v83x_vivo(vi_dir = 0, ai_dir = 1)
# import time

# for i in range(10):
#     time.sleep(0.02)
#     tmp = vivo.get_vi()
#     print(len(tmp))
#     tmp = vivo.get_ai()
#     print(len(tmp))
#     # print(tmp.set_ui(""))

# tmp = vivo.get_vi()
# print(len(tmp))
# vi = Image.frombytes("RGB", (240, 240), tmp)
# vi.save('vi.jpg', quality=95)
# tmp = vivo.get_ai()
# print(len(tmp))
# ai = Image.frombytes("RGB", (128, 192), tmp)
# ai.save('ai.jpg', quality=95)

# while True:
#     time.sleep(0.02)
#     tmp = vivo.get_vi()
#     print(len(tmp))
#     tmp = vivo.get_ai()
#     print(len(tmp))
#     # print(tmp.set_ui(""))
