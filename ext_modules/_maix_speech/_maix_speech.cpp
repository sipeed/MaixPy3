
#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <vector>
#include <list>
#include <array>
#include <map>
#include <set>
#include <typeinfo>

extern "C"
{
#define VERSION_RELEASE 1
#define DR_WAV_IMPLEMENTATION
#include "opts.h"
#include "dr_wav.h"
#include "ms_asr.h"
  opts_t opts;
}

using namespace std;

namespace py = pybind11;

#define debug_line printf("%s:%d %s %s %s \r\n", __FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__)

struct _maix_asr
{
  static bool init;
  static int tick;

  static void my_rawcb(void *data, int len) //data struct: pnyp_t data[len][BEAM_CNT]
  {
    pnyp_t *res = (pnyp_t *)data;
    printf("===================================\n");
    for (int t = 0; t < len; t++)
    {
      pnyp_t *pp = res + BEAM_CNT * t;
      if (1)
      {
        printf("T=%04d ====:", tick);
        tick += 1;
        for (int i = 0; i < 3; i++)
        { //BEAM_CNT
          printf("  %4d %-6s: %.3f;", pp[i].idx, am_vocab[pp[i].idx], ((float)(pp[i].p)));
        }
      }
      printf("\n");
    }
    printf("####\n");
    return;
  }

  static void my_digitcb(void *data, int len)
  {
    char *digit_res = (char *)data;
    char digit_res1[32];
    memset(digit_res1, ' ', 32);
    digit_res1[32 - 1] = 0;
    memcpy(digit_res1, digit_res, strlen(digit_res));
    printf("digit_res1: %s\n", digit_res);
    return;
  }

  static void my_lvcsrcb(void *data, int len)
  {
    char *words = ((char **)data)[0];
    char *pnys = ((char **)data)[1];

    printf("PNYS: %s\nHANS: %s\n", pnys, words);
    return;
  }

  static void my_kwscb(void *data, int len)
  {
    float *p = (float *)data;
    float maxp = -1;
    int maxi = 0;
    for (int i = 0; i < len; i++)
    {
      printf("\tkw%d: %.3f;", i, p[i]);
      if (p[i] > maxp)
      {
        maxp = p[i];
        maxi = i;
      }
    }
    printf("\n");
    return;
  }

  static void set_dig(int flag)
  {
    size_t decoder_args[10];
    int res = 0;
    if (flag)
    {                        //init
      decoder_args[0] = 640; //blank_t ms
      res = ms_asr_decoder_cfg(DECODER_DIG, my_digitcb, &decoder_args, 1);
      if (res != 0)
      {
        printf("DECODER_DIG init error!\n");
      };
    }
    else
    {
      ms_asr_decoder_cfg(DECODER_DIG, NULL, NULL, 0);
    }
    return;
  }

  static void set_kws(int flag)
  {
    static char *my_kw_tbl[3] = {
        (char *)"xiao3 ai4 tong2 xue2",
        (char *)"tian1 mao1 jing1 ling2",
        (char *)"tian1 qi4 zen3 me yang4"};

    static float my_kw_gate[3] = {
        0.1, 0.1, 0.1};

    size_t decoder_args[10];
    int res = 0;
    if (flag)
    { //init
      decoder_args[0] = (size_t)my_kw_tbl;
      decoder_args[1] = (size_t)my_kw_gate;
      decoder_args[2] = 3;
      decoder_args[3] = 1; //auto similar //自动去除音调 近音处理
      printf("qqqq");
      res = ms_asr_decoder_cfg(DECODER_KWS, my_kwscb, &decoder_args, 3);
      printf("aaaa");
      if (res != 0)
      {
        printf("DECODER_KWS init error!\n");
        goto out1;
      };
      char *similar_pnys0[1] = {(char *)"xiang3"}; //每个最多注册10个近音词
      ms_asr_kws_reg_similar((char *)"xiao3", similar_pnys0, 1);
      char *similar_pnys1[3] = {(char *)"xin1", (char *)"ting1", (char *)"jin1"};
      ms_asr_kws_reg_similar((char *)"jing1", similar_pnys1, 3);
    }
    else
    {
      ms_asr_decoder_cfg(DECODER_KWS, NULL, NULL, 0);
    }
  out1:
    return;
  }

