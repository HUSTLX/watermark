#ifndef DCT_H_
#define DCT_H_
#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp" 
#include "opencv2/imgproc/imgproc.hpp"
#include <highgui.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>  
#include <random>
#include <stdlib.h>
#include <time.h>
void DctEmbedWatermark(const cv::Mat& src, cv::Mat& output, long frame_num);
#endif
