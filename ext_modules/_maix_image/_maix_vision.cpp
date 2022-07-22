#include "maix_image.h"

static class maix_vision::imlib_env imlib;

// Mat img
// img.cols 宽
// img.rows 高
// Mat::Mat(int rows, int cols, int type, void* data, size_t step=AUTO_STEP)
maix_vision::maix_vision()
{
}
int Distance(int x1, int y1, int x2, int y2)
{
  int x = abs(x1 - x2);
  int y = abs(y1 - y2);
  return int(round(sqrt(x * x + y * y)));
}
py::list maix_vision::get_blob_color_max(std::vector<int> &roi, int critical, int co)
{
  py::list return_val;
  cv::Mat src(this->_img->height, this->_img->width, any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][2]), this->_img->data);
  cv::Mat in_img;
  src.copyTo(in_img);
  critical = critical > 100 ? 100 : critical;
  critical = critical < 0 ? 0 : critical;
  cv::Rect rect;
  rect.x = std::max(roi[0], 0);
  rect.y = std::max(roi[1], 0);
  rect.width = std::min(roi[2], this->_img->width - roi[0]);
  rect.height = std::min(roi[3], this->_img->height - roi[1]);
  cv::Mat lab_img;
  lab_img = in_img(rect);
  std::vector<cv::Mat> lab_planes;
  split(lab_img, lab_planes);
  int histSize = 256;
  float range[] = {0, 256};
  const float *histRanges = range;
  cv::Mat l_hist, a_hist, b_hist;
  calcHist(&lab_planes[0], 1, 0, cv::Mat(), l_hist, 1, &histSize, &histRanges, true, false);
  calcHist(&lab_planes[1], 1, 0, cv::Mat(), a_hist, 1, &histSize, &histRanges, true, false);
  calcHist(&lab_planes[2], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRanges, true, false);
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
  case 0: // rgb
  {
    return_val.append(lnum);
    return_val.append(anum);
    return_val.append(bnum);
    return return_val;
  }
  break;
  case 1: // lab
  {
    cv::Mat rgb(1, 1, CV_8UC3, cv::Scalar(lnum, anum, bnum));
    cv::Mat lab;
    cvtColor(rgb, lab, cv::COLOR_RGB2Lab);
    lnum = lab.at<cv::Vec3b>(0, 0)[0];
    anum = lab.at<cv::Vec3b>(0, 0)[1];
    bnum = lab.at<cv::Vec3b>(0, 0)[2];
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
  case 2: // hsv
  {
    cv::Mat rgb(1, 1, CV_8UC3, cv::Scalar(lnum, anum, bnum));
    cv::Mat lab;
    cvtColor(rgb, lab, cv::COLOR_RGB2HSV);
    lnum = lab.at<cv::Vec3b>(0, 0)[0];
    anum = lab.at<cv::Vec3b>(0, 0)[1];
    bnum = lab.at<cv::Vec3b>(0, 0)[2];
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

py::list maix_vision::_maix_vision_find_blob(std::vector<std::vector<int>> &thresholds, std::vector<int> roi, int x_stride, int y_stride, bool invert, int area_threshold, int pixels_threshold, bool merge, int margin, int tilt, int co)
{
  py::list return_val;
  cv::Mat src(this->_img->height, this->_img->width, any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][2]), this->_img->data);
  cv::Mat in_img;
  src.copyTo(in_img);
  cv::Mat lab, mask1;
  bool grasy = 0;
  switch (co)
  {
  case 0: // rgb
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
  case 1: // lab
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
      thresholds[i][1] = thresholds[i][2] + 128;
      thresholds[i][2] = thresholds[i][4] + 128;
      thresholds[i][3] = int((thresholds[i][1] * 255) / 100);
      thresholds[i][4] = thresholds[i][3] + 128;
      thresholds[i][5] = thresholds[i][5] + 128;
    }
    break;
  case 2: // hsv
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
    return_val.append(val);
  }
  return return_val;
}

