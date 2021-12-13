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
#include "libmaix_nn_decoder_yolo2.h"

#define debug_line printf("%s:%d %s %s %s \r\n", __FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__)
namespace py = pybind11;



typedef struct
{
    // PyObject* m_numpy;
    libmaix_nn_decoder_yolo2_config_t config;
    libmaix_nn_decoder_t* decoder;
    bool init;
} DecoderYolo2Object;

static int max_index(float *a, int n)
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


class _yolo2_decoder
{
private:
    /* data */
public:
    DecoderYolo2Object Object;
    _yolo2_decoder() {

    }
    _yolo2_decoder(int num, std::vector<float> anchors, std::vector<int> net_in_size, std::vector<int> net_out_size)
    {
        libmaix_err_t err = LIBMAIX_ERR_NONE;
        this->Object.config.anchors = (float*)malloc(anchors.size() * sizeof(float));
        this->Object.config.anchors_num = anchors.size() / 2;
        for(int i = 0 ;i < anchors.size() ; i++)
        {
            this->Object.config.anchors[i] = anchors[i];
        }

        this->Object.config.net_in_width = net_in_size[0];
        this->Object.config.net_in_height = net_in_size[1];
        
        this->Object.config.net_out_width = net_out_size[0];
        this->Object.config.net_out_height = net_out_size[1];

        this->Object.config.input_width = this->Object.config.net_in_width;
        this->Object.config.input_height = this->Object.config.net_in_height;
        this->Object.config.nms_value = 0.3;
        this->Object.config.threshold = 0.5;
        this->Object.decoder = libmaix_nn_decoder_yolo2_create();
        printf("libmaix_nn_decoder_yolo2_create\r\n");
        if(this->Object.decoder)
        {
            printf("libmaix_nn_decoder_yolo2_create init\r\n");
            err = this->Object.decoder->init(this->Object.decoder, (void*)&this->Object.config);
            if(err == LIBMAIX_ERR_NONE)
            {
                this->Object.init = true;
                printf("this->Object.decoder->init success!\r\n");
            }
            else
            {   
                this->Object.init = false;
                printf("this->Object.decoder->init fail!\r\n");
                printf("err code:%d\r\n",err);
            }
            
        }
        else
        {
            printf("libmaix_nn_decoder_yolo2_create fail \r\n");
            this->Object.init = false;
        }
        
    }
    
    std::string decoder_yolo2_str()
    {
        return std::string("Yolo2 decoder object");
    }

