
#include "Python.h"
#include "py_maix_nn.h"

#include <math.h>
#include <string.h>


static PyMethodDef _maix_nn_app_methods[] = {
    {NULL, NULL, 0, NULL}
};


struct PyModuleDef maix_nn_app_module = {
    PyModuleDef_HEAD_INIT,
    "_app",              /* Module name */
    _maix_nn__app_doc,	/* Module _maixMethods */
    -1,			    /* size of per-interpreter state of the module, size of per-interpreter state of the module,*/
    _maix_nn_app_methods,
};


