
from maix import display, camera, image
from PIL import ImageDraw
import time


def show_run_time(func):
    import timeit

    def time_get():
        elapsed_time = timeit.timeit(stmt=func, number=1)
        print(func, elapsed_time)
    return time_get


@show_run_time
def test_null():
    img = camera.read()
    display.show(img)


# <function test_null at 0x1744928> 0.00949729199999183
test_null()


@show_run_time
def test_maix():
    img = camera.capture()
    img.draw_string(10, 10, u'time:%s' % time.time(),
                    1,  color=(255, 0, 0), thickness=1)
    display.show(img)


# <function test_maix at 0x174afd8> 0.012572958999953698
test_maix()


@show_run_time
def test_pil():
    img = camera.camera.old_capture()
    draw = ImageDraw.Draw(img)
    draw.text((10, 10), u'time:%s' % time.time(), (255, 0, 0))
    display.show(img)


# <function test_pil at 0x1777c98> 0.03788966700005858
test_pil()
