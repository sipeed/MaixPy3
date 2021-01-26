
#include "_maix_display.h"

PyDoc_STRVAR(VirtualDisplayObject_type_doc, "VirtualDisplay(width, height) -> VirtualDisplay object.\n");
typedef struct
{
    PyObject_HEAD;
    unsigned int width, height;
} VirtualDisplayObject;

PyDoc_STRVAR(VirtualDisplay_close_doc, "close()\n\nClose VirtualDisplay device.\n");
static PyObject *VirtualDisplay_close(VirtualDisplayObject *self)
{
    Py_RETURN_NONE;
}

static PyObject *VirtualDisplay_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    VirtualDisplayObject *self;

    if ((self = (VirtualDisplayObject *)type->tp_alloc(type, 0)) == NULL)
    {
        return NULL;
    }

    return (PyObject *)self;
}

static void VirtualDisplay_free(VirtualDisplayObject *self)
{
    PyObject *ref = VirtualDisplay_close(self);
    Py_XDECREF(ref);

    Py_TYPE(self)->tp_free((PyObject *)self);
}

static int VirtualDisplay_init(VirtualDisplayObject *self, PyObject *args, PyObject *kwds)
{
    // default init value
    self->width = 240, self->height = 240;

    static char *kwlist[] = {"width", "height", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ii:__init__", kwlist,
                                     &self->width, &self->height))
    {
        return -1;
    }

    if(self != NULL) {
        return 0;
    }

    VirtualDisplay_close(self);
    PyErr_SetFromErrno(PyExc_IOError);

    return -1;
}

static PyObject *VirtualDisplay_enter(PyObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    Py_INCREF(self);
    return self;
}

static PyObject *VirtualDisplay_exit(VirtualDisplayObject *self, PyObject *args)
{
    PyObject *exc_type = 0;
    PyObject *exc_value = 0;
    PyObject *traceback = 0;

    if (!PyArg_UnpackTuple(args, "__exit__", 3, 3, &exc_type, &exc_value, &traceback))
    {
        return 0;
    }

    VirtualDisplay_close(self);
    Py_XDECREF(self);
    Py_RETURN_NONE;
}

/* str */
static PyObject *VirtualDisplay_str(PyObject *object)
{
  return PyUnicode_FromString(__FILE__);
}
PyDoc_STRVAR(Display_draw_doc, "draw()\nDraw image(rgb888) bytes data to lcd.\n");
static PyObject *Display_draw(VirtualDisplayObject *self, PyObject *args)
{
    PyObject *img_bytes = NULL;
    int img_width = 0, img_height = 0;

    if (!PyArg_ParseTuple(args, "Oii", &img_bytes, &img_width, &img_height))
    {
        return NULL;
    }
    
    Py_RETURN_NONE;
}

static PyMethodDef VirtualDisplay_methods[] = {

    {"draw", (PyCFunction)Display_draw, METH_VARARGS, Display_draw_doc},
    {"close", (PyCFunction)VirtualDisplay_close, METH_NOARGS, VirtualDisplay_close_doc},
    {"__enter__", (PyCFunction)VirtualDisplay_enter, METH_NOARGS, NULL},
    {"__exit__", (PyCFunction)VirtualDisplay_exit, METH_NOARGS, NULL},
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
PyDoc_STRVAR(VirtualDisplay_width_doc, "VirtualDisplay internal operate width.\n\n");
static PyObject *VirtualDisplay_get_width(VirtualDisplayObject *self, void *closure)
{
    PyObject *result = Py_BuildValue("i", self->width);
    return result;
}

static int VirtualDisplay_set_width(VirtualDisplayObject *self, PyObject *value, void *closeure)
{
    if (check_user_input("width", value, 640, 480) != 0)
    {
        return -1;
    }
    self->width = PyLong_AsLong(value);
    return 0;
}

/* height */
PyDoc_STRVAR(VirtualDisplay_height_doc, "VirtualDisplay internal operate height.\n\n");
static PyObject *VirtualDisplay_get_height(VirtualDisplayObject *self, void *closure)
{
    PyObject *result = Py_BuildValue("i", self->height);
    return result;
}

static int VirtualDisplay_set_height(VirtualDisplayObject *self, PyObject *value, void *closeure)
{
    if (check_user_input("height", value, 640, 480) != 0)
    {
        return -1;
    }
    self->height = PyLong_AsLong(value);
    return 0;
}

static PyGetSetDef VirtualDisplay_getseters[] = {
    {"width", (getter)VirtualDisplay_get_width, (setter)VirtualDisplay_set_width, VirtualDisplay_width_doc},
    {"height", (getter)VirtualDisplay_get_height, (setter)VirtualDisplay_set_height, VirtualDisplay_height_doc},
    {NULL},
};

PyTypeObject VirtualDisplayObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    VirtualDisplay_name,                           /* tp_name */
    sizeof(VirtualDisplayObject),                      /* tp_basicsize */
    0,                                        /* tp_itemsize */
    (destructor)VirtualDisplay_free,                   /* tp_dealloc */
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
    VirtualDisplay_str,                                /* tp_str */
    0,                                        /* tp_getattro */
    0,                                        /* tp_setattro */
    0,                                        /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    VirtualDisplayObject_type_doc,                     /* tp_doc */
    0,                                        /* tp_traverse */
    0,                                        /* tp_clear */
    0,                                        /* tp_richcompare */
    0,                                        /* tp_weaklistoffset */
    0,                                        /* tp_iter */
    0,                                        /* tp_iternext */
    VirtualDisplay_methods,                            /* tp_methods */
    0,                                        /* tp_members */
    VirtualDisplay_getseters,                          /* tp_getset */
    0,                                        /* tp_base */
    0,                                        /* tp_dict */
    0,                                        /* tp_descr_get */
    0,                                        /* tp_descr_set */
    0,                                        /* tp_dictoffset */
    (initproc)VirtualDisplay_init,                     /* tp_init */
    0,                                        /* tp_alloc */
    VirtualDisplay_new,                                /* tp_new */
};