py::list maix_vision::_maix_vision_find_ball_blob(std::vector<int> &thresholds, int co)
{
  py::list out;
  cv::Mat src(this->_img->height, this->_img->width, any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][2]), this->_img->data);
  cv::Mat in_img;
  src.copyTo(in_img);
  cv::Mat hsv, mask;
  switch (co)
  {
  case 0: // rgb
    if (in_img.channels() != 3)
      return out;
    hsv = in_img;
    break;
  case 1: // lab
    if (in_img.channels() != 3)
      return out;
    cv::cvtColor(in_img, hsv, cv::COLOR_RGB2Lab);
    thresholds[0] = int((thresholds[0] * 255) / 100);
    thresholds[1] = thresholds[1] + 128;
    thresholds[2] = thresholds[2] + 128;
    thresholds[3] = int((thresholds[3] * 255) / 100);
    thresholds[4] = thresholds[4] + 128;
    thresholds[5] = thresholds[5] + 128;
    break;
  case 2: // hsv
    if (in_img.channels() != 3)
      return out;
    cv::cvtColor(in_img, hsv, cv::COLOR_RGB2HSV);
    thresholds[0] = int(thresholds[0] / 2);
    thresholds[1] = int(thresholds[1] * 2.55);
    thresholds[2] = int(thresholds[2] * 2.55);
    thresholds[3] = int(thresholds[3] / 2);
    thresholds[4] = int(thresholds[4] * 2.55);
    thresholds[5] = int(thresholds[5] * 2.55);
    break;
  default:
    return out;
    break;
  }
  cv::inRange(hsv, cv::Scalar(thresholds[0], thresholds[1], thresholds[2]), cv::Scalar(thresholds[3], thresholds[4], thresholds[5]), mask);
  cv::Mat se = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5), cv::Point(-1, -1));
  cv::morphologyEx(mask, mask, cv::MORPH_OPEN, se);
  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hiearchy;
  cv::findContours(mask, contours, hiearchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
  for (size_t i = 0; i < contours.size(); i++)
  {
    if (contours[i].size() < 6)
    {
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
      continue;
    }
    py::list tmp;
    tmp.append(cr_x);
    tmp.append(cr_y);
    tmp.append(cr_w);
    tmp.append(cr_h);
    out.append(tmp);
  }
  return out;
}

#define heigh_t 10
py::dict find_line_old(cv::Mat &src)
{
  cv::Mat src_gray, dst;
  py::dict return_val;
  cv::Mat in_img = src;
  cv::Mat src_gary, mask;
  cv::cvtColor(in_img, src_gray, cv::COLOR_RGB2GRAY); //将图片变成灰度图
  cv::Mat element = getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
  cv::erode(src_gray, src_gray, element);
  cv::dilate(src_gray, src_gray, element);
  cv::threshold(src_gray, src_gray, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
  cv::dilate(src_gray, dst, element); //膨胀
  cv::Rect rect;
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
  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(dst, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE, cv::Point());
  if (contours.size() == 0)
  {

    return std::move(return_val);
  }
  int area = 0, a_n = 0;
  for (size_t i = 1; i < contours.size(); i++)
  {
    int ar = contourArea(contours[i]);
    if (ar > area)
    {
      area = ar;
      a_n = i;
    }
  }
  cv::RotatedRect minRect = cv::minAreaRect(contours[a_n]);
  cv::Point2f rect_points[4];
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
enum adaptiveMethod
{
  meanFilter,
  gaaussianFilter,
  medianFilter
};
void AdaptiveThreshold(cv::Mat &src, cv::Mat &dst, double Maxval, int Subsize, double c, adaptiveMethod method = meanFilter)
{

  if (src.channels() > 1)
    cv::cvtColor(src, src, cv::COLOR_RGB2GRAY);

  cv::Mat smooth;
  switch (method)
  {
  case meanFilter:
    cv::blur(src, smooth, cv::Size(Subsize, Subsize)); //均值滤波
    break;
  case gaaussianFilter:
    cv::GaussianBlur(src, smooth, cv::Size(Subsize, Subsize), 0, 0); //高斯滤波
    break;
  case medianFilter:
    cv::medianBlur(src, smooth, Subsize); //中值滤波
    break;
  default:
    break;
  }
  smooth = smooth - c;
  src.copyTo(dst);
  for (int r = 0; r < src.rows; ++r)
  {
    const uchar *srcptr = src.ptr<uchar>(r);
    const uchar *smoothptr = smooth.ptr<uchar>(r);
    uchar *dstptr = dst.ptr<uchar>(r);
    for (int c = 0; c < src.cols; ++c)
    {
      if (srcptr[c] > smoothptr[c])
      {
        dstptr[c] = Maxval;
      }
      else
        dstptr[c] = 0;
    }
  }
}

py::dict maix_vision::find_line(int func)
{
  cv::Mat src(this->_img->height, this->_img->width, any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][2]), this->_img->data);
  if (func == 0)
  {
    return find_line_old(src);
  }
  py::dict return_val;
  cv::Mat src_gray, dst;
  cv::Mat in_img;
  src.copyTo(in_img);
  // cv::imwrite("/tmp/src.jpg", in_img);
  cv::Mat src_gary;
  cvtColor(in_img, src_gray, cv::COLOR_RGB2GRAY); //将图片变成灰度图
  // cv::imwrite("/tmp/src.jpg", src_gray);
  cv::Mat element = getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
  cv::dilate(src_gray, src_gray, element); // 放大主线
  // cv::imwrite("/tmp/src_gray_0.jpg", src_gray);
  cv::erode(src_gray, src_gray, element); // 消除细线
  // cv::imwrite("/tmp/src_gray_1.jpg", src_gray);
  // AdaptiveThreshold(src_gray, src_gray, 255, block_size, offset, meanFilter); //
  AdaptiveThreshold(src_gray, src_gray, 255, 21, 10, meanFilter); //
  // cv::imwrite("/tmp/src_gray_2.jpg", src_gray);
  cv::dilate(src_gray, dst, element);
  // cv::imwrite("/tmp/dst.jpg", dst);
  cv::Rect rect;
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
  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(dst, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE, cv::Point());
  if (contours.size() == 0)
  {
    return std::move(return_val);
  }
  int area = 0, a_n = 0;
  for (size_t i = 1; i < contours.size(); i++)
  {
    int ar = contourArea(contours[i]);
    if (ar > area)
    {
      area = ar;
      a_n = i;
    }
  }
  cv::RotatedRect minRect = minAreaRect(contours[a_n]);
  cv::Point2f rect_points[4];
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
  return return_val;
}

