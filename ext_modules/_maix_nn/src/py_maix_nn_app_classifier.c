#include "py_maix_nn.h"
#include <string.h>
#include "libmaix_nn.h"
#include "stdlib.h"
#include "libmaix_nn_classifier.h"
#include "py_maix_nn_classifier.h"

PyDoc_STRVAR(app_classifier_type_doc, "neural network model app classifier object.\n");

static int app_classifier_init(App_Classifier_Object *self, PyObject *args, PyObject *kwds);

static PyObject* app_classifier_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    App_Classifier_Object* self = (App_Classifier_Object*)type->tp_alloc(type, 0);
    if (!self)
    {
        PyErr_NoMemory();
        return NULL;
    }
    self->init = false;
    self->class_num = 0;
    self->sample_num = 0;
    self->feature_len = 0;
    self->input_w = 0;
    self->input_h = 0;
    self->classifier_obj = NULL;
    return (PyObject*)self;
}


static void app_classifier_del(App_Classifier_Object *self)
{
    if(self->nn_model)
    {
        Py_DECREF(self->nn_model);
        self->nn_model = NULL;
    }
    if(self->classifier_obj)
    {
        libmaix_classifier_del(&self->classifier_obj);
    }
    self->init = false;
}

static int app_classifier_init(App_Classifier_Object *self, PyObject *args, PyObject *kwds)
{
    if(self->init)
    {
        PyErr_SetString(PyExc_Exception, "already initialized");
        return -1;
    }
    libmaix_err_t err = LIBMAIX_ERR_NONE;
    static char *kwlist[] = {"model", "class_num", "sample_num", "feature_len", "input_w", "input_h", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OIIIII:__init__", kwlist,
                                     &self->nn_model, &self->class_num, &self->sample_num, &self->feature_len, &self->input_w, &self->input_h))
    {
        return -1;
    }
    err = libmaix_classifier_init(&self->classifier_obj, self->nn_model->nn,
                                   self->feature_len, self->input_w, self->input_h, self->class_num, self->sample_num);
    if(err != LIBMAIX_ERR_NONE)
    {
        PyErr_Format(PyExc_Exception, "app classifier init fail, code: %d", err);
        return -1;
    }

    Py_INCREF(self->nn_model);
    self->init = true;
    return 0;
}

static PyObject *app_classifier_str(PyObject *object)
{
  char desc[] = "Classifier App";
  PyObject *dev_desc = NULL;

  dev_desc = PyUnicode_FromString(desc);

  return dev_desc;
}

PyDoc_STRVAR(app_classifier_method_add_class_img_doc, "add class img(feature).\n");
static PyObject* app_classifier_method_add_class_img(App_Classifier_Object *self, PyObject *args, PyObject *kw_args)
{
    if(!self->init)
    {
        PyErr_SetString(PyExc_Exception, "not initialize");
        return NULL;
    }

    libmaix_err_t err = LIBMAIX_ERR_NONE;
    static char *kwlist[] = {"img", NULL};
    PyObject *img        = NULL;
    PyObject* o_input_bytes = NULL;
    bool o_input_bytes_need_free = false;

    if (!PyArg_ParseTupleAndKeywords(args, kw_args, "O:add_class_img", kwlist,
                                     &img))
    {
        return NULL;
    }
    if(PyBytes_Check(img))
    {
        o_input_bytes = img;
    }
    else if(strstr(img->ob_type->tp_name, "Image") >= 0)
    {
        o_input_bytes = PyObject_CallMethod(img, "tobytes", NULL);
        if(o_input_bytes == NULL)
        {
            PyErr_SetString(PyExc_Exception, "tobytes error");
            return NULL;
        }
        o_input_bytes_need_free = true;
    }
    else
    {
        PyErr_SetString(PyExc_ValueError, "not supported input object, only support bytes and PIL.Image object");
        return NULL;
    }
    if((Py_ssize_t)(self->input_w * self->input_h * 3) != PyBytes_Size(o_input_bytes))
    {
        PyErr_Format(PyExc_ValueError, "input shape error, need: %d (%d, %d, %d), but: %d", self->input_w * self->input_h * 3, self->input_h, self->input_w, 3, PyBytes_Size(o_input_bytes));
        goto err0;
    }
    char* input_bytes = PyBytes_AsString(o_input_bytes);
    if(input_bytes == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "get bytes data error");
        goto err0;
    }
    libmaix_image_t img_arg = {
        .width = self->input_w,
        .height = self->input_h,
        .layout = LIBMAIX_NN_LAYOUT_HWC,
        .mode = LIBMAIX_IMAGE_MODE_RGB888,
        .is_data_alloc = false,
        .data = input_bytes
    };
    int idx = -1;
    err = libmaix_classifier_add_class_img(self->classifier_obj, &img_arg, &idx);
    if(err != LIBMAIX_ERR_NONE)
    {
        PyErr_Format(PyExc_Exception, "app classifier add_class_img fail, code: %d", err);
        goto err0;
    }

    if(o_input_bytes_need_free)
        Py_DECREF(o_input_bytes);
    return PyLong_FromLong(idx);
