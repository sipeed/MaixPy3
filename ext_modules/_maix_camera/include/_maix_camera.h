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

#define debug_line printf("%s:%d %s %s %s \r\n", __FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__)
#ifdef __cplusplus
}
#endif

// #define R329Camera
// #define V83xCamera

#if !defined (R329Camera) && !defined (V83xCamera)
#define VirtualCamera
#endif

class _camera
{
private:
public:
    int width, height, dev_id;
    // width operator=(const int num)       //如果使用的话,可能需要重载赋值运算符
    // {
    // 	if(num < 640 || num > 480)
    // }
    _camera(int w, int h, int dev_id);
    ~_camera();
    pybind11::list read();
    void close();
    void __enter__();
    void __exit__();
#ifdef R329Camera
public:
    libmaix_cam_t *cam;
    uint8_t *img_buff;

#endif

#ifdef V83xCamera
public:
    libmaix_cam_t *cam;
    libmaix_image_t *yuv_img;
    libmaix_image_t *rgb_img;

#endif

};










// #ifdef R329Camera
// class r329_camera
// {
// private:
// public:
//     int width, height, dev_id;
//     libmaix_cam_t *cam;
//     uint8_t *img_buff;
//     r329_camera(int w, int h, int dev_id);
//     ~r329_camera();
//     pybind11::object read();
//     void close();
//     void __enter__();
//     void __exit__();
// };
// #endif

// #ifdef V831Camera

// #ifdef __cplusplus
// extern "C"
// {
// #endif
// #include "libmaix_image.h"

// #ifdef __cplusplus
// }
// #endif

// class v831_camera
// {
// private:
// public:
//     int width, height, dev_id;
//     // width operator=(const int num)       //如果使用的话,可能需要重载赋值运算符
//     // {
//     // 	if(num < 640 || num > 480)
//     // }
//     libmaix_cam_t *cam;
//     libmaix_image_t *yuv_img;
//     libmaix_image_t *rgb_img;

//     v831_camera(int w, int h, int dev_id);
//     ~v831_camera();
//     pybind11::list read();
//     void close();
//     void __enter__();
//     void __exit__();
//     // String str();
// };

// #endif



// #ifdef VirtualCamera
// class virtual_camera
// {
// private:
// public:
//     int width, height, dev_id;
//     // width operator=(const int num)       //如果使用的话,可能需要重载赋值运算符
//     // {
//     // 	if(num < 640 || num > 480)
//     // }
//     virtual_camera(int w, int h, int dev_id);
//     ~virtual_camera();
//     pybind11::list read();
//     void close();
//     void __enter__();
//     void __exit__();
// };
// #endif

#endif