/*
函数原型：
_imlib_find_rects(std::vector<int> &roi,uint32_t threshold,int is_xywh = 0)
roi：图像ROI区域，默认为整个图像
threshold：阈值
is_xywh：返回值类型选择
is_xywh = 0：返回值为：(x1,y1,x2,y2,magnitude)
is_xywh = 1:返回值为：（x,y,w,h,magnitude)
*/
py::list maix_vision::_imlib_find_rects(std::vector<int> &roi, uint32_t threshold, int is_xywh)
{
  py::list return_val;
  if (NULL == this->_img)
  {
    py::print("no img");
    return return_val;
  }

  image_t img = {};
  img.w = this->_img->width;
  img.h = this->_img->height;
  img.pixels = (uint8_t *)this->_img->data;
  img.pixfmt = PIXFORMAT_RGB888;

  rectangle_t _roi;

  _roi.x = roi[0];
  _roi.y = roi[1];
  _roi.w = roi[2];
  _roi.h = roi[3];
  //默认整个图像
  if (_roi.w == 0)
    _roi.w = img.w;
  if (_roi.h == 0)
    _roi.h = img.h;

  list_t out;

  fb_alloc_mark();
  imlib_find_rects(&out, &img, &_roi, threshold);
  fb_alloc_free_till_mark();

  for (size_t i = 0; list_size(&out); i++)
  {
    py::list tmps;
    find_rects_list_lnk_data_t lnk_data;
    list_pop_front(&out, &lnk_data);
    if (is_xywh)
    {
      tmps.append(lnk_data.rect.x);
      tmps.append(lnk_data.rect.y);
      tmps.append(lnk_data.rect.w);
      tmps.append(lnk_data.rect.h);
      tmps.append(lnk_data.magnitude);
    }
    else
    {
      tmps.append(lnk_data.corners[0].x);
      tmps.append(lnk_data.corners[0].y);
      tmps.append(lnk_data.corners[2].x);
      tmps.append(lnk_data.corners[2].y);
      tmps.append(lnk_data.magnitude);
    }
    return_val.append(tmps);
  }
  return return_val;
}

