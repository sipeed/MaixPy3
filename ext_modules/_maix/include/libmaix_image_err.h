/**
 * libmaix_image_err.h, maix image lib error enumerate
 * 
 * @copyright © 2020-2021 Sipeed Ltd, All rights reserved
 * @author neucrack
 * @update --2021.1.7--neucrack: create lib
 *         --
 */
#ifndef __LIBMAIX_IMAGE_ERR_H__
#define __LIBMAIX_IMAGE_ERR_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    LIBMAIX_IMAGE_ERR_NONE   = 0,
    LIBMAIX_IMAGE_ERR_PARAM  = 1,
    LIBMAIX_IMAGE_ERR_NO_MEM = 2,
    LIBMAIX_IMAGE_ERR_NOT_IMPLEMENT = 3,
    LIBMAIX_IMAGE_ERR_UNKNOWN,
}libmaix_image_err_t;         // update this enum you MUST update `libmaix_image_err_strs` definition too

extern char* libmaix_image_err_strs[];

static inline char* libmaix_image_get_err_msg(libmaix_image_err_t err)
{
    return libmaix_image_err_strs[err];
}

#ifdef __cplusplus
}
#endif


#endif

