#include "py_maix_nn.h"
#include <string.h>
#include "libmaix_nn.h"
#include "libmaix_image.h"
#include "libmaix_nn_face.h"
#include "stdlib.h"
#include "py_maix_nn_FaceRecognize.h"

PyDoc_STRVAR(app_FaceRecognize_type_doc, "neural network model app FaceRecognize object.\n");

static int app_FaceRecognize_init(App_FaceRecognize_Object *self, PyObject *args, PyObject *kwds);

static PyObject* app_FaceRecognize_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    App_FaceRecognize_Object* self = (App_FaceRecognize_Object*)type->tp_alloc(type, 0);
    if (!self)
    {
        PyErr_NoMemory();
        return NULL;
    }
    self->detect_model = NULL;
    self->fea_model = NULL;
    self->init = false;
    self->recognize_obj = NULL;
    self->fea_len = 0;
    self->fea_temp = NULL;
    self->fea_temp2 = NULL;
    self->pillow_image_module = NULL;
    return (PyObject*)self;
}


static void app_FaceRecognize_del(App_FaceRecognize_Object *self)
{
    if(self->pillow_image_module)
    {
        Py_DECREF(self->pillow_image_module);
        self->pillow_image_module = NULL;    
    }
    if(self->detect_model)
    {
        Py_DECREF(self->detect_model);
        self->detect_model = NULL;
    }
    if(self->fea_model)
    {
        Py_DECREF(self->fea_model);
        self->fea_model = NULL;
    }
    if(self->recognize_obj)
    {
        libmaix_nn_face_recognize_deinit(&self->recognize_obj);
    }
    if(self->fea_temp)
    {
        free(self->fea_temp);
        self->fea_temp = NULL;
    }
    if(self->fea_temp2)
    {
        free(self->fea_temp2);
        self->fea_temp2 = NULL;
    }
    self->init = false;
}

static int app_FaceRecognize_init(App_FaceRecognize_Object *self, PyObject *args, PyObject *kwds)
{
    if(self->init)
    {
        PyErr_SetString(PyExc_Exception, "already initialized");
        return -1;
    }
    libmaix_err_t err = LIBMAIX_ERR_NONE;
    static char *kwlist[] = {"model_detect", "model_fea", "fea_len", "input_shape", "threshold", "nms", "max_face_num", NULL};
    PyObject* o_input_shape = NULL;
    float threshold = 0.5, nms = 0.3;
    int max_face_num = 1;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OOIOffi:__init__", kwlist,
                                     &self->detect_model, &self->fea_model, &self->fea_len, &o_input_shape,
                                     &threshold, &nms, &max_face_num))
    {
        return -1;
    }
    bool ok = false;
    if(PyList_Check(o_input_shape))
    {
        if(PyList_Size(o_input_shape) == 3)
        {
            self->input_w = PyLong_AsLong(PyList_GetItem(o_input_shape, 0));
            self->input_h = PyLong_AsLong(PyList_GetItem(o_input_shape, 1));
            self->input_c = PyLong_AsLong(PyList_GetItem(o_input_shape, 2));   
            ok = true;
        }
    }else if(PyTuple_Check(o_input_shape))
    {
        if(PyTuple_Size(o_input_shape) == 3)
        {
           self->input_w = PyLong_AsLong(PyTuple_GetItem(o_input_shape, 0));
           self->input_h = PyLong_AsLong(PyTuple_GetItem(o_input_shape, 1));
           self->input_c = PyLong_AsLong(PyTuple_GetItem(o_input_shape, 2));
           ok = true;
        }
    }
    if(!ok)
    {
        PyErr_SetString(PyExc_ValueError, "input_shape should be (width, height, channel)");
        return -1;
    }
    self->fea_temp = (float*)malloc(self->fea_len * sizeof(float));
    if(!self->fea_temp)
    {
        PyErr_NoMemory();
        return -1;
    }
    self->fea_temp2 = (float*)malloc(self->fea_len * sizeof(float));
    if(!self->fea_temp2)
    {
        PyErr_NoMemory();
        return -1;
    }
    libmaix_nn_face_config_t config = {
        .detect_nms   = nms,
        .detect_thresh   = threshold,
        .detect_input_w   = self->input_w,
        .detect_input_h   = self->input_h
    };
    err = libmaix_nn_face_recognize_init(&self->recognize_obj, &config, self->detect_model->nn, NULL, self->fea_model->nn, self->fea_len, max_face_num, NULL, NULL);
    if(err != LIBMAIX_ERR_NONE)
    {
        PyErr_Format(PyExc_Exception, "app face_recognize init fail, code: %d", err);
        return -1;
    }
    Py_INCREF(self->detect_model);
    Py_INCREF(self->fea_model);
    self->init = true;
    return 0;
}

