
__width__, __height__, __mode__ = 240, 240, "RGB"

try:
    __fastview__ = None
    from _maix_display import Display
    __fastview__ = Display()
    __width__, __height__ = __fastview__.width, __fastview__.height

    def __draw__(img):
        from PIL import Image
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
    from PIL import Image
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
    global __display__, __mode__, __width__, __height__
    if img is None:
        img = get_display()
    else:
        get_display()

    if remote_show:
        from maix import mjpg
        mjpg.store_mjpg(img)

    if local_show:
        try:
            from maix import image
            if isinstance(img, image.Image):
                if img.width != __width__ or img.height != __height__:
                    img = img.resize(__width__, __height__)
                img = img.tobytes()
        except ImportError as e:
          pass
        if __fastview__:
            __draw__(img)  # underlying optimization
        else:
            from PIL import Image
            if isinstance(img, bytes):
                img = Image.frombytes(__mode__, box, img)
                __thumbnail__(img, __display__)
                __display__.paste(img, (0, 0))
            elif isinstance(img, Image.Image):
                __thumbnail__(img, __display__)
                __display__.paste(img, box)
            __display__.show()


def fill(color=(0, 0, 0, 0), box=None):
    if box is None:
        global __width__, __height__
        box = (0, 0, __width__, __height__)
    if len(box) == 2:
        box = (0, 0) + box
    get_draw().paste(color, box)
    show(__display__)


def clear(c=(0, 0, 0, 0)):
    fill(color=c)


if __name__ == '__main__':
    clear((255, 0, 0))
    fill((255, 0, 0))
    fill((0, 255, 0), (100, 100))
    fill((0, 0, 255), (20, 20, 220, 220))
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
