from maix import camera,display,_maix_image
import time
camera.config(size=(240,240))


import signal
def handle_signal_z(signum, frame):
        print("APP OVER")
        exit(0)
signal.signal(signal.SIGINT, handle_signal_z)


img = _maix_image.new((240,240),(255,0,0),"RGB")
img.draw_string(30,100,"hello word!")
img.show()
time.sleep(3)


while True:
    for i in range(100):
        img = camera.get_image()
        img.draw_string(10,10,"camera.",color=(255,0,0),thickness = 1)
        img.draw_string(10,30,"get_image",color=(255,0,0),thickness = 1)
        display.show(img.tobytes())
    for i in range(100):
        img_bytes = camera.read()
        img = _maix_image.load(img_bytes,(240,240),"RGB")
        img.draw_string(10,10,"_maix_image.",color=(255,0,0),thickness = 1)
        img.draw_string(10,30,"load",color=(255,0,0),thickness = 1)
        display.show(img.tobytes())