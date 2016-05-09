#ifndef MYFORM_H_
#define MYFORM_H_
#include "MyForm2.h"
#include "Arnold.h"
#include "videowatermark.h"
#include "sift.h"
#include "DCT.h"
#include <tesseract/allheaders.h>  
#include <tesseract/baseapi.h> 
#include <stdio.h>
using namespace std;
using namespace System;
using namespace System::Runtime::InteropServices;
namespace watermarkUI {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	/// <summary>
	/// MyForm 摘要
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO:  在此处添加构造函数代码
			//
			
		}
	protected:
		/// <summary>
		/// 清理所有正在使用的资源。
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  Embed_tab;
	private: System::Windows::Forms::TabPage^  Detect_tab;


	protected:
	private: System::Windows::Forms::Button^  inputbutton;
	private: System::Windows::Forms::TextBox^  inputfiletext;
	private: System::Windows::Forms::Label^  inputfilelabel;

	private: System::Windows::Forms::Button^  outputbutton;
	private: System::Windows::Forms::TextBox^  outputfiletext;
	private: System::Windows::Forms::Label^  outputfilelabel;

	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	private: System::Windows::Forms::SaveFileDialog^  saveFileDialog1;
	private: System::Windows::Forms::TabControl^  tabControl2;
	private: System::Windows::Forms::TabPage^  Arnold_tab;
	private: System::Windows::Forms::Button^  arnoldstart;
	private: System::Windows::Forms::PictureBox^  watermark_pic;
	private: System::Windows::Forms::Button^  loadimage;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  detectfilebutton;
	private: System::Windows::Forms::TextBox^  detectfiletext;
	private: System::Windows::Forms::Label^  detectfile;
	private: System::Windows::Forms::TabControl^  tabControl3;
	private: System::Windows::Forms::TabPage^  detect_arnold_tab;
	private: System::Windows::Forms::PictureBox^  detect_watermark;
	private: System::Windows::Forms::Label^  detectwatermarklabel;
	private: System::Windows::Forms::Button^  detect_arnoldbutton;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog3;
	private: System::Windows::Forms::TextBox^  PSNR_text;
	private: System::Windows::Forms::Label^  PSNR_label;
	private: System::Windows::Forms::PictureBox^  final_watermark;
	private: System::Windows::Forms::Label^  accumulation_label;
	private: System::Windows::Forms::Label^  per_frame_label;
	private: System::Windows::Forms::TextBox^  nc_text;
	private: System::Windows::Forms::Label^  NC_label;
	private: System::Windows::Forms::Button^  embeddedwatermark_button;
	private: System::Windows::Forms::TextBox^  embeddedwatermark_text;
	private: System::Windows::Forms::Label^  embeddedwatermark_label;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog4;
	private:
		/// <summary>
		/// 必需的设计器变量。
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 设计器支持所需的方法 - 不要修改
		/// 使用代码编辑器修改此方法的内容。
		/// </summary>
		void InitializeComponent(void)
		{
            System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
            this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
            this->Embed_tab = (gcnew System::Windows::Forms::TabPage());
            this->PSNR_text = (gcnew System::Windows::Forms::TextBox());
            this->PSNR_label = (gcnew System::Windows::Forms::Label());
            this->label1 = (gcnew System::Windows::Forms::Label());
            this->watermark_pic = (gcnew System::Windows::Forms::PictureBox());
            this->loadimage = (gcnew System::Windows::Forms::Button());
            this->tabControl2 = (gcnew System::Windows::Forms::TabControl());
            this->Arnold_tab = (gcnew System::Windows::Forms::TabPage());
            this->arnoldstart = (gcnew System::Windows::Forms::Button());
            this->outputbutton = (gcnew System::Windows::Forms::Button());
            this->outputfiletext = (gcnew System::Windows::Forms::TextBox());
            this->outputfilelabel = (gcnew System::Windows::Forms::Label());
            this->inputbutton = (gcnew System::Windows::Forms::Button());
            this->inputfiletext = (gcnew System::Windows::Forms::TextBox());
            this->inputfilelabel = (gcnew System::Windows::Forms::Label());
            this->Detect_tab = (gcnew System::Windows::Forms::TabPage());
            this->embeddedwatermark_button = (gcnew System::Windows::Forms::Button());
            this->embeddedwatermark_text = (gcnew System::Windows::Forms::TextBox());
            this->embeddedwatermark_label = (gcnew System::Windows::Forms::Label());
            this->nc_text = (gcnew System::Windows::Forms::TextBox());
            this->NC_label = (gcnew System::Windows::Forms::Label());
            this->accumulation_label = (gcnew System::Windows::Forms::Label());
            this->per_frame_label = (gcnew System::Windows::Forms::Label());
            this->final_watermark = (gcnew System::Windows::Forms::PictureBox());
            this->tabControl3 = (gcnew System::Windows::Forms::TabControl());
            this->detect_arnold_tab = (gcnew System::Windows::Forms::TabPage());
            this->detect_arnoldbutton = (gcnew System::Windows::Forms::Button());
            this->detect_watermark = (gcnew System::Windows::Forms::PictureBox());
            this->detectwatermarklabel = (gcnew System::Windows::Forms::Label());
            this->detectfilebutton = (gcnew System::Windows::Forms::Button());
            this->detectfiletext = (gcnew System::Windows::Forms::TextBox());
            this->detectfile = (gcnew System::Windows::Forms::Label());
            this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
            this->saveFileDialog1 = (gcnew System::Windows::Forms::SaveFileDialog());
            this->openFileDialog2 = (gcnew System::Windows::Forms::OpenFileDialog());
            this->openFileDialog3 = (gcnew System::Windows::Forms::OpenFileDialog());
            this->openFileDialog4 = (gcnew System::Windows::Forms::OpenFileDialog());
            this->tabControl1->SuspendLayout();
            this->Embed_tab->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->watermark_pic))->BeginInit();
            this->tabControl2->SuspendLayout();
            this->Arnold_tab->SuspendLayout();
            this->Detect_tab->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->final_watermark))->BeginInit();
            this->tabControl3->SuspendLayout();
            this->detect_arnold_tab->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->detect_watermark))->BeginInit();
            this->SuspendLayout();
            // 
            // tabControl1
            // 
            this->tabControl1->Alignment = System::Windows::Forms::TabAlignment::Left;
            this->tabControl1->Controls->Add(this->Embed_tab);
            this->tabControl1->Controls->Add(this->Detect_tab);
            this->tabControl1->Font = (gcnew System::Drawing::Font(L"Consolas", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->tabControl1->Location = System::Drawing::Point(0, 0);
            this->tabControl1->Multiline = true;
            this->tabControl1->Name = L"tabControl1";
            this->tabControl1->RightToLeft = System::Windows::Forms::RightToLeft::No;
            this->tabControl1->SelectedIndex = 0;
            this->tabControl1->Size = System::Drawing::Size(693, 433);
            this->tabControl1->TabIndex = 0;
            // 
            // Embed_tab
            // 
            this->Embed_tab->BackColor = System::Drawing::Color::White;
            this->Embed_tab->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->Embed_tab->Controls->Add(this->PSNR_text);
            this->Embed_tab->Controls->Add(this->PSNR_label);
            this->Embed_tab->Controls->Add(this->label1);
            this->Embed_tab->Controls->Add(this->watermark_pic);
            this->Embed_tab->Controls->Add(this->loadimage);
            this->Embed_tab->Controls->Add(this->tabControl2);
            this->Embed_tab->Controls->Add(this->outputbutton);
            this->Embed_tab->Controls->Add(this->outputfiletext);
            this->Embed_tab->Controls->Add(this->outputfilelabel);
            this->Embed_tab->Controls->Add(this->inputbutton);
            this->Embed_tab->Controls->Add(this->inputfiletext);
            this->Embed_tab->Controls->Add(this->inputfilelabel);
            this->Embed_tab->Location = System::Drawing::Point(33, 4);
            this->Embed_tab->Name = L"Embed_tab";
            this->Embed_tab->Padding = System::Windows::Forms::Padding(3);
            this->Embed_tab->RightToLeft = System::Windows::Forms::RightToLeft::No;
            this->Embed_tab->Size = System::Drawing::Size(656, 425);
            this->Embed_tab->TabIndex = 0;
            this->Embed_tab->Text = L"嵌入";
            // 
            // PSNR_text
            // 
            this->PSNR_text->Location = System::Drawing::Point(164, 336);
            this->PSNR_text->Name = L"PSNR_text";
            this->PSNR_text->Size = System::Drawing::Size(64, 30);
            this->PSNR_text->TabIndex = 15;
            // 
            // PSNR_label
            // 
            this->PSNR_label->AutoSize = true;
            this->PSNR_label->Location = System::Drawing::Point(51, 339);
            this->PSNR_label->Name = L"PSNR_label";
            this->PSNR_label->Size = System::Drawing::Size(60, 22);
            this->PSNR_label->TabIndex = 14;
            this->PSNR_label->Text = L"PSNR:";
            // 
            // label1
            // 
            this->label1->AutoSize = true;
            this->label1->Location = System::Drawing::Point(51, 195);
            this->label1->Name = L"label1";
            this->label1->Size = System::Drawing::Size(50, 22);
            this->label1->TabIndex = 13;
            this->label1->Text = L"水印";
            // 
            // watermark_pic
            // 
            this->watermark_pic->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
            this->watermark_pic->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->watermark_pic->Location = System::Drawing::Point(164, 235);
            this->watermark_pic->Name = L"watermark_pic";
            this->watermark_pic->Size = System::Drawing::Size(64, 64);
            this->watermark_pic->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
            this->watermark_pic->TabIndex = 12;
            this->watermark_pic->TabStop = false;
            // 
            // loadimage
            // 
            this->loadimage->Location = System::Drawing::Point(53, 245);
            this->loadimage->Name = L"loadimage";
            this->loadimage->Size = System::Drawing::Size(58, 38);
            this->loadimage->TabIndex = 11;
            this->loadimage->Text = L"加载";
            this->loadimage->UseVisualStyleBackColor = true;
            this->loadimage->Click += gcnew System::EventHandler(this, &MyForm::loadimage_Click);
            // 
            // tabControl2
            // 
            this->tabControl2->Controls->Add(this->Arnold_tab);
            this->tabControl2->Location = System::Drawing::Point(276, 181);
            this->tabControl2->Name = L"tabControl2";
            this->tabControl2->SelectedIndex = 0;
            this->tabControl2->Size = System::Drawing::Size(336, 222);
            this->tabControl2->TabIndex = 7;
            // 
            // Arnold_tab
            // 
            this->Arnold_tab->Controls->Add(this->arnoldstart);
            this->Arnold_tab->Location = System::Drawing::Point(4, 31);
            this->Arnold_tab->Name = L"Arnold_tab";
            this->Arnold_tab->Padding = System::Windows::Forms::Padding(3);
            this->Arnold_tab->Size = System::Drawing::Size(328, 187);
            this->Arnold_tab->TabIndex = 0;
            this->Arnold_tab->Text = L"Arnold";
            this->Arnold_tab->UseVisualStyleBackColor = true;
            // 
            // arnoldstart
            // 
            this->arnoldstart->Location = System::Drawing::Point(100, 115);
            this->arnoldstart->Name = L"arnoldstart";
            this->arnoldstart->Size = System::Drawing::Size(76, 34);
            this->arnoldstart->TabIndex = 0;
            this->arnoldstart->Text = L"开始";
            this->arnoldstart->UseVisualStyleBackColor = true;
            this->arnoldstart->Click += gcnew System::EventHandler(this, &MyForm::arnoldstart_Click);
            // 
            // outputbutton
            // 
            this->outputbutton->BackColor = System::Drawing::Color::DodgerBlue;
            this->outputbutton->Location = System::Drawing::Point(527, 112);
            this->outputbutton->Name = L"outputbutton";
            this->outputbutton->Size = System::Drawing::Size(85, 33);
            this->outputbutton->TabIndex = 5;
            this->outputbutton->Text = L"浏览";
            this->outputbutton->UseVisualStyleBackColor = false;
            this->outputbutton->Click += gcnew System::EventHandler(this, &MyForm::outputbutton_Click);
            // 
            // outputfiletext
            // 
            this->outputfiletext->Location = System::Drawing::Point(164, 112);
            this->outputfiletext->Name = L"outputfiletext";
            this->outputfiletext->RightToLeft = System::Windows::Forms::RightToLeft::No;
            this->outputfiletext->Size = System::Drawing::Size(335, 30);
            this->outputfiletext->TabIndex = 6;
            // 
            // outputfilelabel
            // 
            this->outputfilelabel->AutoSize = true;
            this->outputfilelabel->Font = (gcnew System::Drawing::Font(L"Consolas", 15, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->outputfilelabel->Location = System::Drawing::Point(31, 112);
            this->outputfilelabel->Name = L"outputfilelabel";
            this->outputfilelabel->Size = System::Drawing::Size(98, 23);
            this->outputfilelabel->TabIndex = 3;
            this->outputfilelabel->Text = L"输出文件";
            // 
            // inputbutton
            // 
            this->inputbutton->BackColor = System::Drawing::Color::DodgerBlue;
            this->inputbutton->Location = System::Drawing::Point(527, 42);
            this->inputbutton->Name = L"inputbutton";
            this->inputbutton->Size = System::Drawing::Size(85, 33);
            this->inputbutton->TabIndex = 2;
            this->inputbutton->Text = L"浏览";
            this->inputbutton->UseVisualStyleBackColor = false;
            this->inputbutton->Click += gcnew System::EventHandler(this, &MyForm::inputbutton_Click);
            // 
            // inputfiletext
            // 
            this->inputfiletext->Location = System::Drawing::Point(164, 45);
            this->inputfiletext->Name = L"inputfiletext";
            this->inputfiletext->RightToLeft = System::Windows::Forms::RightToLeft::No;
            this->inputfiletext->Size = System::Drawing::Size(335, 30);
            this->inputfiletext->TabIndex = 1;
            // 
            // inputfilelabel
            // 
            this->inputfilelabel->AutoSize = true;
            this->inputfilelabel->Font = (gcnew System::Drawing::Font(L"Consolas", 15, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->inputfilelabel->Location = System::Drawing::Point(31, 46);
            this->inputfilelabel->Name = L"inputfilelabel";
            this->inputfilelabel->Size = System::Drawing::Size(98, 23);
            this->inputfilelabel->TabIndex = 0;
            this->inputfilelabel->Text = L"输入文件";
            // 
            // Detect_tab
            // 
            this->Detect_tab->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->Detect_tab->Controls->Add(this->embeddedwatermark_button);
            this->Detect_tab->Controls->Add(this->embeddedwatermark_text);
            this->Detect_tab->Controls->Add(this->embeddedwatermark_label);
            this->Detect_tab->Controls->Add(this->nc_text);
            this->Detect_tab->Controls->Add(this->NC_label);
            this->Detect_tab->Controls->Add(this->accumulation_label);
            this->Detect_tab->Controls->Add(this->per_frame_label);
            this->Detect_tab->Controls->Add(this->final_watermark);
            this->Detect_tab->Controls->Add(this->tabControl3);
            this->Detect_tab->Controls->Add(this->detect_watermark);
            this->Detect_tab->Controls->Add(this->detectwatermarklabel);
            this->Detect_tab->Controls->Add(this->detectfilebutton);
            this->Detect_tab->Controls->Add(this->detectfiletext);
            this->Detect_tab->Controls->Add(this->detectfile);
            this->Detect_tab->Location = System::Drawing::Point(33, 4);
            this->Detect_tab->Name = L"Detect_tab";
            this->Detect_tab->Padding = System::Windows::Forms::Padding(3);
            this->Detect_tab->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
            this->Detect_tab->Size = System::Drawing::Size(656, 425);
            this->Detect_tab->TabIndex = 1;
            this->Detect_tab->Text = L"提取";
            this->Detect_tab->UseVisualStyleBackColor = true;
            // 
            // embeddedwatermark_button
            // 
            this->embeddedwatermark_button->BackColor = System::Drawing::Color::DodgerBlue;
            this->embeddedwatermark_button->ForeColor = System::Drawing::SystemColors::ControlText;
            this->embeddedwatermark_button->Location = System::Drawing::Point(530, 109);
            this->embeddedwatermark_button->Name = L"embeddedwatermark_button";
            this->embeddedwatermark_button->Size = System::Drawing::Size(82, 30);
            this->embeddedwatermark_button->TabIndex = 13;
            this->embeddedwatermark_button->Text = L"浏览";
            this->embeddedwatermark_button->UseVisualStyleBackColor = false;
            this->embeddedwatermark_button->Click += gcnew System::EventHandler(this, &MyForm::embeddedwatermark_button_Click);
            // 
            // embeddedwatermark_text
            // 
            this->embeddedwatermark_text->Location = System::Drawing::Point(184, 109);
            this->embeddedwatermark_text->Name = L"embeddedwatermark_text";
            this->embeddedwatermark_text->RightToLeft = System::Windows::Forms::RightToLeft::No;
            this->embeddedwatermark_text->Size = System::Drawing::Size(313, 30);
            this->embeddedwatermark_text->TabIndex = 12;
            // 
            // embeddedwatermark_label
            // 
            this->embeddedwatermark_label->AutoSize = true;
            this->embeddedwatermark_label->Font = (gcnew System::Drawing::Font(L"Consolas", 15, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->embeddedwatermark_label->Location = System::Drawing::Point(34, 112);
            this->embeddedwatermark_label->Name = L"embeddedwatermark_label";
            this->embeddedwatermark_label->Size = System::Drawing::Size(98, 23);
            this->embeddedwatermark_label->TabIndex = 11;
            this->embeddedwatermark_label->Text = L"原始水印";
            // 
            // nc_text
            // 
            this->nc_text->Location = System::Drawing::Point(115, 369);
            this->nc_text->Name = L"nc_text";
            this->nc_text->RightToLeft = System::Windows::Forms::RightToLeft::No;
            this->nc_text->Size = System::Drawing::Size(89, 30);
            this->nc_text->TabIndex = 10;
            // 
            // NC_label
            // 
            this->NC_label->AutoSize = true;
            this->NC_label->Location = System::Drawing::Point(58, 372);
            this->NC_label->Name = L"NC_label";
            this->NC_label->RightToLeft = System::Windows::Forms::RightToLeft::No;
            this->NC_label->Size = System::Drawing::Size(60, 22);
            this->NC_label->TabIndex = 9;
            this->NC_label->Text = L"SSIM:";
            this->NC_label->TextAlign = System::Drawing::ContentAlignment::TopRight;
            // 
            // accumulation_label
            // 
            this->accumulation_label->AutoSize = true;
            this->accumulation_label->Location = System::Drawing::Point(140, 327);
            this->accumulation_label->Name = L"accumulation_label";
            this->accumulation_label->Size = System::Drawing::Size(50, 22);
            this->accumulation_label->TabIndex = 8;
            this->accumulation_label->Text = L"综合";
            // 
            // per_frame_label
            // 
            this->per_frame_label->AutoSize = true;
            this->per_frame_label->Location = System::Drawing::Point(34, 327);
            this->per_frame_label->Name = L"per_frame_label";
            this->per_frame_label->Size = System::Drawing::Size(50, 22);
            this->per_frame_label->TabIndex = 7;
            this->per_frame_label->Text = L"单帧";
            // 
            // final_watermark
            // 
            this->final_watermark->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->final_watermark->Location = System::Drawing::Point(140, 236);
            this->final_watermark->Name = L"final_watermark";
            this->final_watermark->Size = System::Drawing::Size(64, 64);
            this->final_watermark->TabIndex = 6;
            this->final_watermark->TabStop = false;
            // 
            // tabControl3
            // 
            this->tabControl3->Controls->Add(this->detect_arnold_tab);
            this->tabControl3->Location = System::Drawing::Point(276, 181);
            this->tabControl3->Name = L"tabControl3";
            this->tabControl3->SelectedIndex = 0;
            this->tabControl3->Size = System::Drawing::Size(336, 222);
            this->tabControl3->TabIndex = 5;
            // 
            // detect_arnold_tab
            // 
            this->detect_arnold_tab->Controls->Add(this->detect_arnoldbutton);
            this->detect_arnold_tab->Location = System::Drawing::Point(4, 31);
            this->detect_arnold_tab->Name = L"detect_arnold_tab";
            this->detect_arnold_tab->Padding = System::Windows::Forms::Padding(3);
            this->detect_arnold_tab->Size = System::Drawing::Size(328, 187);
            this->detect_arnold_tab->TabIndex = 0;
            this->detect_arnold_tab->Text = L"Arnold";
            this->detect_arnold_tab->UseVisualStyleBackColor = true;
            // 
            // detect_arnoldbutton
            // 
            this->detect_arnoldbutton->Location = System::Drawing::Point(98, 105);
            this->detect_arnoldbutton->Name = L"detect_arnoldbutton";
            this->detect_arnoldbutton->Size = System::Drawing::Size(83, 32);
            this->detect_arnoldbutton->TabIndex = 0;
            this->detect_arnoldbutton->Text = L"开始";
            this->detect_arnoldbutton->UseVisualStyleBackColor = true;
            this->detect_arnoldbutton->Click += gcnew System::EventHandler(this, &MyForm::detect_arnoldbutton_Click);
            // 
            // detect_watermark
            // 
            this->detect_watermark->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->detect_watermark->Location = System::Drawing::Point(34, 236);
            this->detect_watermark->Name = L"detect_watermark";
            this->detect_watermark->Size = System::Drawing::Size(64, 64);
            this->detect_watermark->TabIndex = 4;
            this->detect_watermark->TabStop = false;
            // 
            // detectwatermarklabel
            // 
            this->detectwatermarklabel->AutoSize = true;
            this->detectwatermarklabel->Location = System::Drawing::Point(34, 193);
            this->detectwatermarklabel->Name = L"detectwatermarklabel";
            this->detectwatermarklabel->Size = System::Drawing::Size(130, 22);
            this->detectwatermarklabel->TabIndex = 3;
            this->detectwatermarklabel->Text = L"检测到的水印";
            // 
            // detectfilebutton
            // 
            this->detectfilebutton->BackColor = System::Drawing::Color::DodgerBlue;
            this->detectfilebutton->ForeColor = System::Drawing::SystemColors::ControlText;
            this->detectfilebutton->Location = System::Drawing::Point(530, 42);
            this->detectfilebutton->Name = L"detectfilebutton";
            this->detectfilebutton->Size = System::Drawing::Size(82, 30);
            this->detectfilebutton->TabIndex = 2;
            this->detectfilebutton->Text = L"浏览";
            this->detectfilebutton->UseVisualStyleBackColor = false;
            this->detectfilebutton->Click += gcnew System::EventHandler(this, &MyForm::detectfilebutton_Click);
            // 
            // detectfiletext
            // 
            this->detectfiletext->Location = System::Drawing::Point(184, 43);
            this->detectfiletext->Name = L"detectfiletext";
            this->detectfiletext->RightToLeft = System::Windows::Forms::RightToLeft::No;
            this->detectfiletext->Size = System::Drawing::Size(313, 30);
            this->detectfiletext->TabIndex = 1;
            // 
            // detectfile
            // 
            this->detectfile->AutoSize = true;
            this->detectfile->Font = (gcnew System::Drawing::Font(L"Consolas", 15));
            this->detectfile->Location = System::Drawing::Point(34, 45);
            this->detectfile->Name = L"detectfile";
            this->detectfile->Size = System::Drawing::Size(98, 23);
            this->detectfile->TabIndex = 0;
            this->detectfile->Text = L"待测文件";
            // 
            // openFileDialog1
            // 
            this->openFileDialog1->FileName = L"openFileDialog1";
            // 
            // openFileDialog2
            // 
            this->openFileDialog2->FileName = L"openFileDialog2";
            // 
            // openFileDialog3
            // 
            this->openFileDialog3->FileName = L"openFileDialog3";
            // 
            // openFileDialog4
            // 
            this->openFileDialog4->FileName = L"openFileDialog4";
            // 
            // MyForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->BackColor = System::Drawing::SystemColors::ActiveBorder;
            this->ClientSize = System::Drawing::Size(693, 430);
            this->Controls->Add(this->tabControl1);
            this->Font = (gcnew System::Drawing::Font(L"Consolas", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(134)));
            this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
            this->Name = L"MyForm";
            this->Text = L"水印嵌入及检测系统";
            this->tabControl1->ResumeLayout(false);
            this->Embed_tab->ResumeLayout(false);
            this->Embed_tab->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->watermark_pic))->EndInit();
            this->tabControl2->ResumeLayout(false);
            this->Arnold_tab->ResumeLayout(false);
            this->Detect_tab->ResumeLayout(false);
            this->Detect_tab->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->final_watermark))->EndInit();
            this->tabControl3->ResumeLayout(false);
            this->detect_arnold_tab->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->detect_watermark))->EndInit();
            this->ResumeLayout(false);

        }
#pragma endregion
         //用户指定嵌入水印后的输出视频文件的保存路径
private: System::Void outputbutton_Click(System::Object^  sender, System::EventArgs^  e);
		 //用户指定待嵌入水印的视频文件
private: System::Void inputbutton_Click(System::Object^  sender, System::EventArgs^  e);
		 //用Arnold算法开始嵌入水印
private: System::Void arnoldstart_Click(System::Object^  sender, System::EventArgs^  e);
		 //加载要嵌入的水印图像
private: System::Void loadimage_Click(System::Object^  sender, System::EventArgs^  e);
		 //解码视频开始嵌入水印并编码
public: void embed_start(VideoWatermark& processor);
		 //解码视频检测水印
public: void detect_start(VideoWatermark& processor);
		 //用户指定要检测的视频文件
private: System::Void detectfilebutton_Click(System::Object^  sender, System::EventArgs^  e);
		 //用Arnold算法开始检测水印
private: System::Void detect_arnoldbutton_Click(System::Object^  sender, System::EventArgs^  e);
		 //加载嵌入视频的原始水印图像用于计算SSIM
private: System::Void embeddedwatermark_button_Click(System::Object^  sender, System::EventArgs^  e);
};
}
#endif