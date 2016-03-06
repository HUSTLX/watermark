#ifndef SIFT_H_
#define SIFT_H_
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>  
#define OUTER_FACTOR   5   //水印圆环外径比特征点尺寸倍数
#define INNER_FACTOR   0.5 //水印圆环内径比外径倍数
#define IS_CLOCKWISE   0	 //0表示逆时针嵌入水印，反之为1		
#define QSTEP         12  //量化步长         
#define MAX_KEYPOINTS 1600//每次检测水印最多区域个数 
#define THRESHOLD     18  //如果检测到的水印位数超过此阈值则认为嵌入了水印
extern const char* origin_watermark;
int mod(int num, int p);
double angle_x(int Ax, int Ay, int Bx, int By);
IplImage * embedWatermarkInSector(IplImage *I, int W[], int n, std::vector<cv::KeyPoint> keypoints, int m);
bool SortPoints(const cv::KeyPoint &v1, const cv::KeyPoint &v2);
void EmbedWatermark(cv::Mat& image, cv::Mat& output, long frame_num);
bool detecWatermarkInSector(IplImage* I, int x, int y, double sift_orientation, double scale,  int num_watermark, char watermark[], char detected_watermark[]);
unsigned char* DetecWatermark(cv::Mat &image, long frame_num);
#endif
