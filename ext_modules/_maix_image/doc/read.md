Image类 – 图像对象
----------------

  图像对象是机器视觉操作的基本对象。

`class image.Image()`
  实例化一个新的图像对象,该对象内目前没有任何内容,仅作为创建一个方法类集合

- Image.width
  返回以像素计的图像的宽度。

- Image.height
  返回以像素计的图像的高度。

- Image.mode
  返回图像的格式,目前支持"RGB", "RGBA", "L", "RGB16"(RGB565)

- Image.size
  返回以像素计的图像的大小。

- Image.new([size = (240, 240), [color = (0, 0, 0) , [mode = "RGB"]]])
  创建一张新的图片,size为图像尺寸,color为图像填充颜色,mode为图像格式.目前支持"RGB", "RGBA", "L", "RGB16"(RGB565)
  返回图像对象，以便您可以使用 . 表示法调用另一个方法。
  
- Image.open(path)
  从二进制文件中打开一张图片,path为路径名,会统一转换成RGB模式.Image.open("./tmp.jpg")

- Image.save(path)
  将图像对象保存成二进制文件,Image.save("./tmp.jpg")

- Image.load(data, [size = (240, 240) , [mode = "RGB"]])
  在python对象中加载出一张图像,会将python对象的数据copy到Image对象内部.
  date可以是PIL对象,image.Image()对象,bytes对象,numpy对象.
  当data为bytes,numpy对象时,需要提供size和mode参数.
  返回图像对象，以便您可以使用 . 表示法调用另一个方法。

- Image.copy([img = "maix_image"])
  返回一张img类的图像对象,img可以是"maix_image", "PIL", "numpy"
  返回图像对象，以便您可以使用 . 表示法调用另一个方法。

- Image.clear()
  清除image.Image对象内部的图像数据,执行后该对象和执行image.Image()实例化操作后产生的对象一样.

- Image.tobytes()
  返回图像的bytes数据

- Image.resize(w, h, func = 1)
  将图像调整至(w, h)大小,func可选
    0     (INTER_NEAREST 最近邻插值)
    1     (INTER_LINEAR 双线性插值)（默认设置）
    2     (INTER_CUBIC 4x4像素邻域的双三次插值)
    3     (INTER_AREA 使用像素区域关系进行重采样)
    4     (INTER_LANCZOS4 8x8像素邻域的Lanczos插值)

  返回图像对象，以便您可以使用 . 表示法调用另一个方法。

- Image.draw_line(x1, y1, x2, y2, [color = (127, 127, 127), [thickness = 1]])
  在图像上绘制一条从(x0，y0)到(x1，y1)的线。
  color 是RGB888元组。默认为灰色。
  thickness 控制线的粗细像素。
  返回图像对象，以便您可以使用 . 表示法调用另一个方法。

- Image.draw_rectangle(x1, y1, x2, y2, [color = (127, 127, 127), [thickness = 1]])
  在图像上绘制一个矩形。
  color 是RGB888元组。默认为灰色。
  thickness 控制线的粗细像素。当thickness=-1时,将会用color颜色填充整个区域.
  返回图像对象，以便您可以使用 . 表示法调用另一个方法。

- Image.draw_circle(x, y, radius, [color = (127, 127, 127), [thickness = 1]])
  在图像上绘制一个圆形。x, y为圆的中心点坐标, radius为圆的半径
  color 是RGB888元组。默认为灰色。
  thickness 控制线的粗细像素。当thickness=-1时,将会用color颜色填充整个区域.
  返回图像对象，以便您可以使用 . 表示法调用另一个方法。

- Image.draw_ellipse(cx, xy, rx, ry, angle, startAngle, endAngle, [color = (127, 127, 127), [thickness = 1]])
  在图像上绘制椭圆。
  cx, xy, rx, ry ,椭圆的中心坐标和长轴短轴.
  angle 椭圆旋转角度(0~180)
  startAngle椭圆的开始绘图角度(0~360),
  endAngle椭圆的结束绘图角度(0~360),
  color 是RGB888元组。默认为灰色。
  thickness 控制线的粗细像素。当thickness=-1时,将会用color颜色填充整个区域.
  返回图像对象，以便您可以使用 . 表示法调用另一个方法。

- Image.draw_string(x, y, str, [scale = 1.0, [color = (127, 127, 127), [thickness = 1]]])
  从图像中的(x, y)位置开始绘制文本
  scale 可以放大/缩小图像上文本的大小。您可以传递大于0的整数或浮点值。
  color 是RGB888元组。默认为灰色。
  thickness 控制线的粗细像素。您可以传递大于0的整数.
  返回图像对象，以便您可以使用 . 表示法调用另一个方法。

- Image.load_freetype(path)
  从二进制文件中加载字体,加载后执行Image.draw_string()函数将会用该函数加载的字体.

- Image.rotate(rotate = 1.0)
  旋转图像到固定的角度,保持图像的尺寸不变
  rotate旋转角度.您可以传递大于0的浮点值.
  返回图像对象，以便您可以使用 . 表示法调用另一个方法。

- Image.convert(mode = "RGB")
  转换图像的格式.
  mode为图像格式.目前支持"RGB", "RGBA", "L", "RGB16"(RGB565)
  返回图像对象，以便您可以使用 . 表示法调用另一个方法。

- Image.crop(x, y, w, h)
  裁剪图片返回一张全新的图片
  x, y, w, h裁剪图像的位置和大小
  返回图像对象，以便您可以使用 . 表示法调用另一个方法。