    py::object decoder_yolo2_method_run(py::object fmap, float nms, float threshold, std::vector<int> img_size)
    {
        debug_line;
        libmaix_err_t err = LIBMAIX_ERR_NONE;
        this->Object.config.input_width = img_size[0];
        this->Object.config.input_height = img_size[1];
        this->Object.config.nms_value = nms;
        this->Object.config.threshold = threshold;
        debug_line;
        // libmaix_nn_layer_t out_fmap = {
        //     .w = this->Object.config.net_out_width,
        //     .h = this->Object.config.net_out_height,
        //     .c = (this->Object.config.classes_num + 5) * this->Object.config.anchors_num,
        //     .dtype = LIBMAIX_NN_DTYPE_FLOAT,
        //     .layout = LIBMAIX_NN_LAYOUT_HWC,
        //     .need_quantization = false,
        //     .data = NULL,
        //     .buff_quantization = NULL,
        // };
        libmaix_nn_layer_t out_fmap;
        out_fmap.w = this->Object.config.net_out_width;
        out_fmap.h = this->Object.config.net_out_height;
        out_fmap.c = (this->Object.config.classes_num + 5) * this->Object.config.anchors_num;
        out_fmap.dtype = LIBMAIX_NN_DTYPE_FLOAT;
        out_fmap.layout = LIBMAIX_NN_LAYOUT_HWC;
        out_fmap.data = NULL;



debug_line;
        std::string o_out_fmap;
debug_line;
        auto numpy_object_type = py::module::import("numpy").attr("ndarray");
        if (py::isinstance(fmap, numpy_object_type))
        {
            auto tobytes = fmap.attr("tobytes");
            o_out_fmap = tobytes().cast<std::string>();  
        }
        else if(py::isinstance<py::bytes>(fmap))
        {
            o_out_fmap = fmap.cast<std::string>();
        }
        debug_line;
        out_fmap.data = malloc(o_out_fmap.size());
        memcpy(out_fmap.data, o_out_fmap.c_str(),o_out_fmap.size());
        printf("nihao:%d\r\n",o_out_fmap.size());
        debug_line;

        libmaix_nn_decoder_yolo2_result_t yolo2_result;
        this->Object.decoder->decode(this->Object.decoder, &out_fmap, (void*)&yolo2_result);
        debug_line;
        // printf("nihao:%p\r\n",o_out_fmap.c_str());
        py::list boxes;
        py::list probs;
        if(yolo2_result.boxes_num > 0)
        {
            debug_line;
            for (uint32_t i = 0; i < yolo2_result.boxes_num; ++i)
            {
                debug_line;
                int class_id = max_index(yolo2_result.probs[i], this->Object.config.classes_num);
                float prob = yolo2_result.probs[i][class_id];
                if (prob > threshold)
                {
                    debug_line;
                    libmaix_nn_decoder_yolo2_box_t *b = yolo2_result.boxes + i;
                    uint32_t x1 = (uint32_t)(b->x * this->Object.config.input_width - (b->w * this->Object.config.input_width / 2.));
                    uint32_t y1 = (uint32_t)(b->y * this->Object.config.input_height - (b->h * this->Object.config.input_height / 2.));
                    uint32_t w  = (uint32_t)(b->w * this->Object.config.input_width);
                    uint32_t h  = (uint32_t)(b->h * this->Object.config.input_height);
                    py::list tmp_l;
                    tmp_l.append(x1);
                    tmp_l.append(y1);
                    tmp_l.append(w);
                    tmp_l.append(h);
                    boxes.append(tmp_l);
                    py::list o_probs;
                    for(uint32_t j=0; j < this->Object.config.classes_num; ++j)
                    {
                        o_probs.append(yolo2_result.probs[i][j]);
                    }
                    py::list o_probs_item;
                    o_probs_item.append(class_id);
                    o_probs_item.append(o_probs);
                    debug_line;
                }
            }
        }debug_line;
        free(out_fmap.data);
        py::list final_result;debug_line;
        final_result.append(boxes);debug_line;
        final_result.append(probs);debug_line;
        return final_result;

    }




    ~_yolo2_decoder()
    {
        if(this->Object.decoder)
        {
            libmaix_nn_decoder_yolo2_destroy(&this->Object.decoder);
        }
        
    }
};


PYBIND11_MODULE(maix_nn_decoder_yolo2, m)
{
    pybind11::class_<_yolo2_decoder>(m, "decoder")
        .def(pybind11::init<int, std::vector<float>, std::vector<int>, std::vector<int>>(), py::arg("class_num"), py::arg("anchors"), py::arg("net_in_size") = std::vector<int> {224, 224}, py::arg("net_out_size") = std::vector<int> {7, 7})
        .def("__str__", &_yolo2_decoder::decoder_yolo2_str)
        .def("run", &_yolo2_decoder::decoder_yolo2_method_run, py::arg("fmap"), py::arg("nms") = 0.3, py::arg("threshold") = 0.5, py::arg("img_size") = std::vector<int> {224, 224})
        ;
}






// #include "py_maix_nn.h"
// #include <string.h>
// #include "libmaix_nn.h"
// #include "stdlib.h"
// #include "libmaix_nn_decoder_yolo2.h"


// typedef struct
// {
//     libmaix_nn_decoder_yolo2_config_t config;
//     libmaix_nn_decoder_t* decoder;
//     bool init;
// } DecoderYolo2Object;

// PyDoc_STRVAR(decoder_yolo2_type_doc, "neural network model Yolo2 decoder object.\n");

// static int max_index(float *a, int n)
// {
//     int i, max_i = 0;
//     float max = a[0];

//     for (i = 1; i < n; ++i)
//     {
//         if (a[i] > max)
//         {
//             max = a[i];
//             max_i = i;
//         }
//     }
//     return max_i;
// }

// static int decoder_yolo2_init(DecoderYolo2Object *self, PyObject *args, PyObject *kwds);

// static PyObject* decoder_yolo2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
// {
//     // DecoderYolo2Object* self = (DecoderYolo2Object*)type->tp_alloc(type, 0);
//     // if (!self)
//     // {
//     //     PyErr_NoMemory();
//     //     return NULL;
//     // }
//     // return (PyObject*)self;
// }