/*
函数原型：
_imlib_find_lines(std::vector<int> &roi,unsigned int x_stride, unsigned int y_stride,uint32_t threshold, unsigned int theta_margin, unsigned int rho_margin)
roi：图像ROI区域，默认为整个图像
x_stride:霍夫变换时要跳过的 x 像素数
y_stride:霍夫变换时要跳过的 y 像素数
threshold阈值
theta_margin:控制检测到的行的合并
rho_margin:控制检测到的行的合并
*/
py::list maix_vision::_imlib_find_lines(std::vector<int> &roi, unsigned int x_stride, unsigned int y_stride, uint32_t threshold, unsigned int theta_margin, unsigned int rho_margin)
{
  py::list return_val;
  if (NULL == this->_img)
  {
    py::print("no img");
    return return_val;
  }

  image_t img = {};
  img.w = this->_img->width;
  img.h = this->_img->height;
  img.pixels = (uint8_t *)this->_img->data;
  img.pixfmt = PIXFORMAT_RGB888;

  rectangle_t _roi;

  _roi.x = roi[0];
  _roi.y = roi[1];
  _roi.w = roi[2];
  _roi.h = roi[3];
  //默认整个图像
  if (_roi.w == 0)
    _roi.w = img.w;
  if (_roi.h == 0)
    _roi.h = img.h;

  list_t out;

  fb_alloc_mark();
  imlib_find_lines(&out, &img, &_roi, x_stride, y_stride, threshold, theta_margin, rho_margin);
  fb_alloc_free_till_mark();

  for (size_t i = 0; list_size(&out); i++)
  {
    py::list tmps;
    find_lines_list_lnk_data_t lnk_data;
    list_pop_front(&out, &lnk_data);

    tmps.append(lnk_data.line.x1);
    tmps.append(lnk_data.line.y1);
    tmps.append(lnk_data.line.x2);
    tmps.append(lnk_data.line.y2);

    return_val.append(tmps);
  }
  return return_val;
}

/*
函数原型：
_imlib_find_circles(std::vector<int> &roi,unsigned int x_stride, unsigned int y_stride,uint32_t threshold,
unsigned int x_margin, unsigned int y_margin, unsigned int r_margin, unsigned int r_min, unsigned int r_max, unsigned int r_step)
roi：图像ROI区域，默认为整个图像
x_stride:霍夫变换时要跳过的 x 像素数
y_stride:霍夫变换时要跳过的 y 像素数
threshold阈值
x_margin:控制检测到的圆圈的合并.x_margin相隔、y_margin和r_margin像素的圆被合并
y_margin:控制检测到的圆圈的合并.x_margin相隔、y_margin和r_margin像素的圆被合并
r_margin:控制检测到的圆圈的合并.x_margin相隔、y_margin和r_margin像素的圆被合并
r_min:控制检测到的最小圆半径.增加它以加速算​​法.默认为 2
r_max:控制检测到的最大圆半径.减少它以加快算法速度.默认为 min(roi.w/2, roi.h/2)
r_step:控制如何步进半径检测.默认为 2
返回值：
x:圆心x
y:圆心y
r:圆半径
magnitude:检测圆的强度
*/
py::list maix_vision::_imlib_find_circles(std::vector<int> &roi, unsigned int x_stride, unsigned int y_stride, uint32_t threshold,
                                          unsigned int x_margin, unsigned int y_margin, unsigned int r_margin, unsigned int r_min, unsigned int r_max, unsigned int r_step)
{
  py::list return_val;
  if (NULL == this->_img)
  {
    py::print("no img");
    return return_val;
  }

  image_t img = {};
  img.w = this->_img->width;
  img.h = this->_img->height;
  img.pixels = (uint8_t *)this->_img->data;
  img.pixfmt = PIXFORMAT_RGB888;

  rectangle_t _roi;

  _roi.x = roi[0];
  _roi.y = roi[1];
  _roi.w = roi[2];
  _roi.h = roi[3];
  //默认整个图像
  if (_roi.w == 0)
    _roi.w = img.w;
  if (_roi.h == 0)
    _roi.h = img.h;
  //默认为min(roi.w/2, roi.h/2)
  if (r_max == 0)
    r_max = MIN(_roi.w / 2, _roi.h / 2);

  list_t out;

  fb_alloc_mark();
  imlib_find_circles(&out, &img, &_roi, x_stride, y_stride, threshold, x_margin, y_margin, r_margin, r_min, r_max, r_step);
  fb_alloc_free_till_mark();

  for (size_t i = 0; list_size(&out); i++)
  {
    py::list tmps;
    find_circles_list_lnk_data lnk_data;
    list_pop_front(&out, &lnk_data);

    tmps.append(lnk_data.p.x);
    tmps.append(lnk_data.p.y);
    tmps.append(lnk_data.r);
    tmps.append(lnk_data.magnitude);

    return_val.append(tmps);
  }
  return return_val;
}

