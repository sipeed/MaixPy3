
from maix import image

img = image.Image().new(size=(10, 10), color=(255, 0, 0), mode="RGB")
tmp = img.get_histogram(bins=10, roi=(2, 2, 8, 8))

print("l_bins", len(tmp[0]), tmp[0])
print("a_bins", len(tmp[1]), tmp[2])
print("b_bins", len(tmp[2]), tmp[2])

print("get_threshold", tmp.get_threshold())

print("get_statistics", tmp.get_statistics())

print("get_percentile", tmp.get_percentile())
