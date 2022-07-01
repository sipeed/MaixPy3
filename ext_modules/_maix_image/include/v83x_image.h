

#ifdef CONFIG_ARCH_V83X

#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

struct cv_surf
{
  cv::Ptr<cv::xfeatures2d::SURF> surf;
  cv::Ptr<cv::DescriptorMatcher> matcher;;
  std::vector<cv::KeyPoint> queryPoints, trainPoints;
  cv::Mat queryDescriptors, trainDescriptors;
  void *cache_src;

  cv_surf(double hessianThreshold = 100, int nOctaves = 4, int nOctaveLayers = 3, bool extended = false, bool upright = false)
  {
    this->surf = cv::xfeatures2d::SURF::create(hessianThreshold, nOctaves, nOctaveLayers, extended, upright);
    this->matcher = cv::DescriptorMatcher::create("FlannBased");
    this->cache_src = NULL;
    // // ORB
    // cv::Ptr<cv::ORB> orb = cv::ORB::create();
    // {
    //   //提取特征点
    //   std::vector<cv::KeyPoint> keypointsL, keypointsR;
    //   cv::Mat descriptorsL, descriptorsR;
    //   orb->detectAndCompute(imageL, cv::noArray(), keypointsL, descriptorsL);
    //   orb->detectAndCompute(imageR, cv::noArray(), keypointsR, descriptorsR);
    //   //匹配特征点
    //   cv::BFMatcher matcher(cv::NORM_L2);
    //   std::vector<cv::DMatch> matches;
    //   matcher.match(descriptorsL, descriptorsR, matches);
    //   //绘制匹配结果
    //   cv::Mat imgMatches;
    //   cv::drawMatches(imageL, keypointsL, imageR, keypointsR, matches, imgMatches);
    // }
  }

  py::list match(maix_image &src, maix_image &dst, int limit = 20, float filter = 0.5, bool dump = 0)
  {
    if (NULL == src._img || NULL == dst._img) return py::list();

    cv::Mat img_src(src._img->height, src._img->width, CV_8UC3, src._img->data);
    if (this->cache_src != src._img)
    {
      this->surf->detectAndCompute(img_src, cv::Mat(), queryPoints, queryDescriptors);
      this->cache_src = src._img;
    }

    cv::Mat img_dst(dst._img->height, dst._img->width, CV_8UC3, dst._img->data);
    this->surf->detectAndCompute(img_dst, cv::Mat(), trainPoints, trainDescriptors);

    // Struct for DMatch: query descriptor index, train descriptor index, train image index and distance between descriptors.
    // int queryIdx –>是测试图像的特征点描述符（descriptor）的下标，同时也是描述符对应特征点（keypoint)的下标。
    // int trainIdx –> 是样本图像的特征点描述符的下标，同样也是相应的特征点的下标。
    // int imgIdx –>当样本是多张图像的话有用。
    // float distance –>代表这一对匹配的特征点描述符（本质是向量）的欧氏距离，数值越小也就说明两个特征点越相像。
    std::vector<cv::DMatch> matches;

    //如果采用flannBased方法 那么 desp通过orb的到的类型不同需要先转换类型
    if (queryDescriptors.type() != CV_32F || trainDescriptors.type() != CV_32F)
    {
      queryDescriptors.convertTo(queryDescriptors, CV_32F);
      trainDescriptors.convertTo(trainDescriptors, CV_32F);
    }

    this->matcher->match(queryDescriptors, trainDescriptors, matches);

    //特征点按 distance 升序
    std::sort(matches.begin(), matches.end(), [](const cv::DMatch &m1, const cv::DMatch &m2)
              { return m1.distance < m2.distance; });

    //获取最大距离的特征点
    float max_dist = matches[matches.size() - 1].distance;

    //过滤出特征点距离小于 max_dist * filter 以内的特征点
    limit = std::min(limit, (int)matches.size());
    std::vector<cv::DMatch> good_matches(limit);
    if (filter < 1.0f)
    {
      for (int i = 0, stop = (int)(max_dist * filter); i < limit; i++)
      {
        if (matches[i].distance < stop)
        {
          good_matches.push_back(matches[i]);
          break;
        }
      }
    }
    else
    {
      good_matches.assign(matches.begin(), matches.end());
    }

    if (dump)
    {
      {
        cv::Mat imageOutput;
        cv::drawKeypoints(img_src, queryPoints, imageOutput, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        cv::imwrite("cv_surf_img_src.jpg", imageOutput);
      }
      {
        cv::Mat imageOutput;
        cv::drawKeypoints(img_dst, queryPoints, imageOutput, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        cv::imwrite("cv_surf_img_dst.jpg", imageOutput);
      }
      {
        cv::Mat imageOutput;
        drawMatches(img_src, queryPoints, img_dst, trainPoints, good_matches, imageOutput);
        cv::imwrite("cv_surf_img_matches.jpg", imageOutput);
      }
    }

    py::list ret;
    for (auto &m : good_matches)
    {
      auto src_points = queryPoints[m.queryIdx], dst_points = trainPoints[m.trainIdx];
      auto tmp = py::make_tuple(src_points.pt.x, src_points.pt.y, src_points.response, m.distance, dst_points.pt.x, dst_points.pt.y, dst_points.response);
      ret.append(tmp);
    }

    return ret;
  }
};

#endif
