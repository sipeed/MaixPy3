from maix import nn, display, camera , image
import time
from maix.nn import decoder
import numpy as np

def cal_fps(start , end):
    one_second = 1
    one_flash = end - start
    fps = one_second / one_flash
    return  fps


def draw_rectangle_with_title(img, box, disp_str , fps ):
    img.draw_rectangle(box[0], box[1], box[0] + box[2], box[1] + box[3],color=(255, 0, 0), thickness=1)
    img.draw_string(box[0], box[1]+ box[3] ,disp_str, scale=0.5,color=(0, 0, 255), thickness=1)
    img.draw_string(0, 0 ,'FPS :'+str(fps), scale=2 ,color=(0, 0, 255), thickness=2)


model = {
    "bin":"/root/models/aipu_onnx_cards_224_35.bin"
}

options = {
    "model_type":  "aipu",
    "inputs": {
        "input0": (224, 224, 3)
    },
    "outputs": {
        "output0": (7, 7, (1+4+35)*5)
    },
    "mean": [127.5, 127.5, 127.5],
    "norm": [0.0078125, 0.0078125, 0.0078125],
    "scale":[10.872787] ,
}

#加载模型
m = nn.load(model, opt=options)

#获取处理图像大小
w = options["inputs"]["input0"][1]
h = options["inputs"]["input0"][0]

# 设置标签和anchor大小
labels = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "mouse", "microbit", "ruler", "cat", "peer", "ship", "apple", "car", "pan", "dog", "umbrella", "airplane", "clock", "grape", "cup", "left", "right", "front", "stop", "back"]
anchors = [2.44, 2.25, 5.03, 4.91, 3.5, 3.53, 4.16, 3.94, 2.97, 2.84]

# 声明解码器并初始化
yolo2_decoder = decoder.Yolo2(len(labels), anchors, net_in_size=(w, h), net_out_size=(7, 7))

camera.config((h, w))

while 1:
    # 获取摄像头图像
    t  = time.time()
    img = camera.capture()

    #模型前向推理
    out = m.forward(img.tobytes(), quantize  =1 ,layout = "chw")[0]  #返回对象是一个List对象，实际输出的内容是需要进行解包

    #解码器解码（后处理）
    boxes, probs = yolo2_decoder.run(out, nms=0.5, threshold=0.5, img_size=(224,224))

    #图像渲染
    for i, box in enumerate(boxes):
        class_id = probs[i][0]
        prob = probs[i][1][class_id]
        disp_str = "{}:{:.2f}%".format(labels[class_id], prob*100)
        fps = cal_fps(t, time.time())
        draw_rectangle_with_title(img, box, disp_str, fps)

    #图像端侧显示
    display.show(img)


