from maix import Image
import time
import gc

import signal
def handle_signal_z(signum, frame):
        print("APP OVER")
        exit(0)
signal.signal(signal.SIGINT, handle_signal_z)

while True:
    img = Image.new(size=(240,240),color = (255,0,0),mode = "RGB")
    #写字
    img.draw_string(10,200,"hello word!",(0,0,255),1)
    img.show()
    time.sleep(1)
    #画线
    img.draw_line(10,10,10,100,color=(0,255,0),thickness = 1)
    img.show()
    time.sleep(1)
    #画框
    img.draw_rectangle(20,20,50,50,color=(0,255,0),thickness = 1)
    img.show()
    time.sleep(1)
    #画圆
    img.draw_circle(100,100,20,color=(0,255,0),thickness = 1)
    img.show()
    time.sleep(1)
    #画椭圆
    img.draw_ellipse(150,150,20,50,90,0,360,color=(0,255,0),thickness = 1)
    img.show()
    time.sleep(1)


# while True:
#     img = _maix_image.new(size=(240,240),color = (255,0,0),mode = "RGB")
#     img.draw_string(10,200,"hello word!",(0,0,255),1)
#     img.show()
#     img.clear()
#     gc.collect()