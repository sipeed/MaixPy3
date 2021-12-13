#include "maix_image.h"
// Mat img
// img.cols 宽
// img.rows 高
// Mat::Mat(int rows, int cols, int type, void* data, size_t step=AUTO_STEP)
maix_version::maix_version()
{
}
int Distance(int x1, int y1, int x2, int y2)
{
  int x = abs(x1 - x2);
  int y = abs(y1 - y2);
  return int(round(sqrt(x * x + y * y)));
}
py::list maix_version::get_blob_color_max(std::vector<int> &roi, int critical, int co)
{
  py::list return_val;
  cv::Mat src(this->_img->height, this->_img->width, any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][2]), this->_img->data);
  cv::Mat in_img;
  src.copyTo(in_img);
  critical = critical > 100 ? 100 : critical;
  critical = critical < 0 ? 0 : critical;
  cv::Rect rect;
  rect.x = roi[0];
  rect.y = roi[1];
  rect.width = roi[2];
  rect.height = roi[3];
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
  case 0: //rgb
  {
    return_val.append(lnum);
    return_val.append(anum);
    return_val.append(bnum);
    return return_val;
  }
  break;
  case 1: //lab
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
  case 2: //hsv
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
py::list maix_version::_maix_vision_find_blob(std::vector<std::vector<int>> &thresholds, std::vector<int> roi, int x_stride, int y_stride, bool invert, int area_threshold, int pixels_threshold, bool merge, int margin, int tilt, int co)
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
py::list maix_version::_maix_vision_find_ball_blob(std::vector<int> &thresholds, int co)
{
  py::list out;
  cv::Mat src(this->_img->height, this->_img->width, any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][2]), this->_img->data);
  cv::Mat in_img;
  src.copyTo(in_img);
  cv::Mat hsv, mask;
  switch (co)
  {
  case 0: //rgb
    if (in_img.channels() != 3)
      return out;
    hsv = in_img;
    break;
  case 1: //lab
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
  case 2: //hsv
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

py::dict maix_version::find_line(int func)
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
