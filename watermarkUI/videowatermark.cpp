#include "videowatermark.h"
#include "MyForm.h"
using namespace cv;

Mat GetHistValue(const Mat image)
{
	//��ͼƬת����HSV�ռ�
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
	// we compute the histogram from the 0-th and 1-st channels��0��H;1:S
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
	m_pRGBFrame = new AVFrame[1];//RGB֡���ݸ�ֵ    
	m_pYUVFrame = new AVFrame[1];//YUV֡���ݸ�ֵ    
	c = NULL;//������ָ����󸳳�ֵ  
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
	pcodec = avcodec_find_encoder(AV_CODEC_ID_H264);//����h264������  
	if (!pcodec)
	{
		fprintf(stderr, "h264 codec not found\n");
		exit(1);
	}
	encode_width = _size.width;
	encode_height = _size.height;
	c = avcodec_alloc_context3(pcodec);//�������ڷ���һ��AVCodecContext������Ĭ��ֵ�����ʧ�ܷ���NULL��������av_free()�����ͷ�  
	if (!c) {
		fprintf(stderr, "Could not allocate video codec context\n");
		exit(1);
	}
	c->bit_rate = 10089000; //���ò�����������������  ��Ƶ����,ÿ����Ƶ��һ��
	c->width = _size.width;//���ñ�����Ƶ���   
	c->height = _size.height; //���ñ�����Ƶ�߶�  
	c->time_base.den = getFrameRate();//����֡��,numΪ���Ӻ�denΪ��ĸ�������1/25���ʾ25֡/s  
	std::cout << "framerate" << getFrameRate() << std::endl;
	c->time_base.num = 1;
	c->gop_size = 15; //����GOP��С,��ֵ��ʾÿ10֡�����һ��I֡  
	c->max_b_frames = 1;//����B֡�����,��ֵ��ʾ��������B֮֡�䣬����������B֡�����֡��  
	c->pix_fmt = AV_PIX_FMT_YUV420P;//�������ظ�ʽ  
	if (codec_id == AV_CODEC_ID_H264) {
		//av_opt_set(c->priv_data, "preset", "slow", 0);
		av_opt_set(c->priv_data, "tune", "zerolatency", 0);//���ñ���������ʱ�����ǰ��ļ�ʮ֡�������ݵ����  
	}
	if (avcodec_open2(c, pcodec, NULL) < 0) {
		fprintf(stderr, "Could not open codec\n");
		exit(1);
	}
	//if (avcodec_open2(c, codec, NULL) < 0)return;//�򿪱�����  
	nDataLen = _size.width*_size.height * 3;//����ͼ��rgb����������  
	yuv_buff = new uint8_t[nDataLen / 2];//��ʼ����������Ϊyuvͼ��֡׼����仺��  
	rgb_buff = new uint8_t[nDataLen];//��ʼ����������Ϊrgbͼ��֡׼����仺��  
	outbuf_size = 100000;////��ʼ���������������  
	outbuf = new uint8_t[outbuf_size];
	scxt = sws_getContext(c->width, c->height, AV_PIX_FMT_BGR24, c->width, c->height, AV_PIX_FMT_YUV420P, SWS_POINT, NULL, NULL, NULL);//��ʼ����ʽת������  
	std::cout << "encoder init done!" << std::endl;
}
void VideoWatermark::ffmpegEncoderEncode(FILE *file, uint8_t *data, long fnumber)
{
	AVPacket pkt;
	memset(&pkt, 0, sizeof(AVPacket));
	av_init_packet(&pkt);
	memcpy(rgb_buff, data, nDataLen);//����ͼ�����ݵ�rgbͼ��֡������׼������  
	avpicture_fill((AVPicture*)m_pRGBFrame, (uint8_t*)rgb_buff, AV_PIX_FMT_RGB24, encode_width, encode_height);//��rgb_buff��䵽m_pRGBFrame  
	avpicture_fill((AVPicture*)m_pYUVFrame, (uint8_t*)yuv_buff, AV_PIX_FMT_YUV420P, encode_width, encode_height);//��yuv_buff��䵽m_pYUVFrame  
	sws_scale(scxt, m_pRGBFrame->data, m_pRGBFrame->linesize, 0, c->height, m_pYUVFrame->data, m_pYUVFrame->linesize);// ��RGBת��ΪYUV  
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
	avcodec_close(c);//�رձ�����  
	av_free(c);
}
void VideoWatermark::extractFinalWatermark(cv::Mat &watermark, std::vector<std::vector<int>> &watermark_accumulation)
{
	for (int y = 0; y != watermark.rows; y++) 
	{
		for (int x = 0; x != watermark.cols; x++)
		{
			if (watermark.at<uchar>(x, y) > 0)
				watermark_accumulation[x][y] -= 1;
			else
				watermark_accumulation[x][y] += 4;
			if (watermark_accumulation[x][y] > 0)
				watermark.at<uchar>(x, y) = 0;
			else
				watermark.at<uchar>(x, y) = 255;
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

