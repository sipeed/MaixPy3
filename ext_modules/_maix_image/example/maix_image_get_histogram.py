
from maix import image

# img = image.new(size=(10, 10), color=(255, 255, 255), mode="RGB")
img = image.Image().open("ext_modules/_maix_image/example/test_image.jpg")
tmp = img.get_histogram(bins=8, roi=(20, 20, 80, 80))

print("l_bins", len(tmp[0]), tmp[0])
print("a_bins", len(tmp[1]), tmp[2])
print("b_bins", len(tmp[2]), tmp[2])

print("get_threshold", tmp.get_threshold())

print("get_statistics", tmp.get_statistics())

print("get_percentile", tmp.get_percentile())

tmp = img.get_statistics()

print(tmp)

print("l Mean Median Mode STDev Min Max LQ UQ", tmp[0:8])
print("a Mean Median Mode STDev Min Max LQ UQ", tmp[8:16])
print("b Mean Median Mode STDev Min Max LQ UQ", tmp[16:24])

# openmv
# "l_mean":42, "l_median":38, "l_mode":3, "l_stdev":27, "l_min":0, "l_max":100, "l_lq":19, "l_uq":65,
# "a_mean":3, "a_median":1, "a_mode":0, "a_stdev":22, "a_min":-59, "a_max":81, "a_lq":-7, "a_uq":18,
# "b_mean":5, "b_median":2, "b_mode":2, "b_stdev":27, "b_min":-88, "b_max":76, "b_lq":-12, "b_uq":25

# maixpy3
# 42, 37, 3, 27, 0, 98, 18, 66,
# 2, -1, -1, 22, -72, 83, -8, 15,
# 7, 1, -1, 26, -73, 76, -11, 25

print(tmp[4:6], tmp[12:14], tmp[20:22]) ## default while

# img.save("ts.jpg")

'''
l_bins 8 [0.08312499523162842, 0.09140624850988388, 0.1795312464237213, 0.23015624284744263, 0.10546875, 0.1184374988079071, 0.19187499582767487, 0.0]
a_bins 8 [0.0, 0.0, 0.08828125149011612, 0.24265624582767487, 0.559374988079071, 0.10968749970197678, 0.0, 0.0]
b_bins 8 [0.0, 0.0, 0.08828125149011612, 0.24265624582767487, 0.559374988079071, 0.10968749970197678, 0.0, 0.0]
get_threshold None
get_statistics None
get_percentile None
255 255 255
[42, 37, 3, 27, 0, 98, 18, 66, 2, -1, -1, 22, -72, 83, -8, 15, 7, 1, -1, 26, -73, 76, -11, 25]
l Mean Median Mode STDev Min Max LQ UQ [42, 37, 3, 27, 0, 98, 18, 66]
a Mean Median Mode STDev Min Max LQ UQ [2, -1, -1, 22, -72, 83, -8, 15]
b Mean Median Mode STDev Min Max LQ UQ [7, 1, -1, 26, -73, 76, -11, 25]
[0, 98] [-72, 83] [-73, 76]
'''
