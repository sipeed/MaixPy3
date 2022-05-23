
class Retinaface:
    path = {
        #R329
        "bin": "./models/aipu_retinaface.bin"

        # V831
        # "bin": "./models/awnn_retinaface.bin"
        # "param": "./models/awnn_retinaface.param"
    }
    input_size = (320,320,3)
    output0_size = (1,4,5875)
    output1_size = (1,2,5875)
    output2_size = (1,10,5875)
    options = {
    "model_type":  "aipu",

    "inputs": {
        "input0": (320, 320, 3)
    },

    "outputs": {
        "output0":output0_size,    # HWC order
        "output1":output1_size,
        "output2":output2_size
    },

    "mean": [127, 127, 127],
    "norm": [1 , 1  ,1],
    "scale":[32.752407 , 29.865177 , 14.620169],
    "steps":[8, 16, 32, 64],
    "min_sizes":[10, 16, 24, 32, 48, 64, 96, 128, 192, 256],
    "variance":[0.1,0.2]
    }

    def __init__(self) -> None:
        from maix import nn
        print(" - - load model :",self.path)
        self.model = nn.load(self.path , opt = self.options)
        print("- -load ok")
        from maix.nn import decoder
        print("- - load decoder")
        steps = self.options["steps"]
        min_sizes = self.options["min_sizes"]
        variances = self.options["variance"]
        h = self.input_size[0]
        w = self.input_size[1]
        self.decoder = decoder.Retinaface([h,w] ,steps, min_sizes,variances)

    def __del__(self):
        del self.model
        del self.decoder

    def cal_fps(start , end):
        one_second = 1
        one_flash = end - start
        fps = one_second / one_flash
        return  fps

    def  draw_fps(img , fps):
        img.draw_string(0, 0 ,'FPS :'+str(fps), scale=1,color=(255, 0, 255), thickness=1)


    def draw_rectangle(img, box):
        img.draw_rectangle(box[0], box[1], box[2], box[3],color=(230 ,230, 250), thickness=2)

    def draw_point(img,landmark):
        for i in range(5):
            x = landmark[2 * i ]
            y = landmark[2 * i + 1]
            img.draw_rectangle(x-2,y-2, x+2,y+2,color= (193 ,255 ,193), thickness =-1)

#main
from maix import  display, camera
import numpy as np
from maix import nn
import time
global m
m = Retinaface()

while True:
    img = camera.capture().resize(size=(m.input_size[0],m.input_size[1]))
    t = time.time()
    out = m.forward(img.tobytes(), quantize=1, layout = "chw")
    boxes , landmarks = m.decoder.run(out, nms = 0.2 ,score_thresh = 0.7 , outputs_shape =[[1,4,5875],[1,2,5875],[1,10,5875]])
    for i,box in enumerate(boxes):
        m.draw_rectangle(img,box)
        m.draw_point(img , landmarks[i])
    fps =  m.cal_fps(t, time.time())
    m.draw_fps(img ,  fps)
    display.show(img)