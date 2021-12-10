# This file is part of MaixUI
# Copyright (c) sipeed.com 2018, 2021 
#
# Licensed under the MIT license:
#   http://www.opensource.org/licenses/mit-license.php
#

def get_host_ip():
    try:
        import socket
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(('8.8.8.8', 80))
        ip = "IP: " + s.getsockname()[0]
    except Exception as e:
        ip = "IP does not exist"
    finally:
        s.close()
    return ip

class bind:

    def __init__(self, vi_ui_base) -> None:
        self.result = None
        self.vi_ui_base = vi_ui_base

    def __enter__(self):
        self.vi_ui_base.on_get_vi += self.on_data
        self.vi_ui_base.on_set_ui += self.on_draw

        from PIL import Image
        self.logo = Image.open('./resc/logo.png')
        self.ip = get_host_ip

        return self.vi_ui_base

    def __exit__(self, exc_type, exc_val, exc_tb):
        try:
            self.vi_ui_base.on_get_vi -= self.on_data
            self.vi_ui_base.on_set_ui -= self.on_draw

            del self.logo
            del self.ip

            del self.vi_ui_base
            if exc_type != None:
                print(exc_type, exc_val, exc_tb)
        except Exception as e:
            print(e)
        
    def on_draw(self, draw):
        from .common import common_font
        draw.paste(self.logo, (50, 40, 50 + self.logo.size[0], 40 + self.logo.size[1]), self.logo)
        draw.text((25, 195), get_host_ip(), (0xbd, 0xc3, 0xc7, 0x70), common_font)

    def on_data(self, rgb24):
        pass
