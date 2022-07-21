
class MaixVideo():

    def __init__(self):
        self.cam, self._width, self._height = None, 0, 0

    def width(self):
        return self._width

    def height(self):
        return self._height

    def write(self):
        pass  # for file

    def read(self):
        return None

    def config(self, size):
        # print("MaixVideo", size)
        self._width, self._height = size[0], size[1]

    def capture(self, pillow=False):
        tmp = self.read()
        if tmp:
            try:
                if pillow == False:
                    from maix import image
                    # print("self._width, self._height", self._width, self._height)
                    return image.Image().load(tmp, (self._width, self._height), "RGB")
            except Exception as e:
                pass
            try:
                from PIL import Image
                return Image.frombytes("RGB", (self._width, self._height), tmp)
            except Exception as e:
                pass
        else:
            print("[camera.capture] can't get image, camera is busy!!!")
        return None

    def close(self):
        self.__del__()
try:
    try:
        # use mpp vivo on v831
        from maix import display, image
        from _maix_vivo import _v83x_vivo
        class V831VivoMaixVideo(MaixVideo):
            def __init__(self, source="allwinner mpp"):
                self.source = source
                self._vo_dir = self._ai_dir = 0
                super(V831VivoMaixVideo, self).__init__()
                if display.width() < display.height(): # 240 x 320 to 320 x 240
                    # maix-smart or other display.config 240 x 320 => 320 x 240
                    display.config(size=(display.height(), display.width()))
                    self._vo_dir = self._ai_dir = 3 # -90 rotate show
                self.config() # for v831 mpp ready

            def config(self, size=None, _ai_size=(224, 224), exp_gain=(-1, -1)):
                if size == None:
                    size = (display.width(), display.height())
                super(V831VivoMaixVideo, self).config(size)
                if self.cam == None:
                    print('[camera] config input size(%d, %d)' %
                        (self.width(), self.height()))
                    display.__show__ = self.cam = _v83x_vivo(size[0], size[1], _ai_size[0], _ai_size[1], vo_dir = self._vo_dir, ai_dir = self._ai_dir)
                    def __new_draw__(img):
                        if isinstance(img, bytes):
                            display.__show__.set(img)
                    display.__draw__ = __new_draw__
                else:
                    if exp_gain != (-1, -1):
                        self.cam.exp_gain(exp_gain[0], exp_gain[1])
                    else:
                        self.cam.cfg(size[0], size[1])
                        print('[camera] config input size(%d, %d)' %
                            (self.width(), self.height()))

            def read(self, video_num=0, show=False, skip_frame=8):
                if self.cam == None:
                    self.config()
                    for i in range(skip_frame):
                        frame = self.cam.get(False)
                if self.cam: # allow bytes 240*240*3, bytes 224*224*3
                    frame = self.cam.get(show)
                    if len(frame) > 0:
                        return frame[video_num]
                return None

            def __del__(self):
                if self.cam:
                    self.cam = None

        camera = V831VivoMaixVideo()

    except Exception as e:
        # use libmaix on v831
        # from _maix_camera import V831Camera
        from _maix_camera import Camera

        class SpMaixVideo(MaixVideo):

            def __init__(self, source="/dev/videoX"):
                self.source = source
                super(SpMaixVideo, self).__init__()

            def config(self, size=None, video=0, horizontal=1, vertical=1):
                if size == None:
                    from maix import display
                    size = (display.width(), display.height())
                super(SpMaixVideo, self).config(size)
                print('[camera] config input size(%d, %d, %d)' %
                    (self.width(), self.height(), video))
                if self.cam:
                    del self.cam
                self.cam = Camera(self.width(), self.height(), video, horizontal, vertical)

            def read(self):
                if self.cam == None:
                    self.config()
                if self.cam:
                    ret, frame = self.cam.read()
                    if ret:
                        return frame  # bytes
                return None

            def __del__(self):
                if self.cam:
                    self.cam.close()
                    self.cam = None

        camera = SpMaixVideo()
except Exception as e:
    try:
        from cv2 import VideoCapture

        class CvMaixVideo(MaixVideo):

            def __init__(self, source=0):
                self.source = source
                super(CvMaixVideo, self).__init__()

            def config(self, size=(640, 480), source=None):
                if self.cam == None:
                    if source:
                        self.source = source
                    self.cam = VideoCapture(self.source)
                    super(CvMaixVideo, self).config(size)
                    print('[camera] config input size(%s, %d, %d)' %
                        (self.source, self.width(), self.height()))

            def read(self):
                if self.cam == None:
                    self.config()
                ret, frame = self.cam.read()
                if ret:
                    bgr = frame[..., ::-1]  # bgr2rgb
                    return bgr.tobytes()  # bytes
                return None

            def __del__(self):
                if self.cam:
                    self.cam.release()
                    self.cam = None

        camera = CvMaixVideo()
    except Exception as e:
        # camera = MaixVideo()
        raise e

''' # 20220531 dls fix it!!!!
def _del_config(*args, **kwargs):
    # or use camera.camera.config if you knew what you were doing.
    print("[camera.config] is deprecated, use [image.resize] for images.")
'''

# registered interface
capture = camera.capture
read = camera.read
config = camera.config
height = camera.height
width = camera.width
close = camera.close
