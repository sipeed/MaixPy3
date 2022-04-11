from maix import  display, image
import time
tmp = image.Image()
tmp.open("/home/lyx/images/test_filter.jpg")
tmp = tmp.mean(2)
display.show(tmp)
time.sleep(10)