
#include "_maix_display.h"

#ifdef V831Display

#include "libmaix_disp.h"

PyDoc_STRVAR(V831DisplayObject_type_doc, "V831Display(width, height) -> V831Display object.\n");
typedef struct
{
    PyObject_HEAD;
    unsigned int width, height;

    libmaix_disp_t* disp;

} V831DisplayObject;

PyDoc_STRVAR(V831Display_close_doc, "close()\n\nClose V831Display device.\n");
static PyObject *V831Display_close(V831DisplayObject *self)
{

    if (NULL != self->disp)
        libmaix_disp_destroy(&self->disp);

    Py_RETURN_NONE;
}

static PyObject *V831Display_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    V831DisplayObject *self;

    if ((self = (V831DisplayObject *)type->tp_alloc(type, 0)) == NULL)
    {
        return NULL;
    }

    return (PyObject *)self;
}

static void V831Display_free(V831DisplayObject *self)
{
    PyObject *ref = V831Display_close(self);
    Py_XDECREF(ref);

    Py_TYPE(self)->tp_free((PyObject *)self);
}

static int V831Display_init(V831DisplayObject *self, PyObject *args, PyObject *kwds)
{
    // default init value
    self->width = 240, self->height = 240;

    static char *kwlist[] = {"width", "height", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ii:__init__", kwlist,
                                     &self->width, &self->height))
    {
        return -1;
    }

    self->disp = libmaix_disp_creat();
    if(self->disp != NULL) {
        self->disp->swap_rb = 1;
        return 0;
    }

    V831Display_close(self);
    PyErr_SetFromErrno(PyExc_IOError);

    return -1;
}

static PyObject *V831Display_enter(PyObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    Py_INCREF(self);
    return self;
}

static PyObject *V831Display_exit(V831DisplayObject *self, PyObject *args)
{
    PyObject *exc_type = 0;
    PyObject *exc_value = 0;
    PyObject *traceback = 0;

    if (!PyArg_UnpackTuple(args, "__exit__", 3, 3, &exc_type, &exc_value, &traceback))
    {
        return 0;
    }

    V831Display_close(self);
    Py_XDECREF(self);
    Py_RETURN_NONE;
}

/* str */
static PyObject *V831Display_str(PyObject *object)
{
  return PyUnicode_FromString(__FILE__);
}
PyDoc_STRVAR(Display_draw_doc, "draw()\nDraw image(rgb888) bytes data to lcd.\n");
static PyObject *Display_draw(V831DisplayObject *self, PyObject *args)
{
    PyObject *img_bytes = NULL;
    int img_width = 0, img_height = 0;

    if (!PyArg_ParseTuple(args, "Oii", &img_bytes, &img_width, &img_height))
    {
        return Py_None;
    }
    
    if (self->disp->width >= img_width && self->disp->height >= img_height) {
        uint8_t *rgb_data = (uint8_t *)PyBytes_AS_STRING(img_bytes);
        self->disp->draw(self->disp, rgb_data, (self->disp->width - img_width) / 2,(self->disp->height - img_height) / 2, img_width, img_height, 1);
    }

    return Py_None;
}

static PyMethodDef V831Display_methods[] = {

    {"draw", (PyCFunction)Display_draw, METH_VARARGS, Display_draw_doc},
    {"close", (PyCFunction)V831Display_close, METH_NOARGS, V831Display_close_doc},
    {"__enter__", (PyCFunction)V831Display_enter, METH_NOARGS, NULL},
    {"__exit__", (PyCFunction)V831Display_exit, METH_NOARGS, NULL},
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
PyDoc_STRVAR(V831Display_width_doc, "V831Display internal operate width.\n\n");
static PyObject *V831Display_get_width(V831DisplayObject *self, void *closure)
{
    PyObject *result = Py_BuildValue("i", self->width);
    return result;
}

static int V831Display_set_width(V831DisplayObject *self, PyObject *value, void *closeure)
{
    if (check_user_input("width", value, 640, 480) != 0)
    {
        return -1;
    }
    self->width = PyLong_AsLong(value);
    return 0;
}

/* height */
PyDoc_STRVAR(V831Display_height_doc, "V831Display internal operate height.\n\n");
static PyObject *V831Display_get_height(V831DisplayObject *self, void *closure)
{
    PyObject *result = Py_BuildValue("i", self->height);
    return result;
}

static int V831Display_set_height(V831DisplayObject *self, PyObject *value, void *closeure)
{
    if (check_user_input("height", value, 640, 480) != 0)
    {
        return -1;
    }
    self->height = PyLong_AsLong(value);
    return 0;
}

static PyGetSetDef V831Display_getseters[] = {
    {"width", (getter)V831Display_get_width, (setter)V831Display_set_width, V831Display_width_doc},
    {"height", (getter)V831Display_get_height, (setter)V831Display_set_height, V831Display_height_doc},
    {NULL},
};

PyTypeObject V831DisplayObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    V831Display_name,                           /* tp_name */
    sizeof(V831DisplayObject),                      /* tp_basicsize */
    0,                                        /* tp_itemsize */
    (destructor)V831Display_free,                   /* tp_dealloc */
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
    V831Display_str,                                /* tp_str */
    0,                                        /* tp_getattro */
    0,                                        /* tp_setattro */
    0,                                        /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    V831DisplayObject_type_doc,                     /* tp_doc */
    0,                                        /* tp_traverse */
    0,                                        /* tp_clear */
    0,                                        /* tp_richcompare */
    0,                                        /* tp_weaklistoffset */
    0,                                        /* tp_iter */
    0,                                        /* tp_iternext */
    V831Display_methods,                            /* tp_methods */
    0,                                        /* tp_members */
    V831Display_getseters,                          /* tp_getset */
    0,                                        /* tp_base */
    0,                                        /* tp_dict */
    0,                                        /* tp_descr_get */
    0,                                        /* tp_descr_set */
    0,                                        /* tp_dictoffset */
    (initproc)V831Display_init,                     /* tp_init */
    0,                                        /* tp_alloc */
    V831Display_new,                                /* tp_new */
};

#endif