- Image.draw_image(img, x, y)
  将传递的img图像画在image.Image()对象内部的图像上
  img只能传递image.Image()对象的图像.
  返回图像对象，以便您可以使用 . 表示法调用另一个方法。

- Image.get_pixel(x, y)
  得到图像x,y位置的像素值,返回是一个四元组对象,(r, g, b, a),如果为灰度,那只有r有效


maix_version类 – 图像对象视觉操作
-----------------------------

图像对象视觉操作类是对于图像的一系列操作方法的集合

`class image.Image().maix_version()`
实例化一个新的图像对象,该对象内目前没有任何内容,仅作为创建一个方法类集合.该对象被image.Image()所继承,可由image.Image()对象直接操作

- maix_version.get_blob_color(roi = (0, 0, 0, 0), critical = 0, co = 0)
  统计函数,得到图像感兴趣区域的最大颜色值.
  roi 是感兴趣区域的矩形元组(x，y，w，h)。如果未指定，ROI即整个图像的图像矩形。 操作范围仅限于 roi 区域内的像素。尽量选取较小的区域,区域较大时统计效果比较差.
  critical为返回值区域范围,简单将统计到的最大颜色值和critical相加或相减,当为0时,返回原来的色值.
  co为返回的颜色空间模型.可选为
  0       rgb
  1       lab
  2       hsv
  返回值:
  当co为0时返回[r, g, b]
  当co为1时返回[L - critical, A - critical, B - critical, L + critical, A + critical, B + critical]
  当co为2时同1

- maix_version.find_blobs(thresholds, roi = (0,0,0,0), x_stride = 2, y_stride = 2, invert = 0, area_threshold = 10, pixels_threshold = 10, merge = 0, margin = 0, tilt = 0, co = 1)
  查找图像中所有色块，并返回一个包括每个色块的色块对象的列表。
  thresholds 必须是元组列表. [(minL, minA, minB, maxL, maxA, maxB)]
  roi 是感兴趣区域的矩形元组(x，y，w，h)。如果未指定，ROI即整个图像的图像矩形。 操作范围仅限于 roi 区域内的像素。
  x_stride 是查找某色块时需要跳过的x像素的数量。找到色块后，直线填充算法将精确像素。 若已知色块较大，可增加 x_stride 来提高查找色块的速度。
  y_stride 是查找某色块时需要跳过的y像素的数量。找到色块后，直线填充算法将精确像素。 若已知色块较大，可增加 y_stride 来提高查找色块的速度。
  invert 反转阈值操作，像素在已知颜色范围之外进行匹配，而非在已知颜色范围内。
  若一个色块的边界框区域小于 area_threshold ，则会被过滤掉。
  若一个色块的像素数小于 pixels_threshold ，则会被过滤掉。
  merge 若为True，则合并所有没有被过滤掉的色块，这些色块的边界矩形互相交错重叠。
  margin 可在相交测试中用来增大或减小色块边界矩形的大小。例如：边缘为1、相互间边界矩形为1的色块将被合并。
  tilt设置是否查找最小斜矩形框,为0则不查找.
  co为返回的颜色空间模型.可选为,如果不是特殊需要,请保持默认
  0       rgb
  1       lab
  2       hsv
  3       灰度
  返回值:[{'x': 140, 'y': 88, 'w': 15, 'h': 7, 'pixels': 43, 'cx': 147, 'cy': 91}]
  (x, y, w, h)色块的外框,pixels,色块的像素大小,(cx, cy)色块的中心点.

- maix_version.find_ball_color(thresholds, co = 1)
  该函数是在maix_version.find_blobs的基础上通过基尔霍夫圆拟合,并返回拟合的圆.
  thresholds 必须是元组列表. [(minL, minA, minB, maxL, maxA, maxB)]
  co为返回的颜色空间模型.可选为,如果不是特殊需要,请保持默认
  0       rgb
  1       lab
  2       hsv
  返回值:

- maix_version.find_line()
  该函数是内置的寻线函数.通过自适应的图像操作,将图像中的黑线选出来,然后返回黑线的矩形区域,可以作为小车的寻线函数.
  返回值:
  {'rect': [9, 229, 9, 9, 145, 9, 145, 229], 'pixels': 12959, 'cx': 77, 'cy': 119, 'rotation': -1.570796251296997}
  rect为线的框,
  pixels为线的像素大小
  (cx, cy)框的中心点
  rotation为框的偏转角度(弧度制)






















