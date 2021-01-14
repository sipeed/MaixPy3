#ifndef __LIBMAIX_NN_DECODER_YOLO2_H
#define __LIBMAIX_NN_DECODER_YOLO2_H

#include <stdint.h>
#include <stdbool.h>
#include "libmaix_nn_err.h"
#include "libmaix_nn_decoder.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    float x;
    float y;
    float w;
    float h;
}libmaix_nn_decoder_yolo2_box_t;

typedef struct
{
    uint32_t classes_num;
    float    threshold;
    float    nms_value;
    uint32_t anchors_num;
    float*   anchors;
    uint32_t net_in_width;     // net input width, e.g. 224
    uint32_t net_in_height;
    uint32_t net_out_width;    // net output width, e.g. 7
    uint32_t net_out_height;
    uint32_t input_width;      // input image width, e.g. 320, if it not the same as net input, the output box max width is the same as image width
    uint32_t input_height;
}libmaix_nn_decoder_yolo2_config_t;

typedef struct
{
    uint32_t                        boxes_num;
    libmaix_nn_decoder_yolo2_box_t* boxes;
    float**                         probs;      // shape: (boxes_num, classes_num, dtype='float')
}libmaix_nn_decoder_yolo2_result_t;


typedef void (*callback_draw_result_func_t)(uint32_t id, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t class_id, float prob, char* label);

libmaix_nn_err_t libmaix_nn_decoder_yolo2_init(struct libmaix_nn_decoder* obj, void* config);
libmaix_nn_err_t libmaix_nn_decoder_yolo2_deinit(struct libmaix_nn_decoder* obj);
libmaix_nn_err_t libmaix_nn_decoder_yolo2_decode(struct libmaix_nn_decoder* obj, libmaix_nn_layer_t* feature_map, void* result);
void libmaix_nn_decoder_yolo2_draw_result(struct libmaix_nn_decoder* obj, libmaix_nn_decoder_yolo2_result_t* result, uint32_t id, char** labels, callback_draw_result_func_t callback);

libmaix_nn_decoder_t* libmaix_nn_decoder_yolo2_creat();
void libmaix_nn_decoder_yolo2_destroy(libmaix_nn_decoder_t** obj);


#ifdef __cplusplus
}
#endif

#endif