// static void decoder_yolo2_del(DecoderYolo2Object *self)
// {
//     if(this->Object.config.anchors)
//     {
//         free(this->Object.config.anchors);
//         this->Object.config.anchors = NULL;
//     }
//     if(this->Object.decoder)
//     {
//         this->Object.decoder->deinit(this->Object.decoder);
//         libmaix_nn_decoder_yolo2_destroy(&this->Object.decoder);
//     }
//     this->Object.init = false;
// }

// static int decoder_yolo2_init(class_num, anchors, net_in_size, net_out_size)
// {
//     // if(this->Object.init)
//     // {
//     //     PyErr_SetString(PyExc_Exception, "already initialized");
//     //     return -1;
//     // }
//     libmaix_err_t err = LIBMAIX_ERR_NONE;
//     // static char *kwlist[] = {"class_num", "anchors", "net_in_size", "net_out_size", NULL};

//     PyObject *anchors        = NULL;
//     PyObject *net_in_zie     = NULL;
//     PyObject *net_out_zie     = NULL;
//     bool anchors_nedd_decref = false;

//     // this->Object.m_numpy = PyImport_ImportModule("numpy");
//     // if(!this->Object.m_numpy)
//     // {
//     //     PyErr_SetString(PyExc_Exception, "need numpy module");
//     //     return -1;
//     // }

//     // if (!PyArg_ParseTupleAndKeywords(args, kwds, "IO|$OO:__init__", kwlist,
//     //                                  &this->Object.config.classes_num, &anchors, &net_in_zie, &net_out_zie))
//     // {
//     //     return -1;
//     // }
//     if(PyList_Check(anchors))
//     {
//         anchors = PyList_AsTuple(anchors);
//         anchors_nedd_decref = true;
//     }
//     this->Object.config.anchors = (float*)malloc(PyTuple_Size(anchors) * sizeof(float));
//     if(!this->Object.config.anchors)
//     {
//         PyErr_NoMemory();
//         return -1;
//     }
//     this->Object.config.anchors_num = PyTuple_Size(anchors) / 2;
//     for(int i = 0; i < PyTuple_Size(anchors); ++i)
//     {
//         this->Object.config.anchors[i] = (float)PyFloat_AsDouble(PyTuple_GetItem(anchors, i));
//     }
//     if(anchors_nedd_decref)
//     {
//         Py_DECREF(anchors);
//     }
//     if(net_in_zie)
//     {
//         this->Object.config.net_in_width  = PyLong_AsLong(PyTuple_GetItem(net_in_zie, 0));
//         this->Object.config.net_in_height = PyLong_AsLong(PyTuple_GetItem(net_in_zie, 1));
//     }
//     else
//     {
//         this->Object.config.net_in_width  = 224;
//         this->Object.config.net_in_height = 224;
//     }
//     if(net_out_zie)
//     {
//         this->Object.config.net_out_width  = PyLong_AsLong(PyTuple_GetItem(net_out_zie, 0));
//         this->Object.config.net_out_height = PyLong_AsLong(PyTuple_GetItem(net_out_zie, 1));
//     }
//     else
//     {
//         this->Object.config.net_out_width  = 7;
//         this->Object.config.net_out_height = 7;
//     }
//     this->Object.config.input_width = this->Object.config.net_in_width;
//     this->Object.config.input_height = this->Object.config.net_in_height;
//     this->Object.config.nms_value = 0.3;
//     this->Object.config.threshold = 0.5;

//     this->Object.decoder = libmaix_nn_decoder_yolo2_create();
//     if(!this->Object.decoder)
//     {
//         PyErr_SetString(PyExc_Exception, "create yolo2 decoder fail");
//         return -1;
//     }
//     err = this->Object.decoder->init(this->Object.decoder, (void*)&this->Object.config);
//     if(err != LIBMAIX_ERR_NONE)
//     {
//         PyErr_Format(PyExc_Exception, "yolo2 decoder init fail, code: %d", err);
//         return -1;
//     }

//     this->Object.init = true;
//     return 0;
// }

// static PyObject *decoder_yolo2_str(PyObject *object)
// {
//   char desc[] = "Yolo2 decoder object";
//   PyObject *dev_desc = NULL;

//   dev_desc = PyUnicode_FromString(desc);

//   return dev_desc;
// }

