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

#include "mud.h"
#include "libmaix_nn.h"
#include "string.h"

// #include "py_maix_nn_new.h"
#define MAX 5
#define debug_line printf("%s:%d %s %s %s \r\n", __FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__)

namespace py = pybind11;


typedef struct
{
    libmaix_nn_model_path_t  model_path;
    libmaix_nn_opt_param_t opt_param;
    ini_info_t info;
    bool is_init;
    libmaix_nn_t * nn;
    int8_t *quantized_buffer;
    int8_t * int_output_buffer;
    float * float_output_buffer;
    libmaix_nn_layer_t * input_layer;
    libmaix_nn_layer_t * output_layer;
}NNObject;

class _NN
{
private:
    /* data */
public:
    NNObject nn_object;
    std::string layout_str = "hwc";
    int quantize = 1;
    int debug = 0;
    std::string output_fmt = "numpy";
    _NN(std::string mdsc_path);
    ~_NN();
    _NN &_load(std::string mdsc_path);
    py::object _nn_method_forward(py::object inputs ,  bool quantize ,  std::string layout, bool debug ,std::string output_fmt);
    std::string _maix_nn_new_str()
    {
        return std::string("maix nn object");
    }
};

_NN::_NN(std::string mdsc_path)
{
    this->nn_object.nn = NULL;
    this->nn_object.is_init = false;
    this->nn_object.quantized_buffer = NULL;
    this->nn_object.int_output_buffer = NULL;
    this->nn_object.float_output_buffer =NULL;
    this->nn_object.input_layer = NULL;
    this->nn_object.output_layer = NULL;
}

_NN &_NN::_load(std::string mdsc_path)
{
    read_file(mdsc_path.data() ,& this->nn_object.info);
    this->nn_object.nn = build_model(& (this->nn_object.info) , &(this->nn_object.model_path) ,&(this->nn_object.opt_param));
    if(! this->nn_object.nn)
    {
        printf("libmaix  nn object init faild\n");
    }

    // alloc buffer
    this->nn_object.input_layer = (libmaix_nn_layer_t*) malloc(sizeof(libmaix_nn_layer_t) * this->nn_object.info.input_num);
    if(! this->nn_object.input_layer)
    {
        printf("input layer init is faild\n");
    }
    for(int i=0 ; i != this->nn_object.info.input_num ; i++)
    {
        libmaix_nn_layer_t * input = this->nn_object.input_layer + i;
        input->h =  this->nn_object.info.inputs_shape[i][0];
        input->w =  this->nn_object.info.inputs_shape[i][1];
        input->c =  this->nn_object.info.inputs_shape[i][2];
        int _size = input->h * input->w * input->c;
        input->buff_quantization =  (int8_t *)malloc(sizeof(int8_t) * _size);
        if(!input->buff_quantization)
        {
            printf("input quantization buffer is faild\n");
        }
    }

    this->nn_object.output_layer = (libmaix_nn_layer_t*)malloc(sizeof(libmaix_nn_layer_t) * this->nn_object.info.output_num);
    if(! this->nn_object.output_layer)
    {
        printf("input layer init is faild\n");
    }
    for(int j=0 ; j != this->nn_object.info.output_num; j++)
    {
        libmaix_nn_layer_t * output = this->nn_object.output_layer + j;
        output->h =  this->nn_object.info.outputs_shape[j][0];
        output->w =  this->nn_object.info.outputs_shape[j][1];
        output->c =  this->nn_object.info.outputs_shape[j][2];
        int _size = output->h * output->w * output->c;
        output->data = (float*)malloc(sizeof(float) * _size);
        if(! output->data)
        {
            printf("output layer init is faild\n");
        }
    }
    if(this->nn_object.nn &&  this->nn_object.input_layer && this->nn_object.output_layer)
        this->nn_object.is_init = true;

    return *this ;
}

