
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

camera.config((224, 224))


@show_run_time
def test_pillow():
    tmp = camera.capture()
    display.show(tmp.resize((240, 240)))


# <function test_pillow at 0x199fff8> 0.07056433300022036
test_pillow()


@show_run_time
def test_opencv():
    tmp = camera.read()
    display.show(image(tmp, 224, 224).resize(240, 240).tobytes())


# <function test_opencv at 0x104ea58> 0.02121679200001836
test_opencv()


def unit_test():
    import time
    from maix import display, camera, image
    camera.config((224, 224))
    while True:
        tmp = camera.read()
        img = image.load(tmp, 224, 224)
        img.resize(240, 240)
        img.draw_rectangle(20, 20, 220, 220, color=(0, 255, 0), thickness=4)
        img.draw_string(20, 120, str(time.time()), scale=1.2, color=(255, 0, 0), thickness=2)
        display.show(img.tobytes())

unit_test()

if __name__ == "__main__":

    unit_test()