### 期望实现的相关函数
/*
  maix_image &open(std::string filename)           (ok)

  maix_image &save(std::string filename)           (ok)

  maix_image &convert(std::string format)          (ok)

  maix_image &img_new(std::string format)          (ok)

  maix_image &copy(std::string format)             (ok)

  maix_image &paste(std::string format)

  maix_image &filter(std::string format)

  maix_image &blend(std::string format)
  {

  }

  maix_image &split(std::string format)
  {

  }
  maix_image &composite(std::string format)
  {

  }
  maix_image &eval(std::string format)
  {

  }
  maix_image &merge(std::string format)
  {

  }
  maix_image &draft(std::string format)
  {

  }
  maix_image &getbands(std::string format)
  {

  }
  maix_image &getbbox(std::string format)
  {

  }
  maix_image &getdata(std::string format)
  {

  }
  maix_image &getextrema(std::string format)
  {

  }
  maix_image &getpixel(std::string format)
  {

  }

  maix_image &histogram(std::string format)
  {

  }
  maix_image &load(std::string format)
  {

  }

  maix_image &putdata(std::string format)
  {

  }
  maix_image &resize(std::string format)
  {

  }
  maix_image &rotate(std::string format)
  {

  }
  maix_image &seek(std::string format)
  {

  }
  maix_image &tell(std::string format)
  {

  }
  maix_image &thumbnail(std::string format)
  {

  }
  maix_image &transform(std::string format)
  {

  }
  maix_image &transform(std::string format)
  {

  }
*/


  //==================================================================
  //函数名：  _maix_vision_medianBlur
  //作者：    dianjixz
  //日期：    2021-10-27
  //功能：    图像中值滤波函数
  //输入参数：
  //          py::object py_img     python输入图像对象
  //          int m_size            中值滤波的核大小
  //          std::vector<int> size      图像的尺寸(非必须)
  //          int mode              图像的格式(非必须)
  //返回值：
  //          返回中值滤波后的图像,图像格式和输入保持一致；
  //修改记录：
  //==================================================================
  void _maix_vision_medianBlur(int m_size);

  //==================================================================
  //函数名：  _maix_vision_GaussianBlur
  //作者：    dianjixz
  //日期：    2021-10-27
  //功能：    图像高斯滤波函数
  //输入参数：
  //          py::object py_img               python输入图像对象
  //          Size ksize:                     高斯内核大小，这个尺寸与前面两个滤波kernel尺寸不同，ksize.width和ksize.height可以不相同但是这两个值必须为正奇数，如果这两个值为0，他们的值将由sigma计算。
  //          double sigmaX:                  高斯核函数在X方向上的标准偏差
  //          double sigmaY:                  高斯核函数在Y方向上的标准偏差，如果sigmaY是0，则函数会自动将sigmaY的值设置为与sigmaX相同的值，如果sigmaX和sigmaY都是0，这两个值将由ksize.width和ksize.height计算而来。具体可以参考getGaussianKernel()函数查看具体细节。建议将size、sigmaX和sigmaY都指定出来。
  //          int borderType=BORDER_DEFAULT:  推断图像外部像素的某种便捷模式，有默认值BORDER_DEFAULT，如果没有特殊需要不用更
  //          std::vector<int> size                图像的尺寸(非必须)
  //          int mode                        图像的格式(非必须)
  //返回值：
  //          返回高斯滤波后的图像,图像格式和输入保持一致；
  //修改记录：
  //==================================================================
  void _maix_vision_GaussianBlur(std::vector<int> ksize, double sigmaX, double sigmaY, int borderType, std::vector<int> size, int mode);

  //==================================================================
  //函数名：  _maix_vision_Canny
  //作者：    dianjixz
  //日期：    2021-10-27
  //功能：    边缘检测函数
  //输入参数：
  //          py::object py_img     python输入图像对象
  //          int thr_h             最大阈值
  //          int thr_l             最小阈值
  //          std::vector<int> size      图像的尺寸(非必须)
  //          int mode              图像的格式(非必须)
  //返回值：
  //          返回边缘检测函数后的图像,图像格式和输入保持一致；
  //修改记录：
  //==================================================================
  void _maix_vision_Canny(int thr_h, int thr_l);

  //==================================================================
  //函数名：  _maix_vision_HoughCircles
  //作者：    dianjixz
  //日期：    2021-10-27
  //功能：    霍夫圆变换原理及圆检测
  //输入参数：
  //          py::object py_img     python输入图像对象
  //          int method            使用的检测方法，目前opencv只有霍夫梯度法一种方法可用，该参数填HOUGH_GRADIENT即可（opencv 4.1.0下）
  //          double dp             double类型的dp，用来检测圆心的累加器图像的分辨率于输入图像之比的倒数，且此参数允许创建一个比输入图像分辨率低的累加器。上述文字不好理解的话，来看例子吧。例如，如果dp= 1时，累加器和输入图像具有相同的分辨率。如果dp=2，累加器便有输入图像一半那么大的宽度和高度。
  //          double minDist        为霍夫变换检测到的圆的圆心之间的最小距离
  //          double param1         它是第三个参数method设置的检测方法的对应的参数。对当前唯一的方法霍夫梯度法CV_HOUGH_GRADIENT，它表示传递给canny边缘检测算子的高阈值，而低阈值为高阈值的一半。
  //          double param2         也是第三个参数method设置的检测方法的对应的参数，对当前唯一的方法霍夫梯度法HOUGH_GRADIENT，它表示在检测阶段圆心的累加器阈值。它越小的话，就可以检测到更多根本不存在的圆，而它越大的话，能通过检测的圆就更加接近完美的圆形了。
  //          int minRadius         表示圆半径的最小值
  //          int maxRadius         表示圆半径的最大值
  //          std::vector<int> size      图像的尺寸(非必须)
  //          int mode              图像的格式(非必须)
  //返回值：
  //          返回中值滤波后的图像,图像格式和输入保持一致；
  //修改记录：
  //==================================================================
  void _maix_vision_HoughCircles(int method, double dp, double minDist, double param1, double param2, int minRadius, int maxRadius, std::vector<int> size, int mode);

  //==================================================================
  //函数名：  _maix_vision_opencv_calcHist
  //作者：    dianjixz
  //日期：    2021-10-29
  //功能：    opencv计算图像直方图
  //输入参数：
  //          py::object py_img     python输入图像对象
  //          int channels          需要统计直方图的第几通道(默认为0)
  //          std::vector<int> &roi      掩膜，，计算掩膜内的直方图  感兴趣区域(默认全部区域)
  //          int histSize          直方图分成多少个区间，就是 bin的个数(默认256)
  //          std::vector<int> ranges     统计像素值得区间(默认(0,256))
  //          bool uniform          是否对得到的直方图数组进行归一化处理(默认true)
  //          bool accumulate       在多个图像时，是否累计计算像素值得个数(默认false)
  //          std::vector<int> size      图像的尺寸(非必须)(默认(0,0))
  //          int mode              图像的格式(非必须)(默认16)
  //返回值：
  //          返回中值滤波后的图像,图像格式和输入保持一致；
  //修改记录：
  //==================================================================
  py::object _maix_vision_opencv_calcHist(int channels, std::vector<int> &roi, int histSize, std::vector<int> ranges, bool uniform, bool accumulate, std::vector<int> size, int mode);

  //==================================================================
  //函数名：  get_blob_color_max
  //作者：    dianjixz
  //日期：    2021-8-01
  //功能：    获取框内颜色的最大值
  //输入参数：
  //          py::object py_img     python输入图像对象
  //          std::vector<int> roi       感兴趣区域最大阈值
  //          int critical          上下颜色范围宽度(简单扩大范围)
  //          int co                返回的颜色模式  0:rgb   1:Lab   2:HSV
  //          std::vector<int> size      图像的尺寸(非必须)
  //          int mode              图像的格式(非必须)
  //返回值：
  //          返回一个框内颜色的最大值的列表；
  //修改记录：
  //==================================================================
  py::list get_blob_color_max(std::vector<int> &roi, int critical, int co);

  //==================================================================
  //函数名：  _maix_vision_find_blob
  //作者：    dianjixz
  //日期：    2021-8-01
  //功能：    查找颜色区域
  //输入参数：
  //          ector<vector<int>> &thresholds      颜色的阈值
  //          std::vector<int> roi                     感兴趣区域最大阈值
  //          int x_stride                        x方向的最小像素
  //          int y_stride                        y方向的最小像素
  //          bool invert                         是否反转图像
  //          int area_threshold                  最小面积阈值
  //          int pixels_threshold                最小像素阈值
  //          bool merge                          是否合并相邻的像素块
  //          int margin                          合并的最小距离
  //          int tilt                            寻找包裹最小的矩形
  //          int co                              传入的颜色阈值模式  0:rgb   1:Lab   2:HSV    3:L
  //          std::vector<int> size                    图像的尺寸(非必须)
  //          int mode                            图像的格式(非必须)
  //返回值：
  //          返回一个查找到颜色的列表
  //修改记录：
  //==================================================================
  py::list _maix_vision_find_blob(std::vector<std::vector<int>> &thresholds, std::vector<int> roi, int x_stride, int y_stride, bool invert, int area_threshold, int pixels_threshold, bool merge, int margin, int tilt, int co);

  //==================================================================
  //函数名：  _maix_vision_find_ball_blob
  //作者：    dianjixz
  //日期：    2021-8-01
  //功能：    查找符合颜色的小球
  //输入参数：
  //          py::object py_img                   python输入图像对象
  //          ector<vector<int>> &thresholds      颜色的阈值
  //          int co                              传入的颜色阈值模式  0:rgb   1:Lab   2:HSV
  //          std::vector<int> size                    图像的尺寸(非必须)
  //          int mode                            图像的格式(非必须)
  //返回值：
  //          返回一个查找到小球的列表
  //修改记录：
  //==================================================================
  py::list _maix_vision_find_ball_blob(std::vector<int> &thresholds, int co);

  py::dict find_line();

  //==================================================================
  //函数名：  _maix_vision_get_histogram
  //作者：    dianjixz
  //日期：    2021-10-29
  //功能：    在 roi 的所有颜色通道上进行标准化直方图运算，并返回 histogram 对象
  //输入参数：
  //          py::object py_img                        python输入图像对象
  //          vector<vector<int>> &thresholds          thresholds 必须是元组列表。 [(lo, hi), (lo, hi), ..., (lo, hi)] 定义你想追踪的颜色范围,对于灰度图像，每个元组需要包含两个值 - 最小灰度值和最大灰度值。
  //          bool invert                              反转阈值操作，像素在已知颜色范围之外进行匹配，而非在已知颜色范围内。
  //          std::vector<int> &roi                         是感兴趣区域的矩形元组(x，y，w，h)。如果未指定，ROI即整个图像的图像矩形。操作范围仅限于 roi 区域内的像素。
  //          int bins                                 和其他bin是用于直方图通道的箱数。对于灰度图像，使用 bins
  //          std::vector<int> size      图像的尺寸(非必须)(默认(0,0))
  //          int mode              图像的格式(非必须)(默认16)
  //返回值：
  //          返回中值滤波后的图像,图像格式和输入保持一致；
  //修改记录：
  //==================================================================
  py::object _maix_vision_get_histogram(std::vector<std::vector<int>> &thresholds, bool invert, std::vector<int> &roi, int bins, std::vector<int> size, int mode);







  #include "maix_image.h"

