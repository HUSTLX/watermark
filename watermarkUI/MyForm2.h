#pragma once

namespace watermarkUI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// MyForm2 ժҪ
	/// </summary>
	public ref class MyForm2 : public System::Windows::Forms::Form
	{
	public:
		MyForm2(void)
		{
			InitializeComponent();
			//
			//TODO:  �ڴ˴���ӹ��캯������
			//
		}

	protected:
		/// <summary>
		/// ������������ʹ�õ���Դ��
		/// </summary>
		~MyForm2()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::ProgressBar^  progressBar1;
	public: System::Windows::Forms::Button^  progressstop;
	public: System::Windows::Forms::Label^  progresslabel;
	public: bool process_stop=false;
	protected:

	private:
		/// <summary>
		/// ����������������
		
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// �����֧������ķ��� - ��Ҫ�޸�
		/// ʹ�ô���༭���޸Ĵ˷��������ݡ�
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm2::typeid));
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->progressstop = (gcnew System::Windows::Forms::Button());
			this->progresslabel = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(22, 27);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(317, 23);
			this->progressBar1->TabIndex = 0;
			// 
			// progressstop
			// 
			this->progressstop->Location = System::Drawing::Point(129, 67);
			this->progressstop->Name = L"progressstop";
			this->progressstop->Size = System::Drawing::Size(75, 23);
			this->progressstop->TabIndex = 1;
			this->progressstop->Text = L"ȡ��";
			this->progressstop->UseVisualStyleBackColor = true;
			this->progressstop->Click += gcnew System::EventHandler(this, &MyForm2::progressstop_Click);
			// 
			// progresslabel
			// 
			this->progresslabel->AutoSize = true;
			this->progresslabel->Location = System::Drawing::Point(20, 53);
			this->progresslabel->Name = L"progresslabel";
			this->progresslabel->Size = System::Drawing::Size(17, 12);
			this->progresslabel->TabIndex = 2;
			this->progresslabel->Text = L"0%";
			// 
			// MyForm2
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(360, 102);
			this->Controls->Add(this->progresslabel);
			this->Controls->Add(this->progressstop);
			this->Controls->Add(this->progressBar1);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Name = L"MyForm2";
			this->Text = L"������...";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
	     
#pragma endregion
	private: System::Void progressstop_Click(System::Object^  sender, System::EventArgs^  e) {
		process_stop=true;

	}
	};
}
