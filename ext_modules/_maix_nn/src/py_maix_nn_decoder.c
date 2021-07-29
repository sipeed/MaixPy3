
#include "Python.h"
#include "py_maix_nn.h"

#include <math.h>
#include <string.h>


static PyMethodDef _maix_nn_decoder_methods[] = {
    {NULL, NULL, 0, NULL}
};


struct PyModuleDef maix_nn_decoder_module = {
    PyModuleDef_HEAD_INIT,
    "decoder",              /* Module name */
    _maix_nn_decoder_doc,	/* Module _maixMethods */
    -1,			    /* size of per-interpreter state of the module, size of per-interpreter state of the module,*/
    _maix_nn_decoder_methods,
};