// PyDoc_STRVAR(decoder_yolo2_method_run_doc, "run decode process.\n");
// static PyObject* decoder_yolo2_method_run(DecoderYolo2Object *self, PyObject *args, PyObject *kw_args)
// {
//     if(!this->Object.init)
//     {
//         PyErr_SetString(PyExc_Exception, "not initialize");
//         return NULL;
//     }

//     libmaix_err_t err = LIBMAIX_ERR_NONE;
//     static char *kwlist[] = {"fmap", "nms", "threshold", "img_size", NULL};
//     PyObject *fmap        = NULL;
//     float   nms          = 0.3;
//     float   threshold    = 0.5;
//     PyObject *img_size    = NULL;
//     bool fmap_need_decref = false;
//     if (!PyArg_ParseTupleAndKeywords(args, kw_args, "O|$ffO:__init__", kwlist,
//                                      &fmap, &nms, &threshold, &img_size))
//     {
//         return NULL;
//     }
//     if(img_size)
//     {
//         this->Object.config.input_width  = PyLong_AsLong(PyTuple_GetItem(img_size, 0));
//         this->Object.config.input_height = PyLong_AsLong(PyTuple_GetItem(img_size, 1));
//     }
//     else
//     {
//         this->Object.config.input_width = this->Object.config.net_in_width;
//         this->Object.config.input_height = this->Object.config.net_in_height;
//     }
//     this->Object.config.nms_value = nms;
//     this->Object.config.threshold = threshold;

//     libmaix_nn_layer_t out_fmap = {
//         .w = this->Object.config.net_out_width,
//         .h = this->Object.config.net_out_height,
//         .c = (this->Object.config.classes_num + 5) * this->Object.config.anchors_num,
//         .dtype = LIBMAIX_NN_DTYPE_FLOAT,
//         .layout = LIBMAIX_NN_LAYOUT_HWC,
//         .data = NULL
//     };
//     PyObject* o_out_fmap = NULL;
//     if(strstr(fmap->ob_type->tp_name, "ndarray") > 0)
//     {
//         o_out_fmap = PyObject_CallMethod(fmap, "tobytes", NULL);
//         if(o_out_fmap == NULL)
//         {
//             PyErr_SetString(PyExc_ValueError, "ndarray tobytes() error");
//             return NULL;
//         }
//         fmap_need_decref = true;
//     }
//     else if(PyBytes_Check(fmap))
//     {
//         o_out_fmap = fmap;
//     }
//     else
//     {
//         PyErr_SetString(PyExc_ValueError, "not supported fmap object, should be bytes or numpy.ndarray");
//         return NULL;
//     }
//     int expected_size = out_fmap.w * out_fmap.h * out_fmap.c * sizeof(float);
//     if(PyBytes_Size(o_out_fmap) != expected_size)
//     {
//         PyErr_Format(PyExc_ValueError, "fmap size error, need: %d, but: %d", expected_size, PyBytes_Size(o_out_fmap));
//         if(fmap_need_decref)
//             Py_DECREF(o_out_fmap);
//         return NULL;
//     }
//     out_fmap.data = PyBytes_AsString(o_out_fmap);

