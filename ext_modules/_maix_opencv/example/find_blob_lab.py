#!/usr/bin/env python

# vision.find_blob(pic,hsv,tilt=0)函数说明
# 功能:在图片pic中寻找符合hsv颜色区域的色块
# 输入：pic，240*240尺寸的图片bytes数据
#     hsv,hsv阈值数组，前三个是最小值，后三个是最大值
#     tilt斜度选项
#tilt = 0斜度选项 返回值：[{'x': 232, 'y': 32, 'w': 8, 'h': 49, 'pixels': 200, 'cx': 236, 'cy': 56}]，色块列表
#x,y,w,h 色块外框信息
#pixels 色块面积
#cx 色框中心x
#cy 色框中心y
#tilt = 1斜度选项 返回值： [{'x': 236, 'y': 206, 'w': 4, 'h': 7, 'pixels': 16, 'cx': 238, 'cy': 209, 'tilt_Rect': (236.0, 212.0, 236.0, 206.0, 239.0, 206.0, 239.0, 212.0), 'rotation': -1.570796251296997}]
#tilt_Rect 斜框四个顶点
#rotation 斜框斜度

from maix import vision
from maix import camera
from PIL import Image, ImageDraw
from maix import display
import time
test = [(13,11,-91,54,48,-28)]  #蓝色
def find_blob():
    tmp = camera.read()
    if tmp:
        t = time.time()
        ma = vision.find_blob_lab(tmp,test)
        t = time.time() - t
        print("-- forward time: {}s".format(t))
        # print(ma)
        draw = display.get_draw()
        if ma:
            for i in ma:
                draw.rectangle((i["x"], i["y"], i["x"] + i["w"], i["y"] + i["h"]), outline='red', width=1)
            display.show()
        else:
            display.clear()
    else:
        print('tmp err')



if __name__ == "__main__":
    import signal
    def handle_signalm(signum,frame):
        print("father over")
        exit(0)
    signal.signal(signal.SIGINT,handle_signalm)
    camera.config(size=(224,224))
    while True:
        find_blob()

# $env/bin/
# $env/python3/include
# $env/python3/lib


# PYTHONPATH="$env/lib/python3.9/site-packages"
# # PYTHONPATH="$env/usr/lib/python3.8/site-packages:$basepath//ipkg-install//usr/lib/python3.8/site-packages" \


# CC="$env/bin/aarch64-linux-gnu-gcc" \
# CCSHARED="$env/bin/aarch64-linux-gnu-gcc -fpic" \
# CXX="$env/bin/aarch64-linux-gnu-g++" \
# LD="$env/bin/aarch64-linux-gnu-gcc" \
# LDSHARED="$env/bin/aarch64-linux-gnu-gcc -shared" \
# CFLAGS="-I$env/python3/include -I$env/python3/include/python3.9" \
# CPPFLAGS="-I$env/python3/include -I$env/python3/include/python3.9" \
# LDFLAGS="-L$env/python3/lib -L$env/python3/lib/python3.9 -lpython3.9" \
# _PYTHON_HOST_PLATFORM="linux_aarch64" \
# PYTHONDONTWRITEBYTECODE=1 PYTHONOPTIMIZE="" \
# _python_sysroot="./$env/" \
# _python_prefix="/usr" \
# _python_exec_prefix="/usr" \
# PYTHONPATH="$env/host/lib/python3.9/site-packages" \
# $env/host/bin/python3.9 $@



# function pip3.8()
# {
#   CC="$tools/arm-openwrt-linux-muslgnueabi-gcc" \
#   CCSHARED="$tools/arm-openwrt-linux-muslgnueabi-gcc -fpic" \
#   CXX="$tools/arm-openwrt-linux-muslgnueabi-g++" \
#   LD="$tools/arm-openwrt-linux-muslgnueabi-gcc" \
#   LDSHARED="$tools/arm-openwrt-linux-muslgnueabi-gcc -shared" \
#   CFLAGS="-Os -pipe -march=armv7-a -mtune=cortex-a7 -mfpu=neon -fno-caller-saves -Wno-unused-result -mfloat-abi=hard  -Wformat -Werror=format-security -fstack-protector -D_FORTIFY_SOURCE=1 -Wl,-z,now -Wl,-z,relro" \
#   CPPFLAGS="-I$env/usr/include -I$header/usr/include -I$header/include -I$header/include/fortify -I$env/usr/include/python3.8" \
#   LDFLAGS="-L$env/usr/lib -L$env/lib -L$header/usr/lib -L$header/lib -znow -zrelro -lpython3.8" \
#   _PYTHON_HOST_PLATFORM="linux-armv7l" \
#   PYTHONPATH="$env/usr/lib/python3.8/site-packages:$basepath/ipkg-install//usr/lib/python3.8/site-packages" \
#   PYTHONDONTWRITEBYTECODE=1 PYTHONOPTIMIZE="" \
#   _python_sysroot="./$env/" \
#   _python_prefix="/usr" \
#   _python_exec_prefix="/usr" \
#   $env/host/bin/python3.8 -c "import re;import sys;from pip._internal.cli.main import main;sys.argv[0] = re.sub(r'(-script\.pyw|\.exe)?$', '', sys.argv[0]);sys.exit(main());" $@
# }

# cpython-39-x86_64-linux-gnu.so