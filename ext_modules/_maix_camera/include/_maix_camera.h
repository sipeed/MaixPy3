#ifndef _MAIX_CAMERA_H
#define _MAIX_CAMERA_H

#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <vector>
#include <list>
#include <array>
#include <map>
#include <set>
#ifdef __cplusplus
extern "C"
{
#endif
#include "libmaix_cam.h"
#include "libmaix_image.h"
#define debug_line printf("%s:%d %s %s %s \r\n", __FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__)
#ifdef __cplusplus
}
#endif


#define R329Camera
// #define V83xCamera

#if !defined (R329Camera) && !defined (V83xCamera)
#define VirtualCamera
#endif

class _camera
{
private:
public:
    int width, height, dev_id, m, f;
    _camera(int w, int h, int dev_id, int m, int f);
    ~_camera();
    pybind11::list read();
    void close();
    void __enter__();
    void __exit__();

    libmaix_cam_t *cam;
    libmaix_image_t *rgb_img;
};


#endif
