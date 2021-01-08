
#include "_maix.h"

#define _VERSION_ "0.1"
#define _NAME_ "_maix"

PyDoc_STRVAR(_maix_doc, "MaixPy Python3 library.\n");

static PyObject *_maix_help() {
    return PyUnicode_FromString(_maix_doc);
}

static PyMethodDef _maix_methods[] = {
    {"help", (PyCFunction)_maix_help, METH_NOARGS, _maix_doc},
    {NULL}
};

void define_constants(PyObject *module) {
    PyModule_AddObject(module, "_VERSION_", Py_BuildValue("H", _VERSION_));
}

static struct PyModuleDef _maixmodule = {
    PyModuleDef_HEAD_INIT,
    _NAME_,         /* Module name */
    _maix_doc,	/* Module _maixMethods */
    -1,			    /* size of per-interpreter state of the module, size of per-interpreter state of the module,*/
    _maix_methods,
};

PyMODINIT_FUNC PyInit__maix(void)
{

    PyObject *module;

    if (PyType_Ready(&CameraObjectType) < 0) {
        return NULL;
    }

    module = PyModule_Create(&_maixmodule);
    PyObject *version = PyUnicode_FromString(_VERSION_);

    /* Constants */
    define_constants(module);

    /* Set module version */
    PyObject *dict = PyModule_GetDict(module);
    PyDict_SetItemString(dict, "__version__", version);
    Py_DECREF(version);

    /* Register I2CDeviceObject */
    Py_INCREF(&CameraObjectType);
    PyModule_AddObject(module, Camera_name, (PyObject *)&CameraObjectType);

    return module;
}

