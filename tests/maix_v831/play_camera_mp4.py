from maix import display, camera
import pyaudio
import av
import threading
import time

class funation:
    status = 0
    def __init__(self,device=None):
        self.event = self.run
        display.show(camera.capture())
        self.tim = time.time()
        self.device = device
    def __del__(self):
        print("paly exit")
    def play(self):
        try:
            # recommend flv
            # ffmpeg -r 30 -i bad_apple.mp4 -s 240x240 output.mp4
            # adb push ./output.mp4 /mnt/UDISK/
            # adb push ./test.py / && adb shell 'python ./test.py'
            path_to_video = '/root/bad_apple.mp4'
            container = av.open(path_to_video)
            ai_stream = container.streams.audio[0]
            vi_stream = container.streams.video[0]
            fifo = av.AudioFifo()
            p = pyaudio.PyAudio()
            ao = p.open(format=pyaudio.paFloat32, channels=2, rate=22050, output=True)
            for frame in container.decode(video=0, audio=0):
                if 'Audio' in repr(frame):
                    frame.pts = None
                    fifo.write(frame)
                    for frame in fifo.read_many(4096):
                        ao.write(frame.planes[0].to_bytes())
                if 'Video' in repr(frame):
                    display.show(bytes(frame.to_rgb().planes[0]))
                if self.device.funaction_status == -1:
                    ao.stop_stream()
                    ao.close()
                    p.terminate()
                    return
        except Exception as e:
            print(e)
        finally:
            ao.stop_stream()
            ao.close()
            p.terminate()
    def run(self):
        if self.status == 0:
            threading.Thread(target=self.play).start()
            self.status = 1
        time.sleep(0.1)


if __name__ == "__main__":
    import signal
    def handle_signal_z(signum,frame):
        print("APP OVER")
        exit(0)
    signal.signal(signal.SIGINT,handle_signal_z)
    start = funation()
    while True:
        start.event()
