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
#define OUTER_FACTOR   5   //ˮӡԲ���⾶��������ߴ籶��
#define INNER_FACTOR   0.5 //ˮӡԲ���ھ����⾶����
#define IS_CLOCKWISE   0	 //0��ʾ��ʱ��Ƕ��ˮӡ����֮Ϊ1		
#define QSTEP         12  //��������         
#define MAX_KEYPOINTS 1600//ÿ�μ��ˮӡ���������� 
#define THRESHOLD     18  //�����⵽��ˮӡλ����������ֵ����ΪǶ����ˮӡ
extern const char* origin_watermark;
int mod(int num, int p);
double angle_x(int Ax, int Ay, int Bx, int By);
IplImage * embedWatermarkInSector(IplImage *I, int W[], int n, std::vector<cv::KeyPoint> keypoints, int m);
bool SortPoints(const cv::KeyPoint &v1, const cv::KeyPoint &v2);
void EmbedWatermark(cv::Mat& image, cv::Mat& output, long frame_num);
bool detecWatermarkInSector(IplImage* I, int x, int y, double sift_orientation, double scale,  int num_watermark, char watermark[], char detected_watermark[]);
unsigned char* DetecWatermark(cv::Mat &image, long frame_num);
#endif
