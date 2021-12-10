import os

root = os.getcwd() + "/"

def unit_test():
    # from maix import display, camera
    import time
    import _maix_speech
    asr = _maix_speech.Asr()
    # asr.open("hw:0,0", "/root/test_files/am_3332/am_3332_192")
    # asr.open("/root/test_files/1.2.wav", "/root/test_files/cnnctc/cnnctc_3332_192", 2)
    # asr.open("hw:0,0", "/root/test_files/cnnctc/cnnctc_3332_192")
    asr.open("hw:0,0", root + "aipu_l192_flow_66.bin")
    # asr.open("1.2.wav", root + "./aipu_l192_flow_66.bin")
    asr.clear()

    def asr_dig(info):
      print('asr_dig', info)
    asr.set_dig(640, asr_dig)

    keywords = [
      ["qi1 ba1 jiu3", 0.1],
      ["yi1 nian2", 0.2],
      ["san1 si4 wu3", 0.3],
    ]
    similars = [
      ["xiao3", "xiang3"],
      ["jing1", "xin1", "ting1", "jin1"],
    ]
    def asr_kws(keys):
      tmp = max(keys)
      print('asr_kws %s %s' % (keys.index(tmp), str(tmp)))
    asr.set_kws(keywords, similars, asr_kws)

    def asr_lvcsr(pnys, words):
      print('asr_lvcsr', pnys, words)
    asr.set_lvcsr(
      root + "lmS/lg_6m.sfst",
      root + "lmS/lg_6m.sym",
      root + "lmS/phones.bin",
      root + "lmS/words_utf.bin",
      8.0, 10.0, 0.5, 0, asr_lvcsr)

    try:
      for i in range(10):
        # display.show(camera.capture())
        # t.clear()
        asr.run()
    except Exception as e:
      print(e)
    # asr.set_dig(0, lambda:print('exit'))


if __name__ == "__main__":

    unit_test()