maix_version::maix_version()
{
  // LOG_INFO << "Load the maix_Version module success!";
}

void maix_version::version_test()
{
}

int maix_version::Distance(int x1, int y1, int x2, int y2)
{
  int x = abs(x1 - x2);
  int y = abs(y1 - y2);
  return int(round(sqrt(x * x + y * y)));
}

void maix_version::_maix_vision_medianBlur(int m_size)
{
}

void maix_version::_maix_vision_GaussianBlur(std::vector<int> ksize, double sigmaX, double sigmaY, int borderType, std::vector<int> size, int mode)
{
  //     cv::Mat in_img;
  //     py_img_to_in_img(py_img, in_img, size, mode); //获取图像
  //     cv::Mat dist;
  //     cv::GaussianBlur(in_img, dist, cv::Size(ksize[0], ksize[1]), sigmaX, sigmaY);
  //     py::object tmp = out_img_to_py_img(py_img, dist);
  //     return tmp;
}

void maix_version::_maix_vision_Canny(int thr_h, int thr_l)
{
  if (this->_img)
  {
    cv::Mat input(this->_maix_image_width, this->_maix_image_height, any_cast<int>(this->py_to_pram[this->get_to(this->_maix_image_type)][2]), const_cast<char *>((char *)this->_img->data));
    cv::Mat dist;
    cv::Canny(input, dist, thr_h, thr_l);
    this->_maix_image_size = this->_maix_image_width * this->_maix_image_height;
    memcpy(this->_img->data, dist.data, this->_maix_image_size);
    this->_maix_image_type = "L";
    this->_img->mode = any_cast<libmaix_image_mode_t>(this->py_to_pram[0][0]);
  }
  else {
    // std::cout << "please load image!" << std::endl;
  }
}

