
#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <vector>
#include <list>
#include <array>
#include <map>
#include <set>
#include <typeinfo>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgcodecs.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs/legacy/constants_c.h>
#include "opencv2/core/types_c.h"

using namespace cv;
using namespace std;

namespace py = pybind11;
#define heigh_t 10
#define debug_line printf("%s:%d %s %s %s \r\n", __FILE__, __LINE__, __FUNCTION__, __DATE__, __TIME__)

// typedef enum
// {
//     LAB = 0,
//     INVALID ,
//     BINARY,
//     GRAY  ,
//     RGB888,          // supported
//     RGB565,
//     RGBA8888,
//     YUV420SP_NV21,   // supported

// }image_mode_t;

// struct libmaix_image
// {

//   cv::Mat obj;

//   int _load() {

//     return 0;
//   }

//   int _save() {

//     return 0;
//   }

// };

// class _maix_image :public libmaix_image
class _maix_image
{
public:
  py::bytes test(py::bytes &rgb)
  {
    std::string tmp = static_cast<std::string>(rgb);
    cv::Mat input(240, 240, CV_8UC3, const_cast<char *>(tmp.c_str()));

    cv::Mat output = Mat::zeros(240, 240, CV_8UC4);

    Mat gray, edges;
    // Mat standard_hough, probabilistic_hough;
    int min_threshold = 50;
    int max_trackbar = 150;
    int s_trackbar = max_trackbar;
    int p_trackbar = max_trackbar;

    cvtColor(input, gray, COLOR_RGB2GRAY);

    Canny(gray, edges, 50, 200, 3);

    cvtColor(edges, output, COLOR_GRAY2BGRA);

    vector<Vec2f> s_lines;
    // cvtColor(edges, standard_hough, COLOR_GRAY2BGR);

    /// 1. Use Standard Hough Transform
    HoughLines(edges, s_lines, 1, CV_PI / 180, min_threshold + s_trackbar, 0, 0);

    /// Show the result
    for (size_t i = 0; i < s_lines.size(); i++)
    {
      float r = s_lines[i][0], t = s_lines[i][1];
      double cos_t = cos(t), sin_t = sin(t);
      double x0 = r * cos_t, y0 = r * sin_t;
      double alpha = 1000;

      Point pt1(cvRound(x0 + alpha * (-sin_t)), cvRound(y0 + alpha * cos_t));
      Point pt2(cvRound(x0 - alpha * (-sin_t)), cvRound(y0 - alpha * cos_t));
      line(output, pt1, pt2, Scalar(255, 0, 0, 200), 3, cv::LINE_AA);
    }

    // vector<Vec4i> p_lines;
    // cvtColor(edges, probabilistic_hough, COLOR_GRAY2BGR);

    // /// 2. Use Probabilistic Hough Transform
    // HoughLinesP(edges, p_lines, 1, CV_PI / 180, min_threshold + p_trackbar, 30, 10);

    // /// Show the result
    // for (size_t i = 0; i < p_lines.size(); i++)
    // {
    //   Vec4i l = p_lines[i];
    //   line(output, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0, 200), 3, LINE_AA);
    // }

    int size = output.total() * output.elemSize();
    return py::bytes((char *)output.data, size);
    // std::vector buff;
    // cv::imencode(".bmp", image, buff);
    // std::string image_string(reinterpret_cast<char*>(&buff[0]), buff.size());
    // return py::bytes();
  }

  _maix_image()
  {
  }

  ~_maix_image()
  {
  }
};

