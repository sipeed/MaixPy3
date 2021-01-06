
class VideoCapture():

    def __init__(self, source="/dev/video", size=(640, 480)):
        self.source = source
        self.width, self.height = size

    def write(self):
        pass  # for file

    def read(self):
        return b'\x00\xFF\x00' * (self.width * self.height)

    def capture(self):
        from PIL import Image
        return Image.frombytes("RGB", (self.width, self.height), self.read())

    def close(self):
        pass  # for file


camera = VideoCapture()

try:
    # use libmaix on v831
    from libmaix import Camera

    class V831VideoCapture(VideoCapture):

        def __init__(self, source="/sipeed/v831", size=(480, 360)):
            self.source = source
            self.width, self.height = size
            self.cam = Camera(self.width, self.height)

        def read(self):
            return self.cam.read()  # bytes

        def __del__(self):
            self.cam.close()

    camera = V831VideoCapture()
except Exception as e:
    pass

if __name__ == '__main__':
    '''
    def test_cv():
        import cv2
        cap = cv2.VideoCapture(0)
        print(cap)
        while True:
            ret, frame = cap.read()
            print(ret, type(frame))
            if frame is not None:
                cv2.imshow("Video", frame)
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break
        cap.release()
        cv2.destroyAllWindows()
    # test_cv()
    '''
    import display
    # display.clear((255, 0, 0))
    display.show(camera.capture())
