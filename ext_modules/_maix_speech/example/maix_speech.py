
def show_run_time(func):
    import timeit

    def time_get():
        elapsed_time = timeit.timeit(stmt=func, number=1)
        print(func, elapsed_time)
    return time_get


@show_run_time
def code_unit_test():
    import _maix_speech
    t = _maix_speech.Asr()
    t.open()
    t.clear()
    t.run()
    t.clear()
    t.exit()

# unit_test()


@show_run_time
def unit_test():
    # from maix import display, camera
    import time
    import _maix_speech
    asr = _maix_speech.Asr()
    # asr.open("hw:0,0", "/root/test_files/am_3332/am_3332_192")

    # asr.open("/root/test_files/1.2.wav", "/root/test_files/cnnctc/cnnctc_3332_192", 2)
    # asr.open("hw:0,0", "/root/test_files/cnnctc/cnnctc_3332_192")
    asr.open("hw:0,0", "/root/test_files/aipu_l192_flow_66.bin")
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
      "/root/test_files/lmS/lg_6m.sfst",
      "/root/test_files/lmS/lg_6m.sym",
      "/root/test_files/lmS/phones.bin",
      "/root/test_files/lmS/words_utf.bin",
      8.0, 10.0, 0.5, 0, asr_lvcsr)

    for i in range(5):
      # display.show(camera.capture())
      # t.clear()
      asr.run(1)

    # asr.set_dig(0, lambda:print('exit'))


if __name__ == "__main__":

    unit_test()
