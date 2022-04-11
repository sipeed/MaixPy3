from maix import  display, image 
import time
tmp = image.Image()
tmp.open("/home/lyx/Desktop/src2.png")
tmp = tmp.histeq()
display.show(tmp) 
time.sleep(10)