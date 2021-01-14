/**
 * maix neural network lib, debug header
 * 
 * @copyright © 2020-2021 Sipeed Ltd, All rights reserved
 * @author neucrack
 * @update --2020.12.29--neucrack: create lib
 *         --
 */

#ifndef __LIBMAIX_NN_DEBUG_H__
#define __LIBMAIX_NN_DEBUG_H__

#include "libmaix_nn_err.h"
#include "assert.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DEBUG
    #define DEBUG 0
#endif

#if DEBUG // from makefile environment variable
    #include "stdio.h"
    #define NN_DEBUG_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define NN_DEBUG_PRINTF(fmt, ...)
#endif




#ifdef __cplusplus
}
#endif


#endif

