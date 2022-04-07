from maix import  display, image 
import time
tmp = image.Image()
tmp.open("/home/lyx/Desktop/src2.png")
tmp = tmp.gamma_corr()
display.show(tmp) 
time.sleep(10)