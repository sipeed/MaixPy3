#!/usr/bin/env python3

# This file is part of MaixUI
# Copyright (c) sipeed.com 2018, 2021
#
# Licensed under the MIT license:
#   http://www.opensource.org/licenses/mit-license.php
#

# adb shell 'mkdir demo'
# adb push ../demo /demo/
# adb push ./main.py /demo/ && adb shell 'cd /demo && python ./main.py'

from view import vi_ui, ui_home, ai_resnet
from driver import m2dock_key
m2dock_key = m2dock_key.m2dock_key

# def test_view():

#     task = [
#         # ui_home.bind(vi_ui.base),
#         ai_resnet.bind(vi_ui.base),
#         # vi_ui.base
#     ]

#     for t in task:
#         with t as task:
#             for i in range(100):
#                 task.event()

# class app_v83x(m2dock_key):

class app_v83x():
    fun_status = "home"


    def __init__(self) -> None:
        # asyncio.ensure_future(self.on_event())
        self.bind_task = ui_home.bind(vi_ui.base)
        self.work_task = self.bind_task
        
        import _thread
        _thread.start_new_thread(self.thread_event, ())

    def thread_event(self):
        import time
        while True:
            if self.work_task:
                with self.work_task as task:
                    while self.bind_task == self.work_task:
                        task.event()
                    self.work_task = self.bind_task

    # def key_S1_press(self):
    #     print("you")
    #     if self.fun_status == "home":
    #         self.fun_status = "choose"
    #         self.bind_task = fun_switch.bind(vi_ui.base)
    #     elif self.fun_status == "choose":
    #         pass


    #     # if 'ai_resnet' not in str(self.bind_task):
    #     #     self.bind_task = ai_resnet.bind(vi_ui.base)

    # def key_S2_press(self):
    #     print("zuo")
    #     if self.fun_status == "home":
    #         pass
    #     elif self.fun_status == "choose":
    #         pass
    #     # if 'ui_home' not in str(self.bind_task):
    #     #     self.bind_task = ui_home.bind(vi_ui.base)










    async def on_event(self):
        pass
        # while True:
        #     await asyncio.sleep(0.05)


if __name__ == '__main__':
    # test_view()
    import signal
    signal.signal(signal.SIGINT, lambda sig, stack_frame: exit(1))
    signal.signal(signal.SIGQUIT, lambda sig, stack_frame: exit(1))
    import asyncio
    import uvloop
    uvloop.install()
    app = app_v83x()
    loop = asyncio.get_event_loop()
    print(loop)
    loop.run_forever()
