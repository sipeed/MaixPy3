
#include "py_maix_nn.h"

#include "string.h"



static PyObject* _maix_nn_help() {
    return PyUnicode_FromString(_maix_nn_doc);
}

PyDoc_STRVAR(_maix_nn_load_doc, "load model, returen a nn.Model object\n");
static PyObject *_maix_nn_load(PyObject *self, PyObject *args)
{
  Py_RETURN_NONE;
}

static PyMethodDef _maix_nn_methods[] = {
    {"help", (PyCFunction)_maix_nn_help, METH_NOARGS, _maix_nn_doc},
    {"load", (PyCFunction)_maix_nn_load, METH_VARARGS, _maix_nn_load_doc},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef _maix_nn_module = {
    PyModuleDef_HEAD_INIT,
    _MAIX_NN_NAME_,         /* Module name */
    _maix_nn_doc,	/* Module _maixMethods */
    -1,			    /* size of per-interpreter state of the module, size of per-interpreter state of the module,*/
    _maix_nn_methods,
};

PyMODINIT_FUNC PyInit__maix_nn(void)
{

    PyObject *module;

    module = PyModule_Create(&_maix_nn_module);
    PyObject *version = PyUnicode_FromString(_MAIX_NN_VERSION_);


    /* Set module version */
    PyObject *dict = PyModule_GetDict(module);
    PyDict_SetItemString(dict, "__version__", version);
    Py_DECREF(version);

    return module;
}