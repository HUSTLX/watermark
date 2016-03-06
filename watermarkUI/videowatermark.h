#ifndef VIDEOWATERMARK_H_
#define VIDEOWATERMARK_H_
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>  
#include <cv.h>
#include <highgui.h>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/imgproc/imgproc.hpp>
extern "C"
{
#include "include\libavcodec\avcodec.h"
#include "include\libavformat\avformat.h"
#include "include\libavutil\channel_layout.h"
#include "include\libavutil\common.h"
#include "include\libavutil\imgutils.h"
#include "include\libswscale\swscale.h" 
#include "include\x264.h"
#include "include\libavutil\imgutils.h"    
#include "include\libavutil\opt.h"       
#include "include\libavutil\mathematics.h"    
#include "include\libavutil\samplefmt.h" 
};


#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "postproc.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")
#include "Arnold.h"
// The frame processor interface
class FrameProcessor {

  public:
	// processing method
	virtual void embed(cv:: Mat &input, cv:: Mat &output, double&)= 0;
	virtual void detect(cv::Mat &input, cv::Mat &output, double&) = 0;
};

class VideoWatermark {

  public:
/******************************decode*******************************/
	  // the OpenCV video capture object
	  cv::VideoCapture capture;
	  // the callback function to be called 
	  // for the processing of each frame
	  void (*embed)(cv::Mat&, const cv::Mat&, double&);
	  void (*detect)(cv::Mat&, cv::Mat&, double&);
	  // the pointer to the class implementing 
	  // the FrameProcessor interface
	  FrameProcessor *frameProcessor;
	  // a bool to determine if the 
	  // process callback will be called
	  bool callIt;
	  // Input display window name
	  std::string windowNameInput;
	  // Output display window name
	  std::string windowNameOutput;
	  // delay between each frame processing
	  int delay;
	  // number of processed frames 
	  long fnumber;
	  // stop at this frame number
	  long frameToStop;
	  // to stop the processing
	  bool stop;
	  // vector of image filename to be used as input
	  std::vector<std::string> images; 
	  // image vector iterator
	  std::vector<std::string>::const_iterator itImg;
	  // the OpenCV video writer object
	  cv::VideoWriter writer;
	  // output filename
	  std::string outputFile;
	  // current index for output images
	  int currentIndex;
	  // number of digits in output image filename
	  int digits;
	  // extension of output images
	  std::string extension;
	  // to get the next frame 
	  // could be: video file; camera; vector of images
/****************************encode******************************/
	  AVFrame *m_pRGBFrame;   //帧对象  
	  AVFrame *m_pYUVFrame;   //帧对象  
	  AVCodec *pcodec;        //编码器  
	  AVCodecContext *c = NULL; //编码器数据结构对象  
	  uint8_t *yuv_buff;      //yuv图像数据区  
	  uint8_t *rgb_buff;      //rgb图像数据区  
	  SwsContext *scxt;       //图像格式转换对象  
	  uint8_t *outbuf;        //编码出来视频数据缓存  
	  int outbuf_size;        //编码输出数据去大小  
	  int nDataLen;           //rgb图像数据区长度  
	  int encode_width;              //输出视频宽度  
	  int encode_height;             //输出视频高度  
/*********************decode member function**********************/
	  bool readNextFrame(cv::Mat& frame) {

		  if (images.size()==0)
			  return capture.read(frame);
		  else {

			  if (itImg != images.end()) {

				  frame= cv::imread(*itImg);
				  itImg++;
				  return frame.data != 0;
			  }
		  }
	  }
	  // to write the output frame 
	  // could be: video file or images
	  void writeNextFrame(cv::Mat& frame) {

		  //if (extension.length()) { // then we write images
		  if(0){
			  std::stringstream ss;
		      ss << outputFile << std::setfill('0') << std::setw(digits) << currentIndex++ << extension;
			  cv::imwrite(ss.str(),frame);

		  } else { // then write video file

			  writer.write(frame);
		  }
	  }

