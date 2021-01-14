/**
 * maix neural network lib
 * 
 * @copyright © 2020-2021 Sipeed Ltd, All rights reserved
 * @author neucrack
 * @update --2021.1.7--neucrack: create lib
 *         --
 */
#ifndef __LIBMAIX_IMAGEW_H__
#define __LIBMAIX_IMAGEW_H__

#include "libmaix_image_err.h"
#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    LIBMAIX_IMAGE_MODE_INVALID = 0,
    LIBMAIX_IMAGE_MODE_BINARY,
    LIBMAIX_IMAGE_MODE_GRAY  ,
    LIBMAIX_IMAGE_MODE_RGB888,          // supported
    LIBMAIX_IMAGE_MODE_RGB565,
    LIBMAIX_IMAGE_MODE_RGBA8888,
    LIBMAIX_IMAGE_MODE_YUV420SP_NV21,   // supported
}libmaix_image_mode_t;

typedef union
{
    struct
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t reserved;
    }rgb888;
}libmaix_image_color_t;


typedef enum
{
    LIBMAIX_IMAGE_LAYOUT_HWC    = 0,
    LIBMAIX_IMAGE_LAYOUT_CHW    = 1,
}libmaix_image_layout_t;

typedef struct libmaix_image
{
    uint32_t               width;
    uint32_t               height;
    libmaix_image_mode_t   mode;
    libmaix_image_layout_t layout;
    void*                  data;
    bool                   is_data_alloc;  // data need to free when call destory

    libmaix_image_err_t (*convert)(struct libmaix_image* obj, libmaix_image_mode_t mode, void* buff);
    libmaix_image_err_t (*draw_rectangle)(struct libmaix_image* obj, int x, int y, int w, int h, libmaix_image_color_t color, bool fill, int thickness);
	libmaix_image_err_t (*draw_string)(struct libmaix_image* obj, char *str, int x, int y, int size, libmaix_image_color_t color, libmaix_image_color_t* bg);
}libmaix_image_t;



libmaix_image_err_t libmaix_image_module_init();
libmaix_image_err_t libmaix_image_module_deinit();
/**
 * 
 * @param [in] data: data pointer, can be NULL,
 *                   it will automaticlly allocc memory according to the w,h,mode,
 *                   if one of them is `0`, nothing will do
 * @param [in] is_data_alloc: 
 *                   if arg data is not `NULL`,
 *                   this arg indicate the data if need to automatically free in libmaix_image_destroy
 */
libmaix_image_t* libmaix_image_creat(uint32_t w, uint32_t h, libmaix_image_mode_t mode, libmaix_image_layout_t layout, void* data, bool is_data_alloc);
void libmaix_image_destroy(libmaix_image_t** obj);

#ifdef __cplusplus
}
#endif


#endif

