from maix import camera, display, image #引入python模块包
while True:
    tmp = image.Image()
    tmp.open("/home/lyx/Desktop/src.png")
    tmp = tmp.histeq()
    display.show(tmp) 