/**
 * libmaix_nn_err.h, maix neural network lib error enumerate
 * 
 * @copyright © 2020-2021 Sipeed Ltd, All rights reserved
 * @author neucrack
 * @update --2020.12.28--neucrack: create lib
 *         --
 */
#ifndef __LIBMAIX_NN_ERR_H__
#define __LIBMAIX_NN_ERR_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    LIBMAIX_NN_ERR_NONE   = 0,
    LIBMAIX_NN_ERR_PARAM  = 1,
    LIBMAIX_NN_ERR_NO_MEM = 2,
    LIBMAIX_NN_ERR_NOT_IMPLEMENT = 3,
    LIBMAIX_NN_ERR_UNKNOWN,
}libmaix_nn_err_t;         // update this enum you MUST update `libmaix_nn_err_strs` definition too

extern char* libmaix_nn_err_strs[];

static inline char* libmaix_nn_get_err_msg(libmaix_nn_err_t err)
{
    return libmaix_nn_err_strs[err];
}

#ifdef __cplusplus
}
#endif


#endif

