class Yolo:
    path = {
        #R329
        "bin": "./models/aipu_yolo_card.bin"

        # V831
        # "bin": "./models/awnn_yolo_card.bin"
        # "param": "./models/awnn_yolo_card.param"
    }

    input_size = (224, 224, 3)
    output_size =(7, 7, (1+4+35)*5)
    options = {
    "model_type":  "aipu",
    "inputs": {
        "input0": input_size
    },
    "outputs": {
        "output0": output_size
    },
    "mean": [127.5, 127.5, 127.5],
    "norm": [0.0078125, 0.0078125, 0.0078125],
    "scale":[10.872787],# R329 有此选项，V831没有这个选项
    }

    labels = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "mouse", "microbit", "ruler", "cat", "peer", "ship", "apple", "car", "pan", "dog", "umbrella", "airplane", "clock", "grape", ]
    anchors = [2.44, 2.25, 5.03, 4.91, 3.5, 3.53, 4.16, 3.94, 2.97, 2.84]

    def __init__(self) -> None:
        from maix import nn
        print(" - - load model :",self.path)
        self.model = nn.load(self.path , opt = self.options)
        print("- -load ok")
        from maix.nn import decoder
        print("- - load decoder")
        self.decoder = decoder.Yolo2(len(self.labels) , self.anchors , net_in_size = (self.input_size[1], self.input_size[0]) ,net_out_size = (self.output_size[1],self.output_size[0]))
        print("-- decoder load done")

    def __del__(self):
        del self.model
        del self.decoder

    def cal_fps(self ,start , end):
        one_second = 1
        one_flash = end - start
        fps = one_second / one_flash
        return  fps

    def draw_rectangle_with_title(self ,img, box, disp_str , fps ):
        img.draw_rectangle(box[0], box[1], box[0] + box[2], box[1] + box[3],color=(255, 0, 0), thickness=1)
        img.draw_string(box[0], box[1]+ box[3] ,disp_str, scale=0.5,color=(0, 0, 255), thickness=1)
        img.draw_string(0, 0 ,'FPS :'+str(fps), scale=2 ,color=(0, 0, 255), thickness=2)

# maix
from maix import display,camera
import time
global m
m = Yolo()

while True:
    img = camera.capture().resize(size=(m.input_size[0] ,m.input_size[1]))
    t = time.time()
    out = m.model.forward(img.tobytes(), quantize=1, layout = "hwc")
    boxes, probs = m.decoder.run(out, nms=0.5, threshold=0.5, img_size=(224,224))
    for i, box in enumerate(boxes):
        class_id = probs[i][0]
        prob = probs[i][1][class_id]
        disp_str = "{}:{:.2f}%".format(m.labels[class_id], prob*100)
        fps = m.cal_fps(t, time.time())
        m.draw_rectangle_with_title(img, box, disp_str, fps)
    display.show(img)