from .video import MaixVideo

camera = MaixVideo()

try:
    # use libmaix on v831
    from _maix_camera import V831Camera

    class V831MaixVideo(MaixVideo):

        def __init__(self, source="/v831", size=(480, 360)):
            super(V831MaixVideo, self).__init__(size)
            self.source = source
            self.cam = V831Camera(self.width, self.height)

        def read(self):
            ret, frame = self.cam.read()
            if ret:
                return frame # bytes
            return None
        
        def __del__(self):
            self.cam.close()

    camera = V831MaixVideo()
except Exception as e:
    pass

try:
    from cv2 import VideoCapture

    class CvMaixVideo(MaixVideo):

        def __init__(self, source=0, size=(640, 480)):
            super(CvMaixVideo, self).__init__(size)
            self.source = source
            self.cam = VideoCapture(0)

        def read(self):
            ret, frame = self.cam.read()
            if ret:
                bgr = frame[..., ::-1]  # bgr2rgb
                return bgr.tobytes() # bytes
            return None

        def __del__(self):
            self.cam.release()

    camera = CvMaixVideo()
except Exception as e:
    pass

# registered interface
capture = camera.capture
read = camera.read

if __name__ == '__main__':
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
 