  public:
	  // Constructor setting the default values
	  VideoWatermark() : callIt(false), delay(-1), 
		  fnumber(0), stop(false), digits(0), frameToStop(-1), 
		  embed(0), detect(0), frameProcessor(0) {}
	  // set the name of the video file
	  bool setInput(std::string filename) {
		fnumber= 0;
		// In case a resource was already 
		// associated with the VideoCapture instance
		capture.release();
		images.clear();
		// Open the video file
		return capture.open(filename);
	  }
	  // set the camera ID
	  bool setInput(int id) {
		fnumber= 0;
		// In case a resource was already 
		// associated with the VideoCapture instance
		capture.release();
		images.clear();
		// Open the video file
		return capture.open(id);
	  }
	  // set the vector of input images
	  bool setInput(const std::vector<std::string>& imgs) {
		fnumber= 0;
		// In case a resource was already 
		// associated with the VideoCapture instance
		capture.release();
		// the input will be this vector of images
		images= imgs;
		itImg= images.begin();
		return true;
	  }
	  // set the output video file
	  // by default the same parameters than input video will be used
	  bool setOutput(const std::string &filename, int codec=0, double framerate=0.0, bool isColor=true) {
		  outputFile= filename;
		  extension.clear();		  
		  if (framerate==0.0) 
			  framerate= getFrameRate(); // same as input
		  char c[4];
		  // use same codec as input
		  if (codec==0) { 
			  codec= getCodec(c);
		  }
		  // Open output video
		  return writer.open(outputFile, // filename
			  codec, // codec to be used 
			  framerate,      // frame rate of the video
			  getFrameSize(), // frame size
			  isColor);       // color video?
	  }
	  // set the output as a series of image files
	  // extension must be ".jpg", ".bmp" ...
	  bool setOutput(const std::string &filename, // filename prefix
		  const std::string &ext, // image file extension 
		  int numberOfDigits=3,   // number of digits
		  int startIndex=0) {     // start index
		  // number of digits must be positive
		  if (numberOfDigits<0)
			  return false;
		  // filenames and their common extension
		  outputFile= filename;
		  extension= ext;
		  // number of digits in the file numbering scheme
		  digits= numberOfDigits;
		  // start numbering at this index
		  currentIndex= startIndex;
		  return true;
	  }
	  // set the embed callback function that will be called for each frame
	  void setFrameEmbed(void (*frameEmbedCallback)(cv::Mat&, const cv::Mat&,double&)) {

		  // invalidate frame processor class instance
		  frameProcessor= 0;
		  // this is the frame processor function that will be called
		  embed = frameEmbedCallback;
		  callProcess();
	  }
	  // set the detect callback function that will be called for each frame
	  void setFrameDetect(void(*frameDetectCallback)(cv::Mat&, cv::Mat&, double&)) {

		  // invalidate frame processor class instance
		  frameProcessor = 0;
		  // this is the frame processor function that will be called
		  detect = frameDetectCallback;
		  callProcess();
	  }
	  // stop streaming at this frame number
	  void stopAtFrameNo(long frame) {

		  frameToStop= frame;
	  }
	  // process callback to be called
	  void callProcess() {

		  callIt= true;
	  }
	  // do not call process callback
	  void dontCallProcess() {

		  callIt= false;
	  }
	  // to display the processed frames
	  void displayInput(std::string wn) {
		  windowNameInput= wn;
		  cv::namedWindow(windowNameInput, cv::WINDOW_NORMAL);
		  cv::resizeWindow(windowNameInput, 960, 720);
	  }
	  // to display the processed frames
	  void displayOutput(std::string wn) {
		  windowNameOutput = wn;
		  cv::namedWindow(windowNameOutput, cv::WINDOW_NORMAL);
		  cv::resizeWindow(windowNameOutput, 960, 720);
	  }
	  // do not display the processed frames
	  void dontDisplay() {

		  cv::destroyWindow(windowNameInput);
		  cv::destroyWindow(windowNameOutput);
		  windowNameInput.clear();
		  windowNameOutput.clear();
	  }
	  // set a delay between each frame
	  // 0 means wait at each frame
	  // negative means no delay
	  void setDelay(int d) {
	  
		  delay= d;
	  }
	  // a count is kept of the processed frames
	  long getNumberOfProcessedFrames() {
	  
		  return fnumber;
	  }
	  // return the size of the video frame
	  cv::Size getFrameSize() {
		if (images.size()==0) {

			// get size of from the capture device
			int w= static_cast<int>(capture.get(CV_CAP_PROP_FRAME_WIDTH));
			int h= static_cast<int>(capture.get(CV_CAP_PROP_FRAME_HEIGHT));
			return cv::Size(w,h);

		} else { // if input is vector of images

			cv::Mat tmp= cv::imread(images[0]);
			if (!tmp.data) return cv::Size(0,0);
			else return tmp.size();
		}
	  }
	  // return the frame number of the next frame
	  long getFrameNumber() {
		if (images.size()==0) {
			// get info of from the capture device
	 	    long f= static_cast<long>(capture.get(CV_CAP_PROP_POS_FRAMES));
		    return f; 
		} else { // if input is vector of images
			return static_cast<long>(itImg-images.begin());
		}
	  }
	  // return the position in ms
	  double getPositionMS() {
		  // undefined for vector of images
		  if (images.size()!=0) return 0.0;
	 	  double t= capture.get(CV_CAP_PROP_POS_MSEC);
		  return t; 
	  }
	  // return the frame rate
	  double getFrameRate() {
		  // undefined for vector of images
		  if (images.size()!=0) return 0;
	 	  double r= capture.get(CV_CAP_PROP_FPS);
          if (r <= 0)
            r = 30;         
		  return r; 
	  }
	  // return the number of frames in video
	  long getTotalFrameCount() {
		  // for vector of images
		  //if (images.size()!=0) return images.size();

	 	  long t= capture.get(CV_CAP_PROP_FRAME_COUNT);
		  return t; 
	  }
	  // get the codec of input video
	  int getCodec(char codec[4]) {
		  // undefined for vector of images
		  if (images.size()!=0) return -1;
		  union {
			  int value;
			  char code[4]; } returned;
		  returned.value= static_cast<int>(capture.get(CV_CAP_PROP_FOURCC));
          if (returned.value <= 0){
            returned.value = CV_FOURCC('P','I','M','1');          
          }
		  codec[0]= returned.code[0];
		  codec[1]= returned.code[1];
		  codec[2]= returned.code[2];
		  codec[3]= returned.code[3];
		  return returned.value;
	  }	  
	  // go to this frame number
	  bool setFrameNumber(long pos) {
		  // for vector of images
		  if (images.size()!=0) {
			  // move to position in vector
			  itImg= images.begin() + pos;
			  // is it a valid position?
			  if (pos < images.size())
				  return true;
			  else
				  return false;
		  } else { // if input is a capture device
			return capture.set(CV_CAP_PROP_POS_FRAMES, pos);
		  }
	  }
	  // go to this position
	  bool setPositionMS(double pos) {
		  // not defined in vector of images
		  if (images.size()!=0) 
			  return false;
		  else 
		      return capture.set(CV_CAP_PROP_POS_MSEC, pos);
	  }
	  // go to this position expressed in fraction of total film length
	  bool setRelativePosition(double pos) {
		  // for vector of images
		  if (images.size()!=0) {
			  // move to position in vector
			  long posI= static_cast<long>(pos*images.size()+0.5);
			  itImg= images.begin() + posI;
			  // is it a valid position?
			  if (posI < images.size())
				  return true;
			  else
				  return false;
		  } else { // if input is a capture device
			  return capture.set(CV_CAP_PROP_POS_AVI_RATIO, pos);
		  }
	  }
	  // Stop the processing
	  void stopIt() {
		  stop= true;
	  }
	  // Is the process stopped?
	  bool isStopped() {
		  return stop;
	  }
	  // Is a capture device opened?
	  bool isOpened() {
		  return capture.isOpened() || !images.empty();
	  }  
/*********************encode member function**********************/
	  void ffmpegEncoderInit();//初始化   
	  void ffmpegEncoderEncode(FILE *file, uint8_t *data, long fnumber);//编码并写入数据到文件  
	  void ffmpegEncoderClose();//关闭  
/*********************other function**********************/
	  void extractFinalWatermark(cv::Mat &watermark, std::vector<std::vector<int>> &watermark_accumulation);
};
cv::Mat GetHistValue(const cv::Mat image);
cv::Scalar getMSSIM(const cv::Mat& i1, const cv::Mat& i2);
#ifndef MKTAG
#define MKTAG(a,b,c,d) (a | (b << 8) | (c << 16) | (d << 24))
#endif
// required to look up the correct codec ID depending on the FOURCC code,
// this is just a snipped from the file riff.c from ffmpeg/libavformat
typedef struct AVCodecTag {
	int id;
	unsigned int tag;
} AVCodecTag;


