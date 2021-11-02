
import _maix_speech

def show_run_time(func):
    import timeit
    def time_get():
        elapsed_time = timeit.timeit(stmt=func, number=1)
        print(func, elapsed_time)
    return time_get


@show_run_time
def unit_test():
    import _maix_speech


# <function unit_test at 0x199ebd8> 0.011696916999881068
unit_test()

if __name__ == "__main__":

    unit_test()
