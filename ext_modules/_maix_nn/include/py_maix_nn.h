#ifndef _CPY_MAIX_NN_H_
#define _CPY_MAIX_NN_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <Python.h>

/////////////////////////////
#define _MAIX_NN_VERSION_ "0.2.0"
#define _MAIX_NN_NAME_ "_maix_nn"
PyDoc_STRVAR(_maix_nn_doc, "MaixPy Python3 nn(neural network) module.\n");
PyDoc_STRVAR(_maix_nn_functional_doc, "MaixPy Python3 nn(neural network) functional module.\n");
PyDoc_STRVAR(_maix_nn_decoder_doc, "MaixPy Python3 nn(neural network) decoder(post process) module.\n");
/////////////////////////////

PyDoc_STRVAR(maix_nn_model_type_name, "Model");
PyDoc_STRVAR(maix_nn_decoder_yolo2_type_name, "Yolo2");
extern PyTypeObject PyMaix_NN_Model_Type;
extern PyTypeObject PyMaix_NN_Decoder_Yolo2_Type;
extern struct PyModuleDef maix_nn_functional_module;
extern struct PyModuleDef maix_nn_decoder_module;

#ifdef  __cplusplus
}
#endif

#endif