class _maix_vision
{

private:
  /* data */
  py::object PIL_;
  py::object py_img_tobytes_;
  int Distance(int x1, int y1, int x2, int y2)
  {
    int x = abs(x1 - x2);
    int y = abs(y1 - y2);
    return int(round(sqrt(x * x + y * y)));
  }
  int py_img_to_in_img(py::object &py_img, Mat &_out, vector<int> &size, int mode)
  {
    if (py::isinstance<py::bytes>(py_img))
    {
      string tmp = py_img.cast<string>();
      if (size[0] == 0 || size[1] == 0)
      {
        Mat input(240, 240, CV_8UC3, const_cast<char *>(tmp.c_str()));
        input.copyTo(_out);
      }
      else
      {
        cv::Mat input(size[0], size[1], CV_8UC3, const_cast<char *>(tmp.c_str()));
        input.copyTo(_out);
      }
    }
    else
    {
      // auto PIL_ = py::module::import("PIL.Image").attr("Image");
      if (py::isinstance(py_img, this->PIL_))
      {
        // auto tobytes = PIL_.attr("tobytes");
        // auto img_bytes = tobytes(py_img);
        auto img_bytes = this->py_img_tobytes_(py_img);
        string tmp = img_bytes.cast<string>();
        auto img_size = py_img.attr("size").cast<vector<int>>();
        cv::Mat input(img_size[0], img_size[1], CV_8UC3, const_cast<char *>(tmp.c_str()));
        input.copyTo(_out);
      }
    }
    return 0;
  }

public:
  py::bytes test(py::bytes &rgb)
  {
    std::string tmp = static_cast<std::string>(rgb);
    cv::Mat input(240, 240, CV_8UC3, const_cast<char *>(tmp.c_str()));

    cv::Mat output = Mat::zeros(240, 240, CV_8UC4);

    Mat gray, edges;
    // Mat standard_hough, probabilistic_hough;
    int min_threshold = 50;
    int max_trackbar = 150;
    int s_trackbar = max_trackbar;
    int p_trackbar = max_trackbar;

    cvtColor(input, gray, COLOR_RGB2GRAY);

    Canny(gray, edges, 50, 200, 3);

    cvtColor(edges, output, COLOR_GRAY2BGRA);

    vector<Vec2f> s_lines;
    // cvtColor(edges, standard_hough, COLOR_GRAY2BGR);

    /// 1. Use Standard Hough Transform
    HoughLines(edges, s_lines, 1, CV_PI / 180, min_threshold + s_trackbar, 0, 0);

    /// Show the result
    for (size_t i = 0; i < s_lines.size(); i++)
    {
      float r = s_lines[i][0], t = s_lines[i][1];
      double cos_t = cos(t), sin_t = sin(t);
      double x0 = r * cos_t, y0 = r * sin_t;
      double alpha = 1000;

      Point pt1(cvRound(x0 + alpha * (-sin_t)), cvRound(y0 + alpha * cos_t));
      Point pt2(cvRound(x0 - alpha * (-sin_t)), cvRound(y0 - alpha * cos_t));
      line(output, pt1, pt2, Scalar(255, 0, 0, 200), 3, cv::LINE_AA);
    }

    // vector<Vec4i> p_lines;
    // cvtColor(edges, probabilistic_hough, COLOR_GRAY2BGR);

    // /// 2. Use Probabilistic Hough Transform
    // HoughLinesP(edges, p_lines, 1, CV_PI / 180, min_threshold + p_trackbar, 30, 10);

    // /// Show the result
    // for (size_t i = 0; i < p_lines.size(); i++)
    // {
    //   Vec4i l = p_lines[i];
    //   line(output, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0, 200), 3, LINE_AA);
    // }

    int size = output.total() * output.elemSize();
    return py::bytes((char *)output.data, size);
    // std::vector buff;
    // cv::imencode(".bmp", image, buff);
    // std::string image_string(reinterpret_cast<char*>(&buff[0]), buff.size());
    // return py::bytes();
  }

  _maix_vision()
  {
    this->PIL_ = py::module::import("PIL.Image").attr("Image");
    this->py_img_tobytes_ = this->PIL_.attr("tobytes");
  }

  ~_maix_vision()
  {
  }

  py::bytes opencv_test(py::bytes &rgb)
  {
    std::string tmp = static_cast<std::string>(rgb);
    cv::Mat input(240, 240, CV_8UC3, const_cast<char *>(tmp.c_str()));
    int size = input.total() * input.elemSize();
    return py::bytes((char *)input.data, size);
  }

