
#include "_maix.h"

#define _VERSION_ "0.1"
#define _NAME_ "_maix"
PyDoc_STRVAR(_maix_doc, "MaixPy Python3 library.\n");

static PyMethodDef _maix_methods[] = {
    {NULL}
};

void define_constants(PyObject *module) {
    // PyModule_AddObject(module, "I2C_M_NOSTART", Py_BuildValue("H", I2C_M_NOSTART));
    // PyModule_AddObject(module, "I2C_M_NO_RD_ACK", Py_BuildValue("H", I2C_M_NO_RD_ACK));
    // PyModule_AddObject(module, "I2C_M_IGNORE_NAK", Py_BuildValue("H", I2C_M_IGNORE_NAK));
}

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef _maixmodule = {
    PyModuleDef_HEAD_INIT,
    _NAME_,         /* Module name */
    _maix_doc,	/* Module _maixMethods */
    -1,			    /* size of per-interpreter state of the module, size of per-interpreter state of the module,*/
    _maix_methods,
};
#endif


#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC PyInit__maix(void)
#else
PyMODINIT_FUNC init_maix(void)
#endif
{

    PyObject *module;

    if (PyType_Ready(&I2CDeviceObjectType) < 0) {
#if PY_MAJOR_VERSION >= 3
        return NULL;
#else
        return;
#endif
    }

#if PY_MAJOR_VERSION >= 3
    module = PyModule_Create(&_maixmodule);
    PyObject *version = PyUnicode_FromString(_VERSION_);
#else
    module = Py_InitModule3(_NAME_, _maix_methods, _maix_doc);
    PyObject *version = PyString_FromString(_VERSION_);
#endif

    /* Constants */
    define_constants(module);

    /* Set module version */
    PyObject *dict = PyModule_GetDict(module);
    PyDict_SetItemString(dict, "__version__", version);
    Py_DECREF(version);

    /* Register I2CDeviceObject */
    Py_INCREF(&I2CDeviceObjectType);
    PyModule_AddObject(module, I2CDevice_name, (PyObject *)&I2CDeviceObjectType);

#if PY_MAJOR_VERSION >= 3
    return module;
#endif
}

