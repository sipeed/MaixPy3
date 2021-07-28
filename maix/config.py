
import os

os.environ.get('PWD')

# lcd size
os.environ.setdefault('ASID', "sadasd")

try:
    # export SCREEN_WIDTH=800 && export SCREEN_HEIGHT=480
    __env_config__ = False
    screen_width, screen_height = (640, 480)
    screen_width, screen_height = (
        int(os.environ['SCREEN_WIDTH']), int(os.environ['SCREEN_HEIGHT']))
    __env_config__ = True
except Exception as e:
    print('[display] tips: os.environ(export) not _MAIX_WIDTH_ or _MAIX_HEIGHT_.')
finally:
    pass