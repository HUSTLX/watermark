#ifndef ARNOLD_H_
#define ARNOLD_H_
#include <stdlib.h>
#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <random>
#include <stdlib.h>
#include <time.h>
#define EMBED_STRENGTH 30
void Arnold(cv::Mat& src, int times);
void Reverse_Arnold(cv::Mat& src, int times);
void ArnoldEmbedWatermark(cv::Mat& image, const cv::Mat& watermark, double &psnr);
void ArnoldDetectWatermark(cv::Mat& src, cv::Mat& detect_watermark, double &nc);
#endif
