#ifndef _CPY_MAIX_H_
#define _CPY_MAIX_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <Python.h>

PyDoc_STRVAR(Camera_name, "Camera");
extern PyTypeObject CameraObjectType;

PyDoc_STRVAR(Display_name, "Display");
extern PyTypeObject DisplayObjectType;

#ifdef  __cplusplus
}
#endif

#endif

