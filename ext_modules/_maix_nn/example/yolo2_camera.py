



from maix import nn, display, camera , image 
import time
from maix.nn import decoder
import numpy as np

def draw_rectangle_with_title(img, box, disp_str, bg_color=(255, 0, 0, 255), font_color=(255, 255, 255, 255)):
    # # draw = ImageDraw.Draw(img)
    # font_w, font_h = font.getsize(disp_str)
    # draw.rectangle((box[0], box[1], box[0] + box[2], box[1] + box[3]), fill=None, outline=bg_color, width=2)
    # draw.rectangle((box[0], box[1] - font_h, box[0] + font_w, box[1]), fill=bg_color)
    # draw.text((box[0], box[1] - font_h), disp_str, fill=font_color, font=font)

    #    tmp.draw_rectangle(i["x"], i["y"], i["x"] + i["w"],
    #                                i["y"] + i["h"], color=(255, 0, 0), thickness=1)
    img.draw_rectangle(box[0], box[1], box[0] + box[2], box[1] + box[3],color=(255, 0, 0), thickness=1)
    img.draw_string(box[0], box[1]+ box[3] ,disp_str, scale=0.5,color=(0, 0, 255), thickness=1)
    

    

model = {
    "param": "/home/model/face/yolo2_face_awnn.param",
    "bin": "/home/model/face/yolo2_face_awnn.bin"
    # "bin":"./aipu_onnx_cards_224_35.bin"
}

options = {
    "model_type":  "awnn",
    "inputs": {
        "input0": (224, 224, 3)
    },
    "outputs": {
        "output0": (7, 7, (1+4+1)*5)
    },
    "mean": [127.5, 127.5, 127.5],
    "norm": [0.0078125, 0.0078125, 0.0078125],
}
print("-- load model:", model)
m = nn.load(model, opt=options)
print("-- load ok")

print("-- read image")
w = options["inputs"]["input0"][1]
h = options["inputs"]["input0"][0]
# # img.show()
print("-- read image ok")


# labels = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "mouse", "microbit", "ruler", "cat", "peer", "ship", "apple", "car", "pan", "dog", "umbrella", "airplane", "clock", "grape", "cup", "left", "right", "front", "stop", "back"]
# anchors = [2.44, 2.25, 5.03, 4.91, 3.5, 3.53, 4.16, 3.94, 2.97, 2.84]

labels = ["person"]
anchors = [1.19, 1.98, 2.79, 4.59, 4.53, 8.92, 8.06, 5.29, 10.32, 10.65]

yolo2_decoder = decoder.Yolo2(len(labels), anchors, net_in_size=(w, h), net_out_size=(7, 7))




camera.config((224, 224))
while 1:
    img = camera.capture()
    if not img:
        time.sleep(0.01)
        continue
    t = time.time()
    
    out = m.forward(img.tobytes(), quantize=True)
    print("-- forward: ", time.time() - t )


    t = time.time()
    boxes, probs = yolo2_decoder.run(out, nms=0.5, threshold=0.5, img_size=(224,224))
    print("-- decode: ", time.time() - t )

    t = time.time()
    for i, box in enumerate(boxes):
        class_id = probs[i][0]
        prob = probs[i][1][class_id]
        disp_str = "{}:{:.2f}%".format(labels[class_id], prob*100)
        draw_rectangle_with_title(img, box, disp_str)
    print("-- draw: ", time.time() - t )

    t = time.time()
    display.show(img)
    print("-- show: ", time.time() - t )


