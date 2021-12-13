
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

camera.config((224, 224))


@show_run_time
def test_pillow():
    tmp = camera.capture()
    display.show(tmp.resize((240, 240)))


# <function test_pillow at 0x199fff8> 0.07056433300022036
test_pillow()

display.show(camera.capture())


@show_run_time
def test_opencv():


tmp = camera.read()
display.show(image(tmp, 224, 224).resize(240, 240).tobytes())


# <function test_opencv at 0x104ea58> 0.02121679200001836
test_opencv()


def unit_test():


img = Image()
img = img.new()

camera.config((224, 224))
while True:
tmp = camera.read()
img2 = image.load(tmp2, 224, 224).resize(240, 240).draw_rectangle(20, 20, 220, 220, color=(
    0, 255, 0), thickness=4).draw_string(20, 120, str(time.time()), scale=0.6, color=(255, 0, 0), thickness=2)
display.show(img.tobytes())

unit_test()


img = Image("RGB", color="#FFFFFF")
image.draw_ellipse(0, 1, "123")
display.show(image)


# from PIL import ImageDraw
# img = Image("RGB", color="#FFFFFF")
# draw = ImageDraw(img)
# draw.ellipse((i[0]-i[3]/2, i[1]-i[2]/2, i[0]+i[3]/2, i[1]+i[2]/2), fill ='white', outline ='black', width =1)
# display.show(img)


if __name__ == "__main__":

    unit_test()


mk = _maix_image.new(color=(255, 0, 0))
mk.draw_rectangle(0, 0, 100, 130, thickness=3)
display.show(mk.tobytes())


mk = _maix_image.open_file("./test.jpg")


mk = _maix_image.new(color=(255, 255, 255))
mk.draw_rectangle(10, 10, 100, 130, (255, 10, 10), thickness=-1)
__fastview__ = Display()
__fastview__.draw(mk.tobytes())


mk.draw_string(100, 100, "nihao", (255, 255, 255), 1.0, 3)

# from maix import display, camera
[(36, 86, 4, 108, -35, 110)]
[(36, 4, -35, 86, 108, 110)]
# # from maix import display, camera
# from PIL import Image
# image = Image.frombytes(mode="RGB",size=(240,240),data=t)
# # display.show(image)

# from _maix_display import Display
# __fastview__ = Display()
# __fastview__.draw(t, 240, 240)

# display.__fastview__.draw(image.tobytes(), 240, 240)


# from maix import display, camera
# display.show(t)


mk = _maix_image.Image()
md = mk.tobytes()


display.show(camera.capture())

mk = _maix_image.new(color=(255, 0, 0))
print(mk)
mk.cv_Canny(10, 20)

mk.draw_rectangle(0, 0, 100, 100, thickness=3)
display.show(mk.tobytes())


image = Image.frombytes(mode="RGB", size=(240, 240), data=md)

display.show(image)
