#!/usr/bin/env python

# cv.find_line(pic)函数说明
# 功能:在图片pic中寻找线段
# 输入：pic，240*240尺寸的图片bytes数据
# 返回值：[k,b,line,x]，直线的斜率，截距，线段长度，x轴位置（当k，b都为0时有用）

from maix import camera
from _maix_opencv import _v83x_opencv
from PIL import Image ,ImageDraw
from maix import display
cv = _v83x_opencv()

def find_line():
  while True:
    tmp = camera.read()
    if tmp:
      ma = cv.find_line(tmp)
      print(ma)
      if ma:
        draw = display.get_draw()
        if ma[0] == 0:
          ma[0] = 0.0001
        x_b = int(-ma[1]/ma[0])         # 直线与x轴的交点横坐标
        y0  = 0 
        if ma[0] < 0:                       
          y_b = int(ma[1])              # 直线与y轴的交点纵坐标
          x0  = 0
        else:
          x0  = 239                     # 直线与屏幕右边框交点横坐标
          y_b = int(239*ma[0] + ma[1])  # 直线与屏幕右边框交点纵坐标
        draw.line([(x_b, y0), (x0, y_b)],
                  fill='white',
                  width=1)
        display.show()
      else:
        display.clear()


if __name__ == "__main__":
  find_line()