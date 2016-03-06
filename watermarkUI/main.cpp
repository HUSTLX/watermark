#include "ffmpeg.h"
#include "videoprocessor.h"
#include "encoder.h"
#include "sift.h"
int main()
{  
	avcodec_register_all();
	av_register_all();
	VideoProcessor processor;
	processor.setInput("E:/kankan/lang.ts");
	//processor.setInput("./res/test1.h264");
	// Get basic info about video file
	cv::Size size = processor.getFrameSize();
	std::cout << size.width << " " << size.height << std::endl;
	std::cout << processor.getFrameRate() << std::endl;
	std::cout << processor.getTotalFrameCount() << std::endl;
	std::cout << processor.getFrameNumber() << std::endl;
	std::cout << processor.getPositionMS() << std::endl;
	char codec[4];
	processor.getCodec(codec);
	const struct AVCodecTag * fallback_tags[] = {
		codec_bmp_tags, // fallback for avformat < 54.1
		NULL };
	CodecID codec_id = av_codec_get_id(fallback_tags, MKTAG(codec[0], codec[1], codec[2], codec[3]));
	std::cout << "codec_id" << codec_id << codec[0] << codec[1] << codec[2] << codec[3] << std::endl;
	processor.displayInput("Current Frame");
	processor.displayOutput("Output Frame");
	processor.setDelay(1000. / processor.getFrameRate());// Play the video at the original frame rate
	FILE *f = NULL;
	char * filename = "./res/test.h264";
	fopen_s(&f, filename, "wb");//打开文件存储编码完成数据  
	Ffmpeg_Encoder ffmpegobj;
	ffmpegobj.Ffmpeg_Encoder_Init();//初始化编码器  
	ffmpegobj.Ffmpeg_Encoder_Setpara(CODEC_ID_H264, size.width, size.height, processor.getFrameRate());//设置编码器参数 
	processor.setFrameProcessor(EmbedWatermark);
	processor.run(f, ffmpegobj);
	std::cout << processor.getFrameNumber() << std::endl;
	std::cout << processor.getPositionMS() << std::endl;
	cv::waitKey();
	return 0;
}