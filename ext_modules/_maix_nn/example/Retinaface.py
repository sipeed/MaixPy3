import time
import numpy as np
from maix.nn import decoder
from maix import nn, display, camera , image

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

model = {
    "bin": "/root/models/aipu_Retinaface_320.bin"
}

options = {
    "model_type":  "aipu",

    "inputs": {
        "input0": (320, 320, 3)
    },

    "outputs": {
        "output0":(1,4,5875),    # HWC order
        "output1":(1,2,5875),
        "output2":(1,10,5875)
    },

    "mean": [127, 127, 127],
    "norm": [1 , 1  ,1],
    "scale":[32.752407 , 29.865177 , 14.620169],
    "steps":[8, 16, 32, 64],
    "min_sizes":[10, 16, 24, 32, 48, 64, 96, 128, 192, 256],
    "variance":[0.1,0.2]
}

# 模型加载
print("-- load model:", model)
m = nn.load(model , options)
print("load model success!")

# 获取处理图像的基本信息
w = options["inputs"]["input0"][1]
h = options["inputs"]["input0"][0]
print("process image size is (H:{} , W:{})".format(h,w))

#设置摄像头获取处理图像大小
camera.config((h,w))

#初始化解码器
steps = options["steps"]
min_sizes = options["min_sizes"]
variances = options["variance"]

#初始化解码器
print('init decoder')
retinaface_decoder = decoder.Retinaface([h,w] ,steps, min_sizes,variances)
print('init decoder has done')

#动态处理图像
while True:
    img  = camera.capture()
    # 进行模型推理，在原来的基础上添加了 outputfmt 选项来给用户自定义返回的类型
    t = time.time()
    out = m.forward(img.tobytes(), quantize=1, layout = "chw")

    # 进行解码
    boxes , landmarks = retinaface_decoder.run(out, nms = 0.2 ,score_thresh = 0.7 , outputs_shape =[[1,4,5875],[1,2,5875],[1,10,5875]])

    #渲染图像
    for i,box in enumerate(boxes):
        draw_rectangle(img, box)
        draw_point(img,landmarks[i])

    #计算FPS
    fps = cal_fps(t ,time.time())

    # 显示FPS
    draw_fps(img ,  fps)

    #图像显示
    display.show(img)

