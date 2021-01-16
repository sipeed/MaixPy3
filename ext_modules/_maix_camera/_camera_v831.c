
#include "_maix_camera.h"

#ifdef V831Camera

#include "libmaix_cam.h"
#include "libmaix_image.h"

PyDoc_STRVAR(V831CameraObject_type_doc, "V831Camera(width, height) -> V831Camera object.\n");
typedef struct
{
    PyObject_HEAD;
    unsigned int width, height;

    libmaix_cam_t *cam;
    libmaix_image_t* yuv_img;
    libmaix_image_t* rgb_img;

} V831CameraObject;

PyDoc_STRVAR(V831Camera_close_doc, "close()\n\nClose V831Camera device.\n");
static PyObject *V831Camera_close(V831CameraObject *self)
{

    if (NULL != self->cam)
        libmaix_cam_destroy(&self->cam);
    if (NULL != self->rgb_img) {
        libmaix_image_destroy(&self->rgb_img);
    }
    if (NULL != self->yuv_img) {
        libmaix_image_destroy(&self->yuv_img);
    }
    
    libmaix_image_module_deinit();

    Py_RETURN_NONE;
}

static PyObject *V831Camera_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    V831CameraObject *self;

    if ((self = (V831CameraObject *)type->tp_alloc(type, 0)) == NULL)
    {
        return NULL;
    }

    return (PyObject *)self;
}

static void V831Camera_free(V831CameraObject *self)
{
    PyObject *ref = V831Camera_close(self);
    Py_XDECREF(ref);

    Py_TYPE(self)->tp_free((PyObject *)self);
}

