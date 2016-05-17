#include "videowatermark.h"
#include "MyForm.h"
using namespace cv;

Mat GetHistValue(const Mat image)
{
	//将图片转换到HSV空间
	//Mat hsv;
	//cvtColor(image, hsv, CV_BGR2HSV);
	// Quantize the hue to 30 levels
	// and the saturation to 32 levels
	int hbins = 30, sbins = 32;
	int histSize[] = { hbins, sbins };
	// hue varies from 0 to 179, see cvtColor
	float hranges[] = { 0, 256 };
	// saturation varies from 0 (black-gray-white) to
	// 255 (pure spectrum color)
	float sranges[] = { 0, 256 };
	const float* ranges[] = { hranges, sranges };
	MatND hist;
	// we compute the histogram from the 0-th and 1-st channels。0：H;1:S
	int channels[] = { 1, 2 };
	calcHist(&image, 1, channels, Mat(), // do not use mask
		hist, 2, histSize, ranges,
		true, // the histogram is uniform
		false);
	float sumNUM = 0.0;
	for (int h = 0; h < hbins; h++)
		for (int s = 0; s < sbins; s++)
		{
			float binVal = hist.at<float>(h, s);
			sumNUM = sumNUM + binVal;
		}
	hist = hist / sumNUM;
	return hist;
}
void VideoWatermark::ffmpegEncoderInit()
{
	av_register_all();
	avcodec_register_all();
	m_pRGBFrame = new AVFrame[1];//RGB帧数据赋值    
	m_pYUVFrame = new AVFrame[1];//YUV帧数据赋值    
	c = NULL;//解码器指针对象赋初值  
	cv::Size _size = getFrameSize();
	std::cout << "width" << _size.width << "height" << _size.height << std::endl;
	/*char codec[4];
	getCodec(codec);
	const struct AVCodecTag * fallback_tags[] = {
		codec_bmp_tags, // fallback for avformat < 54.1
		NULL };
	CodecID codec_id = av_codec_get_id(fallback_tags, MKTAG(codec[0], codec[1], codec[2], codec[3]));
	std::cout << "codec_id" << codec_id << codec[0] << codec[1] << codec[2] << codec[3] << std::endl;*/
	AVCodecID codec_id = AV_CODEC_ID_H264;
	pcodec = avcodec_find_encoder(AV_CODEC_ID_H264);//查找h264编码器  
	if (!pcodec)
	{
		fprintf(stderr, "h264 codec not found\n");
		exit(1);
	}
	encode_width = _size.width;
	encode_height = _size.height;
	c = avcodec_alloc_context3(pcodec);//函数用于分配一个AVCodecContext并设置默认值，如果失败返回NULL，并可用av_free()进行释放  
	if (!c) {
		fprintf(stderr, "Could not allocate video codec context\n");
		exit(1);
	}
	c->bit_rate = 10089000; //设置采样参数，即比特率  视频码率,每个视频不一样
	c->width = _size.width;//设置编码视频宽度   
	c->height = _size.height; //设置编码视频高度  
	c->time_base.den = getFrameRate();//设置帧率,num为分子和den为分母，如果是1/25则表示25帧/s  
	std::cout << "framerate" << getFrameRate() << std::endl;
	c->time_base.num = 1;
	c->gop_size = 15; //设置GOP大小,该值表示每10帧会插入一个I帧  
	c->max_b_frames = 1;//设置B帧最大数,该值表示在两个非B帧之间，所允许插入的B帧的最大帧数  
	c->pix_fmt = AV_PIX_FMT_YUV420P;//设置像素格式  
	if (codec_id == AV_CODEC_ID_H264) {
		//av_opt_set(c->priv_data, "preset", "slow", 0);
		av_opt_set(c->priv_data, "tune", "zerolatency", 0);//设置编码器的延时，解决前面的几十帧不出数据的情况  
	}
	if (avcodec_open2(c, pcodec, NULL) < 0) {
		fprintf(stderr, "Could not open codec\n");
		exit(1);
	}
	//if (avcodec_open2(c, codec, NULL) < 0)return;//打开编码器  
	nDataLen = _size.width*_size.height * 3;//计算图像rgb数据区长度  
	yuv_buff = new uint8_t[nDataLen / 2];//初始化数据区，为yuv图像帧准备填充缓存  
	rgb_buff = new uint8_t[nDataLen];//初始化数据区，为rgb图像帧准备填充缓存  
	outbuf_size = 100000;////初始化编码输出数据区  
	outbuf = new uint8_t[outbuf_size];
	scxt = sws_getContext(c->width, c->height, AV_PIX_FMT_BGR24, c->width, c->height, AV_PIX_FMT_YUV420P, SWS_POINT, NULL, NULL, NULL);//初始化格式转换函数  
	std::cout << "encoder init done!" << std::endl;
}
void VideoWatermark::ffmpegEncoderEncode(FILE *file, uint8_t *data, long fnumber)
{
	AVPacket pkt;
	memset(&pkt, 0, sizeof(AVPacket));
	av_init_packet(&pkt);
	memcpy(rgb_buff, data, nDataLen);//拷贝图像数据到rgb图像帧缓存中准备处理  
	avpicture_fill((AVPicture*)m_pRGBFrame, (uint8_t*)rgb_buff, AV_PIX_FMT_RGB24, encode_width, encode_height);//将rgb_buff填充到m_pRGBFrame  
	avpicture_fill((AVPicture*)m_pYUVFrame, (uint8_t*)yuv_buff, AV_PIX_FMT_YUV420P, encode_width, encode_height);//将yuv_buff填充到m_pYUVFrame  
	sws_scale(scxt, m_pRGBFrame->data, m_pRGBFrame->linesize, 0, c->height, m_pYUVFrame->data, m_pYUVFrame->linesize);// 将RGB转化为YUV  
	int myoutputlen = 0;
	int returnvalue = avcodec_encode_video2(c, &pkt, m_pYUVFrame, &myoutputlen);
	if (returnvalue < 0) {
		fprintf(stderr, "Error encoding frame\n");
		exit(1);
	}
	if (myoutputlen) {
		//pkt.pts = fnumber;
		fwrite(pkt.data, 1, pkt.size, file);
		av_free_packet(&pkt);
	}
	/*if (returnvalue == 0)
	{
	fwrite(pkt.data, 1, pkt.size, file);
	}
	av_free_packet(&pkt);*/
}
void VideoWatermark::ffmpegEncoderClose()
{
	delete[] m_pRGBFrame;
	delete[] m_pYUVFrame;
	delete[] rgb_buff;
	delete[] yuv_buff;
	delete[] outbuf;
	sws_freeContext(scxt);
	avcodec_close(c);//关闭编码器  
	av_free(c);
}
void VideoWatermark::extractFinalWatermark(cv::Mat &watermark, std::vector<std::vector<int>> &watermark_accumulation)
{
	for (int y = 0; y < watermark.rows; y++) 
	{
		for (int x = 0; x < watermark.cols; x++)
		{
			if (watermark.at<uchar>(y,x) > 0)
				watermark_accumulation[x][y] -= 1;
			else
				watermark_accumulation[x][y] += 4;
			if (watermark_accumulation[x][y] > 0)
				watermark.at<uchar>(y, x) = 0;
			else
				watermark.at<uchar>(y, x) = 255;
		}
	}
}
Scalar getMSSIM(const Mat& i1, const Mat& i2)
{
	const double C1 = 6.5025, C2 = 58.5225;
	/***************************** INITS **********************************/
	int d = CV_32F;
	Mat I1, I2;
	i1.convertTo(I1, d);          // cannot calculate on one byte large values
	i2.convertTo(I2, d);
	Mat I2_2 = I2.mul(I2);        // I2^2
	Mat I1_2 = I1.mul(I1);        // I1^2
	Mat I1_I2 = I1.mul(I2);       // I1 * I2
/*************************** END INITS **********************************/
	Mat mu1, mu2;   // PRELIMINARY COMPUTING
	GaussianBlur(I1, mu1, Size(11, 11), 1.5);
	GaussianBlur(I2, mu2, Size(11, 11), 1.5);
	Mat mu1_2 = mu1.mul(mu1);
	Mat mu2_2 = mu2.mul(mu2);
	Mat mu1_mu2 = mu1.mul(mu2);
	Mat sigma1_2, sigma2_2, sigma12;
	GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;
	GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
	sigma2_2 -= mu2_2;
	GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
	sigma12 -= mu1_mu2;
	///////////////////////////////// FORMULA ////////////////////////////////
	Mat t1, t2, t3;
	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))
	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))
	Mat ssim_map;
	divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;
	Scalar mssim = mean(ssim_map); // mssim = average of ssim map
	return mssim;
}
cv::Mat generateWatermark(string text) {
    cv::Mat image = Mat(WATERMARK_HEIGHT, WATERMARK_WIDTH, CV_8UC1, 255);
    transform(text.begin(), text.end(), text.begin(), toupper);
    string text1 = "",text2="";
    text1 = text.substr(0, min(6,text.size()));
    if (text.size() > 6) text2= text.substr(6);
    putText(image,
        text1,
        { 6,28 },//	left,Bottom corner of the text string in the image.
        FONT_HERSHEY_DUPLEX,//Font type  FONT_HERSHEY_SIMPLEX 	        
        1,//Font scale factor that is multiplied by the font-specific base size.
        0,//Text color.
        2,//Thickness of the lines used to draw a text.
        LINE_4,//Line type. See the line for detailAs.
        false//When true, the image data origin is at the bottom-left corner. Otherwise, it is at the top-left corner.
        );
    putText(image, text2, { 6,55 }, FONT_HERSHEY_DUPLEX, 1, 0, 2, LINE_4, false);
    //imshow("white", image);
    imwrite("watermark.bmp", image);
    return image;
}
