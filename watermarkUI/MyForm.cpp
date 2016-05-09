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
	//Arnold(watermark_img, 5);
	//std::cout << inputfile << std::endl;
	FILE *f = NULL;
	//const char * filename = outputfile.c_str();
    const char * filename = "temp.h264";
	fopen_s(&f, filename, "wb");//打开文件存储编码完成数据  
	vedio.setInput(inputfile);
	//vedio.setInput("./res/test1.h264");
	// Get basic info about video file
	long tatal_frame = vedio.getTotalFrameCount();
    int frame_rate = vedio.getFrameRate();
	vedio.displayInput("Current Frame");
	vedio.displayOutput("watermark Frame");
	vedio.setDelay(1000. / vedio.getFrameRate());// Play the video at the original frame rate
	vedio.ffmpegEncoderInit();//初始化编码器  
	cv::Mat output_frame;
    // previous first frame
    cv::Mat pre_first;	
    // previous second frame
    cv::Mat pre_second;
    // previous frame
    cv::Mat pre_frame;
	// current frame
	cv::Mat cur_frame;
	// next frame
	cv::Mat next_frame;
    char name[100];
	if (!vedio.isOpened())
		return;
	vedio.stop = false;
	vedio.readNextFrame(pre_first); //read the first frame
    vedio.readNextFrame(pre_second); //read the second frame
    vedio.readNextFrame(pre_frame); //read the third frame
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
		if (next_shotDiff.val[0] - pre_shotDiff.val[0]>0.6)
		{
            //interval = 20;
            cout << next_shotDiff.val[0] - pre_shotDiff.val[0] << endl;
			double psnr = 0;
			cur_frame = pre_frame.clone();
			progressForm->progressBar1->Value = double(vedio.fnumber) * 100 / tatal_frame;
			progressForm->progresslabel->Text = (double(vedio.fnumber) * 100 / tatal_frame).ToString("0.00")+"%";
			std::cout << vedio.fnumber << "帧" << next_shotDiff.val[0] << "前一帧" << pre_shotDiff.val[0] << std::endl;
			//sprintf(name, "./res/photo/%dnext.jpg", vedio.fnumber);
			//imwrite(name, next_frame);
			sprintf(name, "./res/photo/%dcur.bmp", vedio.fnumber);
			imwrite(name, cur_frame);
			vedio.embed(cur_frame, watermark_img,psnr);
			PSNR_text->ReadOnly = true;
			sprintf_s(name, "%f", psnr);
			PSNR_text->Text= System::Runtime::InteropServices::Marshal::PtrToStringAnsi((IntPtr)name);
			sprintf(name, "./res/photo/%dembed.bmp", vedio.fnumber);
			imwrite(name, cur_frame);
		}
        //else interval--;
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
    if (vedio.getFrameNumber() == vedio.frameToStop) {
        string temp = "ffmpeg -i " + inputfile + " -vn -acodec pcm_s16le temp.wav";
        system(temp.c_str());    //
        temp = "ffmpeg -i temp.h264 -i temp.wav -vcodec copy -acodec copy " + outputfile;
        system(temp.c_str());
        //system("del temp.h264");
        system("del temp.wav");
    }
    
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
    int frame_rate = vedio.getFrameRate();
	vedio.displayInput("Current Frame");
	vedio.setDelay(1000. / vedio.getFrameRate());// Play the video at the original frame rate
	vector<vector<int>> watermark_accumulation(64, vector<int>(64,0));
    string watermark_path = static_cast<const char*>(Marshal::StringToHGlobalAnsi(embeddedwatermark_text->Text).ToPointer());
    cv::Mat watermark_img(64,64, CV_8UC1,255);
    if (watermark_path != "") {
        watermark_img = cv::imread(watermark_path, 0);
        threshold(watermark_img, watermark_img, 240, 255, CV_THRESH_BINARY);
    }   
    // Tesseract OCR API
    tesseract::TessBaseAPI tess;
    printf("Tesseract-ocr version: %s\n", tess.Version());
    tess.Init(NULL, "eng", tesseract::OEM_DEFAULT);
    tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
	//Arnold(watermark_img, 5);
	cv::Mat watermark;
	cv::Mat output_frame;
    // previous first frame
    cv::Mat pre_first;
    // previous second frame
    cv::Mat pre_second;
    // previous frame
    cv::Mat pre_frame;
	// current frame
	cv::Mat cur_frame;
	// next frame
	cv::Mat next_frame;
	if (!vedio.isOpened())
		return;
	vedio.stop = false;
    vedio.readNextFrame(pre_first); //read the first frame
    vedio.readNextFrame(pre_second); //read the second frame
	vedio.readNextFrame(pre_frame); //read the third frame
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
		if (next_shotDiff.val[0] - pre_shotDiff.val[0]>1)
		{
			double nc = 0;
            if(frame_rate>47)
			    absdiff(pre_second, cur_frame, output_frame);
            else absdiff(pre_frame, cur_frame, output_frame);
			//cur_frame = pre_frame.clone();
			progressForm->progressBar1->Value = double(vedio.fnumber) * 100 / tatal_frame;
			progressForm->progresslabel->Text = (double(vedio.fnumber) * 100 / tatal_frame).ToString("0.00") + "%";
			std::cout << vedio.fnumber << "帧" << next_shotDiff.val[0] << "前一帧" << pre_shotDiff.val[0] << std::endl;
			char name[100];
            //sprintf(name, "./res/photo/%dpre.jpg", vedio.fnumber);
            //imwrite(name, pre_frame);
			//sprintf(name, "./res/photo/%dsecond.jpg", vedio.fnumber);
			//imwrite(name, pre_second);
			//sprintf(name, "./res/photo/%dcur.jpg", vedio.fnumber);
			//imwrite(name, cur_frame);
            cv::Mat channels[3];
            // 把一个3通道图像转换成3个单通道图像  
            split(output_frame, channels);//分离色彩通道  
            cv::Mat image = channels[0].clone();
            threshold(image, image, 5, 255, CV_THRESH_BINARY_INV);
            //imshow("image", image);
            sprintf(name, "./res/detect/%ddiff.bmp", vedio.fnumber);
            imwrite(name, image);
			watermark = watermark_img.clone();
			vedio.detect(image,watermark,nc);
            sprintf(name, "./res/watermark/%dwm.bmp", vedio.fnumber);
            imwrite(name, watermark);
            if (watermark_path != "") {
                cv::Scalar val = getMSSIM(watermark, watermark_img);
                cout << val[0] << "\t" << val[1] << "\t" << val[2] << "\t" << val[3] << endl;
                outfile << vedio.fnumber << "\t" << "frame" << "\t" << nc << std::endl;
                nc_text->ReadOnly = true;
                sprintf_s(name, "%.4f", val[0]);
                nc_text->Text = System::Runtime::InteropServices::Marshal::PtrToStringAnsi((IntPtr)name);
            }
			//Reverse_Arnold(watermark, 5);
			//sprintf(name, "./res/detect/%dd%.4f.bmp", vedio.fnumber,nc);
			//imwrite(name, watermark);
			detect_watermark->Image = gcnew System::Drawing::Bitmap(watermark.cols, watermark.rows, watermark.step, System::Drawing::Imaging::PixelFormat::Format8bppIndexed, (System::IntPtr)watermark.ptr());
			detect_watermark->Refresh();
            tess.SetImage((uchar*)watermark.data, watermark.cols, watermark.rows, 1, watermark.cols);
            // Get the text   
            std::string outText = tess.GetUTF8Text();  
            std::cout << outText << std::endl;
            bool flag = true;
            for (auto c : outText) {
                if (c<=0||127<=c) {
                    flag = false;
                    break;
                }
            }
            if (1) {
                std::cout << "Accept"<<outText << std::endl;
                vedio.extractFinalWatermark(watermark, watermark_accumulation);
                final_watermark->Image = gcnew System::Drawing::Bitmap(watermark.cols, watermark.rows, watermark.step, System::Drawing::Imaging::PixelFormat::Format8bppIndexed, (System::IntPtr)watermark.ptr());
                final_watermark->Refresh();
                tess.SetImage((uchar*)watermark.data, watermark.cols, watermark.rows, 1, watermark.cols);
                // Get the text   
                std::string outText = tess.GetUTF8Text();
                std::cout <<"final:"<< outText << std::endl;
            }
		}
		// increment frame number
		vedio.fnumber++;
		pre_shotDiff = next_shotDiff;
		cur_hist = next_hist;
        pre_first = pre_second.clone();
        pre_second = pre_frame.clone();
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
            tess.Clear();
            tess.End();
		}
	}
	outfile.close();
	progressForm->Close();
    tess.Clear();
    tess.End();
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
	saveFileDialog1->Filter = "avi文件(*.avi)|*.mp4|所有文件(*.*)|*.*";
	saveFileDialog1->FilterIndex = 1;
	saveFileDialog1->FileName = "test.avi";
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
