from _maix_vivo import _v83x_vivo
from _maix_opencv import _v83x_opencv

cv = _v83x_opencv()

# from PIL import Image
# image = Image.frombytes("RGB", (240, 240), cv.test(b""))
# image.show()

# bak = b'\x00\x00\x00\x00' * (240 * 240)

# tmp = cv.test(bak)

# print(len(tmp))

vivo = _v83x_vivo(240, 240, 240, 240, vo_dir=0, ai_dir=0)

# vivo.set_ui((cv.test(bak)))

for i in range(3600):
    # time.sleep(0.02)
    # tmp = vivo.get_vi()
    # print(len(tmp))
    tmp = vivo.get_ai()
    if len(tmp):
      # print(len(tmp))
      vivo.set_ui((cv.test(tmp)))
