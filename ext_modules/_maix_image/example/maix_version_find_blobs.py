#!/usr/bin/env python
# //==================================================================
#   maix_version.find_blobs(thresholds, roi = (0,0,0,0), x_stride = 2, y_stride = 2, invert = 0, area_threshold = 10, pixels_threshold = 10, merge = 0, margin = 0, tilt = 0, co = 1)
#   查找图像中所有色块，并返回一个包括每个色块的色块对象的列表。
#   thresholds 必须是元组列表. [(minL, minA, minB, maxL, maxA, maxB)]
#   roi 是感兴趣区域的矩形元组(x，y，w，h)。如果未指定，ROI即整个图像的图像矩形。 操作范围仅限于 roi 区域内的像素。
#   x_stride 是查找某色块时需要跳过的x像素的数量。找到色块后，直线填充算法将精确像素。 若已知色块较大，可增加 x_stride 来提高查找色块的速度。
#   y_stride 是查找某色块时需要跳过的y像素的数量。找到色块后，直线填充算法将精确像素。 若已知色块较大，可增加 y_stride 来提高查找色块的速度。
#   invert 反转阈值操作，像素在已知颜色范围之外进行匹配，而非在已知颜色范围内。
#   若一个色块的边界框区域小于 area_threshold ，则会被过滤掉。
#   若一个色块的像素数小于 pixel_threshold ，则会被过滤掉。
#   merge 若为True，则合并所有没有被过滤掉的色块，这些色块的边界矩形互相交错重叠。
#   margin 可在相交测试中用来增大或减小色块边界矩形的大小。例如：边缘为1、相互间边界矩形为1的色块将被合并。
#   tilt设置是否查找最小斜矩形框,为0则不查找.
#   co为返回的颜色空间模型.可选为,如果不是特殊需要,请保持默认
#   0       rgb
#   1       lab
#   2       hsv
#   3       灰度
#   返回值:[{'x': 140, 'y': 88, 'w': 15, 'h': 7, 'pixels': 43, 'cx': 147, 'cy': 91}]
#   (x, y, w, h)色块的外框,pixels,色块的像素大小,(cx, cy)色块的中心点.
# 时间：2021年9月16日 ,2021年11月16日更新
# 作者：dianjixz
# //==================================================================
from maix import image, display, camera
import time


class funation:
    green = [(28, -36, -14, 68, -5, 15)]  # 绿色
    red = [(20, 22, -3, 55, 52, 42)]  # 红色
    yellow = [(35, -6, 22, 88, 5, 81)]  # 黄色
    bull = [(13, 11, -91, 54, 48, -28)]  # 蓝色
    white = [(41, 6, -32, 74, 11, -12)]  # 白色
    black = [(10, -3, -28, 50, 10, -4)]  # 黑色

    def __init__(self, device=None):
        self.event = self.run

    def __del__(self):
      print("maix_version.find_blobs will exit!")

    def run(self):
        tmp = camera.capture()
        t = time.time()
        ma = tmp.find_blobs(self.bull)
        t = time.time() - t
        print("-- forward time: {}s".format(t))
        # print(ma)
        if ma:
            for i in ma:
                tmp.draw_rectangle(i["x"], i["y"], i["x"] + i["w"],
                                   i["y"] + i["h"], color=(255, 0, 0), thickness=1)
        display.show(tmp)


if __name__ == "__main__":
    import signal

    def handle_signalm(signum, frame):
        print("father over")
        exit(0)
    signal.signal(signal.SIGINT, handle_signalm)
    camera.config(size=(240, 240))
    start = funation()
    while True:
        start.event()

# tmp.draw_rectangle(10, 10, 20, 20, color=(255, 0, 0), thickness=1)
