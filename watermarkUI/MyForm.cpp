#include "MyForm.h"
[watermarkUI::STAThread]
int main()
{    
	watermarkUI::Application::EnableVisualStyles();//启用可视化界面，不然会很难看    
	watermarkUI::Application::Run(gcnew watermarkUI::MyForm());//创建一个窗体，并进入消息循环   
	//cv::Mat image = cv::imread("./res/lena.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	//cv::imshow("image", image);
	//cv::Mat output;
	//DctEmbedWatermark(image, output,1);
	//cvWaitKey();
	return 0;
}
Void watermarkUI::MyForm::embed_start(VideoWatermark& vedio) {
	std::string inputfile = static_cast<const char*>(Marshal::StringToHGlobalAnsi(inputfiletext->Text).ToPointer());
	std::string outputfile = static_cast<const char*>(Marshal::StringToHGlobalAnsi(outputfiletext->Text).ToPointer());
	cv::Mat watermark_img = cv::imread(static_cast<const char*>(Marshal::StringToHGlobalAnsi(watermark_pic->ImageLocation).ToPointer()),0);
	cout << "watermark_width" << watermark_img.cols << "watermark_height" << watermark_img.rows << endl;
	watermarkUI::MyForm2^ progressForm = gcnew watermarkUI::MyForm2();
	progressForm->Show();
	threshold(watermark_img, watermark_img, 240, 255, CV_THRESH_BINARY_INV);
	Arnold(watermark_img, 5);
	//std::cout << inputfile << std::endl;
	FILE *f = NULL;
	const char * filename = outputfile.c_str();
	fopen_s(&f, filename, "wb");//打开文件存储编码完成数据  
	vedio.setInput(inputfile);
	//vedio.setInput("./res/test1.h264");
	// Get basic info about video file
	long tatal_frame = vedio.getTotalFrameCount();
	vedio.displayInput("Current Frame");
	vedio.displayOutput("watermark Frame");
	vedio.setDelay(1000. / vedio.getFrameRate());// Play the video at the original frame rate
	vedio.ffmpegEncoderInit();//初始化编码器  
	cv::Mat output_frame;
	// current frame
	cv::Mat cur_frame;
	// previous frame
	cv::Mat pre_frame;
	// next frame
	cv::Mat next_frame;
	if (!vedio.isOpened())
		return;
	vedio.stop = false;
	vedio.readNextFrame(pre_frame); //read the first frame
	IplImage* img = &IplImage(pre_frame);
	vedio.ffmpegEncoderEncode(f, (uchar*)img->imageData, 1);//编码第一帧
	vedio.readNextFrame(cur_frame); //read the first frame
	vedio.fnumber = 1;
	cv::Mat pre_hist = GetHistValue(pre_frame);
	cv::Mat cur_hist = GetHistValue(cur_frame);
	cv::Mat pre_histDiff = abs(pre_hist - cur_hist);//计算帧之间的直方图距离
	cv::Scalar pre_shotDiff = sum(pre_histDiff);
	while (!vedio.isStopped())
	{
		// read next frame if any
		if (!vedio.readNextFrame(next_frame))
			break;
		cv::Mat next_hist = GetHistValue(next_frame);
		cv::Mat next_histDiff = abs(next_hist - cur_hist);//计算帧之间的直方图距离
		cv::Scalar next_shotDiff = sum(next_histDiff);
		
		// calling the process function or method
		if ((next_shotDiff.val[0] - pre_shotDiff.val[0])>0.6)
		{
			double psnr = 0;
			cur_frame = pre_frame.clone();
			progressForm->progressBar1->Value = double(vedio.fnumber) * 100 / tatal_frame;
			progressForm->progresslabel->Text = (double(vedio.fnumber) * 100 / tatal_frame).ToString("0.00")+"%";
			std::cout << vedio.fnumber << "帧" << next_shotDiff.val[0] << "前一帧" << pre_shotDiff.val[0] << std::endl;
			char name[100];
			//sprintf(name, "./res/photo/%dnext.jpg", vedio.fnumber);
			//imwrite(name, next_frame);
			sprintf(name, "./res/photo/%dcur.jpg", vedio.fnumber);
			imwrite(name, cur_frame);
			vedio.embed(cur_frame, watermark_img,psnr);
			PSNR_text->ReadOnly = true;
			sprintf_s(name, "%f", psnr);
			PSNR_text->Text= System::Runtime::InteropServices::Marshal::PtrToStringAnsi((IntPtr)name);
			sprintf(name, "./res/photo/%dembed.jpg", vedio.fnumber);
			imwrite(name, cur_frame);
		}
		// increment frame number
		// display input frame
		if (vedio.windowNameInput.length() != 0)
			cv::imshow(vedio.windowNameInput, next_frame);
		if (vedio.windowNameOutput.length() != 0) {
			cv::imshow(vedio.windowNameOutput, cur_frame);
		}
		vedio.fnumber++;
		IplImage* img = &IplImage(cur_frame);
		vedio.ffmpegEncoderEncode(f, (uchar*)img->imageData, vedio.fnumber);//编码  																  
		pre_shotDiff = next_shotDiff;
		cur_hist = next_hist;
		pre_frame = cur_frame.clone();
		cur_frame = next_frame.clone();
		// introduce a delay
		if (vedio.delay >= 0 && cv::waitKey(vedio.delay) >= 0)
			vedio.stopIt();

		// check if we should stop
		if (vedio.frameToStop >= 0 && vedio.getFrameNumber() == vedio.frameToStop || progressForm->process_stop)
		{
			vedio.stopIt();
			progressForm->Close();
			vedio.dontDisplay();
		}
	}
	uint8_t endcode[] = { 0, 0, 1, 0xb7 };
	fwrite(endcode, 1, sizeof(endcode), f); //add sequence end code to have a real mpeg file 
	fclose(f);
	vedio.ffmpegEncoderClose();
	progressForm->Close();
	std::cout << vedio.getFrameNumber() << std::endl;
	std::cout << vedio.getPositionMS() << std::endl;
}
Void watermarkUI::MyForm::detect_start(VideoWatermark& vedio) {
	std::string inputfile = static_cast<const char*>(Marshal::StringToHGlobalAnsi(detectfiletext->Text).ToPointer());
	watermarkUI::MyForm2^ progressForm = gcnew watermarkUI::MyForm2();
	progressForm->Show();
	std::ofstream outfile;//write data for debug
	outfile.open("./res/nc.txt"); //kp.txt save keypoints
	//std::cout << inputfile << std::endl;
	vedio.setInput(inputfile);
	//vedio.setInput("./res/test1.h264");
	long tatal_frame = vedio.getTotalFrameCount();
	vedio.displayInput("Current Frame");
	vedio.setDelay(1000. / vedio.getFrameRate());// Play the video at the original frame rate
	vector<int> final_watermark_row(64, 0);
	vector<vector<int>> watermark_accumulation(64, final_watermark_row);
	cv::Mat watermark_img = cv::imread(static_cast<const char*>(Marshal::StringToHGlobalAnsi(embeddedwatermark_text->Text).ToPointer()),0);
	threshold(watermark_img, watermark_img, 240, 255, CV_THRESH_BINARY);
	Arnold(watermark_img, 5);
	cv::Mat watermark;
	cv::Mat output_frame;
	// current frame
	cv::Mat cur_frame;
	// previous frame
	cv::Mat pre_frame;
	// next frame
	cv::Mat next_frame;
	if (!vedio.isOpened())
		return;
	vedio.stop = false;
	vedio.readNextFrame(pre_frame); //read the first frame
	IplImage* img = &IplImage(pre_frame);
	vedio.readNextFrame(cur_frame); //read the first frame
	vedio.fnumber = 1;
	cv::Mat pre_hist = GetHistValue(pre_frame);
	cv::Mat cur_hist = GetHistValue(cur_frame);
	cv::Mat pre_histDiff = abs(pre_hist - cur_hist);//计算帧之间的直方图距离
	cv::Scalar pre_shotDiff = sum(pre_histDiff);
	while (!vedio.isStopped())
	{
		// read next frame if any
		if (!vedio.readNextFrame(next_frame))
			break;
		cv::Mat next_hist = GetHistValue(next_frame);
		cv::Mat next_histDiff = abs(next_hist - cur_hist);//计算帧之间的直方图距离
		cv::Scalar next_shotDiff = sum(next_histDiff);
		// display input frame
		if (vedio.windowNameInput.length() != 0)
			cv::imshow(vedio.windowNameInput, next_frame);
		// calling the process function or method
		if ((next_shotDiff.val[0] - pre_shotDiff.val[0])>0.6)
		{
			double nc = 0;
			absdiff(pre_frame, cur_frame, output_frame);
			//cur_frame = pre_frame.clone();
			progressForm->progressBar1->Value = double(vedio.fnumber) * 100 / tatal_frame;
			progressForm->progresslabel->Text = (double(vedio.fnumber) * 100 / tatal_frame).ToString("0.00") + "%";
			std::cout << vedio.fnumber << "帧" << next_shotDiff.val[0] << "前一帧" << pre_shotDiff.val[0] << std::endl;
			char name[100];
			//sprintf(name, "./res/photo/%dnext.jpg", vedio.fnumber);
			//imwrite(name, next_frame);
			//sprintf(name, "./res/photo/%dcur.jpg", vedio.fnumber);
			//imwrite(name, cur_frame);
			watermark = watermark_img.clone();
			vedio.detect(output_frame,watermark,nc);
			cv::Scalar val = getMSSIM(watermark, watermark_img);
			cout << val[0] <<"\t"<< val[1] << "\t" << val[2] << "\t" << val[3] << endl;
			Reverse_Arnold(watermark, 5);
			outfile << vedio.fnumber << "\t" << "frame" << "\t" << nc << std::endl;
			nc_text->ReadOnly = true;
			sprintf_s(name, "%.4f", val[0]);
			nc_text->Text = System::Runtime::InteropServices::Marshal::PtrToStringAnsi((IntPtr)name);
			sprintf(name, "./res/detect/%dd%.4f.jpg", vedio.fnumber,nc);
			imwrite(name, watermark);
			detect_watermark->Image = gcnew System::Drawing::Bitmap(watermark.cols, watermark.rows, watermark.step, System::Drawing::Imaging::PixelFormat::Format8bppIndexed, (System::IntPtr)watermark.ptr());
			detect_watermark->Refresh();
			vedio.extractFinalWatermark(watermark, watermark_accumulation);
			final_watermark->Image = gcnew System::Drawing::Bitmap(watermark.cols, watermark.rows, watermark.step, System::Drawing::Imaging::PixelFormat::Format8bppIndexed, (System::IntPtr)watermark.ptr());
			final_watermark->Refresh();
		}
		// increment frame number
		vedio.fnumber++;
		pre_shotDiff = next_shotDiff;
		cur_hist = next_hist;
		pre_frame = cur_frame.clone();
		cur_frame = next_frame.clone();
		// introduce a delay
		if (vedio.delay >= 0 && cv::waitKey(vedio.delay) >= 0)
			vedio.stopIt();
		// check if we should stop
		if (vedio.frameToStop >= 0 && vedio.getFrameNumber() == vedio.frameToStop || progressForm->process_stop)
		{
			vedio.stopIt();
			progressForm->Close();
			vedio.dontDisplay();
			outfile.close();
		}
	}
	outfile.close();
	progressForm->Close();
	std::cout << vedio.getFrameNumber() << std::endl;
	std::cout << vedio.getPositionMS() << std::endl;
}
Void watermarkUI::MyForm::arnoldstart_Click(System::Object^  sender, System::EventArgs^  e) {
	VideoWatermark vedio;
	vedio.setFrameEmbed(ArnoldEmbedWatermark);
	embed_start(vedio);
}
Void watermarkUI::MyForm::inputbutton_Click(System::Object^  sender, System::EventArgs^  e) {
	openFileDialog1->FileName = "";
	if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		inputfiletext->Text = openFileDialog1->FileName;
	}
}
Void watermarkUI::MyForm::outputbutton_Click(System::Object^  sender, System::EventArgs^  e) {
	//设置文件类型 
	saveFileDialog1->Title = "请选择文件夹";
	saveFileDialog1->Filter = "视频文件(*.h264)|*.h264|所有文件(*.*)|*.*";
	saveFileDialog1->FilterIndex = 1;
	saveFileDialog1->FileName = "test.h264";
	//保存对话框是否记忆上次打开的目录
	saveFileDialog1->RestoreDirectory = true;
	//按下确定选择的按钮
	if (saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		//获得文件路径
		outputfiletext->Text = saveFileDialog1->FileName;
	}																			   ////fs输出带文字或图片的文件，就看需求了 
}
Void watermarkUI::MyForm::loadimage_Click(System::Object^  sender, System::EventArgs^  e) {
	openFileDialog2->FileName = "";
	if (openFileDialog2->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		watermark_pic->ImageLocation = openFileDialog2->FileName;

	}
}
Void watermarkUI::MyForm::detectfilebutton_Click(System::Object^  sender, System::EventArgs^  e) {
	openFileDialog3->FileName = "";
	if (openFileDialog3->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		detectfiletext->Text = openFileDialog3->FileName;
	}
}
Void watermarkUI::MyForm::detect_arnoldbutton_Click(System::Object^  sender, System::EventArgs^  e) {
	VideoWatermark vedio;
	vedio.setFrameDetect(ArnoldDetectWatermark);
	detect_start(vedio);
}
Void watermarkUI::MyForm::embeddedwatermark_button_Click(System::Object^  sender, System::EventArgs^  e) {
	openFileDialog4->FileName = "";
	if (openFileDialog4->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		embeddedwatermark_text->Text = openFileDialog4->FileName;
	}
}
