#!/usr/bin/env python
import time

class BME280:

    def __init__(self,
                 mode=1,
                 address=0x76,
                 i2c_dev=None,
                 **kwargs):
        import struct
        # Check that mode is valid.
        if mode not in [1, 2, 3, 4, 5]:
            raise ValueError(
                'Unexpected mode value {0}. Set mode to one of '
                'BME280_ULTRALOWPOWER, BME280_STANDARD, BME280_HIGHRES, or '
                'BME280_ULTRAHIGHRES'.format(mode))
        self._mode = mode
        self.address = address
        # if i2c is None:
        # raise ValueError('An I2C object is required.')
        from maix import i2c
        self.i2c = i2c.I2C(i2c_dev, address)

        # load calibration data
        dig_88_a1 = self.i2c.read(0x88, 26)
        dig_e1_e7 = self.i2c.read(0xE1, 7)
        self.dig_T1, self.dig_T2, self.dig_T3, self.dig_P1, \
            self.dig_P2, self.dig_P3, self.dig_P4, self.dig_P5, \
            self.dig_P6, self.dig_P7, self.dig_P8, self.dig_P9, \
            _, self.dig_H1 = struct.unpack("<HhhHhhhhhhhhBB", dig_88_a1)
        print(dig_e1_e7)
        self.dig_H2, self.dig_H3 = struct.unpack("<hB", dig_e1_e7[:3])
        e4_sign = struct.unpack_from("<b", dig_e1_e7, 3)[0]
        self.dig_H4 = (e4_sign << 4) | (dig_e1_e7[4] & 0xF)

        e6_sign = struct.unpack_from("<b", dig_e1_e7, 5)[0]
        self.dig_H5 = (e6_sign << 4) | (dig_e1_e7[4] >> 4)

        self.dig_H6 = struct.unpack_from("<b", dig_e1_e7, 6)[0]

        self.i2c.write(0xF4, bytes([0x3F]))
        self.t_fine = 0

        from array import array
        # temporary data holders which stay allocated
        self._l1_barray = bytearray(1)
        # self._l8_barray = bytearray(8)
        self._l3_resultarray = array("i", [0, 0, 0])

    def read_raw_data(self, result):
        """ Reads the raw (uncompensated) data from the sensor.
            Args:
                result: array of length 3 or alike where the result will be
                stored, in temperature, pressure, humidity order
            Returns:
                None
        """

        self._l1_barray[0] = self._mode
        self.i2c.write(0xF2, bytes(self._l1_barray))
        self._l1_barray[0] = self._mode << 5 | self._mode << 2 | 1
        self.i2c.write(0xF4, bytes(self._l1_barray))

        sleep_time = 1250 + 2300 * (1 << self._mode)
        sleep_time = sleep_time + 2300 * (1 << self._mode) + 575
        sleep_time = sleep_time + 2300 * (1 << self._mode) + 575
        # time.sleep(sleep_time / 1000)  # Wait the required time

        # burst readout from 0xF7 to 0xFE, recommended by datasheet
        self._l8_barray = self.i2c.read(0xF7, 8)
        readout = self._l8_barray
        # pressure(0xF7): ((msb << 16) | (lsb << 8) | xlsb) >> 4
        raw_press = ((readout[0] << 16) | (readout[1] << 8) | readout[2]) >> 4
        # temperature(0xFA): ((msb << 16) | (lsb << 8) | xlsb) >> 4
        raw_temp = ((readout[3] << 16) | (readout[4] << 8) | readout[5]) >> 4
        # humidity(0xFD): (msb << 8) | lsb
        raw_hum = (readout[6] << 8) | readout[7]

        result[0] = raw_temp
        result[1] = raw_press
        result[2] = raw_hum

    def read_compensated_data(self, result=None):
        """ Reads the data from the sensor and returns the compensated data.
            Args:
                result: array of length 3 or alike where the result will be
                stored, in temperature, pressure, humidity order. You may use
                this to read out the sensor without allocating heap memory
            Returns:
                array with temperature, pressure, humidity. Will be the one from
                the result parameter if not None
        """
        self.read_raw_data(self._l3_resultarray)
        raw_temp, raw_press, raw_hum = self._l3_resultarray
        # temperature
        var1 = ((raw_temp >> 3) - (self.dig_T1 << 1)) * (self.dig_T2 >> 11)
        var2 = (((((raw_temp >> 4) - self.dig_T1) *
                  ((raw_temp >> 4) - self.dig_T1)) >> 12) * self.dig_T3) >> 14
        self.t_fine = var1 + var2
        temp = (self.t_fine * 5 + 128) >> 8

        # pressure
        var1 = self.t_fine - 128000
        var2 = var1 * var1 * self.dig_P6
        var2 = var2 + ((var1 * self.dig_P5) << 17)
        var2 = var2 + (self.dig_P4 << 35)
        var1 = (((var1 * var1 * self.dig_P3) >> 8) +
                ((var1 * self.dig_P2) << 12))
        var1 = (((1 << 47) + var1) * self.dig_P1) >> 33
        if var1 == 0:
            pressure = 0
        else:
            p = 1048576 - raw_press
            p = (((p << 31) - var2) * 3125) // var1
            var1 = (self.dig_P9 * (p >> 13) * (p >> 13)) >> 25
            var2 = (self.dig_P8 * p) >> 19
            pressure = ((p + var1 + var2) >> 8) + (self.dig_P7 << 4)

        # humidity
        h = self.t_fine - 76800
        h = (((((raw_hum << 14) - (self.dig_H4 << 20) -
                (self.dig_H5 * h)) + 16384)
              >> 15) * (((((((h * self.dig_H6) >> 10) *
                            (((h * self.dig_H3) >> 11) + 32768)) >> 10) +
                          2097152) * self.dig_H2 + 8192) >> 14))
        h = h - (((((h >> 15) * (h >> 15)) >> 7) * self.dig_H1) >> 4)
        h = 0 if h < 0 else h
        h = 419430400 if h > 419430400 else h
        humidity = h >> 12

        if result:
            result[0] = temp
            result[1] = pressure
            result[2] = humidity
            return result
        from array import array
        return array("i", (temp, pressure, humidity))

    @property
    def values(self):
        """ human readable values """

        t, p, h = self.read_compensated_data()

        p = p // 256
        pi = p // 100
        pd = p - pi * 100

        hi = h // 1024
        hd = h * 100 // 1024 - hi * 100
        return ("{}C".format(t / 100), "{}.{:02d}hPa".format(pi, pd),
                "{}.{:02d}%".format(hi, hd))