static int V831Camera_init(V831CameraObject *self, PyObject *args, PyObject *kwds)
{
    // default init value
    self->width = 640, self->height = 480;

    static char *kwlist[] = {"width", "height", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ii:__init__", kwlist,
                                     &self->width, &self->height))
    {
        return -1;
    }

    libmaix_image_module_init();
    self->yuv_img = libmaix_image_create(self->width, self->height, LIBMAIX_IMAGE_MODE_YUV420SP_NV21, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
    if(NULL != self->yuv_img)
    {
      self->rgb_img = libmaix_image_create(self->width, self->height, LIBMAIX_IMAGE_MODE_RGB888, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
      if(NULL != self->rgb_img)
      {
        self->cam = libmaix_cam_creat(self->width, self->height);
        if (NULL != self->cam)
        {
            int ret = self->cam->strat_capture(self->cam);
            if (0 == ret)
            {
                return 0;
            }
        }
      }
    }
    V831Camera_close(self);
    PyErr_SetFromErrno(PyExc_IOError);

    return -1;
}

static PyObject *V831Camera_enter(PyObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    Py_INCREF(self);
    return self;
}

static PyObject *V831Camera_exit(V831CameraObject *self, PyObject *args)
{
    PyObject *exc_type = 0;
    PyObject *exc_value = 0;
    PyObject *traceback = 0;

    if (!PyArg_UnpackTuple(args, "__exit__", 3, 3, &exc_type, &exc_value, &traceback))
    {
        return 0;
    }

    Py_XDECREF(self);
    V831Camera_close(self);
    Py_RETURN_NONE;
}

/* str */
static PyObject *V831Camera_str(PyObject *object)
{
  return PyUnicode_FromString(__FILE__);
}

PyDoc_STRVAR(V831Camera_read_doc, "read()\n\nRead image(rgb888) bytes data from device.\n");
static PyObject *V831Camera_read(V831CameraObject *self, PyObject *args)
{
    PyObject *bytes = NULL;

    char *buf = NULL;
    size_t len = self->width * self->height * 3;
    self->yuv_img->mode = LIBMAIX_IMAGE_MODE_YUV420SP_NV21;
    libmaix_err_t ret = self->cam->capture(self->cam, (unsigned char*)self->yuv_img->data);
    if(ret == LIBMAIX_ERR_NONE)
    {
      libmaix_err_t err = self->yuv_img->convert(self->yuv_img, LIBMAIX_IMAGE_MODE_RGB888, &self->rgb_img);
      if(err == LIBMAIX_ERR_NONE)
      {
        buf = self->rgb_img->data;
      }
      else
      {
        ret = err;
        // printf("convert fail: %s\n", libmaix_get_err_msg(err));
      }
    }
    /* Copy data to bytearray and return */
    if (buf != NULL) {
      bytes = PyBytes_FromStringAndSize(buf, len);
    } else {
      bytes = Py_None;
    }
    // Py_INCREF(bytes); // cancel ref Tag
    PyObject* list = PyList_New(2);
    if (!list) Py_RETURN_NONE;
    PyList_SET_ITEM(list, 0, PyBool_FromLong(!ret));
    PyList_SET_ITEM(list, 1, bytes);
    return list;
}

static PyMethodDef V831Camera_methods[] = {

    {"read", (PyCFunction)V831Camera_read, METH_VARARGS, V831Camera_read_doc},
    {"close", (PyCFunction)V831Camera_close, METH_NOARGS, V831Camera_close_doc},
    {"__enter__", (PyCFunction)V831Camera_enter, METH_NOARGS, NULL},
    {"__exit__", (PyCFunction)V831Camera_exit, METH_NOARGS, NULL},
    {NULL},
};

/* Check */
static int check_user_input(const char *name, PyObject *input, int min, int max)
{

    int value;

    if (input == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the last attribute");
        return -1;
    }

    /* Other attribute, integer type */
    if (!PyInt_Check(input))
    {
        PyErr_SetString(PyExc_TypeError, "The last attribute value must be an integer");
        return -1;
    }

    /* Convert value to int */
    value = PyLong_AsLong(input);

    /* Check input value range */
    if (value < min || value > max)
    {
        PyErr_Format(PyExc_ValueError, "invalid ' %s'(%d - %d)", name, min, max);
        return -1;
    }

    return 0;
}

/* width */
PyDoc_STRVAR(V831Camera_width_doc, "V831Camera internal operate width.\n\n");
static PyObject *V831Camera_get_width(V831CameraObject *self, void *closure)
{
    PyObject *result = Py_BuildValue("i", self->width);
    return result;
}

static int V831Camera_set_width(V831CameraObject *self, PyObject *value, void *closeure)
{
    if (check_user_input("width", value, 640, 480) != 0)
    {
        return -1;
    }
    self->width = PyLong_AsLong(value);
    return 0;
}

/* height */
PyDoc_STRVAR(V831Camera_height_doc, "V831Camera internal operate height.\n\n");
static PyObject *V831Camera_get_height(V831CameraObject *self, void *closure)
{
    PyObject *result = Py_BuildValue("i", self->height);
    return result;
}

static int V831Camera_set_height(V831CameraObject *self, PyObject *value, void *closeure)
{
    if (check_user_input("height", value, 640, 480) != 0)
    {
        return -1;
    }
    self->height = PyLong_AsLong(value);
    return 0;
}

static PyGetSetDef V831Camera_getseters[] = {
    {"width", (getter)V831Camera_get_width, (setter)V831Camera_set_width, V831Camera_width_doc},
    {"height", (getter)V831Camera_get_height, (setter)V831Camera_set_height, V831Camera_height_doc},
    {NULL},
};

PyTypeObject V831CameraObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    V831Camera_name,                           /* tp_name */
    sizeof(V831CameraObject),                      /* tp_basicsize */
    0,                                        /* tp_itemsize */
    (destructor)V831Camera_free,                   /* tp_dealloc */
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
    V831Camera_str,                                /* tp_str */
    0,                                        /* tp_getattro */
    0,                                        /* tp_setattro */
    0,                                        /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    V831CameraObject_type_doc,                     /* tp_doc */
    0,                                        /* tp_traverse */
    0,                                        /* tp_clear */
    0,                                        /* tp_richcompare */
    0,                                        /* tp_weaklistoffset */
    0,                                        /* tp_iter */
    0,                                        /* tp_iternext */
    V831Camera_methods,                            /* tp_methods */
    0,                                        /* tp_members */
    V831Camera_getseters,                          /* tp_getset */
    0,                                        /* tp_base */
    0,                                        /* tp_dict */
    0,                                        /* tp_descr_get */
    0,                                        /* tp_descr_set */
    0,                                        /* tp_dictoffset */
    (initproc)V831Camera_init,                     /* tp_init */
    0,                                        /* tp_alloc */
    V831Camera_new,                                /* tp_new */
};

#endif