#if (LIBAVCODEC_VERSION_INT <= AV_VERSION_INT(54, 51, 100))
#define AV_CODEC_ID_H264 CODEC_ID_H264
#define AV_CODEC_ID_H263 CODEC_ID_H263
#define AV_CODEC_ID_H263P CODEC_ID_H263P
#define AV_CODEC_ID_H263I CODEC_ID_H263I
#define AV_CODEC_ID_H261 CODEC_ID_H261
#define AV_CODEC_ID_MPEG4 CODEC_ID_MPEG4
#define AV_CODEC_ID_MSMPEG4V3 CODEC_ID_MSMPEG4V3
#define AV_CODEC_ID_MSMPEG4V2 CODEC_ID_MSMPEG4V2
#define AV_CODEC_ID_MSMPEG4V1 CODEC_ID_MSMPEG4V1
#define AV_CODEC_ID_WMV1 CODEC_ID_WMV1
#define AV_CODEC_ID_WMV2 CODEC_ID_WMV1
#define AV_CODEC_ID_DVVIDEO CODEC_ID_DVVIDEO
#define AV_CODEC_ID_MPEG1VIDEO CODEC_ID_MPEG1VIDEO
#define AV_CODEC_ID_MPEG2VIDEO CODEC_ID_MPEG2VIDEO
#define AV_CODEC_ID_MJPEG CODEC_ID_MJPEG
#define AV_CODEC_ID_LJPEG CODEC_ID_LJPEG
#define AV_CODEC_ID_HUFFYUV CODEC_ID_HUFFYUV
#define AV_CODEC_ID_FFVHUFF CODEC_ID_FFVHUFF
#define AV_CODEC_ID_CYUV CODEC_ID_CYUV
#define AV_CODEC_ID_RAWVIDEO CODEC_ID_RAWVIDEO
#define AV_CODEC_ID_INDEO3 CODEC_ID_INDEO3
#define AV_CODEC_ID_VP3 CODEC_ID_VP3
#define AV_CODEC_ID_ASV1 CODEC_ID_ASV1
#define AV_CODEC_ID_ASV2 CODEC_ID_ASV2
#define AV_CODEC_ID_VCR1 CODEC_ID_VCR1
#define AV_CODEC_ID_FFV1 CODEC_ID_FFV1
#define AV_CODEC_ID_XAN_WC4 CODEC_ID_XAN_WC4
#define AV_CODEC_ID_MSRLE CODEC_ID_MSRLE
#define AV_CODEC_ID_MSVIDEO1 CODEC_ID_MSVIDEO1
#define AV_CODEC_ID_CINEPAK CODEC_ID_CINEPAK
#define AV_CODEC_ID_TRUEMOTION1 CODEC_ID_TRUEMOTION1
#define AV_CODEC_ID_MSZH CODEC_ID_MSZH
#define AV_CODEC_ID_ZLIB CODEC_ID_ZLIB
#define AV_CODEC_ID_SNOW CODEC_ID_SNOW
#define AV_CODEC_ID_4XM CODEC_ID_4XM
#define AV_CODEC_ID_FLV1 CODEC_ID_FLV1
#define AV_CODEC_ID_SVQ1 CODEC_ID_SVQ1
#define AV_CODEC_ID_TSCC CODEC_ID_TSCC
#define AV_CODEC_ID_ULTI CODEC_ID_ULTI
#define AV_CODEC_ID_VIXL CODEC_ID_VIXL
#define AV_CODEC_ID_QPEG CODEC_ID_QPEG
#define AV_CODEC_ID_WMV3 CODEC_ID_WMV3
#define AV_CODEC_ID_LOCO CODEC_ID_LOCO
#define AV_CODEC_ID_THEORA CODEC_ID_THEORA
#define AV_CODEC_ID_WNV1 CODEC_ID_WNV1
#define AV_CODEC_ID_AASC CODEC_ID_AASC
#define AV_CODEC_ID_INDEO2 CODEC_ID_INDEO2
#define AV_CODEC_ID_FRAPS CODEC_ID_FRAPS
#define AV_CODEC_ID_TRUEMOTION2 CODEC_ID_TRUEMOTION2
#define AV_CODEC_ID_FLASHSV CODEC_ID_FLASHSV
#define AV_CODEC_ID_JPEGLS CODEC_ID_JPEGLS
#define AV_CODEC_ID_VC1 CODEC_ID_VC1
#define AV_CODEC_ID_CSCD CODEC_ID_CSCD
#define AV_CODEC_ID_ZMBV CODEC_ID_ZMBV
#define AV_CODEC_ID_KMVC CODEC_ID_KMVC
#define AV_CODEC_ID_VP5 CODEC_ID_VP5
#define AV_CODEC_ID_VP6 CODEC_ID_VP6
#define AV_CODEC_ID_VP6F CODEC_ID_VP6F
#define AV_CODEC_ID_JPEG2000 CODEC_ID_JPEG2000
#define AV_CODEC_ID_VMNC CODEC_ID_VMNC
#define AV_CODEC_ID_TARGA CODEC_ID_TARGA
#define AV_CODEC_ID_NONE CODEC_ID_NONE
#endif