/*
函数原型：
_imlib_find_line_segments(std::vector<int> &roi, unsigned int merge_distance, unsigned int max_theta_diff)
roi：图像ROI区域，默认为整个图像
merge_distance: 要合并的两条线段（在一条线上的任意点）可以相互分隔的最大像素数
max_theta_diff: merge_distance分开的要合并的两条线段的最大 theta 差异度

返回值：
x1:线段坐标x1
y1:线段坐标y1
x2:线段坐标x2
y2:线段坐标y2
magnitude:从霍夫变换返回线的大小
theta：从霍夫变换返回直线的角度(0-179)度
rho：从霍夫变换返回直线的rho值
*/
py::list maix_vision::_imlib_find_line_segments(std::vector<int> &roi, unsigned int merge_distance, unsigned int max_theta_diff)
{
  py::list return_val;
  if (NULL == this->_img)
  {
    py::print("no img");
    return return_val;
  }

  image_t img = {};
  img.w = this->_img->width;
  img.h = this->_img->height;
  img.pixels = (uint8_t *)this->_img->data;
  img.pixfmt = PIXFORMAT_RGB888;

  rectangle_t _roi;

  _roi.x = roi[0];
  _roi.y = roi[1];
  _roi.w = roi[2];
  _roi.h = roi[3];
  //默认整个图像
  if (_roi.w == 0)
    _roi.w = img.w;
  if (_roi.h == 0)
    _roi.h = img.h;

  list_t out;

  fb_alloc_mark();
  imlib_lsd_find_line_segments(&out, &img, &_roi, merge_distance, max_theta_diff);
  fb_alloc_free_till_mark();

  for (size_t i = 0; list_size(&out); i++)
  {
    py::list tmps;
    find_lines_list_lnk_data_t lnk_data;
    list_pop_front(&out, &lnk_data);

    tmps.append(lnk_data.line.x1);
    tmps.append(lnk_data.line.y1);
    tmps.append(lnk_data.line.x2);
    tmps.append(lnk_data.line.y2);
    tmps.append(lnk_data.magnitude);
    tmps.append(lnk_data.theta);
    tmps.append(lnk_data.rho);

    return_val.append(tmps);
  }
  return return_val;
}

