#include "py_maix_nn.h"
#include <string.h>
#include "libmaix_nn.h"
#include "stdlib.h"



typedef struct
{
    PyObject_HEAD;
    libmaix_nn_t* nn;
    float*        quantize_buffer;
    float*        out_buffer;
    PyObject*     inputs;
    PyObject*     outputs;
} ModelObject;

PyDoc_STRVAR(Maix_NN_Object_type_doc, "neural network model object.\n");



static PyObject* Model_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ModelObject* self = (ModelObject*)type->tp_alloc(type, 0);
    if (!self)
    {
        return NULL;
    }
    self->nn = NULL;
    self->quantize_buffer = NULL;
    self->out_buffer      = NULL;
    self->inputs = NULL;
    self->outputs = NULL;
    return (PyObject*)self;
}

static void Model_del(ModelObject *self)
{
    printf("model del\n");
    if(self->nn)
    {
        libmaix_nn_destroy(&(self->nn));
        libmaix_nn_module_deinit();
    }
    if(Py_TYPE(self)->tp_free)
    {
        Py_TYPE(self)->tp_free((PyObject *)self);
    }
}

static int Model_init(ModelObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"model_path", "opt", NULL};
    PyObject *o_model_path = NULL;
    PyObject *o_opt        = NULL;
    libmaix_err_t err = LIBMAIX_ERR_NONE;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|$O:__init__", kwlist,
                                     &o_model_path, &o_opt))
    {
        return -1;
    }
    if(!o_model_path)
    {
        PyErr_SetString(PyExc_ValueError, "arg model_path is needed");
        return -1;
    }
    else if(PyObject_TypeCheck(o_model_path, &PyDict_Type))
    {
    }
    else if(PyObject_TypeCheck(o_model_path, &PyUnicode_Type))
    {
        PyErr_SetString(PyExc_NotImplementedError, "not support str type");
        return -1;
    }
    else if(PyObject_TypeCheck(o_model_path, &PyBytes_Type))
    {
        PyErr_SetString(PyExc_NotImplementedError, "not support bytes type");
        return -1;    }
    else
    {
        PyErr_SetString(PyExc_ValueError, "arg model_path error, need str or dict");
        return -1;
    }
    if(!o_opt || !PyObject_TypeCheck(o_opt, &PyDict_Type))
    {
        PyErr_SetString(PyExc_ValueError, "arg opt is needed");
        return -1;
    }
    PyObject *o_model_type = PyDict_GetItemString(o_opt, "model_type");
    if(!o_model_type)
    {
        PyErr_SetString(PyExc_ValueError, "arg opt need model_type key");
        return -1;
    }
    const char* model_type = (const char*)PyUnicode_DATA(o_model_type);
    if(strcmp(model_type, "awnn") == 0)
    {
        PyObject *o_inputs = PyDict_GetItemString(o_opt, "inputs");
        if(!o_inputs || !PyDict_Check(o_inputs))
        {
            PyErr_SetString(PyExc_ValueError, "arg opt need inputs key, value is dict");
            return -1;
        }
        Py_ssize_t inputs_len = PyDict_Size(o_inputs);
        self->inputs = o_inputs;
        PyObject *o_outputs = PyDict_GetItemString(o_opt, "outputs");
        if(!o_outputs || !PyDict_Check(o_outputs))
        {
            PyErr_SetString(PyExc_ValueError, "arg opt need outputs key, value is dict");
            return -1;
        }
        Py_ssize_t outputs_len = PyDict_Size(o_outputs);
        self->outputs = o_outputs;
        PyObject *o_fl_conv_no_pad = PyDict_GetItemString(o_opt, "first_layer_conv_no_pad");
        if(!o_fl_conv_no_pad || !PyBool_Check(o_fl_conv_no_pad))
        {
            PyErr_SetString(PyExc_ValueError, "arg opt need first_layer_conv_no_pad key, value is True or False");
            return -1;
        }
        bool fl_conv_no_pad = (o_fl_conv_no_pad == Py_True);
        PyObject *o_mean = PyDict_GetItemString(o_opt, "mean");
        if(!o_mean || !PyList_Check(o_mean) || PyList_Size(o_mean)!=3)
        {
            PyErr_SetString(PyExc_ValueError, "arg opt need mean key, value is list");
            return -1;
        }
        PyObject *o_norm = PyDict_GetItemString(o_opt, "norm");
        if(!o_norm || !PyList_Check(o_norm) || PyList_Size(o_norm)!=3)
        {
            PyErr_SetString(PyExc_ValueError, "arg opt need norm key, value is list");
            return -1;
        }
        PyObject *o_param_path = PyDict_GetItemString(o_model_path, "param");
        if(!o_param_path || !PyUnicode_Check(o_param_path))
        {
            PyErr_SetString(PyExc_ValueError, "arg model_path need param key, value is str");
            return -1;
        }
        PyObject *o_bin_path = PyDict_GetItemString(o_model_path, "bin");
        if(!o_bin_path || !PyUnicode_Check(o_bin_path))
        {
            PyErr_SetString(PyExc_ValueError, "arg model_path need bin key, value is str");
            return -1;
        }

        /* load by libmaix API */
        libmaix_nn_module_init();
        libmaix_nn_model_path_t model_path = {
            .awnn.param_path = (char*)PyUnicode_DATA(o_param_path),
            .awnn.bin_path = (char*)PyUnicode_DATA(o_bin_path),
        };
        char* inputs_names[inputs_len];
        char* outputs_names[outputs_len];
        PyObject* keys_inputs = PyDict_Keys(o_inputs);
        PyObject* keys_outputs = PyDict_Keys(o_outputs);
        for(Py_ssize_t i=0; i<inputs_len; ++i)
        {
            inputs_names[i] = (char*)PyUnicode_DATA(PyList_GetItem(keys_inputs, i));
        }
        for(Py_ssize_t i=0; i<outputs_len; ++i)
        {
            outputs_names[i] = (char*)PyUnicode_DATA(PyList_GetItem(keys_outputs, i));
        }
        libmaix_nn_opt_param_t opt_param = {
            .awnn.input_names             = inputs_names,
            .awnn.output_names            = outputs_names,
            .awnn.first_layer_conv_no_pad = fl_conv_no_pad? 1: 0,     // 0/1
            .awnn.input_num               = inputs_len,               // len(input_names)
            .awnn.output_num              = outputs_len,              // len(output_names)
            .awnn.mean                    = {127.5, 127.5, 127.5},
            .awnn.norm                    = {0.00784313725490196, 0.00784313725490196, 0.00784313725490196},
        };
        for(Py_ssize_t i=0; i<3; ++i)
        {
            opt_param.awnn.mean[i] = (float)PyFloat_AsDouble(PyList_GetItem(o_mean, i));
        }
        for(Py_ssize_t i=0; i<3; ++i)
        {
            opt_param.awnn.norm[i] = (float)PyFloat_AsDouble(PyList_GetItem(o_norm, i));
        }
        self->nn = libmaix_nn_creat();
        if(!self->nn)
        {
            PyErr_SetString(PyExc_MemoryError, "libmaix_nn object create fail");
            goto end;
        }
        err = self->nn->init(self->nn);
        if(err != LIBMAIX_ERR_NONE)
        {
            PyErr_Format(PyExc_Exception, "libmaix_nn init fail: %s\n", libmaix_get_err_msg(err));
            goto end;
        }
        printf("-----load now----\n");
        err = self->nn->load(self->nn, &model_path, &opt_param);
        if(err != LIBMAIX_ERR_NONE)
        {
            PyErr_Format(PyExc_Exception, "libmaix_nn load fail: %s\n", libmaix_get_err_msg(err));
            goto end;
        }
        /* load by libmaix API end */
    }
    else
    {
        PyErr_SetString(PyExc_ValueError, "now model_type only support awnn");
        return -1;
    }

    return 0;