const AVCodecTag codec_bmp_tags[] = {
	{ AV_CODEC_ID_H264, MKTAG('H', '2', '6', '4') },
	{ AV_CODEC_ID_H264, MKTAG('h', '2', '6', '4') },
	{ AV_CODEC_ID_H264, MKTAG('X', '2', '6', '4') },
	{ AV_CODEC_ID_H264, MKTAG('x', '2', '6', '4') },
	{ AV_CODEC_ID_H264, MKTAG('a', 'v', 'c', '1') },
	{ AV_CODEC_ID_H264, MKTAG('V', 'S', 'S', 'H') },

	{ AV_CODEC_ID_H263, MKTAG('H', '2', '6', '3') },
	{ AV_CODEC_ID_H263P, MKTAG('H', '2', '6', '3') },
	{ AV_CODEC_ID_H263I, MKTAG('I', '2', '6', '3') }, // intel h263 
	{ AV_CODEC_ID_H261, MKTAG('H', '2', '6', '1') },

	// added based on MPlayer 
	{ AV_CODEC_ID_H263P, MKTAG('U', '2', '6', '3') },
	{ AV_CODEC_ID_H263P, MKTAG('v', 'i', 'v', '1') },

	{ AV_CODEC_ID_MPEG4, MKTAG('F', 'M', 'P', '4') },
	{ AV_CODEC_ID_MPEG4, MKTAG('D', 'I', 'V', 'X') },
	{ AV_CODEC_ID_MPEG4, MKTAG('D', 'X', '5', '0') },
	{ AV_CODEC_ID_MPEG4, MKTAG('X', 'V', 'I', 'D') },
	{ AV_CODEC_ID_MPEG4, MKTAG('M', 'P', '4', 'S') },
	{ AV_CODEC_ID_MPEG4, MKTAG('M', '4', 'S', '2') },
	{ AV_CODEC_ID_MPEG4, MKTAG(0x04, 0, 0, 0) }, // some broken avi use this 

												 // added based on MPlayer 
	{ AV_CODEC_ID_MPEG4, MKTAG('D', 'I', 'V', '1') },
	{ AV_CODEC_ID_MPEG4, MKTAG('B', 'L', 'Z', '0') },
	{ AV_CODEC_ID_MPEG4, MKTAG('m', 'p', '4', 'v') },
	{ AV_CODEC_ID_MPEG4, MKTAG('U', 'M', 'P', '4') },
	{ AV_CODEC_ID_MPEG4, MKTAG('W', 'V', '1', 'F') },
	{ AV_CODEC_ID_MPEG4, MKTAG('S', 'E', 'D', 'G') },

	{ AV_CODEC_ID_MPEG4, MKTAG('R', 'M', 'P', '4') },

	{ AV_CODEC_ID_MSMPEG4V3, MKTAG('D', 'I', 'V', '3') }, // default signature when using MSMPEG4
	{ AV_CODEC_ID_MSMPEG4V3, MKTAG('M', 'P', '4', '3') },

	// added based on MPlayer 
	{ AV_CODEC_ID_MSMPEG4V3, MKTAG('M', 'P', 'G', '3') },
	{ AV_CODEC_ID_MSMPEG4V3, MKTAG('D', 'I', 'V', '5') },
	{ AV_CODEC_ID_MSMPEG4V3, MKTAG('D', 'I', 'V', '6') },
	{ AV_CODEC_ID_MSMPEG4V3, MKTAG('D', 'I', 'V', '4') },
	{ AV_CODEC_ID_MSMPEG4V3, MKTAG('A', 'P', '4', '1') },
	{ AV_CODEC_ID_MSMPEG4V3, MKTAG('C', 'O', 'L', '1') },
	{ AV_CODEC_ID_MSMPEG4V3, MKTAG('C', 'O', 'L', '0') },

	{ AV_CODEC_ID_MSMPEG4V2, MKTAG('M', 'P', '4', '2') },

	//added based on MPlayer 
	{ AV_CODEC_ID_MSMPEG4V2, MKTAG('D', 'I', 'V', '2') },

	{ AV_CODEC_ID_MSMPEG4V1, MKTAG('M', 'P', 'G', '4') },

	{ AV_CODEC_ID_WMV1, MKTAG('W', 'M', 'V', '1') },

	// added based on MPlayer 
	{ AV_CODEC_ID_WMV2, MKTAG('W', 'M', 'V', '2') },
	{ AV_CODEC_ID_DVVIDEO, MKTAG('d', 'v', 's', 'd') },
	{ AV_CODEC_ID_DVVIDEO, MKTAG('d', 'v', 'h', 'd') },
	{ AV_CODEC_ID_DVVIDEO, MKTAG('d', 'v', 's', 'l') },
	{ AV_CODEC_ID_DVVIDEO, MKTAG('d', 'v', '2', '5') },
	{ AV_CODEC_ID_MPEG1VIDEO, MKTAG('m', 'p', 'g', '1') },
	{ AV_CODEC_ID_MPEG1VIDEO, MKTAG('m', 'p', 'g', '2') },
	{ AV_CODEC_ID_MPEG2VIDEO, MKTAG('m', 'p', 'g', '2') },
	{ AV_CODEC_ID_MPEG2VIDEO, MKTAG('M', 'P', 'E', 'G') },
	{ AV_CODEC_ID_MPEG1VIDEO, MKTAG('P', 'I', 'M', '1') },
	{ AV_CODEC_ID_MPEG1VIDEO, MKTAG('V', 'C', 'R', '2') },
	{ AV_CODEC_ID_MPEG1VIDEO, 0x10000001 },
	{ AV_CODEC_ID_MPEG2VIDEO, 0x10000002 },
	{ AV_CODEC_ID_MPEG2VIDEO, MKTAG('D', 'V', 'R', ' ') },
	{ AV_CODEC_ID_MPEG2VIDEO, MKTAG('M', 'M', 'E', 'S') },
	{ AV_CODEC_ID_MJPEG, MKTAG('M', 'J', 'P', 'G') },
	{ AV_CODEC_ID_MJPEG, MKTAG('L', 'J', 'P', 'G') },
	{ AV_CODEC_ID_LJPEG, MKTAG('L', 'J', 'P', 'G') },
	{ AV_CODEC_ID_MJPEG, MKTAG('J', 'P', 'G', 'L') }, // Pegasus lossless JPEG //
	{ AV_CODEC_ID_MJPEG, MKTAG('M', 'J', 'L', 'S') }, // JPEG-LS custom FOURCC for avi - decoder //
	{ AV_CODEC_ID_MJPEG, MKTAG('j', 'p', 'e', 'g') },
	{ AV_CODEC_ID_MJPEG, MKTAG('I', 'J', 'P', 'G') },
	{ AV_CODEC_ID_MJPEG, MKTAG('A', 'V', 'R', 'n') },
	{ AV_CODEC_ID_HUFFYUV, MKTAG('H', 'F', 'Y', 'U') },
	{ AV_CODEC_ID_FFVHUFF, MKTAG('F', 'F', 'V', 'H') },
	{ AV_CODEC_ID_CYUV, MKTAG('C', 'Y', 'U', 'V') },
	{ AV_CODEC_ID_RAWVIDEO, 0 },
	{ AV_CODEC_ID_RAWVIDEO, MKTAG('I', '4', '2', '0') },
	{ AV_CODEC_ID_RAWVIDEO, MKTAG('Y', 'U', 'Y', '2') },
	{ AV_CODEC_ID_RAWVIDEO, MKTAG('Y', '4', '2', '2') },
	{ AV_CODEC_ID_RAWVIDEO, MKTAG('Y', 'V', '1', '2') },
	{ AV_CODEC_ID_RAWVIDEO, MKTAG('U', 'Y', 'V', 'Y') },
	{ AV_CODEC_ID_RAWVIDEO, MKTAG('I', 'Y', 'U', 'V') },
	{ AV_CODEC_ID_RAWVIDEO, MKTAG('Y', '8', '0', '0') },
	{ AV_CODEC_ID_RAWVIDEO, MKTAG('H', 'D', 'Y', 'C') },
	{ AV_CODEC_ID_INDEO3, MKTAG('I', 'V', '3', '1') },
	{ AV_CODEC_ID_INDEO3, MKTAG('I', 'V', '3', '2') },
	{ AV_CODEC_ID_VP3, MKTAG('V', 'P', '3', '1') },
	{ AV_CODEC_ID_VP3, MKTAG('V', 'P', '3', '0') },
	{ AV_CODEC_ID_ASV1, MKTAG('A', 'S', 'V', '1') },
	{ AV_CODEC_ID_ASV2, MKTAG('A', 'S', 'V', '2') },
	{ AV_CODEC_ID_VCR1, MKTAG('V', 'C', 'R', '1') },
	{ AV_CODEC_ID_FFV1, MKTAG('F', 'F', 'V', '1') },
	{ AV_CODEC_ID_XAN_WC4, MKTAG('X', 'x', 'a', 'n') },
	{ AV_CODEC_ID_MSRLE, MKTAG('m', 'r', 'l', 'e') },
	{ AV_CODEC_ID_MSRLE, MKTAG(0x1, 0x0, 0x0, 0x0) },
	{ AV_CODEC_ID_MSVIDEO1, MKTAG('M', 'S', 'V', 'C') },
	{ AV_CODEC_ID_MSVIDEO1, MKTAG('m', 's', 'v', 'c') },
	{ AV_CODEC_ID_MSVIDEO1, MKTAG('C', 'R', 'A', 'M') },
	{ AV_CODEC_ID_MSVIDEO1, MKTAG('c', 'r', 'a', 'm') },
	{ AV_CODEC_ID_MSVIDEO1, MKTAG('W', 'H', 'A', 'M') },
	{ AV_CODEC_ID_MSVIDEO1, MKTAG('w', 'h', 'a', 'm') },
	{ AV_CODEC_ID_CINEPAK, MKTAG('c', 'v', 'i', 'd') },
	{ AV_CODEC_ID_TRUEMOTION1, MKTAG('D', 'U', 'C', 'K') },
	{ AV_CODEC_ID_MSZH, MKTAG('M', 'S', 'Z', 'H') },
	{ AV_CODEC_ID_ZLIB, MKTAG('Z', 'L', 'I', 'B') },
	{ AV_CODEC_ID_4XM, MKTAG('4', 'X', 'M', 'V') },
	{ AV_CODEC_ID_FLV1, MKTAG('F', 'L', 'V', '1') },
	{ AV_CODEC_ID_SVQ1, MKTAG('s', 'v', 'q', '1') },
	{ AV_CODEC_ID_TSCC, MKTAG('t', 's', 'c', 'c') },
	{ AV_CODEC_ID_ULTI, MKTAG('U', 'L', 'T', 'I') },
	{ AV_CODEC_ID_VIXL, MKTAG('V', 'I', 'X', 'L') },
	{ AV_CODEC_ID_QPEG, MKTAG('Q', 'P', 'E', 'G') },
	{ AV_CODEC_ID_QPEG, MKTAG('Q', '1', '.', '0') },
	{ AV_CODEC_ID_QPEG, MKTAG('Q', '1', '.', '1') },
	{ AV_CODEC_ID_WMV3, MKTAG('W', 'M', 'V', '3') },
	{ AV_CODEC_ID_LOCO, MKTAG('L', 'O', 'C', 'O') },
	{ AV_CODEC_ID_THEORA, MKTAG('t', 'h', 'e', 'o') },
};
#endif