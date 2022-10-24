from time import time


class pose:
    mdsc_path = "./pose.mud"
    center_weight_pose = "./center_weight.bin"
    hm_th = 0.1

# ['鼻子'，'左眼'，'右眼'，'左耳'，'右耳'，
#  '左肩'，'右肩'，'左肘'，'右肘'，'左手腕'，
#  '右手腕'，'左臀部'，'右臀部'，'左膝盖'，'右膝盖'，'左脚踝'，
#      '右脚踝']
    links = [[0, 1], [0, 2], [1, 3], [2, 4], [5, 7], [7, 9], [11, 13], [13, 15], [6, 8], [8, 10], [12, 14], [14, 16]]

    def __init__(self) -> None:
        from maix import nn
        self.model = nn.load(self.mdsc_path)
        from maix.nn import decoder
        self.decoder = decoder.pose(self.center_weight_pose, self.hm_th)

    def __del__(self):
        del self.model
        del self.decoder

    def cal_fps(self, img, start, end):
        one_second = 1
        one_flash = end - start
        fps = one_second / one_flash
        img.draw_string(0, 0, 'FPS :'+str(fps), scale=2,
                        color=(0, 0, 255), thickness=2)

    def draw_line(self, img, points, links):
        for start, end in links:
            img.draw_line(points[start][0], points[start]
                          [1], points[end][0], points[end][1])

    def process(self, input):
        t = time()
        out = self.model.forward(input, quantize=1, layout="hwc")
        points = self.decoder.run(out)
        self.draw_line(input, points, self.links)
        self.cal_fps(input, t, time())
