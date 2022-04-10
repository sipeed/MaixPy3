from maix import  display, image 
import time
tmp = image.Image()
tmp.open("/home/lyx/images/test_filter.jpg")
tmp = tmp.lens_corr()
display.show(tmp) 
time.sleep(10)