
#include "_maix_camera.h"

PyDoc_STRVAR(VirtualCameraObject_type_doc, "VirtualCamera(width, height) -> VirtualCamera object.\n");
typedef struct
{
  PyObject_HEAD;
  unsigned int width, height;
} VirtualCameraObject;

PyDoc_STRVAR(VirtualCamera_close_doc, "close()\n\nClose VirtualCamera device.\n");
static PyObject *VirtualCamera_close(VirtualCameraObject *self)
{
  Py_RETURN_NONE;
}

static PyObject *VirtualCamera_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  VirtualCameraObject *self;

  if ((self = (VirtualCameraObject *)type->tp_alloc(type, 0)) == NULL)
  {
    return NULL;
  }

  return (PyObject *)self;
}

static void VirtualCamera_free(VirtualCameraObject *self)
{
  PyObject *ref = VirtualCamera_close(self);
  Py_XDECREF(ref);

  Py_TYPE(self)->tp_free((PyObject *)self);
}

static int VirtualCamera_init(VirtualCameraObject *self, PyObject *args, PyObject *kwds)
{
  // default init value
  self->width = 640, self->height = 480;

  static char *kwlist[] = {"width", "height", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ii:__init__", kwlist,
                                   &self->width, &self->height))
  {
    return -1;
  }

  return 0;
}

static PyObject *VirtualCamera_enter(PyObject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, ""))
    return NULL;

  Py_INCREF(self);
  return self;
}

static PyObject *VirtualCamera_exit(VirtualCameraObject *self, PyObject *args)
{
  PyObject *exc_type = 0;
  PyObject *exc_value = 0;
  PyObject *traceback = 0;

  if (!PyArg_UnpackTuple(args, "__exit__", 3, 3, &exc_type, &exc_value, &traceback))
  {
    return 0;
  }

  Py_XDECREF(self);
  VirtualCamera_close(self);
  Py_RETURN_FALSE;
}

/* str */
static PyObject *VirtualCamera_str(PyObject *object)
{
  return PyUnicode_FromString(__FILE__);
}

PyDoc_STRVAR(VirtualCamera_read_doc, "read()\n\nRead image(rgb888) bytes data from device.\n");
static PyObject *VirtualCamera_read(VirtualCameraObject *self, PyObject *args)
{
    PyObject *bytes = NULL;

    int ret = 0;
    
    char buf[self->width * self->height * 3]; // rgb888
    for (size_t i = 0; i < sizeof(buf); i++)
        buf[i] = random();
    size_t len = sizeof(buf);

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

static PyMethodDef VirtualCamera_methods[] = {

    {"read", (PyCFunction)VirtualCamera_read, METH_VARARGS, VirtualCamera_read_doc},
    {"close", (PyCFunction)VirtualCamera_close, METH_NOARGS, VirtualCamera_close_doc},
    {"__enter__", (PyCFunction)VirtualCamera_enter, METH_NOARGS, NULL},
    {"__exit__", (PyCFunction)VirtualCamera_exit, METH_NOARGS, NULL},
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
PyDoc_STRVAR(VirtualCamera_width_doc, "VirtualCamera internal operate width.\n\n");
static PyObject *VirtualCamera_get_width(VirtualCameraObject *self, void *closure)
{
  PyObject *result = Py_BuildValue("i", self->width);
  return result;
}

static int VirtualCamera_set_width(VirtualCameraObject *self, PyObject *value, void *closeure)
{
  if (check_user_input("width", value, 640, 480) != 0)
  {
    return -1;
  }
  self->width = PyLong_AsLong(value);
  return 0;
}

/* height */
PyDoc_STRVAR(VirtualCamera_height_doc, "VirtualCamera internal operate height.\n\n");
static PyObject *VirtualCamera_get_height(VirtualCameraObject *self, void *closure)
{
  PyObject *result = Py_BuildValue("i", self->height);
  return result;
}

static int VirtualCamera_set_height(VirtualCameraObject *self, PyObject *value, void *closeure)
{
  if (check_user_input("height", value, 640, 480) != 0)
  {
    return -1;
  }
  self->height = PyLong_AsLong(value);
  return 0;
}

static PyGetSetDef VirtualCamera_getseters[] = {
    {"width", (getter)VirtualCamera_get_width, (setter)VirtualCamera_set_width, VirtualCamera_width_doc},
    {"height", (getter)VirtualCamera_get_height, (setter)VirtualCamera_set_height, VirtualCamera_height_doc},
    {NULL},
};

PyTypeObject VirtualCameraObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    VirtualCamera_name,                           /* tp_name */
    sizeof(VirtualCameraObject),                      /* tp_basicsize */
    0,                                        /* tp_itemsize */
    (destructor)VirtualCamera_free,                   /* tp_dealloc */
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
    VirtualCamera_str,                                /* tp_str */
    0,                                        /* tp_getattro */
    0,                                        /* tp_setattro */
    0,                                        /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    VirtualCameraObject_type_doc,                     /* tp_doc */
    0,                                        /* tp_traverse */
    0,                                        /* tp_clear */
    0,                                        /* tp_richcompare */
    0,                                        /* tp_weaklistoffset */
    0,                                        /* tp_iter */
    0,                                        /* tp_iternext */
    VirtualCamera_methods,                            /* tp_methods */
    0,                                        /* tp_members */
    VirtualCamera_getseters,                          /* tp_getset */
    0,                                        /* tp_base */
    0,                                        /* tp_dict */
    0,                                        /* tp_descr_get */
    0,                                        /* tp_descr_set */
    0,                                        /* tp_dictoffset */
    (initproc)VirtualCamera_init,                     /* tp_init */
    0,                                        /* tp_alloc */
    VirtualCamera_new,                                /* tp_new */
};
