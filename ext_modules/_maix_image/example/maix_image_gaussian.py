from maix import image,display
hello_img = image.Image().new(size = (240, 240), 
                              color = (255, 0, 0), mode = "RGB")     #创建一张红色背景图
hello_img = hello_img.gaussian()
display.show(hello_img)   #把这张图显示出来