  py::list get_blob_color_max(py::object py_img, vector<int> &roi, int critical, int co, vector<int> size, int mode)
  {
    py::list return_val;
    Mat in_img;
    py_img_to_in_img(py_img, in_img, size, mode); //获取图像

    critical = critical > 100 ? 100 : critical;
    critical = critical < 0 ? 0 : critical;

    Rect rect;
    rect.x = roi[0];
    rect.y = roi[1];
    rect.width = roi[2];
    rect.height = roi[3];
    Mat lab_img;
    // cvtColor(in_img(rect), lab_img, COLOR_RGB2Lab);

    lab_img = in_img(rect);

    vector<Mat> lab_planes;
    split(lab_img, lab_planes);

    int histSize = 256;
    float range[] = {0, 256};
    const float *histRanges = range;
    Mat l_hist, a_hist, b_hist;
    calcHist(&lab_planes[0], 1, 0, Mat(), l_hist, 1, &histSize, &histRanges, true, false);
    calcHist(&lab_planes[1], 1, 0, Mat(), a_hist, 1, &histSize, &histRanges, true, false);
    calcHist(&lab_planes[2], 1, 0, Mat(), b_hist, 1, &histSize, &histRanges, true, false);

    float lmax = 0, lnum = 0;
    float amax = 0, anum = 0;
    float bmax = 0, bnum = 0;
    for (int i = 0; i < histSize; i++)
    {
      if (l_hist.at<float>(i) > lmax)
      {
        lmax = l_hist.at<float>(i);
        lnum = i;
      }
      if (a_hist.at<float>(i) > amax)
      {
        amax = a_hist.at<float>(i);
        anum = i;
      }
      if (b_hist.at<float>(i) > bmax)
      {
        bmax = b_hist.at<float>(i);
        bnum = i;
      }
    }
    switch (co)
    {
    case 0: //rgb
    {
      return_val.append(lnum);
      return_val.append(anum);
      return_val.append(bnum);
      // return_val.append(int(max_lnum * 100 / 255));
      // return_val.append(max_anum - 128);
      // return_val.append(max_bnum - 128);
      return return_val;
    }
    break;
    case 1: //lab
    {

      Mat rgb(1, 1, CV_8UC3, Scalar(lnum, anum, bnum));
      Mat lab;
      cvtColor(rgb, lab, COLOR_RGB2Lab);
      lnum = lab.at<Vec3b>(0, 0)[0];
      anum = lab.at<Vec3b>(0, 0)[1];
      bnum = lab.at<Vec3b>(0, 0)[2];

      int min_lnum = int(lnum - critical);
      min_lnum = min_lnum < 0 ? 0 : min_lnum;
      int max_lnum = int(lnum + critical);
      max_lnum = max_lnum > 180 ? 180 : max_lnum;
      int min_anum = int(anum - critical);
      min_anum = min_anum < 0 ? 0 : min_anum;
      int max_anum = int(anum + critical);
      max_anum = max_anum > 255 ? 255 : max_anum;
      int min_bnum = int(bnum - critical);
      min_bnum = min_bnum < 0 ? 0 : min_bnum;
      int max_bnum = int(bnum + critical);
      max_bnum = max_bnum > 255 ? 255 : max_bnum;
      return_val.append(int(min_lnum * 100 / 255));
      return_val.append(min_anum - 128);
      return_val.append(min_bnum - 128);
      return_val.append(int(max_lnum * 100 / 255));
      return_val.append(max_anum - 128);
      return_val.append(max_bnum - 128);
      return return_val;
    }
    break;
    case 2: //hsv
    {

      Mat rgb(1, 1, CV_8UC3, Scalar(lnum, anum, bnum));
      Mat lab;
      cvtColor(rgb, lab, COLOR_RGB2HSV);
      lnum = lab.at<Vec3b>(0, 0)[0];
      anum = lab.at<Vec3b>(0, 0)[1];
      bnum = lab.at<Vec3b>(0, 0)[2];

      int min_lnum = int(lnum - critical);
      min_lnum = min_lnum < 0 ? 0 : min_lnum;
      int max_lnum = int(lnum + critical);
      max_lnum = max_lnum > 180 ? 180 : max_lnum;
      int min_anum = int(anum - critical);
      min_anum = min_anum < 0 ? 0 : min_anum;
      int max_anum = int(anum + critical);
      max_anum = max_anum > 255 ? 255 : max_anum;
      int min_bnum = int(bnum - critical);
      min_bnum = min_bnum < 0 ? 0 : min_bnum;
      int max_bnum = int(bnum + critical);
      max_bnum = max_bnum > 255 ? 255 : max_bnum;
      return_val.append(min_lnum);
      return_val.append(min_anum);
      return_val.append(min_bnum);
      return_val.append(max_lnum);
      return_val.append(max_anum);
      return_val.append(max_bnum);
      return return_val;
    }
    break;
    default:

      break;
    }
    return return_val;
  }