void maix_version::_maix_vision_HoughCircles(int method, double dp, double minDist, double param1, double param2, int minRadius, int maxRadius, std::vector<int> size, int mode)
{
  // cv::Mat in_img;
  // this->py_img_to_in_img(py_img, in_img, size, mode); //获取图像
  // cv::Mat dist;
  // vector<cv::Vec3f> circles;
  // cv::HoughCircles(in_img, circles, method, dp, minDist, param1, param2, minRadius, maxRadius);
  // py::list return_val;
  // for (size_t i = 0; i < circles.size(); i++)
  // {
  //   py::list tmp;
  //   tmp.append(circles[i][0]);
  //   tmp.append(circles[i][1]);
  //   tmp.append(circles[i][2]);
  //   return_val.append(tmp);
  // }
  // return return_val;
}

py::object maix_version::_maix_vision_opencv_calcHist(int channels, std::vector<int> &roi, int histSize, std::vector<int> ranges, bool uniform, bool accumulate, std::vector<int> size, int mode)
{
  py::list return_val;
  // cv::Mat in_img;
  // this->py_img_to_in_img(py_img, in_img, size, mode); //获取图像
  // cv::Rect rect;
  // if (roi[2] != 0 && roi[3] != 0)
  // {
  //   rect.x = roi[0];
  //   rect.y = roi[1];
  //   rect.width = roi[2];
  //   rect.height = roi[3];
  // }
  // else
  // {
  //   rect.x = 0;
  //   rect.y = 0;
  //   rect.width = in_img.size[0];
  //   rect.height = in_img.size[1];
  // }
  // cv::Mat mask = cv::Mat::zeros(in_img.size(), CV_8UC1);
  // mask(rect).setTo(255);
  // float range[] = {ranges[0], ranges[1]};
  // const float *histRanges = {range};
  // cv::Mat _hist;
  // calcHist(&in_img, 1, (const int *)&channels, mask, _hist, 1, (const int *)&histSize, &histRanges, uniform, accumulate);
  // for (int i = 0; i < histSize; i++)
  // {
  //   return_val.append(_hist.at<float>(i));
  // }
  return return_val;
}

py::list maix_version::get_blob_color_max(std::vector<int> &roi, int critical, int co)
{
  py::list return_val;
  cv::Mat src(this->_img->width, this->_img->height, any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][2]), this->_img->data);
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
    // return_val.append(int(max_lnum * 100 / 255));
    // return_val.append(max_anum - 128);
    // return_val.append(max_bnum - 128);
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

