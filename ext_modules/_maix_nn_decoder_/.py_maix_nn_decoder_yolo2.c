#include "py_maix_nn.h"
#include <string.h>
#include "libmaix_nn.h"
#include "stdlib.h"
#include "libmaix_nn_decoder_yolo2.h"


typedef struct
{
    PyObject_HEAD;
    PyObject* m_numpy;
    libmaix_nn_decoder_yolo2_config_t config;
    libmaix_nn_decoder_t* decoder;
    bool init;
} DecoderYolo2Object;

PyDoc_STRVAR(decoder_yolo2_type_doc, "neural network model Yolo2 decoder object.\n");

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

static int decoder_yolo2_init(DecoderYolo2Object *self, PyObject *args, PyObject *kwds);

static PyObject* decoder_yolo2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DecoderYolo2Object* self = (DecoderYolo2Object*)type->tp_alloc(type, 0);
    if (!self)
    {
        PyErr_NoMemory();
        return NULL;
    }
    return (PyObject*)self;
}


static void decoder_yolo2_del(DecoderYolo2Object *self)
{
    if(self->config.anchors)
    {
        free(self->config.anchors);
        self->config.anchors = NULL;
    }
    Py_DECREF(self->m_numpy);
    if(self->decoder)
    {
        self->decoder->deinit(self->decoder);
        libmaix_nn_decoder_yolo2_destroy(&self->decoder);
    }
    self->init = false;
}

static int decoder_yolo2_init(DecoderYolo2Object *self, PyObject *args, PyObject *kwds)
{
    if(self->init)
    {
        PyErr_SetString(PyExc_Exception, "already initialized");
        return -1;
    }
    libmaix_err_t err = LIBMAIX_ERR_NONE;
    static char *kwlist[] = {"class_num", "anchors", "net_in_size", "net_out_size", NULL};
    
    PyObject *anchors        = NULL;
    PyObject *net_in_zie     = NULL;
    PyObject *net_out_zie     = NULL;
    bool anchors_nedd_decref = false;

    self->m_numpy = PyImport_ImportModule("numpy");
    if(!self->m_numpy)
    {
        PyErr_SetString(PyExc_Exception, "need numpy module");
        return -1;
    }

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "IO|$OO:__init__", kwlist,
                                     &self->config.classes_num, &anchors, &net_in_zie, &net_out_zie))
    {
        return -1;
    }
    if(PyList_Check(anchors))
    {
        anchors = PyList_AsTuple(anchors);
        anchors_nedd_decref = true;
    }
    self->config.anchors = (float*)malloc(PyTuple_Size(anchors) * sizeof(float));
    if(!self->config.anchors)
    {
        PyErr_NoMemory();
        return -1;
    }
    self->config.anchors_num = PyTuple_Size(anchors) / 2;
    for(int i = 0; i < PyTuple_Size(anchors); ++i)
    {
        self->config.anchors[i] = (float)PyFloat_AsDouble(PyTuple_GetItem(anchors, i));
    }
    if(anchors_nedd_decref)
    {
        Py_DECREF(anchors);
    }
    if(net_in_zie)
    {
        self->config.net_in_width  = PyLong_AsLong(PyTuple_GetItem(net_in_zie, 0));
        self->config.net_in_height = PyLong_AsLong(PyTuple_GetItem(net_in_zie, 1));
    }
    else
    {
        self->config.net_in_width  = 224;
        self->config.net_in_height = 224;
    }
    if(net_out_zie)
    {
        self->config.net_out_width  = PyLong_AsLong(PyTuple_GetItem(net_out_zie, 0));
        self->config.net_out_height = PyLong_AsLong(PyTuple_GetItem(net_out_zie, 1));
    }
    else
    {
        self->config.net_out_width  = 7;
        self->config.net_out_height = 7;
    }
    self->config.input_width = self->config.net_in_width;
    self->config.input_height = self->config.net_in_height;
    self->config.nms_value = 0.3;
    self->config.threshold = 0.5;

    self->decoder = libmaix_nn_decoder_yolo2_create();
    if(!self->decoder)
    {
        PyErr_SetString(PyExc_Exception, "create yolo2 decoder fail");
        return -1;
    }
    err = self->decoder->init(self->decoder, (void*)&self->config);
    if(err != LIBMAIX_ERR_NONE)
    {
        PyErr_Format(PyExc_Exception, "yolo2 decoder init fail, code: %d", err);
        return -1;
    }

    self->init = true;
    return 0;
}

