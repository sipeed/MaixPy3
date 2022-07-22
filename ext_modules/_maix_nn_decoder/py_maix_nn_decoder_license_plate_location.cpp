#include <iostream>
#include <sstream>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <vector>
#include <list>
#include <array>
#include <map>
#include <set>
#include <typeinfo>
#include "libmaix_nn.h"
#include "libmaix_nn_decoder.h"
#include "libmaix_nn_decoder_license_plate_location.h"


#define MAX_LEN 10
// #define ANCHOR_SIZE_NUM 4
// #define MIN_SIZE_LEN 10
#define debug_line printf("%s:%d %s %s %s \r\n", __FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__)

namespace py = pybind11;

typedef struct
{
  // PyObject* m_numpy;
  libmaix_nn_decoder_license_plate_location_config_t config;
  libmaix_nn_decoder_t *decoder;
  bool init;
} DecoderLPLObject;

class _license_plate_location_decoder
{
private:
  /* data */
public:
  DecoderLPLObject object;
  _license_plate_location_decoder(std::vector<int> input_size, std::vector<int> steps, std::vector<int> min_sizes, std::vector<float> variance);
  ~_license_plate_location_decoder();
  std::string decoder_license_plate_location_str()
  {
    return std::string("license_plate_location decoder object");
  }

  py::object decoder_license_plate_location_method_run(py::list fmaps, float nms, float score_thresh, py::list outputs_shape)
  {
    // set nms and  score_thresh
    //  debug_line;
    libmaix_err_t err = LIBMAIX_ERR_NONE;
    this->object.config.nms = nms;
    this->object.config.score_thresh = score_thresh;
    // debug_line;

    // get outputs num
    int output_num = py::len(fmaps);
    libmaix_nn_layer_t outputs[output_num];
    // config

    // output_list
    std::vector<std::string> o_out_fmaps;

    // debug_line;
    auto numpy_object_type = py::module::import("numpy").attr("ndarray");
    for (int id = 0; id != output_num; id++)
    {
      outputs[id].c = this->object.config.channel_num;
      py::list output_shape = outputs_shape[id];
      outputs[id].w = output_shape[1].cast<int>();
      outputs[id].h = output_shape[0].cast<int>();
      outputs[id].dtype = LIBMAIX_NN_DTYPE_FLOAT;
      outputs[id].data = NULL;
      outputs[id].layout = LIBMAIX_NN_LAYOUT_CHW;

      // printf("py license_plate_location decoder  output is %d\n",id);

      if (py::isinstance(fmaps[id], numpy_object_type))
      {
        // debug_line;
        auto tobytes = fmaps[id].attr("tobytes");
        // o_out_fmaps[id] = tobytes().cast<std::string>();
        // debug_line;
        o_out_fmaps.push_back(tobytes().cast<std::string>());
        // debug_line;
      }
      else if (py::isinstance<py::bytes>(fmaps[id]))
      {
        // o_out_fmaps[id]  = fmaps[id].cast<std::string>();
        o_out_fmaps.push_back(fmaps[id].cast<std::string>());
      }
      // debug_line;
      // outputs[id].data = (float *)malloc(o_out_fmaps[id].size());
      // debug_line;
      outputs[id].data = (void*)o_out_fmaps[id].c_str();
      // memcpy(outputs[id].data, o_out_fmaps[id].c_str(), o_out_fmaps[id].size());
      // debug_line;
    }

    libmaix_nn_decoder_license_plate_location_result_t license_plate_location_result;
    // debug_line;
    this->object.decoder->decode(this->object.decoder, outputs, &license_plate_location_result);
    // debug_line;



    // decode sour
    py::list boxes;
    py::list landmarks;
    for (int i = 0; i != license_plate_location_result.num; i++)
    {

      if (license_plate_location_result.plates[i].score > this->object.config.score_thresh)
      {
        // init list
        py::list box;
        py::list landmark;
        // cal box position
        int x1 = license_plate_location_result.plates[i].box.x * this->object.config.input_w;
        int y1 = license_plate_location_result.plates[i].box.y * this->object.config.input_h;
        int x2 = license_plate_location_result.plates[i].box.w * this->object.config.input_w + x1;
        int y2 = license_plate_location_result.plates[i].box.h * this->object.config.input_h + y1;
        box.append(x1);
        box.append(y1);
        box.append(x2);
        box.append(y2);
        // cal landmark
        for (int j = 0; j != 4; j++)
        {
          int x = license_plate_location_result.plates[i].points[j * 2] * this->object.config.input_w;
          int y = license_plate_location_result.plates[i].points[j * 2 + 1] * this->object.config.input_h;
          landmark.append(x);
          landmark.append(y);
        }
        boxes.append(box);
        landmarks.append(landmark);
      }
    }

    py::list result;
    result.append(boxes);
    result.append(landmarks);
    return std::move(result);
  }
};

_license_plate_location_decoder::_license_plate_location_decoder(std::vector<int> input_size, std::vector<int> steps, std::vector<int> min_sizes, std::vector<float> variance)
{
  // set basic parmar
  libmaix_err_t err = LIBMAIX_ERR_NONE;
  this->object.config.input_h = (int)input_size[0];
  this->object.config.input_w = (int)input_size[1];
  this->object.config.variance[0] = (float)variance[0];
  this->object.config.variance[1] = (float)variance[1];

  for (int i = 0; i != steps.size(); i++)
  {
    this->object.config.steps[i] = (int)steps[i];
  }
  for (int j = 0; j != min_sizes.size(); j++)
  {
    this->object.config.min_sizes[j] = (int)min_sizes[j];
  }
  this->object.config.nms = 0.2;
  this->object.config.score_thresh = 0.7;
  this->object.config.channel_num = license_plate_location_get_channel_num(&(this->object.config));

  this->object.decoder = libmaix_nn_decoder_license_plate_location_create();
  if (!this->object.decoder)
  {
    this->object.init = false;
    // printf("libmaix nn license_plate_location object create faild\n");
  }
  // printf("libmaix nn decoder license_plate_location object init\r\n");
  err = this->object.decoder->init(this->object.decoder, &(this->object.config));
  if (err != LIBMAIX_ERR_NONE)
  {
    this->object.init = false;
    printf("libmaix  nn license_plate_location object init faild\n");
  }
  else
  {
    this->object.init = true;
    printf("libmaix  nn license_plate_location object init success\n");
  }
}
_license_plate_location_decoder::~_license_plate_location_decoder()
{
  if (this->object.decoder)
  {
    libmaix_nn_decoder_license_plate_location_deinit(this->object.decoder);
    libmaix_nn_decoder_license_plate_location_destroy(&this->object.decoder);
  }
}

PYBIND11_MODULE(_maix_nn_decoder_license_plate_location, m)
{
  pybind11::class_<_license_plate_location_decoder>(m, "license_plate_location")
      .def(pybind11::init<std::vector<int>, std::vector<int>, std::vector<int>, std::vector<float>>(), py::arg("input_size"), py::arg("steps"), py::arg("min_sizes"), py::arg("varicance"))
      .def("__str__", &_license_plate_location_decoder::decoder_license_plate_location_str)
      .def("run", &_license_plate_location_decoder::decoder_license_plate_location_method_run, py::arg("fmaps"), py::arg("nms"), py::arg("score_thresh"), py::arg("outputs_shape"));
}
