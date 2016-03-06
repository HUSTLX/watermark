#include "Arnold.h"
#include <vector>
//猫脸变换
//src_pic为原来的数组，times为要变换的次数
void Arnold(cv::Mat& src, int times)
{
	int x_old, y_old, x_new, y_new;
	cv::Mat temp = src.clone();
	int height = src.rows;
	int width = src.cols;
	for (int k = 0; k<times; k++)
	{
		for (x_old = 0; x_old<height; x_old++)
			for (y_old = 0; y_old<width; y_old++)
			{
				x_new = (x_old + y_old) % width;
				y_new = (x_old + 2 * y_old) % width;

				temp.at<uchar>(x_new, y_new) = src.at<uchar>(x_old, y_old);//像素位置变换
			}
		for (int i = 0; i<height; i++)
			for (int j = 0; j<width; j++)
				src.at<uchar>(i, j) = temp.at<uchar>(i, j); //将一次变换的结果重新复制给原数组，作为下次的变换数据；
	}
}
//猫脸逆变换
void Reverse_Arnold(cv::Mat& src, int times)
{
	int x_old, y_old, x_new, y_new;
	cv::Mat temp = src.clone();
	int height = src.rows;
	int width = src.cols;
	for (int k = 0; k<times; k++)
	{
		for (x_old = 0; x_old<height; x_old++)
			for (y_old = 0; y_old<width; y_old++)
			{
				x_new = (2 * x_old - y_old + width) % width;
				y_new = (-x_old + y_old + width) % width;
				temp.at<uchar>(x_new, y_new) = src.at<uchar>(x_old, y_old);//像素位置变换
			}
		for (int i = 0; i<height; i++)
			for (int j = 0; j<width; j++)
				src.at<uchar>(i, +j) = temp.at<uchar>(i, j); //将一次变换的结果重新复制给原数组，作为下次的变换数据；
	}
}
void ArnoldEmbedWatermark(cv::Mat& image, const cv::Mat& watermark, double &psnr)
{
    int image_height = image.rows * 7 / 9;
	int image_width = image.cols * 7 / 9;
	const int kEmbedNumPerBlock = image_height * image_width / (64 * 64 * 5);
	int watermark_height = watermark.rows;
	int watermark_width = watermark.cols;
	double step_height = static_cast<double>(image_height) / watermark_height;
	double step_width = static_cast<double>(image_width) / watermark_width;
	int block_height = floor(step_height);
	int block_width = floor(step_width);
	//std::cout << "watermark_height" << watermark_height << "block_height" << block_height << std::endl;
	int rand_max = block_height*block_width - 1;
	static std::default_random_engine e(time(0));
	static std::uniform_int_distribution<unsigned> u(0, rand_max);
	int embed_x, embed_y;
	double mes,embed_count = 0;
	for (int i = 0; i < watermark_height; i++)
	{
		for (int j = 0; j < watermark_width; j++)
		{
			//std::cout << "i" << i << "j" << j << std::endl;
			if (watermark.at<uchar>(i, j) > 0)
			{
				embed_count += 1;
                cv::Rect rect(floor(j*step_width + image.cols / 9), floor(i*step_height + image.rows / 9), block_width, block_height);
				cv::Mat image_roi = image(rect);
				for (int x = 0; x < kEmbedNumPerBlock; x++)
				{
					int rand_num = u(e);
					embed_x = rand_num / block_width;
					embed_y = rand_num % block_width;
					//std::cout << "embed_y" << embed_y << "embed_x" << embed_x << std::endl;
					if (image_roi.at<cv::Vec3b>(embed_x, embed_y)[0]>EMBED_STRENGTH)
						image_roi.at<cv::Vec3b>(embed_x, embed_y)[0] = image_roi.at<cv::Vec3b>(embed_x, embed_y)[0] - EMBED_STRENGTH;
					else
						image_roi.at<cv::Vec3b>(embed_x, embed_y)[0] = image_roi.at<cv::Vec3b>(embed_x, embed_y)[0] + EMBED_STRENGTH;
				}
				image_roi.copyTo(image(rect));
			}
		}
	}
	mes = embed_count*kEmbedNumPerBlock*EMBED_STRENGTH*EMBED_STRENGTH / (image.cols*image.rows * 3);
	psnr = 20 * log10(255/sqrt(mes));
	//std::cout << embed_count << std::endl;
}
void ArnoldDetectWatermark(cv::Mat& src, cv::Mat& detect_watermark, double &nc)
{
    double sum1 = 0, sum2 = 0, sum3 = 0;
    cv::Mat watermark = detect_watermark.clone();
    cv::Mat channels[3];
    // 把一个3通道图像转换成3个单通道图像  
    split(src, channels);//分离色彩通道  
    cv::mean(channels[1]);
    cv::Mat image = channels[0].clone();
    threshold(image, image, 5, 255, CV_THRESH_BINARY_INV);
    //imshow("image", image);
    imwrite("hei.jpg", image);
    std::vector<int> w_sum(image.cols);
    std::vector<int> h_sum(image.rows);
    for (int x = 0; x < image.cols; x++) {
        for (int y = 0; y < image.rows; y++) {
            if (image.at<uchar>(x, y) == 0)
                w_sum[x]++;
        }
        std::cout << x << "rows" << w_sum[x] << std::endl;
    }
    for (int x = 0; x < image.cols; x++) {
        for (int y = 0; y < image.rows; y++) {
            if (image.at<uchar>(x, y) == 0)
                w_sum[x]++;
        }
        std::cout << x << "rows" << w_sum[x] << std::endl;
    }
    int image_height = src.rows;
    int image_width = src.cols;
    const int kEmbedNumPerBlock = image_height * image_width / (64 * 64 * 5);
    double step_height = static_cast<double>(image_height) / 64;
    double step_width = static_cast<double>(image_width) / 64;
    int block_height = floor(step_height);
    int block_width = floor(step_width);
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            cv::Rect rect(floor(j*step_width), floor(i*step_height), block_width, block_height);
            cv::Mat roi = image(rect);
            int embeded_count = 0;
            for (int x = 0; x < block_height; x++)
            {
                for (int y = 0; y < block_width; y++)
                {
                    if (roi.at<uchar>(x, y) == 0)
                    {
                        embeded_count += 1;
                    }
                }
            }
            if (embeded_count > kEmbedNumPerBlock / 3)
                detect_watermark.at<uchar>(i, j) = 0;
            else
                detect_watermark.at<uchar>(i, j) = 255;
            if (watermark.at<uchar>(i, j) == detect_watermark.at<uchar>(i, j))
                sum1++;
            else if (watermark.at<uchar>(i, j) == 255)
                sum2 += (1 / 0.82);
            else
                sum2 += (1 / 0.18);
        }    
    }
    
    nc = sum1 / (sum2 + 64 * 64);
}
/**************************************************************************************
函数名：Projection
功  能：投影法找出矩形区域
输  入： 目标图像des,  原图像 src,图像宽width, 高height
返回值：no
**************************************************************************************/
void Projection(unsigned char* des, const unsigned char* src, int width, int height)
{
    int* h_sum = new int[height];
    int* w_sum = new int[width];
    int up = 0;
    int below = height;
    int left = 0;
    int right = width;
    for (int y = 0; y<height; y++)
    {
        for (int x = 0; x<width; x++)
        {
            des[y*width + x] = 255;
        }
    }
    for (int y = 0; y<height; y++)
    {
        h_sum[y] = 0;
        for (int x = 0; x<width; x++)
        {
            //printf("src %d",src[y*width+x]);
            h_sum[y] = h_sum[y] + src[y*width + x];
        }
        //printf("%d行%d ",y,h_sum[y]);
    }
    for (int y = height / 2; y<height; y++)
    {

        if ((h_sum[y] - h_sum[height / 2])>255 * 60)
        {
            below = y;
            break;
        }
    }
    for (int y = height / 2; y>0; y--)
    {
        if ((h_sum[y] - h_sum[height / 2])>255 * 60)
        {
            up = y;
            break;
        }
    }
    for (int x = 0; x<width; x++)
    {
        w_sum[x] = 0;
        for (int y = up; y<below; y++)
        {
            w_sum[x] = w_sum[x] + src[y*width + x];
        }
        //printf("%d列%d ",x,w_sum[x]);
    }
    int max_r = 0;
    int max_l = 0;
    for (int x = width / 2 + 100; x<width; x++)
    {
        if (w_sum[x]>max_r)
        {
            right = x;
            max_r = w_sum[x];
        }
    }
    for (int x = width / 2 - 100; x>0; x--)
    {
        if (w_sum[x]>max_l)
        {
            left = x;
            max_l = w_sum[x];
        }
    }
    for (int y = up; y<below; y++)
    {
        for (int x = left; x<right; x++)
        {
            des[y*width + x] = 0;
        }
    }
    printf("up%d below%d left%d right%d", up, below, left, right);
}