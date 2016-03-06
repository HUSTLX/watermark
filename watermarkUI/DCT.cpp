#include "DCT.h"
void DctEmbedWatermark(const cv::Mat& image, cv::Mat& output, long frame_num)
{
	output=image.clone();
	imwrite("./res/oringin.bmp", image);
	cv::Mat watermark = cv::imread("./res/name.bmp");
	int image_height = image.rows;
	int image_width = image.cols;
	int watermark_height = watermark.rows;
	int watermark_width = watermark.cols;
	double step_height = (double)image_height / watermark_height;
	double step_width = (double)image_width / watermark_width;
	int block_height = floor(step_height);
	int block_width = floor(step_width);
	printf("block_height%d\n",block_height); 
	printf("block_width%d\n", block_width);
	// Make sure the both image dimensions are a multiple of 2
	if (block_height % 2 == 0)
		block_height = block_height;
	else
		block_height = block_height - 1;
	if (block_width % 2 == 0)
		block_width = block_width;
	else
		block_width = block_width - 1;
	// Grayscale image is 8bits per pixel,
	// but dct() method wants float values!
	cv::Mat dct_image = cv::Mat(image.rows, image.cols, CV_64F);
	image.convertTo(dct_image, CV_64F);
	int index = 0;
	for (int i = 0; i<watermark_height; i++)
		for (int j = 0; j < watermark_width; j++)//watermark_height*watermark_widthµÄ´óÑ­»·
		{
			cv::Rect rect(floor(j*step_width), floor(i*step_height), block_width, block_height);
			printf("x%f  y%f\n", i*step_height, j*step_width);
			cv::Mat image_roi;
			// Let's do the DCT now: image => frequencies
			cv::dct(dct_image(rect), image_roi);
			image_roi.at<double>(1, 0) = 0.0;
			cv::Mat image_out(image_roi.size(), image_roi.type());
			cv::idct(image_roi, image_out);
			image_out.copyTo(output(rect));
		}
	// Save a visualization of the DCT coefficients
	imwrite("./res/dct.bmp", dct_image);
	output.convertTo(output, CV_8UC1);
	imshow("out", output);
	imwrite("./res/output.bmp", output);
}
