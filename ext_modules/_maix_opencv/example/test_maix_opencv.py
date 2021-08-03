# from _maix_vivo import _v83x_vivo
from maix import camera
from _maix_opencv import _v83x_opencv
from PIL import Image ,ImageDraw
from maix import display
cv = _v83x_opencv()

# from PIL import Image
# image = Image.frombytes("RGB", (240, 240), cv.test(b""))
# image.show()

# bak = b'\x00\x00\x00\x00' * (240 * 240)

# tmp = cv.test(bak)

# print(len(tmp))

# def find_blob():
#   while True:
#     tmp = camera.read()
#     if len(tmp):
#       ma = cv.find_blob(tmp,(95,219,0,255,255,255))
#       print(ma)
#       if ma:
#         draw = display.get_draw()
#         for i in ma:
#           draw.rectangle((i[0],i[1],i[2],i[3]),fill ='white',outline ='black',width =1)
#         display.show()
#       else:
#         display.clear()

def find_ball():
  while True:
    tmp = camera.read()
    if len(tmp):
      ma = cv.find_ball(tmp,(95,219,0,255,255,255))
      print(ma)
      if ma:
        draw = display.get_draw()
        for i in ma:
          draw.ellipse((i[0]-i[3]/2, i[1]-i[2]/2, i[0]+i[3]/2, i[1]+i[2]/2),
                        fill ='white',
                        outline ='black',
                        width =1)
        display.show()
      else:
        display.clear()

if __name__ == "__main__":
  find_ball()