/*
函数原型：
_imlib_find_apriltags(std::vector<int> &roi, unsigned int merge_distance, unsigned int max_theta_diff)
roi：图像ROI区域，默认为整个图像
families: 解码的标签系列的位掩码，
  image.TAG16H5   1，
  image.TAG25H7   2，
  image.TAG25H9   4，
  image.TAG36H10  8，
  image.TAG36H11  16，
  image.ARTOOLKIT 32
fx: 以像素为单位的相机 X 焦距
fy: 以像素为单位的相机 Y 焦距
cx: 图像中心image.width()/2
cy: 图像中心image.height()/2

返回值：
x:边界框 坐标x
y:边界框 坐标y
w:边界框 w
h:边界框 h
id:apriltag ID
family: apriltag family
cx: 质心 x 位置
cy: 质心 y 位置
//rotation: 旋转弧度（未实现）
decision_margin：匹配质量 (0.0 - 1.0)
hamming： 接受错误数
goodness： priltag 图像的质量 (0.0 - 1.0)
x_translation： X 方向返回未知单位的平移
y_translation： Y 方向返回未知单位的平移
z_translation： Z 方向返回未知单位的平移
x_rotation： X 平面中 apriltag 的弧度旋转
y_rotation： Y 平面中 apriltag 的弧度旋转
z_rotation： Z 平面中 apriltag 的弧度旋转
*/
py::list maix_vision::_imlib_find_apriltags(std::vector<int> &roi, int families,
                                            float fx, float fy, float cx, float cy)
{
  py::list return_val;
  if (NULL == this->_img)
  {
    py::print("no img");
    return return_val;
  }

  image_t img = {};
  img.w = this->_img->width;
  img.h = this->_img->height;
  img.pixels = (uint8_t *)this->_img->data;
  img.pixfmt = PIXFORMAT_RGB888;

  rectangle_t _roi;

  _roi.x = roi[0];
  _roi.y = roi[1];
  _roi.w = roi[2];
  _roi.h = roi[3];
  //默认整个图像
  if (_roi.w == 0)
    _roi.w = img.w;
  if (_roi.h == 0)
    _roi.h = img.h;

  list_t out;

  fb_alloc_mark();
  imlib_find_apriltags(&out, &img, &_roi, apriltag_families_t(families), fx, fy, cx, cy);
  fb_alloc_free_till_mark();

  for (size_t i = 0; list_size(&out); i++)
  {
    find_apriltags_list_lnk_data lnk_data;
    list_pop_front(&out, &lnk_data);

    py::dict val;
    val["x"] = lnk_data.rect.x;
    val["y"] = lnk_data.rect.y;
    val["w"] = lnk_data.rect.w;
    val["h"] = lnk_data.rect.h;
    val["id"] = lnk_data.id;
    val["family"] = lnk_data.family;

    {
      py::list tmp;
      tmp.append(lnk_data.centroid.x);
      tmp.append(lnk_data.centroid.y);
      val["centroid"] = tmp;
    }

    py::list corners;
    for (int i = 0; i < 4; i++)
    {
      py::list tmp;
      tmp.append(lnk_data.corners[i].x);
      tmp.append(lnk_data.corners[i].y);
      corners.append(tmp);
    }

    val["corners"] = corners;

    val["family"] = lnk_data.family;

    val["x_translation"] = lnk_data.x_translation;
    val["y_translation"] = lnk_data.y_translation;
    val["z_translation"] = lnk_data.z_translation;

    val["decision_margin"] = lnk_data.decision_margin;
    val["hamming"] = lnk_data.hamming;
    val["goodness"] = lnk_data.goodness;

    val["x_rotation"] = lnk_data.x_rotation;
    val["y_rotation"] = lnk_data.y_rotation;
    val["z_rotation"] = lnk_data.z_rotation;

    return_val.append(val);
  }
  return return_val;
}

py::list maix_vision::_imlib_find_qrcodes(std::vector<int> &roi)
{
  py::list return_val;
  if (NULL == this->_img)
  {
    py::print("no img");
    return return_val;
  }

  image_t img = {};
  img.w = this->_img->width;
  img.h = this->_img->height;
  img.pixels = (uint8_t *)this->_img->data;
  img.pixfmt = PIXFORMAT_RGB888;

  rectangle_t _roi;

  _roi.x = roi[0];
  _roi.y = roi[1];
  _roi.w = roi[2];
  _roi.h = roi[3];

  //默认整个图像
  if (_roi.w == 0)
    _roi.w = img.w;
  if (_roi.h == 0)
    _roi.h = img.h;

  list_t out;
  fb_alloc_mark();
  imlib_find_qrcodes(&out, &img, &_roi);
  fb_alloc_free_till_mark();

  for (size_t i = 0; list_size(&out); i++)
  {
    find_qrcodes_list_lnk_data_t lnk_data;
    list_pop_front(&out, &lnk_data);

    py::dict val;
    val["x"] = lnk_data.rect.x;
    val["y"] = lnk_data.rect.y;
    val["w"] = lnk_data.rect.w;
    val["h"] = lnk_data.rect.h;
    val["payload"] = std::string(lnk_data.payload, lnk_data.payload_len);
    val["version"] = lnk_data.version;
    val["ecc_level"] = lnk_data.ecc_level;
    val["mask"] = lnk_data.mask;
    val["data_type"] = lnk_data.data_type;
    val["eci"] = lnk_data.eci;

    py::list corners;
    for (int i = 0; i < 4; i++)
    {
      py::list tmp;
      tmp.append(lnk_data.corners[i].x);
      tmp.append(lnk_data.corners[i].y);
      corners.append(tmp);
    }
    val["corners"] = corners;

    return_val.append(val);
  }

  return return_val;
}

