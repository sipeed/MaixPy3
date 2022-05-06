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
// #include "libmaix_nn_decoder_yolo2.h"
#include "libmaix_nn_decoder_yolo2.h"

#define debug_line printf("%s:%d %s %s %s \r\n", __FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__)

namespace py = pybind11;
typedef struct
{
    bool init;
    libmaix_nn_decoder_yolo2_config_t config;
    libmaix_nn_decoder_t *decoder;
    libmaix_nn_layer_t * fmap_out;
    libmaix_nn_decoder_yolo2_result_t *yolo2_result;
}DecoderYolo2Object;

class  _yolo2_decoder
{
private:
public:
    DecoderYolo2Object object;
     _yolo2_decoder(int class_num  , std::vector<float> anchors ,  std::vector<int>input_size , std::vector<int>output_size);
    ~ _yolo2_decoder();
    int max_index(float *a, int n);
    std::string decoder_yolo2_str();
    py::object decoder_yolo2_method_run(py::object fmap , float nms , float threshold , std::vector<int>output_size);

};



 _yolo2_decoder:: _yolo2_decoder(int class_num  , std::vector<float> anchors ,  std::vector<int>input_size , std::vector<int>output_size) //size (H,W)
{
    libmaix_err_t err = LIBMAIX_ERR_NONE;
    this->object.init = false;
    // decoder object
    try
    {
        this->object.decoder = libmaix_nn_decoder_yolo2_create(libmaix_nn_decoder_yolo2_init , libmaix_nn_decoder_yolo2_deinit , libmaix_nn_decoder_yolo2_decode);
    }
    catch(const std::bad_alloc& e)
    {
        std::cout << "create yolo2 decoder faild" << std::endl;
        std::cerr << e.what() << '\n';
    }
    // config
    int anchor_len = (int)anchors.size();
    this->object.config.anchors = (float *)malloc(sizeof(float) *anchor_len);
    for(int i=0 ;i!=anchor_len ;i++)
    {
        this->object.config.anchors[i] = anchors[i];
    }
    this->object.config.classes_num = class_num;
    this->object.config.anchors_num = anchor_len / 2;
    this->object.config.threshold = 0.5;
    this->object.config.nms_value = 0.3;
    this->object.config.net_in_height = input_size[0];
    this->object.config.net_in_width = input_size[1];
    this->object.config.net_out_height = output_size[0];
    this->object.config.net_out_width = output_size[1];
    // fmap_out
    this->object.fmap_out = (libmaix_nn_layer_t *)malloc(sizeof(libmaix_nn_layer_t));
    if(!this->object.fmap_out)
    {
        std::cout << "allocate output layer faild" << std::endl;
    }
    this->object.fmap_out->w = this->object.config.net_out_width;
    this->object.fmap_out->h = this->object.config.net_out_height;
    this->object.fmap_out->c = this->object.config.anchors_num * (this->object.config.classes_num + 5);
    this->object.fmap_out->dtype =LIBMAIX_NN_DTYPE_FLOAT;
    this->object.fmap_out->data = NULL;

    //decoder result
    this->object.yolo2_result =(libmaix_nn_decoder_yolo2_result_t *)malloc(sizeof(libmaix_nn_decoder_yolo2_result_t));
    if(!this->object.yolo2_result )
    {
        std::cout << "allocate decoder result faild" << std::endl;
    }
    // init
    err = this->object.decoder->init(this->object.decoder , (void *) &this->object.config);
    if(this->object.decoder && err == LIBMAIX_ERR_NONE)
    {
        this->object.init = true;
    }
    else
    {
        std::cout<<"yolo2 decoder object init is faild" <<std::endl;
    }
}

 _yolo2_decoder::~ _yolo2_decoder()
{
    if(this->object.decoder)
    {
        this->object.decoder->deinit(this->object.decoder);
        libmaix_nn_decoder_yolo2_destroy(& this->object.decoder);
    }
}

int _yolo2_decoder::max_index(float *a , int n)
{
    int i, max_i = 0;
    float max = a[0];

    for (i = 1; i < n; ++i)
    {
        if (a[i] > max)
        {
            max = a[i];
            max_i = i;
        }
    }
    return max_i;
}

std::string _yolo2_decoder::decoder_yolo2_str()
{
    return std::string("Yolo2 decoder object");
}

py::object _yolo2_decoder::decoder_yolo2_method_run(py::object fmap , float nms , float threshold , std::vector<int>input_size)
{
    if(this->object.init == false)
    {
        std::cout << "yolo2 decoder init is faild" << std::endl;
        return py::none();
    }
    libmaix_err_t err = LIBMAIX_ERR_NONE;
    this->object.config.nms_value = nms;
    this->object.config.threshold = threshold;
    auto numpy_object_type = py::module::import("numpy").attr("ndarray");
    std::string bytes_buffer ;
    // yolo2 decoder of libmaix is useing HWC format , build data for decode
    if(py::isinstance<py::bytes>(fmap))
    {
        bytes_buffer = fmap.cast<std::string>();
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
    // decode
    err = this->object.decoder->decode(this->object.decoder ,this->object.fmap_out , (void *) this->object.yolo2_result);
    if(err != LIBMAIX_ERR_NONE)
    {
        std::cout << "yolo2 decode is faild " << std::endl;
    }
    py::list boxes;
    py::list probs;
    uint32_t boxes_num = this->object.yolo2_result->boxes_num;
    uint32_t classes_num = this->object.config.classes_num;
    if(boxes_num > 0)
    {
        for(uint32_t i=0 ;i != boxes_num ;i++)
        {
            int class_id = this->max_index(this->object.yolo2_result->probs[i] , this->object.config.classes_num);
            float prob =this->object.yolo2_result->probs[i][class_id];
            if(prob > this->object.config.threshold)
            {
                //box
                libmaix_nn_decoder_yolo2_box_t *b =this->object.yolo2_result->boxes +i ;
                int input_h = input_size[0];
                int input_w = input_size[1];
                uint32_t x1 = (uint32_t )(b->x * input_w - (b->w * input_w / 2.));
                uint32_t y1 = (uint32_t)(b->y * input_h - (b->y * input_h / 2.));
                uint32_t  w = (uint32_t) (b->w * input_w);
                uint32_t h = (uint32_t)(b->h *input_h);
                py::list box;
                box.insert(0,x1);
                box.insert(1,y1);
                box.insert(2,w);
                box.insert(3,h);
                boxes.append(box);
                //prob
                py::list one_box_probs;
                for(uint32_t j =0; j!=classes_num ;j++)
                {
                    float tmp = this->object.yolo2_result->probs[i][j];
                    one_box_probs.append(tmp);
                }
                py::list one_box_item;
                one_box_item.insert(0,class_id);
                one_box_item.insert(1, one_box_probs);
                probs.append(one_box_item);
            }
        }
    }
    py::list result;
    result.append(boxes);
    result.append(probs);
    return  result;
}



PYBIND11_MODULE(_maix_nn_decoder_yolo2, m)
{
  pybind11::class_<_yolo2_decoder>(m, "Yolo2")
      .def(pybind11::init<int, std::vector<float>, std::vector<int>, std::vector<int>>(), py::arg("class_num"), py::arg("anchors"), py::arg("input_size"), py::arg("output_size"))
      .def("__str__", &_yolo2_decoder::decoder_yolo2_str)
      .def("run", &_yolo2_decoder::decoder_yolo2_method_run, py::arg("fmaps"), py::arg("nms"), py::arg("threshold"), py::arg("input_size"));
}