_NN::~_NN()
{
    if(this->nn_object.nn)
    {
        libmaix_nn_module_deinit();
        libmaix_nn_destroy(&this->nn_object.nn);
    }
    int input_num =  this->nn_object.info.input_num;
    int output_num =  this->nn_object.info.output_num;

    if(this->nn_object.input_layer)
    {
        // mult input
        for(int i=0 ; i != input_num ; i++)
        {
            if(this->nn_object.input_layer[i].buff_quantization)
            {
                free(this->nn_object.input_layer[i].buff_quantization);
            }
        }
        free(this->nn_object.input_layer);
    }
    if(this->nn_object.output_layer)
    {
        for(int j=0 ;j != output_num ;j++)
        {
            if(this->nn_object.output_layer[j].data)
            {
                free(this->nn_object.output_layer[j].data);
            }
        }
        free(this->nn_object.output_layer);
    }
}

py::object _NN::_nn_method_forward(py::object inputs ,  bool quantize ,  std::string layout, bool debug ,std::string output_fmt)
{
    if(!inputs)
    {
        printf("nn forward must have inputs\n");
    }
    libmaix_err_t err = LIBMAIX_ERR_NONE;
    libmaix_nn_layout_t outputs_layout = LIBMAIX_NN_LAYOUT_HWC;
    this->layout_str = layout;
    if(this->layout_str.compare("HWC") == 0)
    {
        outputs_layout = LIBMAIX_NN_LAYOUT_HWC;
    }
    else if (this->layout_str.compare("CHW") == 0)
    {
        outputs_layout = LIBMAIX_NN_LAYOUT_CHW;
    }
    else
    {
        std::cout << "not support recurrent output layout yet"<< std::endl;
        return py::none();
    }
    // numpy module


    // else
    // {
    //     std::cout << "not supported input object, only support bytes and PIL.Image object" << std::endl;
    //      return py::none();
    // }
    std::string tmp;
    auto PIL_ = py::module::import("PIL.Image").attr("Image");
    if(this->nn_object.info.input_num == 1) // single input
    {
        if(py::isinstance<py::bytes>(inputs))
        {
            tmp = inputs.cast<std::string>();
            libmaix_nn_layer_t * input = this->nn_object.input_layer;
            input->dtype = (quantize == 1 ? LIBMAIX_NN_DTYPE_INT8 : LIBMAIX_NN_DTYPE_UINT8);
            input->data = (void *)(uint8_t *)tmp.c_str() ;
            input->need_quantization = (quantize == 0 ? false : true);
        }
        else
        {
            if(py::isinstance(inputs,PIL_))
            {
                auto tobytes = PIL_.attr("tobytes");
                auto img_bytes = tobytes(inputs);
                tmp = img_bytes.cast<std::string>();
                 libmaix_nn_layer_t * input = this->nn_object.input_layer;
                input->dtype = (quantize == 1 ? LIBMAIX_NN_DTYPE_INT8 : LIBMAIX_NN_DTYPE_UINT8);
                input->data = (void *)(uint8_t *)tmp.c_str() ;
                input->need_quantization = (quantize == 0 ? false : true);
            }
            else
            {
                std::cout << "not supported input object, only support bytes and PIL.Image object" << std::endl;
                return py::none();
            }
        }
    }
    else // mutil inputs
    {
        int input_num = this->nn_object.info.input_num;
        if(py::isinstance<py::list>(inputs))
        {
            auto  list_input = inputs.cast<py::list>();
            for(int i=0 ; i != input_num ;i++)
            {
                if(py::isinstance<py::bytes>(list_input[i]))
                {
                    tmp = list_input[i].cast<std::string>();
                    libmaix_nn_layer_t * input = this->nn_object.input_layer + i;
                    input->dtype = (quantize == 1 ? LIBMAIX_NN_DTYPE_INT8 : LIBMAIX_NN_DTYPE_UINT8);
                    input->data = (void *)(uint8_t *)tmp.c_str() ;
                    input->need_quantization = (quantize == 0 ? false : true);
                }
                else if(py::isinstance(list_input[i],PIL_))
                {
                    auto tobytes = PIL_.attr("tobytes");
                    auto img_bytes = tobytes(list_input[i]);
                    tmp = img_bytes.cast<std::string>();
                    libmaix_nn_layer_t * input = this->nn_object.input_layer + i;
                    input->dtype = (quantize == 1 ? LIBMAIX_NN_DTYPE_INT8 : LIBMAIX_NN_DTYPE_UINT8);
                    input->data = (void *)(uint8_t *)tmp.c_str() ;
                    input->need_quantization = (quantize == 0 ? false : true);
                }
                else
                {
                    std::cout << "not supported input object, only support bytes and PIL.Image object and these list continer" << std::endl;
                    return py::none();
                }
            }
        }
        else
        {
            std::cout << "not supported input object, only support bytes and PIL.Image object and these list continer" << std::endl;
            return py::none();
        }
    }

    /*libmaix  API*/
    // err = this->nn->forward(this->nn, this->nn_object.input_layer ,this->nn_object.output_layer);
    err = this->nn_object.nn->forward(this->nn_object.nn,this->nn_object.input_layer, this->nn_object.output_layer);
    if(err != LIBMAIX_ERR_NONE)
    {
        std::cout <<"py  forward is faild " <<std::endl;
        return py::none();
    }

    //output feature map
    auto result = py::list();
    if(this->nn_object.info.output_num == 1)
    {
        auto result = py::none();
    }

    for(int i=0 ; i!= this->nn_object.info.output_num ;i++)
    {
        if(output_fmt.compare("numpy") == 0) // numpy
        {
            auto _numpy= py::module::import("numpy").attr("ndarray");
            auto frombuffer = py::module::import("numpy").attr("frombuffer");
            auto _numpy_dtype = _numpy.attr("float32");
            libmaix_nn_layer_t * output = this->nn_object.output_layer + i ;
            int size = output->c * output->h * output->w;
            py::bytes out_bytes ((const char *) output->data , size);
            auto out = frombuffer(out_bytes , _numpy_dtype);
            auto out_reshape = py::none();
            if(outputs_layout = LIBMAIX_NN_LAYOUT_HWC)
            {
               out_reshape = out.attr("reshape")(output->h , output->w ,output->c);
            }
            else
            {
               out_reshape = out.attr("reshape")(output->c,output->h , output->w );
            }

            if(py::isinstance<py::none>(result))
            {
                result = out_reshape;
                break;
            }
            result.append(out_reshape);
        }
        else //buffer
        {
            auto _numpy= py::module::import("numpy").attr("ndarray");
            auto frombuffer = py::module::import("numpy").attr("frombuffer");
            auto _numpy_dtype = _numpy.attr("float32");
            libmaix_nn_layer_t * output = this->nn_object.output_layer + i ;
            int size = output->c * output->h * output->w;
            py::bytes out_bytes ((const char *) output->data , size);
            auto out = frombuffer(out_bytes , _numpy_dtype);
            auto out_reshape_bytes = py::none();
            if(outputs_layout = LIBMAIX_NN_LAYOUT_HWC)
            {
                auto out_reshape = out.attr("reshape")(output->h , output->w ,output->c);
                auto out_reshape_bytes = out.attr("tobytes")();
            }
            else
            {
               out.attr("reshape")(output->c,output->h , output->w );
                auto out_reshape_bytes = out.attr("tobytes")();
            }
            if(py::isinstance<py::none>(result))
            {
                result = out_reshape_bytes;
                break;
            }
            result.append(out_reshape_bytes);
        }
    }
    return result;
}


_NN _maix_nn_load(std::string mud)
{
    return _NN(mud);
}


PYBIND11_MODULE(_maix_nn_new, m)
{
    m.doc() = "This is MaixPy3 NN new\n";
    m.def("load",_maix_nn_load,py::arg("mdsc_path"));

    pybind11::class_<_NN>(m, "nn")
      .def(pybind11::init<std::string>(), py::arg("mdsc_path"))
      .def("__str__", &_NN::_maix_nn_new_str)
      .def("forward", &_NN::_nn_method_forward, py::arg("inputs"), py::arg("quantize"), py::arg("layout"), py::arg("debug") ,py::arg("output_fmt"));
    //   .def("load" ,&_NN::_load,py::arg("mdsc_path"));
}