py::list maix_vision::_imlib_find_barcodes(std::vector<int> &roi)
{
  py::list return_val;
  if (NULL == this->_img)
  {
    py::print("no img");
    return return_val;
  }

  image_t img = {};
  img.w = this->_img->width;
  img.h = this->_img->height;
  img.pixels = (uint8_t *)this->_img->data;
  img.pixfmt = PIXFORMAT_RGB888;

  rectangle_t _roi;

  _roi.x = roi[0];
  _roi.y = roi[1];
  _roi.w = roi[2];
  _roi.h = roi[3];

  //默认整个图像
  if (_roi.w == 0)
    _roi.w = img.w;
  if (_roi.h == 0)
    _roi.h = img.h;

  list_t out;
  fb_alloc_mark();
  imlib_find_barcodes(&out, &img, &_roi);
  fb_alloc_free_till_mark();

  for (size_t i = 0; list_size(&out); i++)
  {
    find_barcodes_list_lnk_data_t lnk_data;
    list_pop_front(&out, &lnk_data);

    py::dict val;
    val["x"] = lnk_data.rect.x;
    val["y"] = lnk_data.rect.y;
    val["w"] = lnk_data.rect.w;
    val["h"] = lnk_data.rect.h;
    val["payload"] = std::string(lnk_data.payload, lnk_data.payload_len);
    val["rotation"] = lnk_data.rotation;
    val["type"] = lnk_data.type;
    val["quality"] = lnk_data.quality;

    py::list corners;
    for (int i = 0; i < 4; i++)
    {
      py::list tmp;
      tmp.append(lnk_data.corners[i].x);
      tmp.append(lnk_data.corners[i].y);
      corners.append(tmp);
    }
    val["corners"] = corners;

    return_val.append(val);
  }

  return return_val;
}

// void imlib_get_histogram(histogram_t *out, image_t *ptr, rectangle_t *roi, list_t *thresholds, bool invert, image_t *other);
maix_image::maix_histogram maix_image::_imlib_get_histogram(std::vector<int> roi_src, std::vector<std::vector<int>> &thresholds_src, bool invert, maix_image &other_src, int bins, int l_bins, int a_bins, int b_bins)
{
  if (NULL == this->_img)
  {
    py::print("no img");
    return maix_histogram();
  }
  image_t img_tmp = {}, *arg_img = &img_tmp;
  arg_img->w = this->_img->width;
  arg_img->h = this->_img->height;
  arg_img->pixels = (uint8_t *)this->_img->data;
  arg_img->pixfmt = PIXFORMAT_RGB888;

  image_t other_img = {}, *other = NULL;
  if (NULL != other_src._img)
  {
    other->w = other_src._img->width;
    other->h = other_src._img->height;
    other->pixels = (uint8_t *)other_src._img->data;
    other->pixfmt = PIXFORMAT_RGB888;
    other = &other_img;
  }

  fb_alloc_mark();

  list_t thresholds;
  list_init(&thresholds, sizeof(color_thresholds_list_lnk_data_t));
  for (auto src : thresholds_src)
  {
    color_thresholds_list_lnk_data_t tmp_ct;
    tmp_ct.LMin = src[0];
    tmp_ct.LMax = src[1];
    tmp_ct.AMin = src[2];
    tmp_ct.AMax = src[3];
    tmp_ct.BMin = src[4];
    tmp_ct.BMax = src[5];
    list_push_back(&thresholds, &tmp_ct);
  }

  if (roi_src[2] == 0)
    roi_src[2] = arg_img->w;
  if (roi_src[3] == 0)
    roi_src[3] = arg_img->h;

  rectangle_t roi = {roi_src[0], roi_src[1], roi_src[2], roi_src[3]};
  histogram_t hist;
  switch (arg_img->pixfmt)
  {
  case PIXFORMAT_GRAYSCALE:
  {
    if (bins >= 2 && bins <= 255)
    {
      hist.LBinCount = bins;
    }
    else
    {
      hist.LBinCount = bins = 255;
    }
    if (hist.LBinCount >= 2)
    {
      hist.ABinCount = 0;
      hist.BBinCount = 0;
      hist.LBins = (float *)fb_alloc(hist.LBinCount * sizeof(float), FB_ALLOC_NO_HINT);
      hist.ABins = NULL;
      hist.BBins = NULL;
      imlib_get_histogram(&hist, arg_img, &roi, &thresholds, invert, other);
      list_free(&thresholds);
    }
    break;
  }
  case PIXFORMAT_RGB565:
  case PIXFORMAT_RGB888:
  {
    if (bins >= 2 && bins <= 255)
    {
      hist.LBinCount = bins;
    }
    else
    {
      hist.LBinCount = bins = 255;
    }
    if (l_bins < 2)
      l_bins = bins;
    hist.LBinCount = l_bins;
    if (a_bins < 2)
      a_bins = bins;
    hist.ABinCount = a_bins;
    if (b_bins < 2)
      b_bins = bins;
    hist.BBinCount = b_bins;

    if (hist.LBinCount >= 2 && hist.ABinCount >= 2 && hist.BBinCount >= 2)
    {
      hist.LBins = (float *)fb_alloc(hist.LBinCount * sizeof(float), FB_ALLOC_NO_HINT);
      hist.ABins = (float *)fb_alloc(hist.ABinCount * sizeof(float), FB_ALLOC_NO_HINT);
      hist.BBins = (float *)fb_alloc(hist.BBinCount * sizeof(float), FB_ALLOC_NO_HINT);
      imlib_get_histogram(&hist, arg_img, &roi, &thresholds, invert, other);
      list_free(&thresholds);
    }
    break;
  }
  }

  auto result = maix_histogram(hist);
  result.pixfmt = arg_img->pixfmt;
  fb_alloc_free_till_mark();
  return result;
}

