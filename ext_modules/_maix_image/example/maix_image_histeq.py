from maix import  display, image 
import time
tmp = image.Image()
tmp.open("/home/lyx/Desktop/src.png")
tmp = tmp.histeq()
display.show(tmp) 
time.sleep(10)