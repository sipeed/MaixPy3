
def show_run_time(func):
    import timeit
    def time_get():
        elapsed_time = timeit.timeit(stmt=func, number=1)
        print(func, elapsed_time)
    return time_get

from maix import display, camera

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

from maix import image

@show_run_time
def test_opencv():
    tmp = camera.read()
    image.load(tmp, 224, 224)
    image.resize(240, 240)
    display.show(image.tobytes())

# <function test_opencv at 0x104ea58> 0.02121679200001836
test_opencv()

while True:
  test_pillow()
