#!/usr/bin/env python

# cv.get_blob_hsv(pic,roi,critical)函数说明
# 功能:在图片pic中寻找符合hsv颜色区域的色块
# 输入：pic，240*240尺寸的图片bytes数据
#     roi,感兴趣区域,他是一个数组，[x,y,w,h]
#critical 范围值 ,检测到区域内最多的颜色，然后加减该值
# 返回值：[140, 6, 134, 150, 16, 144]，HSV阈值列表
#[min_h,min_s,min_v,max_h,max_s,max_v] 该值可被直接用于寻找色块


from maix import camera
from maix import vision
from PIL import Image, ImageDraw
from maix import display

def get_blob_hsv():
    while True:
        tmp = camera.read()
        ma = vision.get_blob_hsv(tmp,[110,110,20,20],5)
        print(ma)
        draw = display.get_draw()
        draw.rectangle((110,110, 130, 130), outline='red', width=1)
        display.show()

        
if __name__ == "__main__":
  get_blob_hsv()
