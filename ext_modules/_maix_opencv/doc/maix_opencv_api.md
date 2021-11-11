## maix.Vision模块相关API描述

### Vision模块的一些图像参数

- Vision.COLOR_RGB

  Vision模块函数的输入mode参数常量,RGB格式图

- Vision.COLOR_RGBA

  Vision模块函数的输入mode参数常量,RGBA格式的图

- Vision.COLOR_L

  Vision模块函数的输入mode参数常量,灰度图

### Vision模块的一些opencv原生函数

- Vision.opecv_medianBlur(py_img, m_size,[size=(240,240),mode=Vision.COLOR_RGB])

  opencv中值滤波函数.
  
  img:输入的图像,支持PIL图,bytes二进制图.bytes二进制图需要提供size,和mode参数.
  
  m_size:中值滤波核大小,该参数可参考opencv::medianBlur函数

  输出是一个经过openmv中值滤波后的图像.
  
- Vision.opecv_GaussianBlur(py_img,ksize,,sigmaX,sigmaY,borderType,[size=(240,240),mode=Vision.COLOR_RGB])

  opencv高斯滤波函数
  
  img:输入的图像,支持PIL图,bytes二进制图.bytes二进制图需要提供size,和mode参数.
  
  ksize:高斯内核大小，这个尺寸与前面两个滤波kernel尺寸不同
  
  sigmaX:                  高斯核函数在X方向上的标准偏差
  
  sigmaY:                  高斯核函数在Y方向上的标准偏差，如果sigmaY是0，则函数会自动将sigmaY的值设置为与sigmaX相同的值，如果sigmaX和sigmaY都是0，这两个值将由ksize.width和ksize.height计算而来。具体可以参考getGaussianKernel()函数查看具体细节。建议将size、sigmaX和sigmaY都指定出来。
  
  返回一个经过滤波的后与输入格式相同的原图像
  
- Vision.opecv_Canny(py_img,thr_h,thr_l,[size=(240,240),mode=Vision.COLOR_RGB])

  opencv Canny图像算法,

  img:输入的图像,支持PIL图,bytes二进制图.bytes二进制图需要提供size,和mode参数.

  thr_h:最大阈值

  thr_l:最小阈值

  返回一个经过处理的与原格式相同的图像

- Vision.opecv_HoughCircles(py_img,method=3,dp,minDist,param1,param2,minRadius,maxRadius,[size=(240,240),mode=Vision.COLOR_RGB])

  method:使用的检测方法，目前opencv只有霍夫梯度法一种方法可用，该参数填Vision.HOUGH_GRADIENT即可    

  dp:double类型的dp，用来检测圆心的累加器图像的分辨率于输入图像之比的倒数，且此参数允许创建一个比输入图像分辨率低的累加器。

  minDist:为霍夫变换检测到的圆的圆心之间的最小距离

  param1:它是第三个参数method设置的检测方法的对应的参数。对当前唯一的方法霍夫梯度法Vision.CV_HOUGH_GRADIENT，它表示传递给canny边缘检测算子的高阈值，而低阈值为高阈值的一半。  

  param2:也是第三个参数method设置的检测方法的对应的参数，对当前唯一的方法霍夫梯度法HOUGH_GRADIENT，它表示在检测阶段圆心的累加器阈值。它越小的话，就可以检测到更多根本不存在的圆，而它越大的话，能通过检测的圆就更加接近完美的圆形了。

  minRadius:表示圆半径的最小值

  maxRadius:表示圆半径的最大值