static PyObject *app_FaceRecognize_str(PyObject *object)
{
  char desc[] = "FaceRecognize App";
  PyObject *dev_desc = NULL;

  dev_desc = PyUnicode_FromString(desc);

  return dev_desc;
}

PyDoc_STRVAR(app_FaceRecognize_method_get_feature_doc, "get faces info from image.\n");
static PyObject* app_FaceRecognize_method_get_feature(App_FaceRecognize_Object *self, PyObject *args, PyObject *kw_args)
{
    if(!self->init)
    {
        PyErr_SetString(PyExc_Exception, "not initialize");
        return NULL;
    }

    libmaix_err_t err = LIBMAIX_ERR_NONE;
    static char *kwlist[] = {"img", "std_img", NULL};
    PyObject *img        = NULL;
    PyObject* o_input_bytes = NULL;
    bool o_input_bytes_need_free = false;
    int std_img = 0;


    if (!PyArg_ParseTupleAndKeywords(args, kw_args, "O|i:get_faces", kwlist,
                                     &img, &std_img))
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
    int face_num = 1; // only max face
    face_obj_t* face_objs = NULL;
    err = libmaix_nn_face_get_feature(self->recognize_obj, &img_arg, &face_num, NULL, &face_objs, std_img);
    if(err != LIBMAIX_ERR_NONE)
    {
        PyErr_Format(PyExc_Exception, "app FaceRecognize get feature fail, code: %d", err);
        goto err0;
    }
    if(o_input_bytes_need_free)
        Py_DECREF(o_input_bytes);
    PyObject* result = PyList_New(face_num);
    for(int i=0; i<face_num; ++i)
    {
        PyObject* item = NULL;
        if(std_img)
            item = PyList_New(5);
        else
            item = PyList_New(4);
        PyObject* feature = PyList_New(self->fea_len);
        PyObject* item_rect = PyList_New(4);
        PyObject* item_keypoints = PyList_New(5);
        for(int j = 0; j < self->fea_len; ++j)
        {
            PyList_SET_ITEM(feature, j, PyFloat_FromDouble(face_objs[i].feature[j]));
        }
        PyList_SET_ITEM(item_rect, 0, PyLong_FromLong(face_objs[i].x1));
        PyList_SET_ITEM(item_rect, 1, PyLong_FromLong(face_objs[i].y1));
        PyList_SET_ITEM(item_rect, 2, PyLong_FromLong(face_objs[i].x2 - face_objs[i].x1));
        PyList_SET_ITEM(item_rect, 3, PyLong_FromLong(face_objs[i].y2 - face_objs[i].y1));
        for(int k=0; k<5; ++k)
        {
            PyObject* o_point = PyList_New(2);
            PyList_SET_ITEM(o_point, 0, PyLong_FromLong(face_objs[i].key_point.point[k].x));
            PyList_SET_ITEM(o_point, 1, PyLong_FromLong(face_objs[i].key_point.point[k].y));
            PyList_SET_ITEM(item_keypoints, k, o_point);
        }
        // item: [ prob, [x,y,w,h], [[x,y], [x,y], [x,y], [x,y], [x,y]], feature ],
        PyList_SET_ITEM(item, 0, PyFloat_FromDouble(face_objs[i].prob));
        PyList_SET_ITEM(item, 1, item_rect);
        PyList_SET_ITEM(item, 2, item_keypoints);
        PyList_SET_ITEM(item, 3, feature);
        if(std_img)
        {
            if(!self->pillow_image_module)
            {
                self->pillow_image_module = PyImport_ImportModule("PIL.Image");
                if(!self->pillow_image_module)
                {
                    PyErr_SetString(PyExc_EnvironmentError, "need PIL.Image module");
                    return NULL;
                }
            }
            PyObject *o_size = PyTuple_New(2);
            PyTuple_SetItem(o_size, 0, PyLong_FromLong(face_objs[i].std_img.width));
            PyTuple_SetItem(o_size, 1, PyLong_FromLong(face_objs[i].std_img.height));
            PyObject *call_args = Py_BuildValue("(sOy#)", "RGB", o_size, (const char*)face_objs[i].std_img.data, face_objs[i].std_img.width * face_objs[i].std_img.height * 3);
            PyObject *call_keywords = PyDict_New();
            PyObject* o_result_pil_img = PyObject_Call(PyObject_GetAttrString(self->pillow_image_module, "frombytes"), call_args, call_keywords);
            Py_DECREF(call_args); Py_DECREF(call_keywords);
            Py_DECREF(o_size);
            PyList_SET_ITEM(item, 4, o_result_pil_img);
        }
        PyList_SET_ITEM(result, i, item);
    }
    return result;
err0:
    if(o_input_bytes_need_free)
        Py_DECREF(o_input_bytes);
    return NULL;
}


