import io
import os

from PIL import Image

# remote jupyter options
remote, _remote_show = None, False

def jupyter(show=False, clear=True):
  try:
    global remote, _remote_show
    _remote_show = show
    if remote != None:
      remote.clear_output = clear
      remote._start_display()
  except Exception as e:
    remote = None

__width__, __height__, __mode__= 240, 240, "RGB"

try:
    __fastview__ = None
    from _maix_display import Display
    __fastview__ = Display()
    __width__, __height__ = __fastview__.width, __fastview__.height
    def __draw__(img):
        global __fastview__
        if isinstance(img, bytes):
          __fastview__.draw(img, __fastview__.width, __fastview__.height)
        elif isinstance(img, Image.Image):
          img = img.resize(
              (__fastview__.width, __fastview__.height), Image.ANTIALIAS)
          __fastview__.draw(img.tobytes(), __fastview__.width,
                            __fastview__.height)
except ModuleNotFoundError as e:
    pass
except Exception as e:
    pass

__display__ = None

def get_display():
    global __display__
    if __display__ is None:
      global __width__, __height__, __mode__
      __display__ = Image.new(__mode__, (__width__, __height__))
    return __display__

def get_draw():
    from PIL import ImageDraw
    disp = get_display()
    if disp:
        tmp = ImageDraw.Draw(disp)
        tmp.paste = disp.paste
        return tmp
    return None

def __thumbnail__(src, dst):
    w, h = src.size
    if w > dst.width or h > dst.height:
        src.thumbnail((dst.width, dst.height))

def show(img=None, box=(0, 0), local_show=True, remote_show=True):
    global __display__, _remote_show
    if img is None:
        img = __display__
    if local_show:
        if __fastview__:
            __draw__(img)  # underlying optimization
        else:
            if isinstance(img, bytes):
                img = Image.frombytes("RGB", box, img)
                __thumbnail__(img, __display__)
                __display__.paste(img, (0, 0))
            elif isinstance(img, Image.Image):
                __thumbnail__(img, __display__)
                __display__.paste(img, box)
            __display__.show()
    if remote_show and _remote_show:
        from maix import mjpg
        mjpg.store_mjpg(img)


def fill(box=(0, 0), color=(0, 0, 0, 0)):
    if len(box) == 2:
        global __width__, __height__
        box = box + (__width__, __height__)
    get_draw().paste(color, box)
    show(__display__)


def clear(c=(0, 0, 0, 0)):
    fill(color=c)


if __name__ == '__main__':
    clear((255, 0, 0))
    fill((20, 20, 220, 220), (0, 0, 0))
    fill((40, 40, 200, 200), (0, 0, 255))
    # show()

    from PIL import Image, ImageDraw
    image = Image.new("RGB", (100, 60), "#FFFFFF")
    draw = ImageDraw.Draw(image)
    draw.text((10, 10), u'hello world', (0, 0, 0))
    image = image.rotate(90)
    # image.show()
    # show(image, (0, 120))

    import numpy as np
    tmp = np.ndarray((10, 10, 3), np.uint8)
    tmp[:, :, 0] = 0
    tmp[:, :, 1] = 255
    tmp[:, :, 2] = 0
    image = Image.fromarray(tmp)

    tmp = b'\x00\xFF\x00' * (300 * 240)
    # show(tmp, (300, 240))
    # image = Image.frombytes("RGB", (300, 240), tmp)
    # print(image.tobytes())

    from PIL import Image, ImageDraw
    image = Image.new("RGBA", (40, 40), "#FFFFFFFF")
    
    draw = ImageDraw.Draw(image)
    draw.text((0, 0), u'hello world', (0, 0, 0))
    # image.show()
    show(image)