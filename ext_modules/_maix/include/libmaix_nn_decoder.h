/**
 * maix neural network lib, network decoder
 * 
 * @copyright © 2020-2021 Sipeed Ltd, All rights reserved
 * @author neucrack
 * @update --2020.12.28--neucrack: create lib
 *         --
 */
#ifndef __LIBMAIX_NN_DECODER_H__
#define __LIBMAIX_NN_DECODER_H__

#include "libmaix_nn_err.h"
#include "libmaix_nn.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct libmaix_nn_decoder
{
    libmaix_nn_err_t (*init)(struct libmaix_nn_decoder *obj, void* config);
    libmaix_nn_err_t (*deinit)(struct libmaix_nn_decoder *obj);
    libmaix_nn_err_t (*decode)(struct libmaix_nn_decoder *obj, libmaix_nn_layer_t* feature_map, void* result);

    void* data;
} libmaix_nn_decoder_t;

typedef libmaix_nn_err_t (*libmaic_nn_decoder_init_func_t)(struct libmaix_nn_decoder *obj, void* config);
typedef libmaix_nn_err_t (*libmaic_nn_decoder_deinit_func_t)(struct libmaix_nn_decoder *obj);
typedef libmaix_nn_err_t (*libmaic_nn_decoder_decode_func_t)(struct libmaix_nn_decoder *obj, libmaix_nn_layer_t* feature_map, void* result);


libmaix_nn_decoder_t* libmaix_nn_decoder_creat(libmaic_nn_decoder_init_func_t init_func, libmaic_nn_decoder_deinit_func_t deinit_func, 
                                                libmaic_nn_decoder_decode_func_t decode_func);


void libmaix_nn_decoder_destroy(libmaix_nn_decoder_t** obj);


#ifdef __cplusplus
}
#endif


#endif

