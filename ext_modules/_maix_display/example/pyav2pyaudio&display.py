import pyaudio
from maix import display, camera
import av
# camera.config()

display.show(camera.capture())

try:
    # recommend flv
    # ffmpeg -r 30 -i bad_apple.mp4 -s 240x240 output.mp4
    # adb push ./output.mp4 /mnt/UDISK/
    # adb push ./test.py / && adb shell 'python ./test.py'
    path_to_video = '/mnt/UDISK/output.mp4'
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
except Exception as e:
    print(e)
finally:
    ao.stop_stream()
    ao.close()
    p.terminate()