static PyObject *decoder_yolo2_str(PyObject *object)
{
  char desc[] = "Yolo2 decoder object";
  PyObject *dev_desc = NULL;

  dev_desc = PyUnicode_FromString(desc);

  return dev_desc;
}

PyDoc_STRVAR(decoder_yolo2_method_run_doc, "run decode process.\n");
static PyObject* decoder_yolo2_method_run(DecoderYolo2Object *self, PyObject *args, PyObject *kw_args)
{
    if(!self->init)
    {
        PyErr_SetString(PyExc_Exception, "not initialize");
        return NULL;
    }

    libmaix_err_t err = LIBMAIX_ERR_NONE;
    static char *kwlist[] = {"fmap", "nms", "threshold", "img_size", NULL};
    PyObject *fmap        = NULL;
    float   nms          = 0.3;
    float   threshold    = 0.5;
    PyObject *img_size    = NULL;
    bool fmap_need_decref = false;
    if (!PyArg_ParseTupleAndKeywords(args, kw_args, "O|$ffO:__init__", kwlist,
                                     &fmap, &nms, &threshold, &img_size))
    {
        return NULL;
    }
    if(img_size)
    {
        self->config.input_width  = PyLong_AsLong(PyTuple_GetItem(img_size, 0));
        self->config.input_height = PyLong_AsLong(PyTuple_GetItem(img_size, 1));
    }
    else
    {
        self->config.input_width = self->config.net_in_width;
        self->config.input_height = self->config.net_in_height;
    }
    self->config.nms_value = nms;
    self->config.threshold = threshold;

    libmaix_nn_layer_t out_fmap = {
        .w = self->config.net_out_width,
        .h = self->config.net_out_height,
        .c = (self->config.classes_num + 5) * self->config.anchors_num,
        .dtype = LIBMAIX_NN_DTYPE_FLOAT,
        .layout = LIBMAIX_NN_LAYOUT_HWC,
        .data = NULL
    };
    PyObject* o_out_fmap = NULL;
    if(strstr(fmap->ob_type->tp_name, "ndarray") > 0)
    {
        o_out_fmap = PyObject_CallMethod(fmap, "tobytes", NULL);
        if(o_out_fmap == NULL)
        {
            PyErr_SetString(PyExc_ValueError, "ndarray tobytes() error");
            return NULL;
        }
        fmap_need_decref = true;
    }
    else if(PyBytes_Check(fmap))
    {
        o_out_fmap = fmap;
    }
    else
    {
        PyErr_SetString(PyExc_ValueError, "not supported fmap object, should be bytes or numpy.ndarray");
        return NULL;
    }
    int expected_size = out_fmap.w * out_fmap.h * out_fmap.c * sizeof(float);
    if(PyBytes_Size(o_out_fmap) != expected_size)
    {
        PyErr_Format(PyExc_ValueError, "fmap size error, need: %d, but: %d", expected_size, PyBytes_Size(o_out_fmap));
        if(fmap_need_decref)
            Py_DECREF(o_out_fmap);
        return NULL;
    }
    out_fmap.data = PyBytes_AsString(o_out_fmap);

    libmaix_nn_decoder_yolo2_result_t yolo2_result;
    err = self->decoder->decode(self->decoder, &out_fmap, (void*)&yolo2_result);
    if(fmap_need_decref)
        Py_DECREF(o_out_fmap);
    if(err != LIBMAIX_ERR_NONE)
    {
        PyErr_Format(PyExc_Exception, "decode fail, code: %d", err);
        return NULL;
    }
    PyObject* boxes = PyList_New(0);
    PyObject* probs = PyList_New(0);
    if(yolo2_result.boxes_num > 0)
    {
        for (uint32_t i = 0; i < yolo2_result.boxes_num; ++i)
        {
            int class_id = max_index(yolo2_result.probs[i], self->config.classes_num);
            float prob = yolo2_result.probs[i][class_id];
            if (prob > threshold)
            {
                libmaix_nn_decoder_yolo2_box_t *b = yolo2_result.boxes + i;
                if(img_size)
                {
                    uint32_t x1 = (uint32_t)(b->x * self->config.input_width - (b->w * self->config.input_width / 2.));
                    uint32_t y1 = (uint32_t)(b->y * self->config.input_height - (b->h * self->config.input_height / 2.));
                    uint32_t w  = (uint32_t)(b->w * self->config.input_width);
                    uint32_t h  = (uint32_t)(b->h * self->config.input_height);
                    PyObject* box = PyList_New(4);
                    PyObject* tmp_o = PyLong_FromLong(x1);
                    printf("yinyong1 :%d \r\n",tmp_o->ob_refcnt);
                    PyList_SetItem(box, 0, tmp_o);
                    printf("yinyong2 :%d \r\n",tmp_o->ob_refcnt);
                    PyList_SetItem(box, 1, PyLong_FromLong(y1));
                    PyList_SetItem(box, 2, PyLong_FromLong(w));
                    PyList_SetItem(box, 3, PyLong_FromLong(h));
                    PyList_Append(boxes, box);
                    Py_DECREF(box);
                }
                else
                {
                    PyObject* box = PyList_New(4);
                    PyList_SetItem(box, 0, PyFloat_FromDouble(b->x - b->w / 2.));
                    PyList_SetItem(box, 1, PyFloat_FromDouble(b->y - b->h / 2.));
                    PyList_SetItem(box, 2, PyFloat_FromDouble(b->w));
                    PyList_SetItem(box, 3, PyFloat_FromDouble(b->h));
                    PyList_Append(boxes, box);
                    Py_DECREF(box);
                }
                PyObject* o_probs = PyList_New(0);
                for(uint32_t j=0; j<self->config.classes_num; ++j)
                {
                    PyObject* tmp_o = PyFloat_FromDouble(yolo2_result.probs[i][j]);
                    PyList_Append(o_probs, tmp_o);
                    Py_DECREF(tmp_o);
                }
                PyObject* o_probs_item = PyList_New(2);
                PyList_SetItem(o_probs_item, 0, PyLong_FromLong(class_id));
                PyList_SetItem(o_probs_item, 1, o_probs);
                PyList_Append(probs, o_probs_item);
                // Py_DECREF(o_probs);
                Py_DECREF(o_probs_item);
            }
        }
    }

    PyObject* final_result = PyList_New(0);
    PyList_Append(final_result, boxes);
    PyList_Append(final_result, probs);
    Py_DECREF(boxes);
    Py_DECREF(probs);
    return final_result;
}



