from maix import image, display, camera
import time


def test_get_blob_color():
    img = image.Image().new(color=(100, 100, 200))
    colors = img.get_blob_color((100, 100, 10, 10), 0, 0)
    print(colors)


test_get_blob_color()