//     libmaix_nn_decoder_yolo2_result_t yolo2_result;
//     err = this->Object.decoder->decode(this->Object.decoder, &out_fmap, (void*)&yolo2_result);
//     if(fmap_need_decref)
//         Py_DECREF(o_out_fmap);
//     if(err != LIBMAIX_ERR_NONE)
//     {
//         PyErr_Format(PyExc_Exception, "decode fail, code: %d", err);
//         return NULL;
//     }
//     PyObject* boxes = PyList_New(0);
//     PyObject* probs = PyList_New(0);
//     if(yolo2_result.boxes_num > 0)
//     {
//         for (uint32_t i = 0; i < yolo2_result.boxes_num; ++i)
//         {
//             int class_id = max_index(yolo2_result.probs[i], this->Object.config.classes_num);
//             float prob = yolo2_result.probs[i][class_id];
//             if (prob > threshold)
//             {
//                 libmaix_nn_decoder_yolo2_box_t *b = yolo2_result.boxes + i;
//                 if(img_size)
//                 {
//                     uint32_t x1 = (uint32_t)(b->x * this->Object.config.input_width - (b->w * this->Object.config.input_width / 2.));
//                     uint32_t y1 = (uint32_t)(b->y * this->Object.config.input_height - (b->h * this->Object.config.input_height / 2.));
//                     uint32_t w  = (uint32_t)(b->w * this->Object.config.input_width);
//                     uint32_t h  = (uint32_t)(b->h * this->Object.config.input_height);
//                     PyObject* box = PyList_New(4);
//                     PyList_SetItem(box, 0, PyLong_FromLong(x1));
//                     PyList_SetItem(box, 1, PyLong_FromLong(y1));
//                     PyList_SetItem(box, 2, PyLong_FromLong(w));
//                     PyList_SetItem(box, 3, PyLong_FromLong(h));
//                     PyList_Append(boxes, box);
//                     Py_DECREF(box);
//                 }
//                 else
//                 {
//                     PyObject* box = PyList_New(4);
//                     PyList_SetItem(box, 0, PyFloat_FromDouble(b->x - b->w / 2.));
//                     PyList_SetItem(box, 1, PyFloat_FromDouble(b->y - b->h / 2.));
//                     PyList_SetItem(box, 2, PyFloat_FromDouble(b->w));
//                     PyList_SetItem(box, 3, PyFloat_FromDouble(b->h));
//                     PyList_Append(boxes, box);
//                     Py_DECREF(box);
//                 }
//                 PyObject* o_probs = PyList_New(0);
//                 for(uint32_t j=0; j<this->Object.config.classes_num; ++j)
//                 {
//                   // PyList_Append(o_probs, PyFloat_FromDouble(yolo2_result.probs[i][j]));
//                   PyObject *tmp = PyFloat_FromDouble(yolo2_result.probs[i][j]);
//                   PyList_Append(o_probs, tmp);
//                   Py_DECREF(tmp);
//                 }
//                 PyObject* o_probs_item = PyList_New(2);
//                 PyList_SetItem(o_probs_item, 0, PyLong_FromLong(class_id));
//                 PyList_SetItem(o_probs_item, 1, o_probs);
//                 PyList_Append(probs, o_probs_item);
//                 Py_DECREF(o_probs_item);
//             }
//         }
//     }

//     PyObject* final_result = PyList_New(0);
//     PyList_Append(final_result, boxes);
//     PyList_Append(final_result, probs);
//     Py_DECREF(boxes);
//     Py_DECREF(probs);
//     return final_result;
// }



// static PyMethodDef decoder_yolo2_methods[] = {
//     {"run", (PyCFunction)decoder_yolo2_method_run, METH_VARARGS | METH_KEYWORDS, decoder_yolo2_method_run_doc},
//     {NULL, NULL, 0, NULL},
// };


// PyTypeObject PyMaix_NN_Decoder_Yolo2_Type = {
//     PyVarObject_HEAD_INIT(NULL, 0)
//     maix_nn_decoder_yolo2_type_name,                  /* tp_name */
//     sizeof(DecoderYolo2Object),                      /* tp_basicsize */
//     0,                                        /* tp_itemsize */
//     (destructor)decoder_yolo2_del,                    /* tp_dealloc */
//     0,                                        /* tp_print */
//     0,                                        /* tp_getattr */
//     0,                                        /* tp_setattr */
//     0,                                        /* tp_compare */
//     0,                                        /* tp_repr */
//     0,                                        /* tp_as_number */
//     0,                                        /* tp_as_sequence */
//     0,                                        /* tp_as_mapping */
//     0,                                        /* tp_hash */
//     0,                                        /* tp_call */
//     decoder_yolo2_str,                                /* tp_str */
//     0,                                        /* tp_getattro */
//     0,                                        /* tp_setattro */
//     0,                                        /* tp_as_buffer */
//     Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
//     decoder_yolo2_type_doc,                  /* tp_doc */
//     0,                                        /* tp_traverse */
//     0,                                        /* tp_clear */
//     0,                                        /* tp_richcompare */
//     0,                                        /* tp_weaklistoffset */
//     0,                                        /* tp_iter */
//     0,                                        /* tp_iternext */
//     decoder_yolo2_methods,                            /* tp_methods */
//     0,                                        /* tp_members */
//     0,                                        /* tp_getset */
//     0,                                        /* tp_base */
//     0,                                        /* tp_dict */
//     0,                                        /* tp_descr_get */
//     0,                                        /* tp_descr_set */
//     0,                                        /* tp_dictoffset */
//     (initproc)decoder_yolo2_init,             /* tp_init */
//     0,                                        /* tp_alloc */
//     decoder_yolo2_new,                        /* tp_new */
// };