end:
    /* load by libmaix API error deal*/
    if(self->nn)
    {
        libmaix_nn_destroy(&(self->nn));
    }
    libmaix_nn_module_deinit();
    /* load by libmaix API error deal end*/
}

static PyObject *Model_str(PyObject *object)
{
  char desc[] = "Model object";
  PyObject *dev_desc = NULL;

  dev_desc = PyUnicode_FromString(desc);

  return dev_desc;
}

PyDoc_STRVAR(Model_forward_doc, "forward network.\n");
static PyObject* Model_forward(ModelObject *self, PyObject *args, PyObject *kw_args)
{
    libmaix_err_t err = LIBMAIX_ERR_NONE;
    PyObject *o_inputs = NULL;
    int quantize = 0;
    Py_ssize_t buflen, readlen, recvlen = 0;
    static char *kwlist[] = {"inputs", "quantize", NULL};
    /* Get the buffer's memory */
    if (!PyArg_ParseTupleAndKeywords(args, kw_args, "O|$p:forward", kwlist,
                                     &o_inputs, &quantize))
    {
        return NULL;
    }
    if(!o_inputs)
    {
        PyErr_SetString(PyExc_ValueError, "input error");
        return NULL;
    }
    if(PyList_Check(o_inputs))
    {
        //TODO: multipul input
        PyErr_SetString(PyExc_NotImplementedError, "not support multiple input yet");
        return NULL;
    }
    //TODO: support numpy and bytes and list
    if(strstr(o_inputs->ob_type->tp_name, "Image") < 0)
    {
        PyErr_SetString(PyExc_ValueError, "not supported input object");
        return NULL;
    }
    PyObject* o_input_bytes = PyObject_CallMethod(o_inputs, "tobytes", NULL);
    if(o_input_bytes == NULL)
    {
        return NULL;
    }
    char* input_bytes = PyBytes_AsString(o_input_bytes);
    if(input_bytes == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "get bytes data error");
        return NULL;
    }
    PyObject* o_width = PyObject_GetAttrString(o_inputs, "width");
    if(o_width == NULL)
    {
        return NULL;
    }
    long input_width = PyLong_AsLong(o_width);
    PyObject* o_height = PyObject_GetAttrString(o_inputs, "height");
    if(o_height == NULL)
    {
        return NULL;
    }
    long input_height = PyLong_AsLong(o_height);
    PyObject* o_mode = PyObject_GetAttrString(o_inputs, "mode");
    if(o_mode == NULL)
    {
        return NULL;
    }
    if(strcmp(PyUnicode_DATA(o_mode), "RGB") != 0)
    {
        PyErr_SetString(PyExc_NotImplementedError, "only support RGB mode picture");
        return NULL;
    }
    long input_channel = 3;
    PyObject* o_inputs_shape = PyDict_Values(self->inputs);
    PyObject* o_input_shape = PyList_GetItem(o_inputs_shape, 0);
    long input_w=0, input_h=0, input_c=0;
    if(PyTuple_Check(o_input_shape))
    {
        input_w = PyLong_AsLong(PyTuple_GetItem(o_input_shape, 1));
        input_h = PyLong_AsLong(PyTuple_GetItem(o_input_shape, 0));
        input_c = PyLong_AsLong(PyTuple_GetItem(o_input_shape, 2));
    }
    else
    {
        input_w = PyLong_AsLong(PyList_GetItem(o_input_shape, 1));
        input_h = PyLong_AsLong(PyList_GetItem(o_input_shape, 0));
        input_c = PyLong_AsLong(PyList_GetItem(o_input_shape, 2));
    }
    if(input_w != input_width || input_h != input_height || input_c != input_channel)
    {
        PyErr_Format(PyExc_ValueError, "input shape error, need:(%d, %d, %d), but(%d, %d, %d)", input_h, input_w, input_c, input_height, input_width, input_channel);
        return NULL;
    }
    // printf("input shape, need:(%d, %d, %d), actually (%d, %d, %d)\n", input_h, input_w, input_c, input_height, input_width, input_channel);
    /* libmaix API */
    libmaix_nn_layer_t input = {
        .w = input_width,
        .h = input_height,
        .c = input_channel,
        .dtype = (quantize == 0 ? LIBMAIX_NN_DTYPE_INT8 : LIBMAIX_NN_DTYPE_UINT8),
        .data = input_bytes,
        .need_quantization = (quantize == 0 ? false : true),
        .buff_quantization = NULL
    };
    PyObject* o_outputs_shape = PyDict_Values(self->outputs);
    PyObject* o_output_shape = PyList_GetItem(o_outputs_shape, 0);
    long output_w=0, output_h=0, output_c=0;
    if(PyTuple_Check(o_output_shape))
    {
        output_w = PyLong_AsLong(PyTuple_GetItem(o_output_shape, 1));
        output_h = PyLong_AsLong(PyTuple_GetItem(o_output_shape, 0));
        output_c = PyLong_AsLong(PyTuple_GetItem(o_output_shape, 2));
    }
    else
    {
        output_w = PyLong_AsLong(PyList_GetItem(o_output_shape, 1));
        output_h = PyLong_AsLong(PyList_GetItem(o_output_shape, 0));
        output_c = PyLong_AsLong(PyList_GetItem(o_output_shape, 2));
    }
    libmaix_nn_layer_t out_fmap = {
        .w = output_w,
        .h = output_h,
        .c = output_c,
        .dtype = LIBMAIX_NN_DTYPE_FLOAT,
        .data = NULL
    };
    Py_ssize_t _size = input.w * input.h * input.c;
    if(input.need_quantization)
    {
        if(!self->quantize_buffer)
        {
            uint8_t* quantize_buffer = (uint8_t*)malloc(_size);
            if(!quantize_buffer)
            {
                PyErr_Format(PyExc_MemoryError, "no memory for quantize buffer, size:%d", _size);
                return NULL;
            }
            self->quantize_buffer = quantize_buffer;
        }
        input.buff_quantization = self->quantize_buffer;
    }
    _size = out_fmap.w * out_fmap.h * out_fmap.c * sizeof(float);
    if(!self->out_buffer)
    {
        self->out_buffer = (float*)malloc(_size);
        if(!self->out_buffer)
        {
            PyErr_Format(PyExc_MemoryError, "no memory for out buffer, need size:%d", _size);
            return NULL;
        }
    }
    out_fmap.data = self->out_buffer;
    err = self->nn->forward(self->nn, &input, &out_fmap);
    if(err != LIBMAIX_ERR_NONE)
    {
        PyErr_Format(PyExc_Exception, "libmaix_nn forward fail: %s\n", libmaix_get_err_msg(err));
        return NULL;
    }
    PyObject* result_bytes = PyBytes_FromStringAndSize((const char*)out_fmap.data, _size);
    PyObject* numpy = PyImport_ImportModule("numpy");
    if (numpy == NULL)
        return NULL;
    PyObject *call_args = Py_BuildValue("(O)", result_bytes);
    PyObject *call_keywords = PyDict_New();
    PyDict_SetItemString(call_keywords, "dtype", PyUnicode_FromString("float32"));
    PyObject* o_result_numpy = PyObject_Call(PyObject_GetAttrString(numpy, "frombuffer"), call_args, call_keywords);
    Py_DECREF(call_args);
    Py_DECREF(call_keywords);
    return o_result_numpy;
}
static PyMethodDef Model_methods[] = {
    {"forward", (PyCFunction)Model_forward, METH_VARARGS | METH_KEYWORDS, Model_forward_doc},
    {NULL, NULL, 0, NULL},
};


