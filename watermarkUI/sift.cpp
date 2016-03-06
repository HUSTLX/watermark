#include "sift.h"
using namespace std;
using namespace cv;
const char* origin_watermark = "10101010101010101010";
int mod(int value, int p)
{
	if (value >= 0)
	{
		return (value%p);
	}
	else
	{
		int t = (value%p + p);
		if (t >= p)
		{
			return t = t - p;
		}
		else
			return (value%p + p);
	}
}
int even_odd_quantization(int num, int bit_watermark)
{
	int result;
	int value = num / QSTEP;
	if (bit_watermark == 0)
	{
		if (mod(value, 2) == 0)
			result = value*QSTEP;
		else
			result = (value - 1)*QSTEP;
	}
	else
	{
		if (mod(value, 2) == 0)
			result = (value + 1)*QSTEP;
		else
			result = value*QSTEP;
	}
	return result;
}
double angle_x(int Ax, int Ay, int Bx, int By)
{
	if (Bx == Ax)
	{
		if (Ay>By)
			return 90;
		else
			return 270;
	}
	double pi = 3.141592535;
	double distance_x = Bx - Ax;
	double distance_y = By - Ay;
	double angle;
	if (distance_x>0 && distance_y >= 0)
	{
		angle = atan(distance_y / distance_x);
	}
	else if (distance_x <= 0 && distance_y>0)
	{
		angle = pi / 2 + atan(abs(distance_x) / distance_y);
	}
	else if (distance_x<0 && distance_y <= 0)
	{
		angle = pi + atan(distance_y / distance_x);
	}
	else if (distance_x >= 0 && distance_y<0)
	{
		angle = 3 * pi / 2 + atan(distance_x / abs(distance_y));
	}
	return angle / pi * 180;
}
IplImage * embedWatermarkInSector(IplImage *I, int watermark[], int num_watermark, vector<KeyPoint> keypoints)
{
	IplImage* new_I = cvCloneImage(I);	
	double delta_angle = 360 / num_watermark;         //每个扇区占多少的角度
	uchar* data_img = (uchar*)new_I->imageData;
	int step_img = new_I->widthStep;
	for (int p = 0; p<keypoints.size(); p++)
	{
		int x = round(keypoints[p].pt.x);
		int y = round(keypoints[p].pt.y);
		//double outer_r = keypoints[p].size * OUTER_FACTOR;
		double outer_r = 10.0 * OUTER_FACTOR;
		double inner_r = INNER_FACTOR * outer_r;
		//double sift_orientation = keypoints[p].angle;
		double sift_orientation = 0;
		for (int i = x - outer_r; i <= x + outer_r; i++)
		{
			for (int j = y - outer_r; j <= y + outer_r; j++)
			{
				if (((i - x)*(i - x) + (j - y)*(j - y) <outer_r*outer_r) && ((i - x)*(i - x) + (j - y)*(j - y) >inner_r*inner_r))
				{
					double alpha = angle_x(i, j, x, y);
					int sector_num = floor((alpha - sift_orientation) / delta_angle);
					sector_num = mod(sector_num, num_watermark);
					if (!IS_CLOCKWISE)
					{
						int temp = data_img[j*step_img + i];
						temp = even_odd_quantization(temp,watermark[sector_num]);
						data_img[j*step_img + i] = temp;
					}
					else
					{
						int temp = data_img[j*step_img + i];
						temp = even_odd_quantization(temp, watermark[num_watermark - sector_num - 1]);
						data_img[j*step_img + i] = temp;
					}
				}
			}
		}
	}
	return new_I;
}
bool SortPoints(const KeyPoint &v1, const KeyPoint &v2)
{
	return (v1.response) > (v2.response);//升序排列
}
void EmbedWatermark(cv::Mat& image, cv::Mat& output,long frame_num)
{
	int num_watermark = strlen(origin_watermark);
	int *watermark = new int[num_watermark];
	for (int i = 0; i<num_watermark; i++)
	{
		if (origin_watermark[i] == '1') watermark[i] = 1;
		else watermark[i] = 0;
	}
	if (image.empty()) {
		cout<<"frame no data"<<endl;
		//return -1;
	}
	//SIFT特征点检测
	//SiftFeatureDetector sift;
	cv::SIFT sift(
		200, //The number of best features to retain.
		3, //The number of layers in each octave
		0.1, //The larger the threshold, the less features are produced by the detector.
		10, //the larger the edgeThreshold, the less features are filtered out(more features are retained
		1.6);//If your image is captured with a weak camera with soft lenses, you might want to reduce the number 
	vector<KeyPoint> keypoints(100000);
	sift.detect(image, keypoints);
	//SiftDescriptorExtractor extractor;
	//Mat descriptors;
	//extractor.compute(image, keypoints, descriptors);
	//筛选SIFT特征点，只尺度信息为8~20的特征点
	//KeyPointsFilter::runByKeypointSize(keypoints, 8, 20);
	//剔除掉图像边界处的点
	int width = image.rows;
	int height = image.cols;
	vector<KeyPoint> keypoints1;//边界处的点被剔除掉的结果
	for (int i = 0; i<keypoints.size(); i++)
	{
		int x = round(keypoints[i].pt.x);
		int y = round(keypoints[i].pt.y);
		//double r = keypoints[i].size*OUTER_FACTOR;
		double r = 10.0*OUTER_FACTOR;
		if (x + r >= height || y + r >= width || x - r <= 0 || y - r <= 0)
		{
			continue;
		}
		keypoints1.push_back(keypoints[i]);
	}
	//剔除掉建立区域互相重叠的SIFT特征点
	vector<KeyPoint> temp_keypoint1;
	vector<KeyPoint> temp_keypoint2;
	vector<KeyPoint> final_keypoint;
	temp_keypoint1 = keypoints1;
	while (temp_keypoint1.empty() != true)
	{
		//temp_keypoint1中的点进行排序
		std::sort(temp_keypoint1.begin(), temp_keypoint1.end(), SortPoints);
		final_keypoint.push_back(temp_keypoint1[0]);
		int x = round(temp_keypoint1[0].pt.x);
		int y = round(temp_keypoint1[0].pt.y);
		//double rad = temp_keypoint1[0].size;
		double rad = 10.0;
		//temp_keypoint1.erase(0,1);
		temp_keypoint2.clear();
		for (vector<KeyPoint>::iterator iter = temp_keypoint1.begin(); iter != temp_keypoint1.end(); iter++)
		{
			int temp_x = (*iter).pt.x;
			int temp_y = (*iter).pt.y;
			//double scale = (*iter).size;
			double scale = 10.0;
			if ((((temp_x - x)*(temp_x - x) + (temp_y - y)*(temp_y - y))) > ((scale + rad)*OUTER_FACTOR)*((scale + rad)*OUTER_FACTOR))
			{
				temp_keypoint2.push_back(*iter);
			}
		}
		temp_keypoint1 = temp_keypoint2;
	}
	cout << "final_keypoint" << '\t'<<final_keypoint.size() << endl;
	//嵌入水印
	IplImage* img = &IplImage(image);
	CvSize sz;
	sz.height = img->height;
	sz.width = img->width;
	IplImage* b_img = cvCreateImage(sz, img->depth, 1);
	IplImage* g_img = cvCreateImage(sz, img->depth, 1);
	IplImage* r_img = cvCreateImage(sz, img->depth, 1);
	cvSplit(img, b_img, g_img, r_img, NULL);
	IplImage* newb_img = embedWatermarkInSector(b_img, watermark, num_watermark, final_keypoint);
	std::string outputfile = "./res/photo/";
	std::string extension = ".jpg";
	std::stringstream newb;
	newb << outputfile << frame_num <<"newb"<< extension;
	cvSaveImage(newb.str().c_str(), newb_img);
	cvMerge(newb_img, g_img, r_img, NULL, img);
	output = Mat(img, false);
#if 0                       //画出sift点
	drawKeypoints(output,     //输入图像
	final_keypoint,      //特征点矢量
	output,      //输出图像
	cv::Scalar::all(-1),      //绘制特征点的颜色，为随机
	cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);//以特征点为中心画圆，圆的半径表示特征点的大小，直线表示特征点的方向

	std::ofstream outfile;//write data for debug
	outfile.open("./res/kp1.txt"); //kp.txt save keypoints
	if (outfile.is_open())
	{
		std::vector<cv::KeyPoint>::iterator itvc;
		for (itvc = final_keypoint.begin(); itvc != final_keypoint.end(); itvc++)
			outfile << itvc->pt << "\t" << itvc->size << "\t" << itvc->angle << "\t" << itvc->response << "\t" << itvc->octave << "\t" << itvc->class_id << std::endl;
		outfile.close();
	}
	else
		std::cout << "could not open the file!" << std::endl;
	std::stringstream ebeded_img;
	ebeded_img << outputfile << frame_num << "ebeded_img" << extension;
	cv::imwrite(ebeded_img.str(), output);
#endif
	delete[] watermark;
	cvReleaseImage(&b_img);
	cvReleaseImage(&newb_img);
	cvReleaseImage(&r_img);
	cvReleaseImage(&g_img);
}
int get_even_odd_quantization(int value)
{
	int M = ceil(((float)value) / QSTEP);
	if (mod(M, 2) == 0)
		return 0;
	else
		return 1;
}
bool detecWatermarkInSector(IplImage* I, int x, int y, double sift_orientation, double scale, int num_watermark, unsigned char watermark[], unsigned char detected_watermark[])
{
	double delta_angle = 360 / num_watermark;
	double	outer_r = 10.0*OUTER_FACTOR;
	double	inner_r = outer_r*INNER_FACTOR;
	vector<int> count1(num_watermark, 0);
	vector<int> count0(num_watermark, 0);
	for (int i = x - outer_r; i<x + outer_r; i++)
	{
		for (int j = y - outer_r; j<y + outer_r; j++)
		{
			if ((i - x)*(i - x) + (j - y)*(j - y) <outer_r*outer_r && (i - x)*(i - x) + (j - y)*(j - y) >inner_r*inner_r)
			{
				double alpha = angle_x(i, j, x, y);
				int sector_num = floor((alpha - sift_orientation) / delta_angle);        //第几个扇区，1 2 3 4 …… n9
				sector_num = mod(sector_num, num_watermark);
				if (!IS_CLOCKWISE)
				{
					uchar* data_img = (uchar*)I->imageData;
					int step_img = I->widthStep;
					int temp = data_img[j*step_img + i];
					int v = get_even_odd_quantization(temp);
					if (v == 1)
						count1[sector_num] = count1[sector_num] + 1;
					else
						count0[sector_num] = count0[sector_num] + 1;
				}
				else
				{
					uchar* data_img = (uchar*)I->imageData;
					int step_img = I->widthStep;
					int temp = data_img[j*step_img + i];
					int v = get_even_odd_quantization(temp);
					if (v == 1)
						count1[num_watermark - sector_num - 1] = count1[num_watermark - sector_num - 1] + 1;
					else
						count0[num_watermark - sector_num - 1] = count0[num_watermark - sector_num - 1] + 1;
				}
			}
		}
	}
	for (int kk = 0; kk<num_watermark; kk++)
	{
		if (count1[kk]>count0[kk])
			detected_watermark[kk] = 1;
		else
			detected_watermark[kk] = 0;
	}
	//判断是否嵌入水印
	int rightbitnum = 0;
	for (int kk = 0; kk<num_watermark; kk++)
	{
		//printf("%i",detected_watermark[kk]);
		if (watermark[kk] == detected_watermark[kk])
			rightbitnum++;
	}
	//printf("\num_watermark");
	if (rightbitnum >= THRESHOLD)
	{
		int i = 0;
		while(i < num_watermark) printf("%i", detected_watermark[i++]);
		return true;
	}	
	else
		return false;
}
//检测水印的函数
//输入变量：		imagefilename	需要检测水印的图片的文件名
//					watermark				嵌入的水印序列数组
//					num_watermark				嵌入水印序列长度
//					OUTER_FACTOR				圆环面积的倍数
//					QSTEP			量化步长
//					THRESHOLD				判断是否是水印的阈值
//					M				最多需要检测多少个区域
//					watermarkedimagefilename	存储下来嵌入水印的图像的文件名
//					mod				模式选择 当mod=0时，是检测水印有无模式，只要检测到一个区域含有水印程序就运行结束
//									当mod=1时，是检测水印信息的模式，会遍历一定数量的区域
//返回变量：		int				大于0，表示图片嵌入水印，模式0下，只返回1，模式1下，返回值是检测出水印区域个数
//									等于0 表示图片不含水印
//									小于0 检测水印失败
unsigned char* DetecWatermark(Mat &image, long frame_num)
{
	
	int num_watermark = strlen(origin_watermark);
	unsigned char *watermark = new unsigned char[num_watermark];
	unsigned char *watermark_temp1 = new unsigned char[num_watermark];
	unsigned char *watermark_temp2 = new unsigned char[num_watermark];
	unsigned char *detected_watermark = new unsigned char[num_watermark];
	for (int i = 0; i<num_watermark; i++)
	{
		if (origin_watermark[i] == '1') watermark[i] = 1;
		else watermark[i] = 0;
		watermark_temp1[i] = 0;
		watermark_temp2[i] = 0;
	}
	cv::SIFT sift(
		200, //The number of best features to retain.
		3, //The number of layers in each octave
		0.1, //The larger the threshold, the less features are produced by the detector.
		10, //the larger the edgeThreshold, the less features are filtered out(more features are retained
		1.6);//If your image is captured with a weak camera with soft lenses, you might want to reduce the number 
	vector<KeyPoint> keypoints(100000);
	sift.detect(image, keypoints);
	//SiftDescriptorExtractor extractor;
	//Mat descriptors;
	//extractor.compute(image, keypoints, descriptors);
	//筛选SIFT特征点，只尺度信息为8~20的特征点
	//KeyPointsFilter::runByKeypointSize(keypoints, 8, 20);
	//剔除掉图像边界处的点
	int width = image.rows;
	int height = image.cols;
	vector<KeyPoint> keypoints1;//边界处的点被剔除掉的结果
	for (int i = 0; i<keypoints.size(); i++)
	{
		int x = round(keypoints[i].pt.x);
		int y = round(keypoints[i].pt.y);
		//double r = keypoints[i].size*OUTER_FACTOR;
		double r = 10.0*OUTER_FACTOR;
		if (x + r >= height || y + r >= width || x - r <= 0 || y - r <= 0)
		{
			continue;
		}
		keypoints1.push_back(keypoints[i]);
	}
	//剔除掉建立区域互相重叠的SIFT特征点
	vector<KeyPoint> final_keypoint;
	/*vector<KeyPoint> temp_keypoint1;
	vector<KeyPoint> temp_keypoint2;
	temp_keypoint1 = keypoints1;
	while (temp_keypoint1.empty() != true)
	{
		//temp_keypoint1中的点进行排序
		std::sort(temp_keypoint1.begin(), temp_keypoint1.end(), SortPoints);
		final_keypoint.push_back(temp_keypoint1[0]);
		int x = temp_keypoint1[0].pt.x;
		int y = temp_keypoint1[0].pt.y;
		double rad = temp_keypoint1[0].size;
		//temp_keypoint1.erase(0,1);
		temp_keypoint2.clear();
		for (vector<KeyPoint>::iterator iter = temp_keypoint1.begin(); iter != temp_keypoint1.end(); iter++)
		{
			int temp_x = (*iter).pt.x;
			int temp_y = (*iter).pt.y;
			double scale = (*iter).size;
			if ((((temp_x - x)*(temp_x - x) + (temp_y - y)*(temp_y - y))) > ((scale + rad)*OUTER_FACTOR)*((scale + rad)*OUTER_FACTOR))
			{
				temp_keypoint2.push_back(*iter);
			}
		}
		temp_keypoint1 = temp_keypoint2;
	}*/
	final_keypoint = keypoints1;
	cout << "final_keypoint" << '\t' << final_keypoint.size() << endl;
	std::ofstream outfile;//write data for debug
	outfile.open("./res/rekp1.txt"); //kp.txt save keypoints
	if (outfile.is_open())
	{
		std::vector<cv::KeyPoint>::iterator itvc;
		for (itvc = final_keypoint.begin(); itvc != final_keypoint.end(); itvc++)
			outfile << itvc->pt << "\t" << itvc->size << "\t" << itvc->angle << "\t" << itvc->response << "\t" << itvc->octave << "\t" << itvc->class_id << std::endl;
		outfile.close();
	}
	else
		std::cout << "could not open the file!" << std::endl;
	int search_region_num;
	if (MAX_KEYPOINTS>final_keypoint.size())
	{
		search_region_num = final_keypoint.size();
		//std::cout << search_region_num << std::endl;
	}
	else
	{
		search_region_num = MAX_KEYPOINTS;
	}
	IplImage* img = &IplImage(image);
	CvSize sz;
	sz.height = img->height;
	sz.width = img->width;
	IplImage* b_img = cvCreateImage(sz, img->depth, 1);
	IplImage* g_img = cvCreateImage(sz, img->depth, 1);
	IplImage* r_img = cvCreateImage(sz, img->depth, 1);
	cvSplit(img, b_img, g_img, r_img, NULL);
	std::string outputfile = "./res/photo/";
	std::string extension = ".jpg";
	std::stringstream detect_bimg;
	detect_bimg << outputfile << frame_num << "detect_bimg" << extension;
	cvSaveImage(detect_bimg.str().c_str(), b_img);
	int detected_regions = 0;
	for (int p = 0; p<search_region_num; p++)
	{
		bool isWatermarked = false;
		int x = round(final_keypoint[p].pt.x);
		int y = round(final_keypoint[p].pt.y);
		double outer_r = final_keypoint[p].size;
		//double sift_orientation = final_keypoint[p].angle;
		double sift_orientation = 0;
		isWatermarked = detecWatermarkInSector(b_img, x, y, sift_orientation, outer_r,  num_watermark, watermark, watermark_temp1);
		if (isWatermarked == true)
		{
			detected_regions++;
			printf("\ntrue%d\n", detected_regions);
			for (int count = 0; count < num_watermark; count++)
			{
				if (watermark_temp1[count] == 1) watermark_temp2[count] = watermark_temp2[count] + 1;
				else watermark_temp2[count] = watermark_temp2[count] - 1;
			}
		}
		//else printf("\n flase\n");
	}
	for (int count = 0; count<num_watermark; count++)
	{
		if (watermark_temp2[count] >= 0) detected_watermark[count] = '1';
		else detected_watermark[count] = '0';
	}
	std::cout << detected_regions << std::endl;
	delete[]watermark;
	delete[]watermark_temp1;
	delete[]watermark_temp2;
	cvReleaseImage(&b_img);
	cvReleaseImage(&r_img);
	cvReleaseImage(&g_img);
	return detected_watermark;
}
