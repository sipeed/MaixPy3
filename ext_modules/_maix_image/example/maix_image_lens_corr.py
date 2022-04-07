from maix import  display, image 
import time
tmp = image.Image()
tmp.open("/home/lyx/datasets/test_data")
tmp = tmp.gamma_corr()
# display.show(tmp) 
# time.sleep(10)