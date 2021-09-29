#ifndef _MAIX_CAMERA_H
#define _MAIX_CAMERA_H

#ifdef  __cplusplus
extern "C" {
#endif

#include <Python.h>

/* Macros needed for Python 3 */
#ifndef PyInt_Check
#define PyInt_Check PyLong_Check
#define PyInt_FromLong PyLong_FromLong
#define PyInt_AsLong PyLong_AsLong
#define PyInt_Type PyLong_Type
#endif

PyDoc_STRVAR(VirtualCamera_name, "VirtualCamera");
extern PyTypeObject VirtualCameraObjectType;

// #define V831Camera
#ifdef V831Camera
PyDoc_STRVAR(V831Camera_name, "V831Camera");
extern PyTypeObject V831CameraObjectType;
#endif

#ifdef R329Camera
PyDoc_STRVAR(R329Camera_name, "R329Camera");
extern PyTypeObject R329CameraObjectType;
#endif





#ifdef  __cplusplus
}
#endif

#endif

