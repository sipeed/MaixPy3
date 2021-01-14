/**
 * maix neural network lib
 * 
 * @copyright © 2020-2021 Sipeed Ltd, All rights reserved
 * @author neucrack
 * @update --2020.12.28--neucrack: create lib
 *         --
 */
#ifndef __LIBMAIX_NN_H__
#define __LIBMAIX_NN_H__

#include "libmaix_nn_err.h"
#include "libmaix_nn_debug.h"
#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    LIBMAIX_NN_DTYPE_UINT8  = 0,
    LIBMAIX_NN_DTYPE_INT8   = 1,
    LIBMAIX_NN_DTYPE_FLOAT  = 2,
}libmaix_nn_dtype_t;               // data type

typedef enum
{
    LIBMAIX_NN_LAYOUT_HWC    = 0,
    LIBMAIX_NN_LAYOUT_CHW    = 1,
}libmaix_nn_layout_t;


typedef union
{
    struct
    {
        char* param_path;
        char* bin_path;
    }awnn;
    struct
    {
        char* model_path;
        char* reserved;
    }normal;
}libmaix_nn_model_path_t;

typedef struct
{
    int w;
    int h;
    int c;
    libmaix_nn_dtype_t dtype;
    libmaix_nn_layout_t layout;
    void* data;
}libmaix_nn_layer_t;

typedef union
{
    struct
    {
        char**   input_names;
        char**   output_names;
        uint8_t  first_layer_conv_no_pad; // 0/1
        uint8_t  input_num;               // len(input_names)
        uint8_t  output_num;              // len(output_names)
        float    mean[3];
        float    norm[3];
    }awnn;
}libmaix_nn_opt_param_t; // optional param for model


typedef struct libmaix_nn
{
    void* config;
    libmaix_nn_opt_param_t* opt_param;

    libmaix_nn_err_t (*init)(struct libmaix_nn *obj);
    libmaix_nn_err_t (*deinit)(struct libmaix_nn *obj);
    libmaix_nn_err_t (*load)(struct libmaix_nn *obj, const libmaix_nn_model_path_t* path, libmaix_nn_opt_param_t* opt_param);
    /**
     * @param [in] inputs: dtype and data may be changed by forward
     *                      for awnn, it need to quantize input to int8 if dtype is uint8,
     *                                if dtype is int8, will do nothing, other dtype will return LIBMAIX_NN_ERR_PARAM error
     * @param [out] outputs: output feature maps, outputs[i].data must NOT be NULL, dtype is LIBMAIX_NN_DTYPE_FLOAT
     */
    libmaix_nn_err_t (*forward)(struct libmaix_nn *obj, libmaix_nn_layer_t* inputs, libmaix_nn_layer_t* outputs);
}libmaix_nn_t;

libmaix_nn_err_t libmaix_nn_module_init();
libmaix_nn_err_t libmaix_nn_module_deinit();
libmaix_nn_t* libmaix_nn_creat();
void libmaix_nn_destroy(libmaix_nn_t** obj);

#ifdef __cplusplus
}
#endif


#endif