static PyMethodDef decoder_yolo2_methods[] = {
    {"run", (PyCFunction)decoder_yolo2_method_run, METH_VARARGS | METH_KEYWORDS, decoder_yolo2_method_run_doc},
    {NULL, NULL, 0, NULL},
};


PyTypeObject PyMaix_NN_Decoder_Yolo2_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    maix_nn_decoder_yolo2_type_name,                  /* tp_name */
    sizeof(DecoderYolo2Object),                      /* tp_basicsize */
    0,                                        /* tp_itemsize */
    (destructor)decoder_yolo2_del,                    /* tp_dealloc */
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
    decoder_yolo2_str,                                /* tp_str */
    0,                                        /* tp_getattro */
    0,                                        /* tp_setattro */
    0,                                        /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    decoder_yolo2_type_doc,                  /* tp_doc */
    0,                                        /* tp_traverse */
    0,                                        /* tp_clear */
    0,                                        /* tp_richcompare */
    0,                                        /* tp_weaklistoffset */
    0,                                        /* tp_iter */
    0,                                        /* tp_iternext */
    decoder_yolo2_methods,                            /* tp_methods */
    0,                                        /* tp_members */
    0,                                        /* tp_getset */
    0,                                        /* tp_base */
    0,                                        /* tp_dict */
    0,                                        /* tp_descr_get */
    0,                                        /* tp_descr_set */
    0,                                        /* tp_dictoffset */
    (initproc)decoder_yolo2_init,             /* tp_init */
    0,                                        /* tp_alloc */
    decoder_yolo2_new,                        /* tp_new */
};

