
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
    from maix import display, camera
    import time
    import _maix_speech
    t = _maix_speech.Asr()
    try:
      t.open()
      t.clear()
      for i in range(30):
        display.show(camera.capture())
        # t.clear()
        t.run()
        # time.sleep(0.1)
    finally:
      t.exit()


if __name__ == "__main__":

    unit_test()