PyDoc_STRVAR(app_FaceRecognize_method_compare_doc, "compare feature, return score: 0.0~100.0.\n");
static PyObject* app_FaceRecognize_method_compare(App_FaceRecognize_Object *self, PyObject *args, PyObject *kw_args)
{
    if(!self->init)
    {
        PyErr_SetString(PyExc_Exception, "not initialize");
        return NULL;
    }

    static char *kwlist[] = {"feature_a", "feature_b", NULL};
    PyObject *fea_a = NULL;
    PyObject* fea_b = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kw_args, "OO:predict", kwlist,
                                     &fea_a, &fea_b))
    {
        return NULL;
    }
    if(!PyList_Check(fea_a) || !PyList_Check(fea_a))
    {
        PyErr_Format(PyExc_ValueError, "should be list");
        return NULL;
    }
    for(int i = 0; i < self->fea_len; ++i)
    {
        self->fea_temp[i] = (float)PyFloat_AsDouble(PyList_GetItem(fea_a, i));
        self->fea_temp2[i] = (float)PyFloat_AsDouble(PyList_GetItem(fea_b, i));
    }
    float  score = libmaix_nn_feature_compare_float(self->fea_temp, self->fea_temp2, self->fea_len);

    return PyFloat_FromDouble(score);
}

static PyMethodDef app_FaceRecognize_methods[] = {
    {"get_faces", (PyCFunction)app_FaceRecognize_method_get_feature, METH_VARARGS | METH_KEYWORDS, app_FaceRecognize_method_get_feature_doc},
    {"compare", (PyCFunction)app_FaceRecognize_method_compare, METH_VARARGS | METH_KEYWORDS, app_FaceRecognize_method_compare_doc},
    {NULL, NULL, 0, NULL},
};


PyTypeObject PyMaix_NN_app_FaceRecognize_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    maix_nn_app_FaceRecognize_type_name,                  /* tp_name */
    sizeof(App_FaceRecognize_Object),                      /* tp_basicsize */
    0,                                        /* tp_itemsize */
    (destructor)app_FaceRecognize_del,                    /* tp_dealloc */
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
    app_FaceRecognize_str,                                /* tp_str */
    0,                                        /* tp_getattro */
    0,                                        /* tp_setattro */
    0,                                        /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    app_FaceRecognize_type_doc,                  /* tp_doc */
    0,                                        /* tp_traverse */
    0,                                        /* tp_clear */
    0,                                        /* tp_richcompare */
    0,                                        /* tp_weaklistoffset */
    0,                                        /* tp_iter */
    0,                                        /* tp_iternext */
    app_FaceRecognize_methods,                            /* tp_methods */
    0,                                        /* tp_members */
    0,                                        /* tp_getset */
    0,                                        /* tp_base */
    0,                                        /* tp_dict */
    0,                                        /* tp_descr_get */
    0,                                        /* tp_descr_set */
    0,                                        /* tp_dictoffset */
    (initproc)app_FaceRecognize_init,             /* tp_init */
    0,                                        /* tp_alloc */
    app_FaceRecognize_new,                        /* tp_new */
};

