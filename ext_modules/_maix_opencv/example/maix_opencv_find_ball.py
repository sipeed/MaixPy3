#!/usr/bin/env python

# vision.find_ball(pic,hsv)函数说明
# 功能:在图片pic中寻找符合hsv颜色区域的小球
# 输入：pic，240*240尺寸的图片bytes数据
#     hsv,hsv阈值数组，前三个是最小值，后三个是最大值
# 返回值：[[cr_x,cr_y,cr_w,cr_h]]，拟合小球的中心值，高和宽

from maix import camera
from maix import vision
from PIL import Image, ImageDraw
from maix import display

def find_ball():
  while True:
    tmp = camera.read()
    if len(tmp):
      ma = vision.find_ball(tmp,(95,219,0,255,255,255))
      print(ma)
      if ma:
        draw = display.get_draw()
        for i in ma:
          draw.ellipse((i[0]-i[3]/2, i[1]-i[2]/2, i[0]+i[3]/2, i[1]+i[2]/2),
                        fill ='white',
                        outline ='black',
                        width =1)
        display.show()
      else:
        display.clear()

if __name__ == "__main__":
  find_ball()