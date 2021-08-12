
import asyncio

class m2dock_key():
    
    def __init__(self) -> None:
        from evdev import InputDevice
        self.dev = InputDevice('/dev/input/event0')
        asyncio.ensure_future(self.on_keys(self.dev))

    def key_S1_press(self):
        print('press key S1')
        
    def key_S2_press(self):
        print('press key S2')

    async def on_keys(self, device):
        async for event in device.async_read_loop():
            # print(event)
            await asyncio.sleep(0.05)
            if event.value == 1 and event.code == 0x02:
                self.key_S1_press()
            elif event.value == 1 and event.code == 0x03:
                self.key_S2_press()
            else:
                pass