  static void set_lvcsr(int flag)
  {
    size_t decoder_args[10];
    int res = 0;
    if (flag)
    { //init

      decoder_args[0] = (size_t)"/root/test_files/lmS/lg_6m.sfst";
      decoder_args[1] = (size_t)"/root/test_files/lmS/lg_6m.sym";
      decoder_args[2] = (size_t)"/root/test_files/lmS/phones.bin";
      decoder_args[3] = (size_t)"/root/test_files/lmS/words_utf.bin";
      {
        float tmp = (8.0);
        memcpy(&decoder_args[4], &tmp, sizeof(float));
      }
      {
        float tmp = (10.0);
        memcpy(&decoder_args[5], &tmp, sizeof(float));
      }
      {
        float tmp = (0.5);
        memcpy(&decoder_args[6], &tmp, sizeof(float));
      }
      decoder_args[7] = (size_t)0; //printf("#\n");
      res = ms_asr_decoder_cfg(DECODER_LVCSR, my_lvcsrcb, &decoder_args, 0);
      if (res != 0)
      {
        printf("DECODER_LVCSR init error!\n");
      };
    }
    else
    {
      ms_asr_decoder_cfg(DECODER_LVCSR, NULL, NULL, 0);
    }
    return;
  }

  void clear()
  {
    if (!init)
      return;
    ms_asr_clear();
  }

  bool open()
  {
    if (init)
      return init;

    am_args_t am_args = {"/root/test_files/cnnctc/cnnctc_3332_192", 192, 6, 6, CN_PNYTONE, 1};

    int res = ms_asr_init(DEVICE_WAV, "/root/test_files/1.2.wav", &am_args, 0x00);

    if (res != 0)
    {
      printf("ms_asr_init error!\n");
      return -1;
    }

    // if (opts.do_raw)
    {
      res = ms_asr_decoder_cfg(DECODER_RAW, my_rawcb, NULL, 0);
      if (res == 0)
      {
        // if (opts.do_dig)
        set_dig(1);
        // if (opts.do_kws)
        set_kws(1);
        // if (opts.do_lvcsr)
        set_lvcsr(1);
        init = true;
      }
    }
    return init;
  }

  bool run()
  {
    if (!init)
      return false;
    int frames = ms_asr_run(1); //1 frame default 768ms(am_xxyy_192)
    if (frames < 1)
    {
      printf("run out\n");
      return false;
    }
    return true;
  }

  void exit()
  {
    if (init)
    {
      init = false;
      ms_asr_deinit();
    }
  }

  _maix_asr()
  {
  }

  ~_maix_asr()
  {
    exit();
  }
};

bool _maix_asr::init = false;
int _maix_asr::tick = 0;

PYBIND11_MODULE(_maix_speech, m)
{
  m.def("test_callback1", [](const py::object &func)
        { return func(); });
  m.def("test_callback2", [](const py::object &func)
        { return func("Hello", 'x', true, 5); });
  m.def("test_callback3", [](const std::function<int(int)> &func)
        { return "func(43) = " + std::to_string(func(43)); });
  m.def("test_callback4", []() -> std::function<int(int)> { return [](int i)
                                                            { return i + 1; }; });
  m.def("test_callback5", []()
        { return py::cpp_function([](int i)
                                  { return i + 1; },
                                  py::arg("number")); });

  pybind11::class_<_maix_asr>(m, "Asr")
      .def(pybind11::init<>())
      .def("open", &_maix_asr::open)
      .def("run", &_maix_asr::run)
      .def("exit", &_maix_asr::exit)
      .def("clear", &_maix_asr::clear);
}