from maix import display, image
image.load_freetype("/root/sans_l.ttf")
from maix import pwm
import time

class BgImage:
    def __init__(self):
        from maix import display, image
        self.image = image.Image().new((240, 240), (0x2c, 0x3e, 0x50), "RGB")
        self.info = ""
        self.height = 40
        self.pwm_led = pwm.PWM(8)
        self.pwm_led.export()
        self.pwm_led.period = 20000000  # 表示 pwm 的周期，单位 ns
        self.pwm_led.duty_cycle = 200000  # 表示占空比，单位 ns
        self.pwm_led.enable = True        # 表示是否使能 pwm
        from maix import gpio
        self.led = gpio.gpio(13, "H", 1)
        from maix import i2c
        print(i2c.scan())
        self.bme=BME280(i2c_dev='/dev/i2c-2')
    def __del__(self):
        self.led.release()
        self.pwm_led.unexport()
    def draw_clear(self):
        self.height = 40
        self.image = image.Image().new((240, 240), (0x2c, 0x3e, 0x50), "RGB")
        self.draw_next(self.info)
    def draw_next(self, info=""):
        self.info = info
        self.image.draw_string(20, self.height, info, 2, (0xbd, 0xc3, 0xc7))
        self.height += 40
        if self.height > 220:
            self.draw_clear()
    def led_pwm_on(self):
        self.led.set_value(1)
        for i in range(20000000, 0, -1000000):
            self.pwm_led.duty_cycle = i
            import time
            time.sleep(0.01)
    def led_pwm_off(self):
        self.led.set_value(0)
        for i in range(0, 20000000, +1000000):
            self.pwm_led.duty_cycle = i
            import time
            time.sleep(0.01)
    def wsd(self):
        t, p, h = self.bme.read_compensated_data()
        # print(t, p, h)
        p = p // 256
        pi = p // 100
        pd = p - pi * 100
        p = pi + (pd / 100)
        hi = h // 1024
        hd = h * 100 // 1024 - hi * 100
        h = hi + (hd / 100)
        # res = "T{:.1f}\xb0C, P{:.2f}hPa, H{:.2f}%".format(t/100, p, h)
        return t/100, p, h

bg = BgImage()
bg.led_pwm_off()
bg.image.draw_string(40, 0, "进入语音助手", 2, (0xbd, 0xc3, 0xc7))
bg.draw_next("你好！什么事？")
display.show(bg.image)

import _maix_speech
asr = _maix_speech.Asr()
asr.open("hw:0,0", "/root/test_files/cnnctc/cnnctc_3332_192")
asr.clear()

def asr_lvcsr(pnys, words):
  # print('asr_lvcsr', pnys, words)
  result = ('lvcsr: %s %s' % (pnys, words))
  global bg, asr
  if '.' in pnys:
    print(pnys)
    bg.draw_next(pnys)
    asr.clear()
    if "开" in pnys:
      bg.draw_clear()
      bg.led_pwm_on()
      bg.draw_next("已打开")
    if "关" in pnys:
      bg.draw_clear()
      bg.led_pwm_off()
      bg.draw_next("在关了！")
    if "天气" in pnys:
      bg.draw_clear()
      t, p, h = bg.wsd()
      bg.draw_next("温度 {:.1f}℃".format(t))
      bg.draw_next("压强 {:.2f}hPa".format(p))
      bg.draw_next("湿度 {:.2f}%".format(h))

asr.set_lvcsr(
  "/root/test_files/lmS/lg_6m.sfst",
  "/root/test_files/lmS/lg_6m.sym",
  "/root/test_files/lmS/phones.bin",
  "/root/test_files/lmS/words_utf.bin",
  8.0, 10.0, 0.5, 0, asr_lvcsr)

for i in range(600):
  asr.run(1)
  display.show(bg.image)

asr.set_dig(0, lambda:print('exit'))
asr.set_kws([])
asr.set_lvcsr("")

