# how to get rtsp

It need ptqt5 & maixpy3.

cd MaixPy3

- shell `python3 -c "from maix import rpycs; rpycs.start()"`

then run your rtsp client on pyqt or other.

- shell `python3 ./bin/rtsp_pyqt/main_client.py /dev/camera 192.168.0.156 18811 18813` on MaixPy3/bin/rtspc_pyqt.
