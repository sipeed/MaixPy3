/**
 * maix image lib, debug header
 * 
 * @copyright © 2020-2021 Sipeed Ltd, All rights reserved
 * @author neucrack
 * @update --2021.1.7--neucrack: create lib
 *         --
 */

#ifndef __LIBMAIX_IMAGE_DEBUG_H__
#define __LIBMAIX_IMAGE_DEBUG_H__

#include "libmaix_image_err.h"
#include "assert.h"
#include "stdio.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DEBUG
    #define DEBUG 0
#endif

#if DEBUG // from makefile environment variable
    #define LIBMAIX_IMAGE_DEBUG_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define LIBMAIX_IMAGE_DEBUG_PRINTF(fmt, ...)
#endif


#define LIBMAIX_IMAGE_ERROR_PRINTF(fmt, ...) do{ \
        printf("[ERROR] libmaix_image: "); \
        printf(fmt, ##__VA_ARGS__); \
    }while(0)
#define LIBMAIX_IMAGE_ERROR(err_num) LIBMAIX_IMAGE_ERROR_PRINTF("%s\n", libmaix_image_get_err_msg(err_num))


#ifdef __cplusplus
}
#endif


#endif

