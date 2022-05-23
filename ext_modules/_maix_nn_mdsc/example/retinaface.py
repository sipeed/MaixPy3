from http.client import ImproperConnectionState
from pydoc import doc
from statistics import variance
from time import time

class Retinaface():
    mdsc_path = "/root/mud/v831_retinaface.mud"

    # v831
    variances = [0.1, 0.2]
    steps = [8, 16, 32]
    min_sizes = [16, 32, 64, 128, 256, 512]
    #R329
    # variances = [0.1, 0.2]
    # steps = [8, 16, 32, 64]
    # min_sizes = [10, 16, 24, 32, 48, 64, 96, 128, 192, 256]

    def __init__(self) -> None:
        from maix import nn
        self.model = nn.load(self.mdsc_path,opt = None)
        from maix.nn import decoder
        self.decoder = decoder.Retinaface([224,224] , self.steps , self.min_sizes, self.variances)

    def __del__(self):
        del self.model
        del self.decoder

    def cal_fps(self ,start , end):
        one_second = 1
        one_flash = end - start
        fps = one_second / one_flash
        return  fps

    def  draw_fps(self,img , fps):
        img.draw_string(0, 0 ,'FPS :'+str(fps), scale=1,color=(255, 0, 255), thickness=1)


    def draw_rectangle(self,img, box):
        img.draw_rectangle(box[0], box[1], box[2], box[3],color=(230 ,230, 250), thickness=2)

    def draw_point(self,img,landmark):
        for i in range(5):
            x = landmark[2 * i ]
            y = landmark[2 * i + 1]
            img.draw_rectangle(x-2,y-2, x+2,y+2,color= (193 ,255 ,193), thickness =-1)


    def process(self,input):
        out = self.model.forward(input, quantize=1, layout = "chw") # retinaface decoder only support chw layout
        boxes , landmarks = self.decoder.run(out, nms = 0.2 ,score_thresh = 0.7 , outputs_shape =[[1,4,2058],[1,2,2058],[1,10,2058]])
        for i,box in enumerate(boxes):
            self.draw_rectangle(input,box)
            self.draw_point(input , landmarks[i])

def main():
    from maix import display, camera
    app  = Retinaface()

    while True:
        img = camera.capture().resize(size=(224,224))
        app.process(img)
        display.show(img)
        # break
main()