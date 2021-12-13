
from maix import image, display, camera
import time


import signal


def handle_signal_z(signum, frame):
    print("APP OVER")
    exit(0)


signal.signal(signal.SIGINT, handle_signal_z)


def test_draw():
    img = image.Image().new(size=(240, 240), color=(255, 0, 0), mode="RGB")
    # draw_string
    img.draw_string(10, 200, "hello word!", scale=0.5,
                    color=(0, 0, 255), thickness=1)
    display.show(img)
    time.sleep(1)
    # draw_line
    img.draw_line(10, 10, 10, 100, color=(0, 255, 0), thickness=1)
    display.show(img)
    time.sleep(1)
    # draw_rectangle
    img.draw_rectangle(20, 20, 50, 50, color=(0, 255, 0), thickness=1)
    display.show(img)
    time.sleep(1)
    # draw_circle
    img.draw_circle(100, 100, 20, color=(0, 255, 0), thickness=1)
    display.show(img)
    time.sleep(1)
    # draw_ellipse
    img.draw_ellipse(150, 150, 20, 50, 90, 0, 360,
                     color=(0, 255, 0), thickness=1)
    display.show(img)
    time.sleep(1)


test_draw()


def test_img_copy():
    img = image.Image().new(color=(255, 0, 0))
    print(img)
    display.show(img)
    time.sleep(0.5)
    imga = img.copy()
    print(imga)
    imga.draw_rectangle(0, 0, 240, 240, (0, 255, 0), -1)
    display.show(imga)


test_img_copy()


def test_set_pixel():
    img = image.Image()
    img.new(color=(255, 0, 0))
    for i in range(10, 50):
        for j in range(10, 20):
            img.set_pixel(i, j, (0, 0, 255))
    display.show(img)


test_set_pixel()


def test_crop():
    img = image.Image().new(color=(255, 0, 0))
    img.draw_string(100, 100, "nihao", color=(0, 0, 255))
    display.show(img)
    mk = img.crop(90, 90, 100, 50)
    print(mk)
    display.show(mk.resize(240, 240))


test_crop()


def test_draw_image():
    img = image.Image().new(color=(255, 0, 0))
    img.draw_string(100, 100, "nihao", color=(0, 0, 255))
    display.show(img)
    # time.sleep(0.5)
    mk = img.crop(90, 90, 100, 50)
    imga = image.Image().new(color=(0, 255, 0))
    imga.draw_image(mk, 10, 10)
    display.show(imga)


test_draw_image()

def test_rotate():
    img = image.Image().new(color=(255, 0, 0))
    img.draw_string(100, 100, "nihao", color=(0, 0, 255))
    for i in range(5):
        time.sleep(0.5)
        display.show(img.rotate(72))


test_rotate()


def test_convert():
    img = image.Image().new(color=(255, 0, 0))
    print(img)
    img.convert("L")
    print(img)
    img.convert("RGB")
    print(img)
    display.show(img)


test_convert()


while True:
    display.show(camera.capture())
