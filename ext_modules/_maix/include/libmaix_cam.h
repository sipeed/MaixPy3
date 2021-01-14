#ifndef __LIBMAIX_CAM_H
#define __LIBMAIX_CAM_H

#include <stdio.h>
#include <stdlib.h>

struct libmaix_cam_t
{
    int width;      /* 摄像头采集图像的宽度 */
    int height;     /* 摄像头采集图像的高度 */

    unsigned int fram_size; /* 一帧图片占用内存大小 */

    int (*strat_capture)(struct libmaix_cam_t *cam);

    /**
     * @brief 获取一帧图像
     * 
     * @param [in] cam: cam对象
     * @param [in] buf: 图片缓存内存地址，
     *            buf至少要有`fram_size`这么大的空间
     * 
     * @return 0:成功，1: 未准备好， 其他: 出错
    */
    int (*capture)(struct libmaix_cam_t *cam, unsigned char *buf);

    void *reserved;
};

/**
 * @brief 创建cam对象, 目前只支持创建一个对象
 * 
 * @param [in] w: 设置采集图像的宽度
 * @param [in] h: 设置采集图像的高度
 * 
 * @return 创建的对象；NULL:出错
*/
struct libmaix_cam_t * libmaix_cam_creat(int w, int h);

/**
 * @brief 销毁cam对象
*/
void libmaix_cam_destroy(struct libmaix_cam_t **cam);

#endif /* __LIBMAIX_CAM_H */
