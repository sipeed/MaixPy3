
#include "_maix_display.h"

#define debug_line printf("%s:%d %s %s %s \r\n", __FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__)
#ifdef R329Display

#include "libmaix_disp.h"
static void *disp_img = NULL;

PyDoc_STRVAR(R329DisplayObject_type_doc, "R329Display(width, height) -> R329Display object.\n");
typedef struct
{
    PyObject_HEAD;
    unsigned int width, height;
    libmaix_disp_t *disp;
} R329DisplayObject;

PyDoc_STRVAR(R329Display_close_doc, "close()\n\nClose R329Display device.\n");
static PyObject *R329Display_close(R329DisplayObject *self)
{
    free(disp_img);
    if (NULL != self->disp)
        libmaix_disp_destroy(&self->disp);

    Py_RETURN_NONE;
}

static PyObject *R329Display_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    R329DisplayObject *self;
    if ((self = (R329DisplayObject *)type->tp_alloc(type, 0)) == NULL)
    {
        return NULL;
    }
    return (PyObject *)self;
}

static void R329Display_free(R329DisplayObject *self)
{
    PyObject *ref = R329Display_close(self);
    Py_XDECREF(ref);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static int R329Display_init(R329DisplayObject *self, PyObject *args, PyObject *kwds)
{
    // default init value
    self->width = 240, self->height = 240;
    static char *kwlist[] = {"width", "height", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ii:__init__", kwlist,
                                     &self->width, &self->height))
    {
        return -1;
    }
    self->disp = libmaix_disp_create();

    R329Display_close(self);
    PyErr_SetFromErrno(PyExc_IOError);
    return -1;
}

static PyObject *R329Display_enter(PyObject *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    Py_INCREF(self);
    return self;
}

static PyObject *R329Display_exit(R329DisplayObject *self, PyObject *args)
{
    PyObject *exc_type = 0;
    PyObject *exc_value = 0;
    PyObject *traceback = 0;
    if (!PyArg_UnpackTuple(args, "__exit__", 3, 3, &exc_type, &exc_value, &traceback))
    {
        return 0;
    }
    R329Display_close(self);
    Py_XDECREF(self);
    Py_RETURN_NONE;
}

/* str */
static PyObject *R329Display_str(PyObject *object)
{
    return PyUnicode_FromString(__FILE__);
}

static unsigned short make16color(unsigned char r, unsigned char g, unsigned char b)
{
    return (
        (((r >> 3) & 31) << 11) |
        (((g >> 2) & 63) << 5) |
        ((b >> 3) & 31));
}

PyDoc_STRVAR(Display_draw_doc, "draw()\nDraw image(rgb888) bytes data to lcd.\n");
//传递过来的是888格式的图像,需要转成565格式的图像
static PyObject *Display_draw(R329DisplayObject *self, PyObject *args)
{
    PyObject *img_bytes = NULL;
    int img_width = 0, img_height = 0;
    if (!PyArg_ParseTuple(args, "Oii", &img_bytes, &img_width, &img_height))
    {
        return NULL;
    }
    if (NULL != self->disp)
    {
        if (self->disp->width >= img_width && self->disp->height >= img_height)
        {

            uint8_t *rgb_data = (uint8_t *)PyBytes_AS_STRING(img_bytes);
            uint16_t *rgb = NULL;
            rgb = (uint16_t *)disp_img;
            for (int i = 0; i < self->disp->width * self->disp->height; i++)
            {
                *rgb = make16color(rgb_data[0], rgb_data[1], rgb_data[2]);
                rgb_data += 3;
                rgb++;
            }
            if (disp_img != NULL)
            {
                self->disp->draw(self->disp, disp_img);
            }
        }
    }

    Py_RETURN_NONE;
}

static PyMethodDef R329Display_methods[] = {

    {"draw", (PyCFunction)Display_draw, METH_VARARGS, Display_draw_doc},
    {"close", (PyCFunction)R329Display_close, METH_NOARGS, R329Display_close_doc},
    {"__enter__", (PyCFunction)R329Display_enter, METH_NOARGS, NULL},
    {"__exit__", (PyCFunction)R329Display_exit, METH_NOARGS, NULL},
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
PyDoc_STRVAR(R329Display_width_doc, "R329Display internal operate width.\n\n");
static PyObject *R329Display_get_width(R329DisplayObject *self, void *closure)
{
    PyObject *result = Py_BuildValue("i", self->width);
    return result;
}

static int R329Display_set_width(R329DisplayObject *self, PyObject *value, void *closeure)
{
    if (check_user_input("width", value, 640, 480) != 0)
    {
        return -1;
    }
    self->width = PyLong_AsLong(value);
    return 0;
}

/* height */
PyDoc_STRVAR(R329Display_height_doc, "R329Display internal operate height.\n\n");
static PyObject *R329Display_get_height(R329DisplayObject *self, void *closure)
{
    PyObject *result = Py_BuildValue("i", self->height);
    return result;
}

static int R329Display_set_height(R329DisplayObject *self, PyObject *value, void *closeure)
{
    if (check_user_input("height", value, 640, 480) != 0)
    {
        return -1;
    }
    self->height = PyLong_AsLong(value);
    return 0;
}

static PyGetSetDef R329Display_getseters[] = {
    {"width", (getter)R329Display_get_width, (setter)R329Display_set_width, R329Display_width_doc},
    {"height", (getter)R329Display_get_height, (setter)R329Display_set_height, R329Display_height_doc},
    {NULL},
};

PyTypeObject R329DisplayObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
        R329Display_name,                     /* tp_name */
    sizeof(R329DisplayObject),                /* tp_basicsize */
    0,                                        /* tp_itemsize */
    (destructor)R329Display_free,             /* tp_dealloc */
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
    R329Display_str,                          /* tp_str */
    0,                                        /* tp_getattro */
    0,                                        /* tp_setattro */
    0,                                        /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    R329DisplayObject_type_doc,               /* tp_doc */
    0,                                        /* tp_traverse */
    0,                                        /* tp_clear */
    0,                                        /* tp_richcompare */
    0,                                        /* tp_weaklistoffset */
    0,                                        /* tp_iter */
    0,                                        /* tp_iternext */
    R329Display_methods,                      /* tp_methods */
    0,                                        /* tp_members */
    R329Display_getseters,                    /* tp_getset */
    0,                                        /* tp_base */
    0,                                        /* tp_dict */
    0,                                        /* tp_descr_get */
    0,                                        /* tp_descr_set */
    0,                                        /* tp_dictoffset */
    (initproc)R329Display_init,               /* tp_init */
    0,                                        /* tp_alloc */
    R329Display_new,                          /* tp_new */
};

#endif
