
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
extern "C" {
  #define DR_WAV_IMPLEMENTATION
  #include "opts.h"
  #include "dr_wav.h"
  #include "ms_asr.h"
  opts_t opts;
}
// using namespace cv;
using namespace std;

namespace py = pybind11;

#define debug_line printf("%s:%d %s %s %s \r\n", __FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__)

class _maix_asr
{
public:
  py::bytes test(py::bytes &rgb)
  {
    //   puts("test _maix_asr");
    return py::none();
  }

  _maix_asr()
  {
    am_args_t am_args = { };

    int res = ms_asr_init(2, "123", &am_args, 0);

  }

  ~_maix_asr()
  {
    ms_asr_deinit();
  }
};

PYBIND11_MODULE(_maix_speech, m)
{
  pybind11::class_<_maix_asr>(m, "Asr")
      .def(pybind11::init<>())
      .def("draw_ellipse", &_maix_asr::test)
      .def("draw_circle", &_maix_asr::test)
      .def("draw_line", &_maix_asr::test)
      .def("load_freetype", &_maix_asr::test);
}
