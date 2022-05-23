class Yolo:
    mdsc_path = "/root/mud/v831_yolo_person.mud"
    labels = ["person"]
    anchors = [4.72, 6.26, 1.39, 3.53, 0.78, 1.9, 0.35, 0.95, 2.49, 4.87]

    def __init__(self) -> None:
        from maix import nn
        print(" - - load model :",self.mdsc_path)
        # self.model = nn.load(self.model, opt=self.options)
        self.model = nn.load(self.mdsc_path)
        print("- -load ok")
        from maix.nn import decoder
        print("- - load decoder")
        self.decoder = decoder.Yolo2(len(self.labels) , self.anchors , net_in_size = (224, 224) ,net_out_size = (7,7))
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
        img.draw_rectangle(box[0], box[1], box[0] + box[2], box[1] + box[3],color=(255, 0, 0), thickness=2)
        img.draw_string(box[0], box[1]+ box[3] ,disp_str, scale=0.5,color=(0, 0, 255), thickness=2)
        img.draw_string(0, 0 ,'FPS :'+str(fps), scale=2 ,color=(0, 0, 255), thickness=2)

# maix
from maix import display,camera
import time
global m
m = Yolo()

while True:
    img = camera.capture().resize(size=(224,224))
    t = time.time()
    out = m.model.forward(img, quantize=1, layout = "hwc")
    boxes, probs = m.decoder.run(out, nms=0.5, threshold=0.4, img_size=(224,224))
    for i, box in enumerate(boxes):
        class_id = probs[i][0]
        prob = probs[i][1][class_id]
        disp_str = "{}:{:.2f}%".format(m.labels[class_id], prob*100)
        fps = m.cal_fps(t, time.time())
        m.draw_rectangle_with_title(img, box, disp_str, fps)
    display.show(img)