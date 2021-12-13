#include "py_maix_nn.h"
#include <string.h>
#include "libmaix_nn.h"
#include "stdlib.h"
#include <sys/time.h>


PyDoc_STRVAR(Maix_NN_Model_Object_type_doc, "neural network model object.\n");



static PyObject* Model_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ModelObject* self = (ModelObject*)type->tp_alloc(type, 0);
    if (!self)
    {
        PyErr_NoMemory();
        return NULL;
    }
    self->nn = NULL;
    self->quantize_buffer = NULL;
    self->out_buffer      = NULL;
    self->inputs = NULL;
    self->outputs = NULL;
    return (PyObject*)self;
}

static void free_layers_data_mem(float** p, int len)
{
    if(p)
    {
        for(int i=0; i < len; ++i)
        {
            if(p[i])
            {
                free(p[i]);
                p[i] = NULL;
            }
        }
        free(p);
        p = NULL;
    }
}

static void Model_del(ModelObject *self)
{
    if(!self->is_init)
        return;
    free_layers_data_mem(self->out_buffer, self->outputs_len);
    if(self->quantize_buffer)
    {
        free(self->quantize_buffer);
        self->quantize_buffer = NULL;
    }
    if(self->nn)
    {
        libmaix_nn_destroy(&(self->nn));
        libmaix_nn_module_deinit();
    }
    if(Py_TYPE(self)->tp_free)
    {
        Py_TYPE(self)->tp_free((PyObject *)self);
    }
    if(self->inputs)
    {
        Py_DECREF(self->inputs);
        self->inputs = NULL;
    }
    if(self->outputs)
    {
        Py_DECREF(self->outputs);
        self->outputs = NULL;
    }
    Py_DECREF(self->m_numpy);
}