err0:
    if(o_input_bytes_need_free)
        Py_DECREF(o_input_bytes);
    return NULL;
}


PyDoc_STRVAR(app_classifier_method_add_sample_img_doc, "add sample img(feature).\n");
static PyObject* app_classifier_method_add_sample_img(App_Classifier_Object *self, PyObject *args, PyObject *kw_args)
{
    if(!self->init)
    {
        PyErr_SetString(PyExc_Exception, "not initialize");
        return NULL;
    }

    libmaix_err_t err = LIBMAIX_ERR_NONE;
    static char *kwlist[] = {"img", NULL};
    PyObject *img        = NULL;
    PyObject* o_input_bytes = NULL;
    bool o_input_bytes_need_free = false;

    if (!PyArg_ParseTupleAndKeywords(args, kw_args, "O:add_sample_img", kwlist,
                                     &img))
    {
        return NULL;
    }
    if(PyBytes_Check(img))
    {
        o_input_bytes = img;
    }
    else if(strstr(img->ob_type->tp_name, "Image") >= 0)
    {
        o_input_bytes = PyObject_CallMethod(img, "tobytes", NULL);
        if(o_input_bytes == NULL)
        {
            PyErr_SetString(PyExc_Exception, "tobytes error");
            return NULL;
        }
        o_input_bytes_need_free = true;
    }
    else
    {
        PyErr_SetString(PyExc_ValueError, "not supported input object, only support bytes and PIL.Image object");
        return NULL;
    }
    if((Py_ssize_t)(self->input_w * self->input_h * 3) != PyBytes_Size(o_input_bytes))
    {
        PyErr_Format(PyExc_ValueError, "input shape error, need: %d (%d, %d, %d), but: %d", self->input_w * self->input_h * 3, self->input_h, self->input_w, 3, PyBytes_Size(o_input_bytes));
        goto err0;
    }
    char* input_bytes = PyBytes_AsString(o_input_bytes);
    if(input_bytes == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "get bytes data error");
        goto err0;
    }
    libmaix_image_t img_arg = {
        .width = self->input_w,
        .height = self->input_h,
        .layout = LIBMAIX_NN_LAYOUT_HWC,
        .mode = LIBMAIX_IMAGE_MODE_RGB888,
        .is_data_alloc = false,
        .data = input_bytes
    };
    int idx = -1;
    err = libmaix_classifier_add_sample_img(self->classifier_obj, &img_arg, &idx);
    if(err != LIBMAIX_ERR_NONE)
    {
        PyErr_Format(PyExc_Exception, "app classifier add_sample_img fail, code: %d", err);
        goto err0;
    }

    if(o_input_bytes_need_free)
        Py_DECREF(o_input_bytes);
    return PyLong_FromLong(idx);
err0:
    if(o_input_bytes_need_free)
        Py_DECREF(o_input_bytes);
    return NULL;
}

