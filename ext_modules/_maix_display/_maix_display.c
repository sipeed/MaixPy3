
#include "_maix_display.h"

#define _VERSION_ "0.2"
#define _NAME_ "_maix_display"

PyDoc_STRVAR(_maix_display_doc, "MaixPy3 Display\n");

static PyObject *_maix_display_help() {
    return PyUnicode_FromString(_maix_display_doc);
}

static PyMethodDef _maix_display_methods[] = {
    {"help", (PyCFunction)_maix_display_help, METH_NOARGS, _maix_display_doc},
    {NULL}
};

void define_constants(PyObject *module) {
    PyModule_AddObject(module, "_VERSION_", Py_BuildValue("s", _VERSION_));
}

static struct PyModuleDef _maix_displaymodule = {
    PyModuleDef_HEAD_INIT,
    _NAME_,         /* Module name */
    _maix_display_doc,	/* Module _maix_displayMethods */
    -1,			    /* size of per-interpreter state of the module, size of per-interpreter state of the module,*/
    _maix_display_methods,
};

PyMODINIT_FUNC PyInit__maix_display(void)
{
    PyObject *module;

    module = PyModule_Create(&_maix_displaymodule);
    PyObject *version = PyUnicode_FromString(_VERSION_);

    /* Constants */
    define_constants(module);

    /* Set module version */
    PyObject *dict = PyModule_GetDict(module);
    PyDict_SetItemString(dict, "__version__", version);
    Py_DECREF(version);

    if (PyType_Ready(&VirtualDisplayObjectType) < 0) {
        return NULL;
    }

    /* Register VirtualDisplayObjectType */
    Py_INCREF(&VirtualDisplayObjectType);
    PyModule_AddObject(module, VirtualDisplay_name, (PyObject *)&VirtualDisplayObjectType);

#ifdef V831Display
    if (PyType_Ready(&V831DisplayObjectType) < 0) {
        return NULL;
    }

    /* Register V831DisplayObjectType */
    Py_INCREF(&V831DisplayObjectType);
    PyModule_AddObject(module, V831Display_name, (PyObject *)&V831DisplayObjectType);
#endif

    return module;
}
