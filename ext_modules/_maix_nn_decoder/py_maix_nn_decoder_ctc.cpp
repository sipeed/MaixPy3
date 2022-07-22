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
#include "libmaix_nn_decoder_ctc.h"
#include "libmaix_cv_image.h"

#define debug_line printf("%s:%d %s %s %s \r\n", __FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__)

namespace py=pybind11;

typedef struct
{
    libmaix_nn_decoder_ctc_config_t config;
    libmaix_nn_decoder_t *decoder;
    libmaix_nn_layer_t * fmap_out;
    libmaix_nn_decoder_ctc_result_t *ctc_result;
    bool init;
} DecoderCTCObject;

class _ctc_decoder
{
    private:
    public:
    DecoderCTCObject object ;
    _ctc_decoder(std::vector<int> output_shape);
    ~_ctc_decoder();
    std::string decoder_CTC_str()
    {
        return std::string("CTC decoder object");
    }
    py::object decoder_CTC_run(py::object fmap)
    {
        libmaix_err_t err = LIBMAIX_ERR_NONE;
        libmaix_nn_layer_t output;

        output.c = this->object.config.N;
        output.h = this->object.config.C;
        output.w = this->object.config.T;
        output.dtype = LIBMAIX_NN_DTYPE_FLOAT;
        output.data = NULL;
        output.layout = LIBMAIX_NN_LAYOUT_CHW;

        auto numpy_object_type = py::module::import("numpy").attr("ndarray");
        std::string bytes_buffer ;
        if(py::isinstance<py::bytes>(fmap))
        {
            bytes_buffer = fmap.cast<std::string>();
            cv::
            this->object.fmap_out->data = (void *)(float *)bytes_buffer.c_str();
        }
        else if (py::isinstance(fmap , numpy_object_type))
        {
            auto tobytes = fmap.attr("tobytes");
            bytes_buffer = tobytes.cast<std::string>();
            this->object.fmap_out->data = (void *)(float *)bytes_buffer.c_str();
        }
        else
        {
            std::cout << "the output feature map only support bytes or ndarray" << std::endl;
        }


        err = this->object.decoder->decode(this->object.decoder ,this->object.fmap_out , (void *) this->object.ctc_result);
        if(err != LIBMAIX_ERR_NONE)
        {
            std::cout << "CTC decode is faild " << std::endl;
        }
        py::list idxs;
        for(int i=0 ; i != this->object.ctc_result->length ;i++)
        {
            int idx =  int( this->object.ctc_result->no_repeat_idx[i]);
            idxs.append(idx);
        }
        return idxs;
    }
};

_ctc_decoder::_ctc_decoder(std::vector<int> output_shape)
{
    libmaix_err_t err = LIBMAIX_ERR_NONE;

    int C = output_shape[0];
    int H = output_shape[1];
    int W = output_shape[2];

    this->object.fmap_out = (libmaix_nn_layer_t *)malloc(sizeof(libmaix_nn_layer_t));
    if(!this->object.fmap_out)
    {
        std::cout << "allocate output layer faild" << std::endl;
    }
    this->object.fmap_out->w = W;
    this->object.fmap_out->h = H;
    this->object.fmap_out->c = C;
    this->object.fmap_out->dtype =LIBMAIX_NN_DTYPE_FLOAT;
    this->object.fmap_out->data = NULL;

    this->object.ctc_result = (libmaix_nn_decoder_ctc_result_t *)malloc(sizeof(libmaix_nn_decoder_ctc_result_t));
    this->object.config.N  = C;
    this->object.config.C  = H;
    this->object.config.T  = W;
    this->object.config.lpr_max_lenght = 8;
    this->object.decoder = libmaix_nn_decoder_ctc_create();
    if (!this->object.decoder)
    {
        this->object.init = false;

    }
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

_ctc_decoder::~_ctc_decoder()
{
    if (this->object.decoder)
    {
        libmaix_nn_decoder_ctc_deinit(this->object.decoder);
        libmaix_nn_decoder_ctc_destroy(&this->object.decoder);
    }
}

PYBIND11_MODULE(_maix_nn_decoder_CTC, m)
{
  pybind11::class_<_ctc_decoder>(m, "CTC")
      .def(pybind11::init<std::vector<int> >(), py::arg("output_shape"))
      .def("__str__", &_ctc_decoder::decoder_CTC_str)
      .def("run", &_ctc_decoder::decoder_CTC_run, py::arg("fmaps"));
}
