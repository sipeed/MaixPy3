#!/usr/bin/env python

# //==================================================================
# //函数名：  vision.opecv_Canny函数说明
# //作者：    dianjixz
# //日期：    2021-10-28
# //功能：    边缘检测函数
# //输入参数：  vision.opecv_Canny(img, thr_h, thr_l, size=(0,0),mode=0)
# //          py_img            图像对象,PIL图像或bytes图像(如果不是240*240的rgb数据需要提供size和mode参数)
# //          thr_h             最大阈值
# //          thr_l             最小阈值
# //          size              图像的尺寸(非必须,默认240*240)
# //          mode              图像的格式(非必须,默认vision.COLOR_RGB)
# //返回值：
# //          返回边缘检测函数后的图像,图像格式和输入保持一致；
# //修改记录：2021年11月17日,删除了该函数的实现,以后需要可以考虑加上
# //==================================================================


import signal
from maix import camera, display, Image
import time
camera.config(size=(240, 240))


def handle_signal_z(signum, frame):
    print("APP OVER")
    exit(0)


signal.signal(signal.SIGINT, handle_signal_z)

while True:
    img = camera.capture()
    img.cv_Canny(50, 50)
    img.show()