py::list maix_version::_maix_vision_find_blob(std::vector<std::vector<int>> &thresholds, std::vector<int> roi,
                                              int x_stride, int y_stride, bool invert, int area_threshold, int pixels_threshold, bool merge, int margin,
                                              int tilt, int co)
{
  py::list return_val;
  cv::Mat src(this->_img->width, this->_img->height, any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][2]), this->_img->data);
  cv::Mat in_img;
  src.copyTo(in_img);
  cv::Mat lab, mask1;
  bool grasy = 0;
  switch (co) //转换颜色空间以及颜色阈值
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
  default: //不在颜色阈值内,返回
    return return_val;
    break;
  }
  cv::Mat mask = cv::Mat::zeros(lab.size(), CV_8UC1);
  if (grasy)
  {
    for (size_t i = 0; i < thresholds.size(); i++)
    {
      cv::inRange(lab, cv::Scalar(thresholds[i][0]), cv::Scalar(thresholds[i][1]), mask1); //分割通道,阈值化
      mask = mask + mask1;
    }
  }
  else
  {
    for (size_t i = 0; i < thresholds.size(); i++)
    {
      cv::inRange(lab, cv::Scalar(thresholds[i][0], thresholds[i][1], thresholds[i][2]), cv::Scalar(thresholds[i][3], thresholds[i][4], thresholds[i][5]), mask1); //分割通道,阈值化
      mask = mask + mask1;
    }
  }
  if (invert) //是否反转图像
  {
    cv::bitwise_not(mask, mask);
  }

  cv::Mat se = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(x_stride, y_stride), cv::Point(-1, -1)); //开运算,去除噪点
  cv::morphologyEx(mask, mask, cv::MORPH_OPEN, se);
  if (margin != 0)
  {
    cv::Mat se_t = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(margin, margin), cv::Point(-1, -1)); //闭运算,链接相邻比较近的色块
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, se_t);
  }

  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hiearchy;
  cv::findContours(mask, contours, hiearchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE); //查找最小矩形轮廓
  if (contours.size() == 0)
  {
    return return_val;
  }
  for (size_t i = 0; i < contours.size(); i++)
  {
    py::dict val;
    cv::Rect rects = cv::boundingRect(contours[i]);
    val["x"] = int(rects.x);
    val["y"] = int(rects.y);
    val["w"] = int(rects.width);
    val["h"] = int(rects.height);
    if (int(rects.width) * int(rects.width) < area_threshold)
    {
      continue;
    }
    if (int(cv::contourArea(contours[i])) < pixels_threshold)
    {
      continue;
    }
    val["pixels"] = int(cv::contourArea(contours[i]));

    val["cx"] = int(rects.x + rects.width / 2);
    val["cy"] = int(rects.y + rects.height / 2);

    if (tilt) //是否需要最小包裹矩形轮廓
    {
      cv::RotatedRect minRect = cv::minAreaRect(contours[i]);
      cv::Point2f rect_points[4];
      minRect.points(rect_points);
      py::tuple tmp3 = py::make_tuple(rect_points[0].x, rect_points[0].y, rect_points[1].x, rect_points[1].y, rect_points[2].x, rect_points[2].y, rect_points[3].x, rect_points[3].y);
      val["tilt_Rect"] = tmp3;
      int tmp1 = this->Distance(int(rect_points[0].x), int(rect_points[0].y), int(rect_points[1].x), int(rect_points[1].y));
      int tmp2 = this->Distance(int(rect_points[0].x), int(rect_points[0].y), int(rect_points[3].x), int(rect_points[3].y));
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
  cv::Mat src(this->_img->width, this->_img->height, any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][2]), this->_img->data);
  cv::Mat in_img;
  src.copyTo(in_img);
  cv::Mat hsv, mask;
  switch (co) //转换颜色空间以及颜色阈值
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
  default: //不在颜色阈值内,返回
    return out;
    break;
  }
  cv::inRange(hsv, cv::Scalar(thresholds[0], thresholds[1], thresholds[2]), cv::Scalar(thresholds[3], thresholds[4], thresholds[5]), mask);
  // cout << hsv_da <<endl;
  cv::Mat se = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5), cv::Point(-1, -1));
  cv::morphologyEx(mask, mask, cv::MORPH_OPEN, se);
  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hiearchy;
  cv::findContours(mask, contours, hiearchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
  for (size_t i = 0; i < contours.size(); i++)
  {
    // 当拟合点数少于6个时，不进行拟合
    if (contours[i].size() < 6)
    {
      break;
    }
    // 圆拟合
    cv::RotatedRect rrt = fitEllipse(contours[i]);
    int cr_x, cr_y, cr_w, cr_h;
    cr_x = rrt.center.x;
    cr_y = rrt.center.y;
    cr_w = rrt.size.width;
    cr_h = rrt.size.height;
    // 当图形长宽相差太大 或者 图形面积太小时，不进行处理
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

  return out;
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

  //阈值处理
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
#define heigh_t 10
py::dict maix_version::find_line()
{
  py::dict return_val;
  cv::Mat src_gray, dst;
  cv::Mat src(this->_img->width, this->_img->height, any_cast<int>(py_to_pram[this->get_to(this->_maix_image_type)][2]), this->_img->data);
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

py::object maix_version::_maix_vision_get_histogram(std::vector<std::vector<int>> &thresholds, bool invert, std::vector<int> &roi, int bins, std::vector<int> size, int mode)
{
  py::list return_val;
  // cv::Mat in_img;
  // this->py_img_to_in_img(py_img, in_img, size, mode); //获取图像
  // cv::Rect rect;
  // if (roi[2] != 0 && roi[3] != 0)
  // {
  //   rect.x = roi[0];
  //   rect.y = roi[1];
  //   rect.width = roi[2];
  //   rect.height = roi[3];
  // }
  // else
  // {
  //   rect.x = 0;
  //   rect.y = 0;
  //   rect.width = in_img.size[0];
  //   rect.height = in_img.size[1];
  // }
  // cv::Mat mask = cv::Mat::zeros(in_img.size(), CV_8UC1);
  // mask(rect).setTo(255);
  // // vector<float> ranges1;
  // // float range[] = {ranges[0], ranges[1]};
  // // const float *histRanges = {range};
  // // cv::Mat _hist;
  // // calcHist(&in_img, 1, (const int*)&channels, mask, _hist, 1, (const int*)&histSize, &histRanges, uniform, accumulate);
  // // for(int i=0;i<histSize;i++)
  // // {
  // //   return_val.append(_hist.at<float>(i));
  // // }
  return return_val;
}



    /*
LIBMAIX_IMAGE_MODE_INVALID -> LIBMAIX_IMAGE_MODE_INVALID   :    0
LIBMAIX_IMAGE_MODE_INVALID -> LIBMAIX_IMAGE_MODE_BINARY   :     1
LIBMAIX_IMAGE_MODE_INVALID -> LIBMAIX_IMAGE_MODE_GRAY   :       2
LIBMAIX_IMAGE_MODE_INVALID -> LIBMAIX_IMAGE_MODE_RGB888   :     3
LIBMAIX_IMAGE_MODE_INVALID -> LIBMAIX_IMAGE_MODE_RGB565   :     4
LIBMAIX_IMAGE_MODE_INVALID -> LIBMAIX_IMAGE_MODE_RGBA8888   :   5
LIBMAIX_IMAGE_MODE_INVALID -> LIBMAIX_IMAGE_MODE_YUV420SP_NV21   :      6
LIBMAIX_IMAGE_MODE_INVALID -> LIBMAIX_IMAGE_MODE_YUV422_YUYV   :        7
LIBMAIX_IMAGE_MODE_INVALID -> LIBMAIX_IMAGE_MODE_BGR888   :     8
LIBMAIX_IMAGE_MODE_BINARY -> LIBMAIX_IMAGE_MODE_INVALID   :     256
LIBMAIX_IMAGE_MODE_BINARY -> LIBMAIX_IMAGE_MODE_BINARY   :      257
LIBMAIX_IMAGE_MODE_BINARY -> LIBMAIX_IMAGE_MODE_GRAY   :        258
LIBMAIX_IMAGE_MODE_BINARY -> LIBMAIX_IMAGE_MODE_RGB888   :      259
LIBMAIX_IMAGE_MODE_BINARY -> LIBMAIX_IMAGE_MODE_RGB565   :      260
LIBMAIX_IMAGE_MODE_BINARY -> LIBMAIX_IMAGE_MODE_RGBA8888   :    261
LIBMAIX_IMAGE_MODE_BINARY -> LIBMAIX_IMAGE_MODE_YUV420SP_NV21   :       262
LIBMAIX_IMAGE_MODE_BINARY -> LIBMAIX_IMAGE_MODE_YUV422_YUYV   :         263
LIBMAIX_IMAGE_MODE_BINARY -> LIBMAIX_IMAGE_MODE_BGR888   :      264
LIBMAIX_IMAGE_MODE_GRAY -> LIBMAIX_IMAGE_MODE_INVALID   :       512
LIBMAIX_IMAGE_MODE_GRAY -> LIBMAIX_IMAGE_MODE_BINARY   :        513
LIBMAIX_IMAGE_MODE_GRAY -> LIBMAIX_IMAGE_MODE_GRAY   :  514
LIBMAIX_IMAGE_MODE_GRAY -> LIBMAIX_IMAGE_MODE_RGB888   :        515
LIBMAIX_IMAGE_MODE_GRAY -> LIBMAIX_IMAGE_MODE_RGB565   :        516
LIBMAIX_IMAGE_MODE_GRAY -> LIBMAIX_IMAGE_MODE_RGBA8888   :      517
LIBMAIX_IMAGE_MODE_GRAY -> LIBMAIX_IMAGE_MODE_YUV420SP_NV21   :         518
LIBMAIX_IMAGE_MODE_GRAY -> LIBMAIX_IMAGE_MODE_YUV422_YUYV   :   519
LIBMAIX_IMAGE_MODE_GRAY -> LIBMAIX_IMAGE_MODE_BGR888   :        520
LIBMAIX_IMAGE_MODE_RGB888 -> LIBMAIX_IMAGE_MODE_INVALID   :     768
LIBMAIX_IMAGE_MODE_RGB888 -> LIBMAIX_IMAGE_MODE_BINARY   :      769
LIBMAIX_IMAGE_MODE_RGB888 -> LIBMAIX_IMAGE_MODE_GRAY   :        770
LIBMAIX_IMAGE_MODE_RGB888 -> LIBMAIX_IMAGE_MODE_RGB888   :      771
LIBMAIX_IMAGE_MODE_RGB888 -> LIBMAIX_IMAGE_MODE_RGB565   :      772
LIBMAIX_IMAGE_MODE_RGB888 -> LIBMAIX_IMAGE_MODE_RGBA8888   :    773
LIBMAIX_IMAGE_MODE_RGB888 -> LIBMAIX_IMAGE_MODE_YUV420SP_NV21   :       774
LIBMAIX_IMAGE_MODE_RGB888 -> LIBMAIX_IMAGE_MODE_YUV422_YUYV   :         775
LIBMAIX_IMAGE_MODE_RGB888 -> LIBMAIX_IMAGE_MODE_BGR888   :      776
LIBMAIX_IMAGE_MODE_RGB565 -> LIBMAIX_IMAGE_MODE_INVALID   :     1024
LIBMAIX_IMAGE_MODE_RGB565 -> LIBMAIX_IMAGE_MODE_BINARY   :      1025
LIBMAIX_IMAGE_MODE_RGB565 -> LIBMAIX_IMAGE_MODE_GRAY   :        1026
LIBMAIX_IMAGE_MODE_RGB565 -> LIBMAIX_IMAGE_MODE_RGB888   :      1027
LIBMAIX_IMAGE_MODE_RGB565 -> LIBMAIX_IMAGE_MODE_RGB565   :      1028
LIBMAIX_IMAGE_MODE_RGB565 -> LIBMAIX_IMAGE_MODE_RGBA8888   :    1029
LIBMAIX_IMAGE_MODE_RGB565 -> LIBMAIX_IMAGE_MODE_YUV420SP_NV21   :       1030
LIBMAIX_IMAGE_MODE_RGB565 -> LIBMAIX_IMAGE_MODE_YUV422_YUYV   :         1031
LIBMAIX_IMAGE_MODE_RGB565 -> LIBMAIX_IMAGE_MODE_BGR888   :      1032
LIBMAIX_IMAGE_MODE_RGBA8888 -> LIBMAIX_IMAGE_MODE_INVALID   :   1280
LIBMAIX_IMAGE_MODE_RGBA8888 -> LIBMAIX_IMAGE_MODE_BINARY   :    1281
LIBMAIX_IMAGE_MODE_RGBA8888 -> LIBMAIX_IMAGE_MODE_GRAY   :      1282
LIBMAIX_IMAGE_MODE_RGBA8888 -> LIBMAIX_IMAGE_MODE_RGB888   :    1283
LIBMAIX_IMAGE_MODE_RGBA8888 -> LIBMAIX_IMAGE_MODE_RGB565   :    1284
LIBMAIX_IMAGE_MODE_RGBA8888 -> LIBMAIX_IMAGE_MODE_RGBA8888   :  1285
LIBMAIX_IMAGE_MODE_RGBA8888 -> LIBMAIX_IMAGE_MODE_YUV420SP_NV21   :     1286
LIBMAIX_IMAGE_MODE_RGBA8888 -> LIBMAIX_IMAGE_MODE_YUV422_YUYV   :       1287
LIBMAIX_IMAGE_MODE_RGBA8888 -> LIBMAIX_IMAGE_MODE_BGR888   :    1288
LIBMAIX_IMAGE_MODE_YUV420SP_NV21 -> LIBMAIX_IMAGE_MODE_INVALID   :      1536
LIBMAIX_IMAGE_MODE_YUV420SP_NV21 -> LIBMAIX_IMAGE_MODE_BINARY   :       1537
LIBMAIX_IMAGE_MODE_YUV420SP_NV21 -> LIBMAIX_IMAGE_MODE_GRAY   :         1538
LIBMAIX_IMAGE_MODE_YUV420SP_NV21 -> LIBMAIX_IMAGE_MODE_RGB888   :       1539
LIBMAIX_IMAGE_MODE_YUV420SP_NV21 -> LIBMAIX_IMAGE_MODE_RGB565   :       1540
LIBMAIX_IMAGE_MODE_YUV420SP_NV21 -> LIBMAIX_IMAGE_MODE_RGBA8888   :     1541
LIBMAIX_IMAGE_MODE_YUV420SP_NV21 -> LIBMAIX_IMAGE_MODE_YUV420SP_NV21   :        1542
LIBMAIX_IMAGE_MODE_YUV420SP_NV21 -> LIBMAIX_IMAGE_MODE_YUV422_YUYV   :  1543
LIBMAIX_IMAGE_MODE_YUV420SP_NV21 -> LIBMAIX_IMAGE_MODE_BGR888   :       1544
LIBMAIX_IMAGE_MODE_YUV422_YUYV -> LIBMAIX_IMAGE_MODE_INVALID   :        1792
LIBMAIX_IMAGE_MODE_YUV422_YUYV -> LIBMAIX_IMAGE_MODE_BINARY   :         1793
LIBMAIX_IMAGE_MODE_YUV422_YUYV -> LIBMAIX_IMAGE_MODE_GRAY   :   1794
LIBMAIX_IMAGE_MODE_YUV422_YUYV -> LIBMAIX_IMAGE_MODE_RGB888   :         1795
LIBMAIX_IMAGE_MODE_YUV422_YUYV -> LIBMAIX_IMAGE_MODE_RGB565   :         1796
LIBMAIX_IMAGE_MODE_YUV422_YUYV -> LIBMAIX_IMAGE_MODE_RGBA8888   :       1797
LIBMAIX_IMAGE_MODE_YUV422_YUYV -> LIBMAIX_IMAGE_MODE_YUV420SP_NV21   :  1798
LIBMAIX_IMAGE_MODE_YUV422_YUYV -> LIBMAIX_IMAGE_MODE_YUV422_YUYV   :    1799
LIBMAIX_IMAGE_MODE_YUV422_YUYV -> LIBMAIX_IMAGE_MODE_BGR888   :         1800
LIBMAIX_IMAGE_MODE_BGR888 -> LIBMAIX_IMAGE_MODE_INVALID   :     2048
LIBMAIX_IMAGE_MODE_BGR888 -> LIBMAIX_IMAGE_MODE_BINARY   :      2049
LIBMAIX_IMAGE_MODE_BGR888 -> LIBMAIX_IMAGE_MODE_GRAY   :        2050
LIBMAIX_IMAGE_MODE_BGR888 -> LIBMAIX_IMAGE_MODE_RGB888   :      2051
LIBMAIX_IMAGE_MODE_BGR888 -> LIBMAIX_IMAGE_MODE_RGB565   :      2052
LIBMAIX_IMAGE_MODE_BGR888 -> LIBMAIX_IMAGE_MODE_RGBA8888   :    2053
LIBMAIX_IMAGE_MODE_BGR888 -> LIBMAIX_IMAGE_MODE_YUV420SP_NV21   :       2054
LIBMAIX_IMAGE_MODE_BGR888 -> LIBMAIX_IMAGE_MODE_YUV422_YUYV   :         2055
LIBMAIX_IMAGE_MODE_BGR888 -> LIBMAIX_IMAGE_MODE_BGR888   :      2056
*/