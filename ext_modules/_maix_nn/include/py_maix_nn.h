#ifndef _CPY_MAIX_NN_H_
#define _CPY_MAIX_NN_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <Python.h>

/////////////////////////////
#define _MAIX_NN_VERSION_ "0.1.0"
#define _MAIX_NN_NAME_ "_maix_nn"
PyDoc_STRVAR(_maix_nn_doc, "MaixPy Python3 nn(neural network) module.\n");
/////////////////////////////

PyDoc_STRVAR(maix_nn_model_type_name, "Model");
extern PyTypeObject PyMaix_NN_Model_Type;

#ifdef  __cplusplus
}
#endif

#endif

