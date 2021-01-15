
#include "_maix.h"

#include "libmaix_cam.h"
#include "libmaix_image.h"

/* Macros needed for Python 3 */
#ifndef PyInt_Check
#define PyInt_Check PyLong_Check
#define PyInt_FromLong PyLong_FromLong
#define PyInt_AsLong PyLong_AsLong
#define PyInt_Type PyLong_Type
#endif

PyDoc_STRVAR(CameraObject_type_doc, "Camera(width, height) -> Camera object.\n");
typedef struct
{
    PyObject_HEAD;
    unsigned int width, height;

    libmaix_cam_t *cam;
    libmaix_image_t* img;

} CameraObject;

PyDoc_STRVAR(Camera_close_doc, "close()\n\nClose Camera device.\n");
static PyObject *Camera_close(CameraObject *self)
{

    if (NULL != self->cam)
        libmaix_cam_destroy(&self->cam);
    if (NULL != self->img)
        libmaix_image_destroy(&self->img);
    libmaix_image_module_deinit();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Camera_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    CameraObject *self;

    if ((self = (CameraObject *)type->tp_alloc(type, 0)) == NULL)
    {
        return NULL;
    }

    Py_INCREF(self);
    return (PyObject *)self;
}

static void Camera_free(CameraObject *self)
{
    PyObject *ref = Camera_close(self);
    Py_XDECREF(ref);

    Py_TYPE(self)->tp_free((PyObject *)self);
}

static int Camera_init(CameraObject *self, PyObject *args, PyObject *kwds)
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
    self->img = libmaix_image_create(self->width, self->height, LIBMAIX_IMAGE_MODE_YUV420SP_NV21, LIBMAIX_IMAGE_LAYOUT_HWC, NULL, true);
    if(NULL != self->img)
    {
        self->cam = libmaix_cam_creat(self->width, self->height);
        if (NULL != self->cam)
        {
            int ret = self->cam->strat_capture(self->cam);
            if (0 == ret)
            {
                Py_INCREF(self);
                return 0;
            }
        }
    }
    Camera_close(self);
    PyErr_SetFromErrno(PyExc_IOError);

    return -1;
}

static PyObject *Camera_enter(PyObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    Py_INCREF(self);
    return self;
}

static PyObject *Camera_exit(CameraObject *self, PyObject *args)
{
    PyObject *exc_type = 0;
    PyObject *exc_value = 0;
    PyObject *traceback = 0;

    if (!PyArg_UnpackTuple(args, "__exit__", 3, 3, &exc_type, &exc_value, &traceback))
    {
        return 0;
    }

    /* Close Camera bus */
    Camera_close(self);
    Py_RETURN_FALSE;
}

/* str */
static PyObject *Camera_str(PyObject *object)
{
  char desc[128];
  PyObject *dev_desc = NULL;
  // CameraObject *self = (CameraObject *)object;
  // Camera_get_device_desc(&self->dev, desc, sizeof(desc));

  dev_desc = PyUnicode_FromString(desc);

  Py_INCREF(dev_desc);
  return dev_desc;
}

/* file read */
PyDoc_STRVAR(Camera_read_doc, "read()\n\nRead image(rgb888) bytes data from device.\n");
static PyObject *Camera_read(CameraObject *self, PyObject *args)
{
    PyObject *bytes = NULL;

    int ret = 0;
    
    char *buf = NULL;
    size_t len = self->width * self->height * 3;
    ret = self->cam->capture(self->cam, (unsigned char*)self->img->data);
    if(ret == 0)
    {
      libmaix_err_t err0 = self->img->convert(self->img, LIBMAIX_IMAGE_MODE_RGB888, NULL);
      if(err0 == LIBMAIX_ERR_NONE)
      {
        buf = self->img->data;
      }
      else
      {
        // PyErr_Format(PyExc_ValueError, "libmaix_image_get_err_msg (%s)", libmaix_image_get_err_msg(err0));
        ret = err0;
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

static PyMethodDef Camera_methods[] = {

    {"read", (PyCFunction)Camera_read, METH_VARARGS, Camera_read_doc},
    {"close", (PyCFunction)Camera_close, METH_NOARGS, Camera_close_doc},
    {"__enter__", (PyCFunction)Camera_enter, METH_NOARGS, NULL},
    {"__exit__", (PyCFunction)Camera_exit, METH_NOARGS, NULL},
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
PyDoc_STRVAR(Camera_width_doc, "Camera internal operate width.\n\n");
static PyObject *Camera_get_width(CameraObject *self, void *closure)
{
    PyObject *result = Py_BuildValue("i", self->width);
    Py_INCREF(result);
    return result;
}

static int Camera_set_width(CameraObject *self, PyObject *value, void *closeure)
{
    if (check_user_input("width", value, 1, 1000) != 0)
    {
        return -1;
    }
    self->width = PyLong_AsLong(value);
    return 0;
}

/* height */
PyDoc_STRVAR(Camera_height_doc, "Camera internal operate height.\n\n");
static PyObject *Camera_get_height(CameraObject *self, void *closure)
{
    PyObject *result = Py_BuildValue("i", self->height);
    Py_INCREF(result);
    return result;
}

static int Camera_set_height(CameraObject *self, PyObject *value, void *closeure)
{
    if (check_user_input("height", value, 1, 1000) != 0)
    {
        return -1;
    }
    self->height = PyLong_AsLong(value);
    return 0;
}

static PyGetSetDef Camera_getseters[] = {
    {"width", (getter)Camera_get_width, (setter)Camera_set_width, Camera_width_doc},
    {"height", (getter)Camera_get_height, (setter)Camera_set_height, Camera_height_doc},
    {NULL},
};

PyTypeObject CameraObjectType = {
#if PY_MAJOR_VERSION >= 3
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL) 0, /* ob_size */
#endif
        Camera_name,                           /* tp_name */
    sizeof(CameraObject),                      /* tp_basicsize */
    0,                                        /* tp_itemsize */
    (destructor)Camera_free,                   /* tp_dealloc */
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
    Camera_str,                                /* tp_str */
    0,                                        /* tp_getattro */
    0,                                        /* tp_setattro */
    0,                                        /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    CameraObject_type_doc,                     /* tp_doc */
    0,                                        /* tp_traverse */
    0,                                        /* tp_clear */
    0,                                        /* tp_richcompare */
    0,                                        /* tp_weaklistoffset */
    0,                                        /* tp_iter */
    0,                                        /* tp_iternext */
    Camera_methods,                            /* tp_methods */
    0,                                        /* tp_members */
    Camera_getseters,                          /* tp_getset */
    0,                                        /* tp_base */
    0,                                        /* tp_dict */
    0,                                        /* tp_descr_get */
    0,                                        /* tp_descr_set */
    0,                                        /* tp_dictoffset */
    (initproc)Camera_init,                     /* tp_init */
    0,                                        /* tp_alloc */
    Camera_new,                                /* tp_new */
};
