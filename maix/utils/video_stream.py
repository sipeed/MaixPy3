
import sys
import _maix
from maix import camera, display

class VideoStream:
    FRAME_HEADER_LENGTH = 5
    DEFAULT_IMAGE_SHAPE = (380, 280)
    VIDEO_LENGTH = 500
    DEFAULT_FPS = 30 # 24 fps

    # if it's present at the end of chunk,
    # it's the last chunk for current jpeg (end of frame)
    JPEG_EOF = b'\xff\xd9'

    def __init__(self, file_path: str):
        # for simplicity, mjpeg is assumed to be on working directory
        # print(file_path)
        if file_path == '/dev/display':
            self.TYPE = 0 # display
            self._stream = None
            __class__.VIDEO_LENGTH = sys.maxsize
        elif file_path == '/dev/camera':
            self._stream = camera
            self.TYPE = 1 # camera
            __class__.VIDEO_LENGTH = sys.maxsize
        else:
            self._stream = open(file_path, 'rb')
            self.TYPE = 2 # file
        # frame number is zero-indexed
        # after first frame is sent, this is set to zero
        self.current_frame_number = -1

    def close(self):
        if self._stream:
            self._stream.close()

    def get_next_frame(self) -> bytes:
        # sample video file format is as follows:
        # - 5 digit integer `frame_length` written as 5 bytes, one for each digit (ascii)
        # - `frame_length` bytes follow, which represent the frame encoded as a JPEG
        # - repeat until EOF
        if self.TYPE == 0:
            # import random
            # display.clear((random.randint(10, 100), 0, 0))
            frame = _maix.rgb2jpg(display.tobytes(), display.__width__, display.__height__)
            frame_length = len(frame)
        elif self.TYPE == 1:
            # import io
            # img = self._stream.capture()
            # tmp = io.BytesIO()
            # img.save(tmp, format='jpeg', quality=75)
            # frame = tmp.getvalue()
            frame = _maix.rgb2jpg(self._stream.read(), self._stream.width, self._stream.height)
            frame_length = len(frame)
        elif self.TYPE == 2:
            try:
                frame_length = self._stream.read(self.FRAME_HEADER_LENGTH)
            except ValueError:
                raise EOFError
            frame_length = int(frame_length.decode())
            frame = self._stream.read(frame_length)
        self.current_frame_number += 1
        return bytes(frame)
