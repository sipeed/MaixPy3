#include "maix_image.h"
// Mat img
// img.cols 宽
// img.rows 高
// Mat::Mat(int rows, int cols, int type, void* data, size_t step=AUTO_STEP)
maix_custom::maix_custom()
{
}

py::list maix_custom::_find_ball_blob(std::vector<std::vector<int>> &thresholds, std::vector<int> roi, int x_stride, int y_stride, bool invert, int area_threshold, int pixels_threshold, bool merge, int margin, int tilt, int h_min, int w_min, int co)
{
    py::list return_val;
    cv::Mat src(this->_img->height, this->_img->width, any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][2]), this->_img->data);
    cv::Mat in_img;
    src.copyTo(in_img);
    cv::Mat lab, mask1;
    bool grasy = 0;
    switch (co)
    {
    case 0: //rgb
        if (in_img.channels() != 3)
            return return_val;
        if (roi[2] != 0 && roi[3] != 0)
        {
            cv::Rect rect(roi[0], roi[1], roi[2], roi[3]);
            lab = in_img(rect);
        }
        else
        {
            lab = in_img;
        }
        break;
    case 1: //lab
        if (in_img.channels() != 3)
            return return_val;
        if (roi[2] != 0 && roi[3] != 0)
        {
            cv::Rect rect(roi[0], roi[1], roi[2], roi[3]);
            cvtColor(in_img(rect), lab, cv::COLOR_RGB2Lab);
        }
        else
        {
            cvtColor(in_img, lab, cv::COLOR_RGB2Lab);
        }
        for (size_t i = 0; i < thresholds.size(); i++)
        {
            thresholds[i][0] = int((thresholds[i][0] * 255) / 100);
            thresholds[i][1] = thresholds[i][1] + 128;
            thresholds[i][2] = thresholds[i][2] + 128;
            thresholds[i][3] = int((thresholds[i][3] * 255) / 100);
            thresholds[i][4] = thresholds[i][4] + 128;
            thresholds[i][5] = thresholds[i][5] + 128;
        }
        break;
    case 2: //hsv
        if (in_img.channels() != 3)
            return return_val;
        if (roi[2] != 0 && roi[3] != 0)
        {
            cv::Rect rect(roi[0], roi[1], roi[2], roi[3]);
            cvtColor(in_img(rect), lab, cv::COLOR_RGB2HSV);
        }
        else
        {
            cvtColor(in_img, lab, cv::COLOR_RGB2HSV);
        }
        for (size_t i = 0; i < thresholds.size(); i++)
        {
            thresholds[i][0] = int(thresholds[i][0] / 2);
            thresholds[i][1] = int(thresholds[i][1] * 2.55);
            thresholds[i][2] = int(thresholds[i][2] * 2.55);
            thresholds[i][3] = int(thresholds[i][3] / 2);
            thresholds[i][4] = int(thresholds[i][4] * 2.55);
            thresholds[i][5] = int(thresholds[i][5] * 2.55);
        }
        break;
    case 3: //灰度
        if (in_img.channels() != 1)
            return return_val;
        if (roi[2] != 0 && roi[3] != 0)
        {
            cv::Rect rect(roi[0], roi[1], roi[2], roi[3]);
            lab = in_img(rect);
        }
        else
        {
            lab = in_img;
        }
        grasy = 1;
        break;
    default:
        return return_val;
        break;
    }
    cv::Mat mask = cv::Mat::zeros(lab.size(), CV_8UC1);
    if (grasy)
    {
        for (size_t i = 0; i < thresholds.size(); i++)
        {
            cv::inRange(lab, cv::Scalar(thresholds[i][0]), cv::Scalar(thresholds[i][1]), mask1);
            mask = mask + mask1;
        }
    }
    else
    {
        for (size_t i = 0; i < thresholds.size(); i++)
        {
            cv::inRange(lab, cv::Scalar(thresholds[i][0], thresholds[i][1], thresholds[i][2]), cv::Scalar(thresholds[i][3], thresholds[i][4], thresholds[i][5]), mask1);
            mask = mask + mask1;
        }
    }
    if (invert)
    {
        cv::bitwise_not(mask, mask);
    }
    cv::Mat se = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(x_stride, y_stride), cv::Point(-1, -1));
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, se);
    if (margin != 0)
    {
        cv::Mat se_t = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(margin, margin), cv::Point(-1, -1));
        cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, se_t);
    }
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hiearchy;
    cv::findContours(mask, contours, hiearchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    if (contours.size() == 0)
    {
        return return_val;
    }
    for (size_t i = 0; i < contours.size(); i++)
    {
        py::dict val;
        cv::Rect rects = cv::boundingRect(contours[i]);
        if (int(rects.width) * int(rects.width) < area_threshold)
        {
            continue;
        }
        if (int(cv::contourArea(contours[i])) < pixels_threshold)
        {
            continue;
        }
        if(int(rects.width) < w_min)
        {
            continue;
        }
        if(int(rects.height) < h_min)
        {
            continue;
        }
        val["x"] = int(rects.x);
        val["y"] = int(rects.y);
        val["w"] = int(rects.width);
        val["h"] = int(rects.height);
        val["pixels"] = int(cv::contourArea(contours[i]));
        val["cx"] = int(rects.x + rects.width / 2);
        val["cy"] = int(rects.y + rects.height / 2);
        if (tilt)
        {
            cv::RotatedRect minRect = cv::minAreaRect(contours[i]);
            cv::Point2f rect_points[4];
            minRect.points(rect_points);
            py::tuple tmp3 = py::make_tuple(rect_points[0].x, rect_points[0].y, rect_points[1].x, rect_points[1].y, rect_points[2].x, rect_points[2].y, rect_points[3].x, rect_points[3].y);
            val["tilt_Rect"] = tmp3;
            int tmp1 = Distance(int(rect_points[0].x), int(rect_points[0].y), int(rect_points[1].x), int(rect_points[1].y));
            int tmp2 = Distance(int(rect_points[0].x), int(rect_points[0].y), int(rect_points[3].x), int(rect_points[3].y));
            float x1, y1, k;
            if (tmp1 > tmp2)
            {
                x1 = rect_points[1].x - rect_points[0].x;
                y1 = rect_points[1].y - rect_points[0].y;
                k = atan(y1 / x1);
            }
            else
            {
                x1 = rect_points[3].x - rect_points[0].x;
                y1 = rect_points[3].y - rect_points[0].y;
                k = atan(y1 / x1);
            }
            val["rotation"] = k;
        }
        if (contours[i].size() < 6)
        {
            val["ball"] = false;
            val["ball_obj"] = py::none();
            return_val.append(val);
            continue;
        }
        cv::RotatedRect rrt = fitEllipse(contours[i]);
        int cr_x, cr_y, cr_w, cr_h;
        cr_x = rrt.center.x;
        cr_y = rrt.center.y;
        cr_w = rrt.size.width;
        cr_h = rrt.size.height;
        if ((abs(cr_w - cr_h) > 10) ||
            (cr_w * cr_h) < 400)
        {
            val["ball"] = false;
            val["ball_obj"] = py::none();
            return_val.append(val);
        }
        else
        {
            int cr_x1, cr_y1, cr_x2, cr_y2;
            cr_x1 = cr_x - cr_w / 2;
            cr_x2 = cr_x + cr_w / 2;
            cr_y1 = cr_y - cr_h / 2;
            cr_y2 = cr_y + cr_h / 2;
            py::list tmp;
            tmp.append(cr_x);
            tmp.append(cr_y);
            tmp.append(cr_w);
            tmp.append(cr_h);
            tmp.append(cr_x1);
            tmp.append(cr_y1);
            tmp.append(cr_x2);
            tmp.append(cr_y2);
            val["ball"] = true;
            val["ball_obj"] = tmp;
            return_val.append(val);
        }
    }
    return return_val;
}