  py::list find_blob_lab(py::object py_img, vector<vector<int>> &thresholds, vector<int> size, int mode, vector<int> roi, int x_stride, int y_stride, bool invert, int area_threshold, int pixels_threshold, bool merge, int margin, int tilt)
  {
    py::list return_val;
    Mat in_img;
    py_img_to_in_img(py_img,in_img,size,mode);

    Mat lab, mask1;
    if (roi[2] != 0 && roi[3] != 0)
    {
      Rect rect(roi[0], roi[1], roi[2], roi[3]);
      cvtColor(in_img(rect), lab, COLOR_RGB2Lab);
    }
    else
    {
      cvtColor(in_img, lab, COLOR_RGB2Lab);
    }
    Mat mask = Mat::zeros(lab.size(), CV_8UC1);
    for (int i = 0; i < thresholds.size(); i++)
    {
      inRange(lab, Scalar(int((thresholds[i][0] * 255) / 100), thresholds[i][1] + 128, thresholds[i][2] + 128), Scalar(int((thresholds[i][3] * 255) / 100), thresholds[i][4] + 128, thresholds[i][5] + 128), mask1);
      mask = mask + mask1;
    }
    if (invert)
    {
      bitwise_not(mask, mask);
    }

    Mat se = getStructuringElement(MORPH_RECT, Size(x_stride, y_stride), Point(-1, -1)); //开运算,去除噪点
    morphologyEx(mask, mask, MORPH_OPEN, se);
    if (margin != 0)
    {
      Mat se_t = getStructuringElement(MORPH_RECT, Size(margin, margin), Point(-1, -1)); //闭运算,链接相邻比较近的色块
      morphologyEx(mask, mask, MORPH_CLOSE, se_t);
    }

    vector<vector<Point>> contours;
    vector<Vec4i> hiearchy;
    findContours(mask, contours, hiearchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    if (contours.size() == 0)
    {
      return return_val;
    }
    for (int i = 0; i < contours.size(); i++)
    {
      py::dict val;
      Rect rects = boundingRect(contours[i]);
      val["x"] = int(rects.x);
      val["y"] = int(rects.y);
      val["w"] = int(rects.width);
      val["h"] = int(rects.height);
      if (int(rects.width) * int(rects.width) < area_threshold)
      {
        continue;
      }
      if (int(contourArea(contours[i])) < pixels_threshold)
      {
        continue;
      }
      val["pixels"] = int(contourArea(contours[i]));

      val["cx"] = int(rects.x + rects.width / 2);
      val["cy"] = int(rects.y + rects.height / 2);

      if (tilt)
      {
        RotatedRect minRect = minAreaRect(contours[i]);
        Point2f rect_points[4];
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
      return_val.append(val);
    }
    return return_val;
  }

  py::list find_ball_lab(py::object py_img, vector<int> &thresholds, vector<int> size, int mode)
  {
    Mat in_img;
    py_img_to_in_img(py_img, in_img, size, mode);

    Mat hsv, mask;
    cvtColor(in_img, hsv, COLOR_RGB2Lab);
    inRange(hsv, Scalar(int(thresholds[0] * 255 / 100), thresholds[1] + 128, thresholds[2] + 128), Scalar(int(thresholds[3] * 255 / 100), thresholds[4] + 128, thresholds[5] + 128), mask);
    // cout << hsv_da <<endl;
    Mat se = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
    morphologyEx(mask, mask, MORPH_OPEN, se);
    vector<vector<Point>> contours;
    vector<Vec4i> hiearchy;
    findContours(mask, contours, hiearchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    py::list out;
    for (int i = 0; i < contours.size(); i++)
    {
      /* 当拟合点数少于6个时，不进行拟合 */
      if (contours[i].size() < 6)
      {
        break;
      }
      // 圆拟合
      RotatedRect rrt = fitEllipse(contours[i]);
      int cr_x, cr_y, cr_w, cr_h;
      cr_x = rrt.center.x;
      cr_y = rrt.center.y;
      cr_w = rrt.size.width;
      cr_h = rrt.size.height;
      /* 当图形长宽相差太大 或者 图形面积太小时，不进行处理 */
      if ((abs(cr_w - cr_h) > 10) ||
          (cr_w * cr_h) < 400)
      {
        break;
      }
      py::list tmp;
      tmp.append(cr_x);
      tmp.append(cr_y);
      tmp.append(cr_w);
      tmp.append(cr_h);
      out.append(tmp);
    }
    return std::move(out);
  }

  // py::list find_ball_hsv(py::object py_img, vector<int> &thresholds, vector<int> size, int mode)
  // {
  //   // py::list find_ball(py::bytes &rgb, vector<int> &hsv_da)
  //   // {

  //   Mat in_img;
  //   if (py::isinstance<py::bytes>(py_img))
  //   {
  //     string tmp = py_img.cast<string>();
  //     if (size[0] == 0 || size[1] == 0)
  //     {
  //       cv::Mat input(240, 240, CV_8UC3, const_cast<char *>(tmp.c_str()));

  //       in_img = input;
  //     }
  //     else
  //     {
  //       cv::Mat input(size[0], size[1], CV_8UC3, const_cast<char *>(tmp.c_str()));
  //       in_img = input;
  //     }
  //   }
  //   else
  //   {
  //     auto PIL_ = py::module::import("PIL.Image").attr("Image");

  //     if (py::isinstance(py_img, PIL_))
  //     {
  //       auto tobytes = PIL_.attr("tobytes");
  //       auto img_bytes = tobytes(py_img);
  //       string tmp = py_img.cast<string>();

  //       auto mdd = py_img.attr("size").cast<vector<int>>();
  //       cv::Mat input(mdd[0], mdd[1], CV_8UC3, const_cast<char *>(tmp.c_str()));
  //       in_img = input;
  //     }
  //   }
  //   Mat hsv, mask;
  //   cvtColor(in_img, hsv, COLOR_RGB2HSV);
  //   inRange(hsv, Scalar(thresholds[0], thresholds[1], thresholds[2]), Scalar(thresholds[3], thresholds[4], thresholds[5]), mask);
  //   // cout << hsv_da <<endl;
  //   Mat se = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
  //   morphologyEx(mask, mask, MORPH_OPEN, se);
  //   vector<vector<Point>> contours;
  //   vector<Vec4i> hiearchy;
  //   findContours(mask, contours, hiearchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

  //   py::list out;

  //   for (int i = 0; i < contours.size(); i++)
  //   {
  //     /* 当拟合点数少于6个时，不进行拟合 */
  //     if (contours[i].size() < 6)
  //     {
  //       break;
  //     }

  //     // 圆拟合
  //     RotatedRect rrt = fitEllipse(contours[i]);

  //     int cr_x, cr_y, cr_w, cr_h;

  //     cr_x = rrt.center.x;
  //     cr_y = rrt.center.y;
  //     cr_w = rrt.size.width;
  //     cr_h = rrt.size.height;

  //     /* 当图形长宽相差太大 或者 图形面积太小时，不进行处理 */
  //     if ((abs(cr_w - cr_h) > 10) ||
  //         (cr_w * cr_h) < 400)
  //     {
  //       break;
  //     }

  //     py::list tmp;

  //     tmp.append(cr_x);
  //     tmp.append(cr_y);
  //     tmp.append(cr_w);
  //     tmp.append(cr_h);

  //     out.append(tmp);
  //   }
  //   return std::move(out);
  // }
  // std::string tmp = static_cast<std::string>(rgb);
  // cv::Mat input(240, 240, CV_8UC3, const_cast<char *>(tmp.c_str()));

  // A* c = new A(1);
  py::dict find_line(py::object py_img, vector<int> size, int mode)
  {
    Mat src_gray, dst;
    py::dict return_val;
    Mat in_img;
    py_img_to_in_img(py_img, in_img, size, mode);
    Mat src_gary, mask;
    cvtColor(in_img, src_gray, COLOR_RGB2GRAY); //将图片变成灰度图
    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
    erode(src_gray, src_gray, element);
    dilate(src_gray, src_gray, element);
    threshold(src_gray, src_gray, 0, 255, THRESH_BINARY | THRESH_OTSU);
    dilate(src_gray, dst, element); //膨胀
    Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.width = dst.cols;
    rect.height = heigh_t;
    dst(rect).setTo(255);
    rect.x = 0;
    rect.y = dst.rows - heigh_t;
    rect.width = dst.cols;
    rect.height = heigh_t;
    dst(rect).setTo(255);
    rect.x = 0;
    rect.y = 0;
    rect.width = heigh_t;
    rect.height = dst.rows;
    dst(rect).setTo(255);
    rect.x = dst.cols - heigh_t;
    rect.y = 0;
    rect.width = heigh_t;
    rect.height = dst.rows;
    dst(rect).setTo(255);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(dst, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point());
    if (contours.size() == 0)
    {

      return std::move(return_val);
    }
    int area = 0, a_n = 0;
    for (int i = 1; i < contours.size(); i++)
    {
      int ar = contourArea(contours[i]);
      if (ar > area)
      {
        area = ar;
        a_n = i;
      }
    }
    RotatedRect minRect = minAreaRect(contours[a_n]);
    Point2f rect_points[4];
    minRect.points(rect_points);
    py::list tmps;
    tmps.append(int(rect_points[0].x));
    tmps.append(int(rect_points[0].y));
    tmps.append(int(rect_points[1].x));
    tmps.append(int(rect_points[1].y));
    tmps.append(int(rect_points[2].x));
    tmps.append(int(rect_points[2].y));
    tmps.append(int(rect_points[3].x));
    tmps.append(int(rect_points[3].y));
    return_val["rect"] = tmps;
    return_val["pixels"] = area;
    int cx, cy;
    cx = int((((rect_points[0].x - rect_points[1].x) + (rect_points[2].x - rect_points[1].x)) / 2) + rect_points[1].x);
    cy = int((((rect_points[0].y - rect_points[1].y) + (rect_points[2].y - rect_points[1].y)) / 2) + rect_points[1].y);
    return_val["cx"] = cx;
    return_val["cy"] = cy;
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
    return_val["rotation"] = k;
    return std::move(return_val);
  }
};

PYBIND11_MODULE(_maix_opencv, m)
{
  pybind11::class_<_maix_vision>(m, "Vision")
      .def(pybind11::init<>())
      .def("get_blob_lab", &_maix_vision::get_blob_color_max, py::arg("py_img"), py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("critical") = 0, py::arg("color") = 0, py::arg("size") = std::vector<int>{0, 0}, py::arg("mode") = 0)
      .def("get_blob_color_max", &_maix_vision::get_blob_color_max, py::arg("py_img"), py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("critical") = 0, py::arg("color") = 0, py::arg("size") = std::vector<int>{0, 0}, py::arg("mode") = 0)
      .def("find_blob_lab", &_maix_vision::find_blob_lab, py::arg("py_img"), py::arg("thresholds"), py::arg("size") = std::vector<int>{0, 0}, py::arg("mode") = 0, py::arg("roi") = std::vector<int>{0, 0, 0, 0}, py::arg("x_stride") = 2, py::arg("y_stride") = 2, py::arg("invert") = 0, py::arg("area_threshold") = 10, py::arg("pixels_threshold") = 10, py::arg("merge") = 0, py::arg("margin") = 0, py::arg("tilt") = 0)
      .def("find_ball_lab", &_maix_vision::find_ball_lab, py::arg("py_img"), py::arg("thresholds"), py::arg("size") = std::vector<int>{0, 0}, py::arg("mode") = 0)
      .def("find_line", &_maix_vision::find_line, py::arg("py_img"), py::arg("size") = std::vector<int>{0, 0}, py::arg("mode") = 0);

  pybind11::class_<_maix_image>(m, "Image")
      .def(pybind11::init<>())
      .def("open", &_maix_image::test)
      .def("save", &_maix_image::test)
      .def("format", &_maix_image::test)
      .def("size", &_maix_image::test)
      .def("tobytes", &_maix_image::test)
      .def("resize", &_maix_image::test)
      .def("rotate", &_maix_image::test)
      .def("crop", &_maix_image::test)
      .def("convert", &_maix_image::test)
      .def("mode", &_maix_image::test)
      .def("draw_ellipse", &_maix_image::test)
      .def("draw_string", &_maix_image::test)
      .def("draw_circle", &_maix_image::test)
      .def("draw_rectangle", &_maix_image::test)
      .def("draw_line", &_maix_image::test)
      .def("load_freetype", &_maix_image::test);
}
