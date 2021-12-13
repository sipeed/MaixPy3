#!/usr/bin/env python
# maix_version.find_line()函数说明
# 功能:在道路图片pic中寻找线
# 返回值：字典类型，rect：矩形的四个顶点，pixels：矩形的面积，cx、cy:矩形的中心坐标，rotation：矩形的倾斜角
#{'rect': [9, 229, 9, 9, 145, 9, 145, 229], 'pixels': 12959, 'cx': 77, 'cy': 119, 'rotation': -1.570796251296997}
# 时间:2021年9月16日, 2021年11月16日更新
# 作者：dianjixz
from maix import image, display, camera
import time


class funation:
    def __init__(self, device=None):
      self.event = self.run

    def __del__(self):
      pass

    def run(self):
      tmp = camera.capture()
      t = time.time()
      ma = tmp.find_line()
      t = time.time() - t
      print("-- forward time: {}s".format(t))
      # print(ma)
      tmp.draw_line(ma["rect"][0], ma["rect"][1], ma["rect"][2],
                    ma["rect"][3], color=(255, 255, 255), thickness=1)
      tmp.draw_line(ma["rect"][2], ma["rect"][3], ma["rect"][4],
                    ma["rect"][5], color=(255, 255, 255), thickness=1)
      tmp.draw_line(ma["rect"][4], ma["rect"][5], ma["rect"][6],
                    ma["rect"][7], color=(255, 255, 255), thickness=1)
      tmp.draw_line(ma["rect"][6], ma["rect"][7], ma["rect"][0],
                    ma["rect"][1], color=(255, 255, 255), thickness=1)
      tmp.draw_circle(ma["cx"], ma["cy"], 4,
                      color=(255, 255, 255), thickness=1)
      display.show(tmp)


if __name__ == "__main__":
    import signal

    def handle_signal_z(signum, frame):
        print("APP OVER")
        exit(0)
    signal.signal(signal.SIGINT, handle_signal_z)
    camera.config(size=(240, 240))
    start = funation()
    while True:
        start.event()
