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
      ma = cv.test_find_line(tmp)
      print(ma)
      draw = display.get_draw()
      draw.line([(ma["rect"][0], ma["rect"][1]), (ma["rect"][2], ma["rect"][3])],fill='white',width=1)
      draw.line([(ma["rect"][2], ma["rect"][3]), (ma["rect"][4], ma["rect"][5])],fill='white',width=1)
      draw.line([(ma["rect"][4], ma["rect"][5]), (ma["rect"][6], ma["rect"][7])],fill='white',width=1)
      draw.line([(ma["rect"][6], ma["rect"][7]), (ma["rect"][0], ma["rect"][1])],fill='white',width=1)
      draw.ellipse(((ma["cx"]-2, ma["cy"]-2), (ma["cx"]+2, ma["cy"]+2)), fill=None, width=1)
      display.show()
if __name__ == "__main__":
  find_line()