
from maix import image
img = image.Image().new(size=(240, 240), color=(255, 0, 0), mode="RGB")
tmp = img.get_histogram()
print(tmp[0], tmp[1], tmp[2])