PyDoc_STRVAR(app_classifier_method_train_doc, "train.\n");
static PyObject* app_classifier_method_train(App_Classifier_Object *self, PyObject *args, PyObject *kw_args)
{
    if(!self->init)
    {
        PyErr_SetString(PyExc_Exception, "not initialize");
        return NULL;
    }
    libmaix_err_t err = LIBMAIX_ERR_NONE;

    err = libmaix_classifier_train(self->classifier_obj);
    if(err != LIBMAIX_ERR_NONE)
    {
        PyErr_Format(PyExc_Exception, "train error, code:%d\n", err);
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}


PyDoc_STRVAR(app_classifier_method_predict_doc, "predict class.\n");
static PyObject* app_classifier_method_predict(App_Classifier_Object *self, PyObject *args, PyObject *kw_args)
{
    if(!self->init)
    {
        PyErr_SetString(PyExc_Exception, "not initialize");
        return NULL;
    }

    libmaix_err_t err = LIBMAIX_ERR_NONE;
    static char *kwlist[] = {"img", NULL};
    PyObject *img        = NULL;
    PyObject* o_input_bytes = NULL;
    bool o_input_bytes_need_free = false;

    if (!PyArg_ParseTupleAndKeywords(args, kw_args, "O:predict", kwlist,
                                     &img))
    {
        return NULL;
    }
    if(PyBytes_Check(img))
    {
        o_input_bytes = img;
    }
    else if(strstr(img->ob_type->tp_name, "Image") >= 0)
    {
        o_input_bytes = PyObject_CallMethod(img, "tobytes", NULL);
        if(o_input_bytes == NULL)
        {
            PyErr_SetString(PyExc_Exception, "tobytes error");
            return NULL;
        }
        o_input_bytes_need_free = true;
    }
    else
    {
        PyErr_SetString(PyExc_ValueError, "not supported input object, only support bytes and PIL.Image object");
        return NULL;
    }
    if((Py_ssize_t)(self->input_w * self->input_h * 3) != PyBytes_Size(o_input_bytes))
    {
        PyErr_Format(PyExc_ValueError, "input shape error, need: %d (%d, %d, %d), but: %d", self->input_w * self->input_h * 3, self->input_h, self->input_w, 3, PyBytes_Size(o_input_bytes));
        goto err0;
    }
    char* input_bytes = PyBytes_AsString(o_input_bytes);
    if(input_bytes == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "get bytes data error");
        goto err0;
    }
    libmaix_image_t img_arg = {
        .width = self->input_w,
        .height = self->input_h,
        .layout = LIBMAIX_NN_LAYOUT_HWC,
        .mode = LIBMAIX_IMAGE_MODE_RGB888,
        .is_data_alloc = false,
        .data = input_bytes
    };
    float distance = 9999;
    int idx = -1;
    err = libmaix_classifier_predict(self->classifier_obj, &img_arg, &idx, &distance);
    if(err != LIBMAIX_ERR_NONE)
    {
        PyErr_Format(PyExc_Exception, "app classifier predict fail, code: %d", err);
        goto err0;
    }

    if(o_input_bytes_need_free)
        Py_DECREF(o_input_bytes);
    PyObject* result = PyTuple_Pack(2, PyLong_FromLong(idx), PyLong_FromLong(distance));
    return result;
err0:
    if(o_input_bytes_need_free)
        Py_DECREF(o_input_bytes);
    return NULL;
}

PyDoc_STRVAR(app_classifier_method_save_doc, "save features.\n");
static PyObject* app_classifier_method_save(App_Classifier_Object *self, PyObject *args, PyObject *kw_args)
{
    if(!self->init)
    {
        PyErr_SetString(PyExc_Exception, "not initialize");
        return NULL;
    }

    libmaix_err_t err = LIBMAIX_ERR_NONE;
    static char *kwlist[] = {"path", NULL};
    const char* path = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kw_args, "s:save", kwlist,
                                     &path))
    {
        return NULL;
    }
    if(!path)
    {
        PyErr_Format(PyExc_ValueError, "path error, should be str type");
        return NULL;
    }
    err = libmaix_classifier_save(self->classifier_obj, path);
    if(err != LIBMAIX_ERR_NONE)
    {
        PyErr_Format(PyExc_Exception, "app classifier save fail, code: %d", err);
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}


static PyMethodDef app_classifier_methods[] = {
    {"add_class_img", (PyCFunction)app_classifier_method_add_class_img, METH_VARARGS | METH_KEYWORDS, app_classifier_method_add_class_img_doc},
    {"add_sample_img", (PyCFunction)app_classifier_method_add_sample_img, METH_VARARGS | METH_KEYWORDS, app_classifier_method_add_sample_img_doc},
    {"train", (PyCFunction)app_classifier_method_train, METH_VARARGS | METH_KEYWORDS, app_classifier_method_train_doc},
    {"predict", (PyCFunction)app_classifier_method_predict, METH_VARARGS | METH_KEYWORDS, app_classifier_method_predict_doc},
    {"save", (PyCFunction)app_classifier_method_save, METH_VARARGS | METH_KEYWORDS, app_classifier_method_save_doc},
    {NULL, NULL, 0, NULL},
};


PyTypeObject PyMaix_NN_app_Classifier_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    maix_nn_app_classifier_type_name,                  /* tp_name */
    sizeof(App_Classifier_Object),                      /* tp_basicsize */
    0,                                        /* tp_itemsize */
    (destructor)app_classifier_del,                    /* tp_dealloc */
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
    app_classifier_str,                                /* tp_str */
    0,                                        /* tp_getattro */
    0,                                        /* tp_setattro */
    0,                                        /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    app_classifier_type_doc,                  /* tp_doc */
    0,                                        /* tp_traverse */
    0,                                        /* tp_clear */
    0,                                        /* tp_richcompare */
    0,                                        /* tp_weaklistoffset */
    0,                                        /* tp_iter */
    0,                                        /* tp_iternext */
    app_classifier_methods,                            /* tp_methods */
    0,                                        /* tp_members */
    0,                                        /* tp_getset */
    0,                                        /* tp_base */
    0,                                        /* tp_dict */
    0,                                        /* tp_descr_get */
    0,                                        /* tp_descr_set */
    0,                                        /* tp_dictoffset */
    (initproc)app_classifier_init,             /* tp_init */
    0,                                        /* tp_alloc */
    app_classifier_new,                        /* tp_new */
};