py::list maix_vision::_imlib_find_blobs(std::vector<std::vector<int>> &thresholds_src, std::vector<int> roi_src, int x_stride, int y_stride, bool invert, int area_threshold, int pixels_threshold, bool merge, int margin, int x_hist_bins_max, int y_hist_bins_max)
{
  py::list return_val;
  if (NULL == this->_img)
  {
    py::print("no img");
    return return_val;
  }

  image_t img_tmp = {}, *arg_img = &img_tmp;
  arg_img->w = this->_img->width;
  arg_img->h = this->_img->height;
  arg_img->pixels = (uint8_t *)this->_img->data;
  arg_img->pixfmt = PIXFORMAT_RGB888;

  if (roi_src[2] == 0)
    roi_src[2] = arg_img->w;
  if (roi_src[3] == 0)
    roi_src[3] = arg_img->h;

  rectangle_t roi = {roi_src[0], roi_src[1], roi_src[2], roi_src[3]};
  if (roi_src[2] == 0)
    roi_src[2] = arg_img->w;
  if (roi_src[3] == 0)
    roi_src[3] = arg_img->h;

  list_t out;

  fb_alloc_mark();

  list_t thresholds;
  list_init(&thresholds, sizeof(color_thresholds_list_lnk_data_t));
  for (auto src : thresholds_src)
  {
    color_thresholds_list_lnk_data_t tmp_ct;
    tmp_ct.LMin = src[0];
    tmp_ct.LMax = src[1];
    tmp_ct.AMin = src[2];
    tmp_ct.AMax = src[3];
    tmp_ct.BMin = src[4];
    tmp_ct.BMax = src[5];
    list_push_back(&thresholds, &tmp_ct);
  }

  imlib_find_blobs(&out, arg_img, &roi, x_stride, y_stride, &thresholds, invert,
                   area_threshold, pixels_threshold, merge, margin,
                   NULL, NULL, NULL, NULL, x_hist_bins_max, y_hist_bins_max);
  fb_alloc_free_till_mark();

  for (size_t i = 0; list_size(&out); i++)
  {
    find_blobs_list_lnk_data_t lnk_data;
    list_pop_front(&out, &lnk_data);

    py::dict val;
    val["x"] = lnk_data.rect.x;
    val["y"] = lnk_data.rect.y;
    val["w"] = lnk_data.rect.w;
    val["h"] = lnk_data.rect.h;
    val["pixels"] = lnk_data.pixels;
    val["centroid_x"] = lnk_data.centroid_x;
    val["centroid_y"] = lnk_data.centroid_y;
    val["rotation"] = lnk_data.rotation;
    val["code"] = lnk_data.code;
    val["count"] = lnk_data.count;

    return_val.append(val);
  }

  return return_val;
}
