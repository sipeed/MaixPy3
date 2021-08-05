#!/usr/bin/env python

# cv.find_blob(pic,hsv)函数说明
# 功能:在图片pic中寻找符合hsv颜色区域的色块
# 输入：pic，240*240尺寸的图片bytes数据
#     hsv,hsv阈值数组，前三个是最小值，后三个是最大值
# 返回值：[x1,y1,x2,y2]，色块对角线的坐标
from maix import camera
from _maix_opencv import _v83x_opencv
from PIL import Image, ImageDraw
from maix import display
cv = _v83x_opencv()

def find_blob():
  while True:
    tmp = camera.read()
    if len(tmp):
      ma = cv.find_blob(tmp, (95, 219, 0, 255, 255, 255))
      print(ma)
      if ma:
        draw = display.get_draw()
        for i in ma:
          draw.rectangle((i[0], i[1], i[2], i[3]),
                         fill='white', outline='black', width=1)
        display.show()
      else:
        display.clear()
        
if __name__ == "__main__":
  find_blob()
