
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

struct maix_asr
{
  static bool init;
  static int tick;

  static void asr_rawcb(void *data, int len) //data struct: pnyp_t data[len][BEAM_CNT]
  {
    // pnyp_t *res = (pnyp_t *)data;
    // printf("===================================\n");
    // for (int t = 0; t < len; t++)
    // {
    //   pnyp_t *pp = res + BEAM_CNT * t;
    //   if (1)
    //   {
    //     printf("T=%04d ====:", tick);
    //     tick += 1;
    //     for (int i = 0; i < 3; i++)
    //     { //BEAM_CNT
    //       printf("  %4d %-6s: %.3f;", pp[i].idx, am_vocab[pp[i].idx], ((float)(pp[i].p)));
    //     }
    //   }
    //   printf("\n");
    // }
    // printf("####\n");
    return;
  }

  static py::object py_asr_digit_cb;

  static void _asr_digit_cb(void *data, int len)
  {
    // printf("digit_res1: %s : %d %d\n", tmp.c_str(), tmp.length(), len);
    if (py_asr_digit_cb != py::none())
    {
      py_asr_digit_cb(string((const char *)data).c_str());
    }
  }

  void set_dig(int blank_ms, py::object cb)
  {
    if (blank_ms != 0)
    {
      size_t decoder_args[1] = {640};
      int res = ms_asr_decoder_cfg(DECODER_DIG, _asr_digit_cb, &decoder_args, 1);
      if (res == 0)
      {
        py_asr_digit_cb = cb;
      }
      else
      {
        py_asr_digit_cb = py::none();
      }
    }
    else
    {
      ms_asr_decoder_cfg(DECODER_DIG, NULL, NULL, 0);
    }
  }

  static py::object py_asr_kws_cb;

  static void _asr_kws_cb(void *data, int len)
  {
    if (py_asr_kws_cb != py::none())
    {
      py::list tmps;
      float *nums = (float *)data;
      for (int i = 0; i < len; i++)
        tmps.append(nums[i]);
      py_asr_kws_cb(tmps);
    }
  }

  void set_kws(py::list keywords, py::list similars, py::object cb, int auto_similar)
  {
    int kl = py::len(keywords);
    if (keywords != py::none() && kl > 0)
    {
      std::string asr_kw_str[kl];
      char *asr_kw_tbl[kl] = { 0 }; // .cast<std::string>()).c_str(); have q
      float asr_kw_gate[kl];

      for (int i = 0; i < kl; i++) {
        py::list tmp = static_cast<py::list>(keywords[i]);
        // asr_kw_tbl[i] = (char *)(tmp[0].cast<std::string>()).c_str();
        asr_kw_str[i] = tmp[0].cast<std::string>();
        asr_kw_tbl[i] = (char *)asr_kw_str[i].c_str();
        asr_kw_gate[i] = tmp[1].cast<float>();
        // printf("set_kws keywords %s %f \r\n", asr_kw_tbl[i], asr_kw_gate[i]);
      }

      // for (int i = 0; i < kl; i++) {
      //   printf("set_kws my_kw_tbl %s %f \r\n", asr_kw_tbl[i], asr_kw_gate[i]);
      // }

      size_t decoder_args[4];
      decoder_args[0] = (size_t)asr_kw_tbl;
      decoder_args[1] = (size_t)asr_kw_gate;
      decoder_args[2] = kl;
      decoder_args[3] = auto_similar; //自动去除音调 近音处理
      int res = ms_asr_decoder_cfg(DECODER_KWS, _asr_kws_cb, &decoder_args, 4);

      if (res == 0)
      {
        int sl = py::len(similars);
        if (sl > 0) {
          for (int i = 0; i < sl; i++) {
            py::list tmp = similars[i].cast<py::list>();
            int len = py::len(tmp) - 1;
            char *similar_pnys[len] = { }; // remove self and similar < 10
            for (int i = 0, sum = len > 10 ? 10 : len; i < sum; i++) {
                similar_pnys[i] = (char *)tmp[i + 1].cast<std::string>().c_str();
            }
            ms_asr_kws_reg_similar((char *)tmp[0].cast<std::string>().c_str(), similar_pnys, len);
          }
        }
        py_asr_kws_cb = cb;
      }
    }
    else
    {
      ms_asr_decoder_cfg(DECODER_KWS, NULL, NULL, 0);
    }
  }