PyTypeObject PyMaix_NN_Model_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    maix_nn_model_type_name,                  /* tp_name */
    sizeof(ModelObject),                      /* tp_basicsize */
    0,                                        /* tp_itemsize */
    (destructor)Model_del,                    /* tp_dealloc */
    0,                                        /* tp_print */
    0,                                        /* tp_getattr */
    0,                                        /* tp_setattr */
    0,                                        /* tp_compare */
    0,                                        /* tp_repr */
    0,                                        /* tp_as_number */
    0,                                        /* tp_as_sequence */
    0,                                        /* tp_as_mapping */
    0,                                        /* tp_hash */
    0,                                        /* tp_call */
    Model_str,                                /* tp_str */
    0,                                        /* tp_getattro */
    0,                                        /* tp_setattro */
    0,                                        /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    Maix_NN_Object_type_doc,                  /* tp_doc */
    0,                                        /* tp_traverse */
    0,                                        /* tp_clear */
    0,                                        /* tp_richcompare */
    0,                                        /* tp_weaklistoffset */
    0,                                        /* tp_iter */
    0,                                        /* tp_iternext */
    Model_methods,                            /* tp_methods */
    0,                                        /* tp_members */
    0,                                        /* tp_getset */
    0,                                        /* tp_base */
    0,                                        /* tp_dict */
    0,                                        /* tp_descr_get */
    0,                                        /* tp_descr_set */
    0,                                        /* tp_dictoffset */
    (initproc)Model_init,                     /* tp_init */
    0,                                        /* tp_alloc */
    Model_new,                                /* tp_new */
};