static int Model_init(ModelObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"model_path", "opt", NULL};
    PyObject *o_model_path = NULL;
    PyObject *o_opt        = NULL;
    libmaix_err_t err = LIBMAIX_ERR_NONE;
    self->m_numpy = PyImport_ImportModule("numpy");
    if(!self->m_numpy)
    {
        PyErr_SetString(PyExc_EnvironmentError, "need numpy module");
        return -1;
    }

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
        PyObject *o_mean = PyDict_GetItemString(o_opt, "mean");
        if(!o_mean || !(PyFloat_Check(o_mean) || (PyList_Check(o_mean) && (PyList_Size(o_mean)==3 || PyList_Size(o_mean)==1))))
        {
            PyErr_SetString(PyExc_ValueError, "arg opt need mean key, value is float or list");
            return -1;
        }
        PyObject *o_norm = PyDict_GetItemString(o_opt, "norm");
        if(!o_norm || !(PyFloat_Check(o_norm) || (PyList_Check(o_norm) && (PyList_Size(o_norm)==3 || PyList_Size(o_norm)==1))))
        {
            PyErr_SetString(PyExc_ValueError, "arg opt need norm key, value is float or list");
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
        Py_INCREF(self->inputs);
        Py_INCREF(self->outputs);

        /* load by libmaix API */
        libmaix_nn_module_init();
        libmaix_nn_model_path_t model_path = {
            .awnn.param_path = (char*)PyUnicode_DATA(o_param_path),
            .awnn.bin_path = (char*)PyUnicode_DATA(o_bin_path),
        };
        // encrypted model?
        bool encrypt = false;
        PyObject *o_encrypt = PyDict_GetItemString(o_opt, "encrypt");
        if(o_encrypt)
        {
            if(PyLong_AsLong(o_encrypt))
            {
                encrypt = true;
            }
        }
        int inputs_id[inputs_len];
        int outputs_id[outputs_len];
        char* inputs_names[inputs_len];
        char* outputs_names[outputs_len];
        PyObject* keys_inputs = PyDict_Keys(o_inputs);
        PyObject* keys_outputs = PyDict_Keys(o_outputs);
        PyObject* temp = NULL;
        for(Py_ssize_t i=0; i<inputs_len; ++i)
        {
            temp = PyList_GetItem(keys_inputs, i);
            if(PyLong_Check(temp))
            {
                encrypt = true;
                inputs_id[i] = PyLong_AsLong(temp);
            }
            else
            {
                inputs_names[i] = (char*)PyUnicode_DATA(temp);
            }
        }
        for(Py_ssize_t i=0; i<outputs_len; ++i)
        {
            temp = PyList_GetItem(keys_outputs, i);
            if(PyLong_Check(temp))
            {
                encrypt = true;
                outputs_id[i] = PyLong_AsLong(temp);
            }
            else
            {
                outputs_names[i] = (char*)PyUnicode_DATA(temp);
            }
        }
        libmaix_nn_opt_param_t opt_param = {
            .awnn.input_names             = NULL,
            .awnn.output_names            = NULL,
            .awnn.input_num               = inputs_len,               // len(input_names)
            .awnn.output_num              = outputs_len,              // len(output_names)
            .awnn.mean                    = {127.5, 127.5, 127.5},
            .awnn.norm                    = {0.0078125, 0.0078125, 0.0078125},
        };
        if(!encrypt)
        {
            opt_param.awnn.input_names             = inputs_names;
            opt_param.awnn.output_names            = outputs_names;
            opt_param.awnn.encrypt = false;
        }
        else
        {
            opt_param.awnn.input_ids             = inputs_id;
            opt_param.awnn.output_ids            = outputs_id;
            opt_param.awnn.encrypt = true;
        }
        if(PyFloat_Check(o_mean))
        {
            opt_param.awnn.mean[0] = (float)PyFloat_AsDouble(o_mean);
            opt_param.awnn.mean[1] = opt_param.awnn.mean[0];
            opt_param.awnn.mean[2] = opt_param.awnn.mean[0];
        }
        else
        {
            Py_ssize_t i=0;
            for(; i<PyList_Size(o_mean); ++i)
            {
                opt_param.awnn.mean[i] = (float)PyFloat_AsDouble(PyList_GetItem(o_mean, i));
            }
            for(Py_ssize_t j=i; j<3; ++j)
            {
                opt_param.awnn.mean[j] = opt_param.awnn.mean[i-1];
            }
        }
        if(PyFloat_Check(o_norm))
        {
            opt_param.awnn.norm[0] = (float)PyFloat_AsDouble(o_norm);
            opt_param.awnn.norm[1] = opt_param.awnn.norm[0];
            opt_param.awnn.norm[2] = opt_param.awnn.norm[0];
        }
        else
        {
            Py_ssize_t i=0;
            for(; i<PyList_Size(o_norm); ++i)
            {
                opt_param.awnn.norm[i] = (float)PyFloat_AsDouble(PyList_GetItem(o_norm, i));
            }
            for(Py_ssize_t j=i; j<3; ++j)
            {
                opt_param.awnn.norm[j] = opt_param.awnn.norm[i-1];
            }
        }
        self->inputs_len = inputs_len;
        self->outputs_len = outputs_len;
        self->nn = libmaix_nn_create();
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
        err = self->nn->load(self->nn, &model_path, &opt_param);
        if(err != LIBMAIX_ERR_NONE)
        {
            PyErr_Format(PyExc_Exception, "libmaix_nn load fail: %s\n", libmaix_get_err_msg(err));
            goto end;
        }
        Py_DECREF(keys_inputs);
        Py_DECREF(keys_outputs);
        /* load by libmaix API end */
    }
    else if (strcmp(model_type, "normal") == 0)
    {
        /*load by libmaix R329 AIP */
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
        PyObject *o_mean = PyDict_GetItemString(o_opt, "mean");
        if(!o_mean || !(PyFloat_Check(o_mean) || (PyList_Check(o_mean) && (PyList_Size(o_mean)==3 || PyList_Size(o_mean)==1))))
        {
            PyErr_SetString(PyExc_ValueError, "arg opt need mean key, value is float or list");
            return -1;
        }
        PyObject *o_norm = PyDict_GetItemString(o_opt, "norm");
        if(!o_norm || !(PyFloat_Check(o_norm) || (PyList_Check(o_norm) && (PyList_Size(o_norm)==3 || PyList_Size(o_norm)==1))))
        {
            PyErr_SetString(PyExc_ValueError, "arg opt need norm key, value is float or list");
            return -1;
        }

        // PyObject *o_param_path = PyDict_GetItemString(o_model_path, "param");
        // if(!o_param_path || !PyUnicode_Check(o_param_path))
        // {
        //     PyErr_SetString(PyExc_ValueError, "arg model_path need param key, value is str");
        //     return -1;
        // }
        PyObject *o_bin_path = PyDict_GetItemString(o_model_path, "bin");
        if(!o_bin_path || !PyUnicode_Check(o_bin_path))
        {
            PyErr_SetString(PyExc_ValueError, "arg model_path need bin key, value is str");
            return -1;
        }

        Py_INCREF(self->inputs);
        Py_INCREF(self->outputs);

        /* load by libmaix API */
        libmaix_nn_module_init();
        libmaix_nn_model_path_t model_path = {
            .normal.model_path = (char*)PyUnicode_DATA(o_bin_path),
        };


        // encrypted model?
        bool encrypt = false;
        PyObject *o_encrypt = PyDict_GetItemString(o_opt, "encrypt");
        if(o_encrypt)
        {
            if(PyLong_AsLong(o_encrypt))
            {
                encrypt = true;
            }
        }
        int inputs_id[inputs_len];
        int outputs_id[outputs_len];
        char* inputs_names[inputs_len];
        char* outputs_names[outputs_len];
        PyObject* keys_inputs = PyDict_Keys(o_inputs);
        PyObject* keys_outputs = PyDict_Keys(o_outputs);
        PyObject* temp = NULL;
        for(Py_ssize_t i=0; i<inputs_len; ++i)
        {
            temp = PyList_GetItem(keys_inputs, i);
            if(PyLong_Check(temp))
            {
                encrypt = true;
                inputs_id[i] = PyLong_AsLong(temp);
            }
            else
            {
                inputs_names[i] = (char*)PyUnicode_DATA(temp);
            }
        }
        for(Py_ssize_t i=0; i<outputs_len; ++i)
        {
            temp = PyList_GetItem(keys_outputs, i);
            if(PyLong_Check(temp))
            {
                encrypt = true;
                outputs_id[i] = PyLong_AsLong(temp);
            }
            else
            {
                outputs_names[i] = (char*)PyUnicode_DATA(temp);
            }
        }
        libmaix_nn_opt_param_t opt_param = {
            .normal.input_names             = NULL,
            .normal.output_names            = NULL,
            .normal.input_num               = inputs_len,               // len(input_names)
            .normal.output_num              = outputs_len,              // len(output_names)
            .normal.mean                    = {127.5, 127.5, 127.5},
            .normal.norm                    = {0.0078125, 0.0078125, 0.0078125},
        };
        if(!encrypt)
        {
            opt_param.normal.input_names             = inputs_names;
            opt_param.normal.output_names            = outputs_names;
            opt_param.normal.encrypt = false;
        }
        else
        {
            opt_param.normal.input_ids             = inputs_id;
            opt_param.normal.output_ids            = outputs_id;
            opt_param.normal.encrypt = true;
        }
        if(PyFloat_Check(o_mean))
        {
            opt_param.normal.mean[0] = (float)PyFloat_AsDouble(o_mean);
            opt_param.normal.mean[1] = opt_param.normal.mean[0];
            opt_param.normal.mean[2] = opt_param.normal.mean[0];
        }
        else
        {
            Py_ssize_t i=0;
            for(; i<PyList_Size(o_mean); ++i)
            {
                opt_param.normal.mean[i] = (float)PyFloat_AsDouble(PyList_GetItem(o_mean, i));
            }
            for(Py_ssize_t j=i; j<3; ++j)
            {
                opt_param.normal.mean[j] = opt_param.normal.mean[i-1];
            }
        }
        if(PyFloat_Check(o_norm))
        {
            opt_param.normal.norm[0] = (float)PyFloat_AsDouble(o_norm);
            opt_param.normal.norm[1] = opt_param.normal.norm[0];
            opt_param.normal.norm[2] = opt_param.normal.norm[0];
        }
        else
        {
            Py_ssize_t i=0;
            for(; i<PyList_Size(o_norm); ++i)
            {
                opt_param.normal.norm[i] = (float)PyFloat_AsDouble(PyList_GetItem(o_norm, i));
            }
            for(Py_ssize_t j=i; j<3; ++j)
            {
                opt_param.normal.norm[j] = opt_param.normal.norm[i-1];
            }
        }
        self->inputs_len = inputs_len;
        self->outputs_len = outputs_len;
        self->nn = libmaix_nn_create();
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
        err = self->nn->load(self->nn, &model_path, &opt_param);
        if(err != LIBMAIX_ERR_NONE)
        {
            PyErr_Format(PyExc_Exception, "libmaix_nn load fail: %s\n", libmaix_get_err_msg(err));
            goto end;
        }
        Py_DECREF(keys_inputs);
        Py_DECREF(keys_outputs);
    }

    else
    {
        PyErr_SetString(PyExc_ValueError, "now model_type only support awnn");
        return -1;
    }
    self->is_init = true;
    return 0;