  static py::object py_asr_lvcsr_cb;

  static void _asr_lvcsr_cb(void *data, int len)
  {
    if (py_asr_lvcsr_cb != py::none())
    {
      py_asr_lvcsr_cb(string((const char *)((char **)data)[0]), string((const char *)((char **)data)[1]));
    }
    return;
  }

  void set_lvcsr(string sfst_name, string sym_name, string phones_txt, string words_txt, float beam, float bg_prob, float scale, bool is_mmap, py::object cb)
  {
    if (sfst_name != "")
    {
      size_t decoder_args[8];
      decoder_args[0] = (size_t)sfst_name.c_str();
      decoder_args[1] = (size_t)sym_name.c_str();
      decoder_args[2] = (size_t)phones_txt.c_str();
      decoder_args[3] = (size_t)words_txt.c_str();
      memcpy(&decoder_args[4], &beam, sizeof(beam));
      memcpy(&decoder_args[5], &bg_prob, sizeof(bg_prob));
      memcpy(&decoder_args[6], &scale, sizeof(scale));
      decoder_args[7] = (size_t)is_mmap;
      int res = ms_asr_decoder_cfg(DECODER_LVCSR, _asr_lvcsr_cb, &decoder_args, 8);
      if (res == 0)
      {
        py_asr_lvcsr_cb = cb;
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

  bool open(string device_name, string model_name, int device_type, int model_in_len, int strip_l, int strip_r, int phone_type, int agc)
  {
    if (init)
      return init;

    string _device(device_name), _model(model_name);

    am_args_t am_args = {(char *)_model.c_str(), model_in_len, strip_l, strip_r, phone_type, agc};

    int res = ms_asr_init(device_type, (char *)_device.c_str(), &am_args, 0);

    if (res == 0)
    {
      res = ms_asr_decoder_cfg(DECODER_RAW, asr_rawcb, NULL, 0);
      if (res == 0)
      {
        init = true;
      }
    }

    return init;
  }

  bool run(int frame)
  {
    if (!init)
      return false;
    int frames = ms_asr_run(frame); //1 frame default 768ms(am_xxyy_192)
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

  maix_asr()
  {
  }

  ~maix_asr()
  {
    exit();
  }
};

bool maix_asr::init = false;
int maix_asr::tick = 0;
py::object maix_asr::py_asr_digit_cb;
py::object maix_asr::py_asr_kws_cb;
py::object maix_asr::py_asr_lvcsr_cb;

PYBIND11_MODULE(_maix_speech, m)
{
  pybind11::class_<maix_asr>(m, "Asr")
      .def(pybind11::init<>())
      .def("open", &maix_asr::open,
           py::arg("device_name"), py::arg("model_name"), py::arg("device_type") = DEVICE_MIC,
           py::arg("model_in_len") = 192, py::arg("strip_l") = 6, py::arg("strip_r") = 6,
           py::arg("phone_type") = CN_PNYTONE, py::arg("agc") = 1)
      .def("run", &maix_asr::run, py::arg("frame") = 1)
      .def("set_dig", &maix_asr::set_dig, py::arg("blank_ms") = 0, py::arg("cb") = py::none())
      // float beam, float bg_prob, float scale, bool is_mmap,
      .def("set_lvcsr", &maix_asr::set_lvcsr, py::arg("sfst_name") = "", py::arg("sym_name") = "",
           py::arg("phones_txt") = "", py::arg("words_txt") = "", py::arg("beam") = 8.0,
           py::arg("bg_prob") = 10.0, py::arg("scale") = 0.5, py::arg("is_mmap") = 0, py::arg("cb") = py::none())
      .def("set_kws", &maix_asr::set_kws,
           py::arg("keywords") = py::list(), py::arg("similars") = py::list(),
           py::arg("cb") = py::none(), py::arg("auto_similar") = true)
      .def("exit", &maix_asr::exit)
      .def("clear", &maix_asr::clear);
}
