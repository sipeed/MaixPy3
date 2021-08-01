from typing import Tuple
from .video import MaixVideo

camera = MaixVideo()

try:
    try:
        # use mpp vivo on v831
        from _maix_vivo import _v83x_vivo
        class V831VivoMaixVideo(MaixVideo):
            def __init__(self, source="/v831"):
                self.source = source
                self.cam = None

            def config(self, size=(240, 240), _vo_dir=0, _ai_dir=0):
                if self.cam == None:
                    super(V831VivoMaixVideo, self).__init__(size)
                    try:
                        from PIL import Image
                        from maix import display
                        self.cam = _v83x_vivo(display.__width__, display.__height__, self.width(), self.height(), vo_dir = _vo_dir, ai_dir = _ai_dir)
                        display.__display__ = Image.new("RGBA", (display.__width__, display.__height__), "#00000000")
                        display.__fastview__ = self.cam
                        def __new_draw__(img):
                            if isinstance(img, bytes):
                                display.__fastview__.set(img)
                            elif isinstance(img, Image.Image):
                                if (img.mode == "RGB"):
                                    tmp = Image.new("RGBA", (display.__width__, display.__height__), "#00000000")
                                    tmp.paste(img)
                                    display.__fastview__.set(tmp.tobytes())
                                elif (img.mode == "RGBA"):
                                    display.__fastview__.set(img.tobytes())
                                    display.__display__ = Image.new("RGBA", (display.__width__, display.__height__), "#00000000")
                                else:
                                    print("unknown image mode")

                        display.__draw__ = __new_draw__
                    except ModuleNotFoundError as e:
                        pass
                    except Exception as e:
                        pass

                    print('[camera] config input size(%d, %d)' %
                        (self.width(), self.height()))

            def read(self, video_num=1, show=True):
                if self.cam == None:
                    print('[camera] run config(size=(w, h)) before capture.')
                    self.config()
                if self.cam:
                    frame = self.cam.get(show)
                    if len(frame) == 2:
                        return frame[video_num]  # bytes 240*240*3, bytes 224*224*3
                return None

            def __del__(self):
                if self.cam:
                    self.cam = None
                    
        camera = V831VivoMaixVideo()

    except Exception as e:
        # use libmaix on v831
        from _maix_camera import V831Camera

        class V831MaixVideo(MaixVideo):

            def __init__(self, source="/v831"):
                self.source = source
                self.cam = None

            def config(self, size=(480, 360)):
                if self.cam == None:
                    super(V831MaixVideo, self).__init__(size)
                    self.cam = V831Camera(self.width(), self.height(), 0)
                    import time
                    time.sleep(0.2) # wait init
                    print('[camera] config input size(%d, %d)' %
                        (self.width(), self.height()))

            def read(self):
                if self.cam == None:
                    print('[camera] run config(size=(w, h)) before capture.')
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

        camera = V831MaixVideo()
except Exception as e:
    pass

try:
    from cv2 import VideoCapture

    class CvMaixVideo(MaixVideo):

        def __init__(self, source=0):
            super(CvMaixVideo, self).__init__((640, 480))
            self.source = source
            self.cam = VideoCapture(0)

        def read(self):
            ret, frame = self.cam.read()
            if ret:
                bgr = frame[..., ::-1]  # bgr2rgb
                return bgr.tobytes()  # bytes
            return None

        def __del__(self):
            self.cam.release()

    camera = CvMaixVideo()
except Exception as e:
    pass

# registered interface
capture = camera.capture
read = camera.read
config = camera.config
height = camera.height
width = camera.width
close = camera.close

if __name__ == '__main__':
    camera.config((224, 224))
    from maix import display
    display.clear((255, 0, 0))
    display.show(camera.capture())
    # tmp = camera.read()
    # import _maix
    # frame = _maix.rgb2jpg(camera.rgbbuf, camera.width, camera.height)
    # print(len(frame) // 1024, camera.width, camera.height)
    # from PIL import Image
    # from io import BytesIO
    # img = Image.open(BytesIO(frame))
    # img.show()