end:
    /* load by libmaix API error deal*/
    if(self->nn)
    {
        libmaix_nn_destroy(&(self->nn));
    }
    libmaix_nn_module_deinit();
    /* load by libmaix API error deal end*/
    return (int)err;
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
    PyObject* o_input_bytes = NULL;
    bool o_input_bytes_need_free = false;
    libmaix_nn_layer_t* out_fmap = NULL;
    struct timeval start, end;
    int64_t interval_s;

    if(!self->is_init)
    {
        PyErr_SetString(PyExc_PermissionError, "not initialize yet");
        return NULL;
    }
    libmaix_err_t err = LIBMAIX_ERR_NONE;
    PyObject *o_inputs = NULL;
    const char* layout_str = "hwc";
    int quantize = 1;
    int debug = 0;
    static char *kwlist[] = {"inputs", "quantize", "layout", "debug", NULL};
    /* Get the buffer's memory */
    if (!PyArg_ParseTupleAndKeywords(args, kw_args, "O|$psp:forward", kwlist,
                                     &o_inputs, &quantize, &layout_str, &debug))
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
    libmaix_nn_layout_t outputs_layout = LIBMAIX_NN_LAYOUT_HWC;
    if(strcmp(layout_str, "hwc") == 0)
    {
        outputs_layout = LIBMAIX_NN_LAYOUT_HWC;
    }
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
    Py_DECREF(o_inputs_shape);

    if(PyBytes_Check(o_inputs))
    {
        o_input_bytes = o_inputs;
    }
    else if(strstr(o_inputs->ob_type->tp_name, "Image") >= 0)
    {
        o_input_bytes = PyObject_CallMethod(o_inputs, "tobytes", NULL);
        if(o_input_bytes == NULL)
        {
            PyErr_SetString(PyExc_Exception, "tobytes error");
            return NULL;
        }
        o_input_bytes_need_free = true;
        Py_DECREF(o_inputs);
    }
    else
    {
        PyErr_SetString(PyExc_ValueError, "not supported input object, only support bytes and PIL.Image object");
        return NULL;
    }
    if((Py_ssize_t)(input_w * input_h * input_c) != PyBytes_Size(o_input_bytes))
    {
        PyErr_Format(PyExc_ValueError, "input shape error, need: %d (%d, %d, %d), but: %d", input_w * input_h * input_c, input_h, input_w, input_c, PyBytes_Size(o_input_bytes));
        goto err0;
    }
    char* input_bytes = PyBytes_AsString(o_input_bytes);
    if(input_bytes == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "get bytes data error");
        goto err0;
    }

    /* libmaix API */
    libmaix_nn_layer_t input = {
        .w = input_w,
        .h = input_h,
        .c = input_c,
        .dtype = (quantize == 1 ? LIBMAIX_NN_DTYPE_INT8 : LIBMAIX_NN_DTYPE_UINT8),
        .data = input_bytes,
        .need_quantization = (quantize == 0 ? false : true),
        .buff_quantization = NULL
    };
    Py_ssize_t _size = input.w * input.h * input.c;
    if(input.need_quantization)
    {
        if(!self->quantize_buffer)
        {
            int8_t* quantize_buffer = (int8_t*)malloc(_size);
            if(!quantize_buffer)
            {
                PyErr_Format(PyExc_MemoryError, "no memory for quantize buffer, size:%d", _size);
                goto err0;
            }
            self->quantize_buffer = quantize_buffer;
        }
        input.buff_quantization = self->quantize_buffer;
    }
    // outputs
    out_fmap = (libmaix_nn_layer_t*)malloc(self->outputs_len * sizeof(libmaix_nn_layer_t));
    if(!out_fmap)
    {
        PyErr_NoMemory();
        goto err0;
    }
    PyObject* o_outputs_shape = PyDict_Values(self->outputs);
    for(int i=0; i<self->outputs_len; ++i)
    {
        PyObject* o_output_shape = PyList_GetItem(o_outputs_shape, i);
        long output_w=0, output_h=0, output_c=0;
        if(PyTuple_Check(o_output_shape))
        {
            output_h = PyLong_AsLong(PyTuple_GetItem(o_output_shape, 0));
            output_w = PyLong_AsLong(PyTuple_GetItem(o_output_shape, 1));
            output_c = PyLong_AsLong(PyTuple_GetItem(o_output_shape, 2));
        }
        else
        {
            output_h = PyLong_AsLong(PyList_GetItem(o_output_shape, 0));
            output_w = PyLong_AsLong(PyList_GetItem(o_output_shape, 1));
            output_c = PyLong_AsLong(PyList_GetItem(o_output_shape, 2));
        }
        out_fmap[i].w = output_w;
        out_fmap[i].h = output_h;
        out_fmap[i].c = output_c;
        out_fmap[i].dtype = LIBMAIX_NN_DTYPE_FLOAT;
        out_fmap[i].layout = outputs_layout;
        out_fmap[i].data = NULL;
    }
    Py_DECREF(o_outputs_shape);

    if(!self->out_buffer)
    {
        self->out_buffer = (float**)malloc(self->outputs_len * sizeof(float*));
        if(!self->out_buffer)
        {
            PyErr_Format(PyExc_MemoryError, "no memory for out buffer, need size:%d", self->outputs_len * sizeof(float*));
            goto err1;
        }
        memset(self->out_buffer, 0, self->outputs_len * sizeof(float*));
        for(int i=0; i<self->outputs_len; ++i)
        {
            _size = out_fmap[i].w * out_fmap[i].h * out_fmap[i].c * sizeof(float);
            self->out_buffer[i] = (float*)malloc(_size);
            if(!self->out_buffer[i])
            {
                free_layers_data_mem(self->out_buffer, self->outputs_len);
                PyErr_Format(PyExc_MemoryError, "no memory for out buffer, need size:%d", sizeof(float*));
                goto err1;
            }
        }
    }
    for(int i=0; i < self->outputs_len; ++i)
    {
        out_fmap[i].data = self->out_buffer[i];
    }
    if(debug)
    {
        gettimeofday(&start, NULL);
    }
    err = self->nn->forward(self->nn, &input, out_fmap);
    if(err != LIBMAIX_ERR_NONE)
    {
        PyErr_Format(PyExc_Exception, "libmaix_nn forward fail: %s\n", libmaix_get_err_msg(err));
        goto err1;
    }
    if(debug)
    {
        gettimeofday(&end, NULL );
        interval_s  =(int64_t)(end.tv_sec - start.tv_sec)*1000000ll + end.tv_usec - start.tv_usec;
        printf("forward use time: %lld us\n", interval_s);
    }

    PyObject* result = Py_None;
    PyObject* o_result_numpy2 = Py_None;

    if(self->outputs_len > 1)
    {
        result = PyList_New(0);
    }
    for(int i=0; i < self->outputs_len; ++i)
    {
        PyObject* result_bytes = PyBytes_FromStringAndSize((const char*)out_fmap[i].data, out_fmap[i].w * out_fmap[i].h * out_fmap[i].c * sizeof(float));
        PyObject *call_args = Py_BuildValue("(O)", result_bytes);
        PyObject *call_keywords = PyDict_New();
        PyObject *tmp = PyUnicode_FromString("float32");
        PyDict_SetItemString(call_keywords, "dtype", tmp);
        PyObject* o_frombuffer = PyObject_GetAttrString(self->m_numpy, "frombuffer");
        PyObject* o_result_numpy = PyObject_Call(o_frombuffer, call_args, call_keywords);
        Py_DECREF(o_frombuffer);
        Py_DECREF(tmp);
        Py_DECREF(call_keywords);
        Py_DECREF(call_args);
        Py_DECREF(result_bytes);
        if(outputs_layout == LIBMAIX_NN_LAYOUT_CHW)
        {
            o_result_numpy2 = PyObject_CallMethod(o_result_numpy, "reshape", "(iii)", out_fmap[i].c, out_fmap[i].h, out_fmap[i].w);
        }
        else
        {
            o_result_numpy2 = PyObject_CallMethod(o_result_numpy, "reshape", "(iii)", out_fmap[i].h, out_fmap[i].w, out_fmap[i].c);
        }
        Py_DECREF(o_result_numpy);
        if(result == Py_None)
        {
            result = o_result_numpy2;
            // Py_DECREF(o_result_numpy2);
            break;
        }
        PyList_Append(result, o_result_numpy2);
        Py_DECREF(o_result_numpy2);
    }
    if(out_fmap)
        free(out_fmap);
    if(o_input_bytes_need_free)
        Py_DECREF(o_input_bytes);
    return result;
err1:
    if(out_fmap)
        free(out_fmap);
err0:
    if(o_input_bytes_need_free)
        Py_DECREF(o_input_bytes);
    return NULL;
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
    Maix_NN_Model_Object_type_doc,                  /* tp_doc */
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

