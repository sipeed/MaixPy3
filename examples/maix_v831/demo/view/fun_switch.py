class bind:
    fun  = 0
    def __init__(self, vi_ui_base) -> None:
        self.result = None
        self.vi_ui_base = vi_ui_base

    def __enter__(self):
        self.vi_ui_base.on_get_vi += self.on_data
        self.vi_ui_base.on_set_ui += self.on_draw


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
        if self.fun == 0:
            pass
        elif self.fun == 1:
            pass
        elif self.fun ==2:
            pass







        from .common import common_font
        from PIL import Image
        canvas = Image.new("RGB", (240, 240), "#2c3e50")
        draw.paste(canvas,(0,0))
        draw.paste(self.logo, (50, 40, 50 + self.logo.size[0], 40 + self.logo.size[1]), self.logo)
        draw.text((25, 195), get_host_ip(), (0xbd, 0xc3, 0xc7, 0xff), common_font)
    def on_data(self, rgb24):
        pass