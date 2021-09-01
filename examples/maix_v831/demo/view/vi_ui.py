# This file is part of MaixUI
# Copyright (c) sipeed.com 2018, 2021 
#
# Licensed under the MIT license:
#   http://www.opensource.org/licenses/mit-license.php
#

from core import events

class vi_ui(events.Events):

    def __init__(self) -> None:
        super(vi_ui, self).__init__()

    def __set_ui(self, draw):
        import time
        draw.text((10, 10), u'time:%s' % time.time(), (255, 0, 0))

    def __get_vi(self, rgb24):
        print('get_vi', len(rgb24))

    def __enter__(self):
        from maix import camera
        camera.config(size=(240, 240))
        self.on_get_vi += self.__get_vi
        self.on_set_ui += self.__set_ui
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        try:
            self.on_get_vi -= self.__get_vi
            self.on_set_ui -= self.__set_ui
            if exc_type != None:
                print(exc_type, exc_val, exc_tb)
        except Exception as e:
            print(e)
        
    def event(self):
        from maix import camera, display
        self.on_get_vi(camera.read())
        self.on_set_ui(display.get_draw())
        self.on_event(self)
        display.show()

    def unit_test(self):
        with self as task:
            for i in range(50):
                task.event()

base = vi_ui()

if __name__ == '__main__':
    base.unit_test()
