
import time
from maix import display, camera
from _maix_display import Display
import _maix_image
from PIL import Image
from maix import camera
from maix.image import Image
from maix import display, camera, Image
from maix import display, camera, image


def show_run_time(func):
    import timeit

    def time_get():
        elapsed_time = timeit.timeit(stmt=func, number=1)
        print(func, elapsed_time)
    return time_get


@show_run_time
def test_quick():
    tmp = camera.read()
    display.show(tmp)


# <function test_quick at 0x199ebd8> 0.011696916999881068
test_quick()

@show_run_time
def test_pillow():
    tmp = camera.capture()
    display.show(tmp.resize((240, 240)))


# <function test_pillow at 0x199fff8> 0.07056433300022036
test_pillow()

display.show(camera.capture())

