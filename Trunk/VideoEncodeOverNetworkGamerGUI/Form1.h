#pragma once


namespace VideoEncodeOverNetworkGamerGUI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Runtime::InteropServices;

	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ComboBox^  FC_D_WindowName;
	protected: 


	private: System::Windows::Forms::Label^  FC_L_Resize;
	private: System::Windows::Forms::Label^  FC_L_Width;
	private: System::Windows::Forms::TextBox^  FC_E_Width;
	protected: 




	private: System::Windows::Forms::Label^  FC_L_Height;
	private: System::Windows::Forms::TextBox^  FC_E_Height;


	private: System::Windows::Forms::ComboBox^  FC_D_Resize;

	private: System::Windows::Forms::Label^  FC_L_BPP;
	private: System::Windows::Forms::Label^  FC_L_Compression;
	private: System::Windows::Forms::ComboBox^  FC_D_Compress;




	private: System::Windows::Forms::Label^  FC_L_FPS;
	private: System::Windows::Forms::TextBox^  FC_E_FPS;


	private: System::Windows::Forms::ComboBox^  FC_D_BPP;
	private: System::Windows::Forms::Label^  FC_L_Stat_FPS0;
	private: System::Windows::Forms::Label^  FC_L_StatFPS1;
	private: System::Windows::Forms::Label^  FC_L_Stat_BW0;
	private: System::Windows::Forms::Label^  FC_Stat_BW1;
	private: System::Windows::Forms::Button^  FC_B_Apply;

	private: System::Windows::Forms::Label^  FC_L_InclBorder;
	private: System::Windows::Forms::CheckBox^  FC_C_BorderIncl;
	private: System::Windows::Forms::Label^  FC_L_ARW0;


	private: System::Windows::Forms::Label^  FC_L_ARW1;
	private: System::Windows::Forms::Label^  FC_L_ARH0;
	private: System::Windows::Forms::Label^  FC_L_ARH1;
	private: System::Windows::Forms::Label^  FC_L_CPU0;
	private: System::Windows::Forms::Label^  FC_L_CPU1;
	private: System::Windows::Forms::Label^  FC_L_Port;

	private: System::Windows::Forms::TextBox^  FC_E_Port;
	private: System::Windows::Forms::Button^  FC_B_WindowRefresh;








	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->FC_D_WindowName = (gcnew System::Windows::Forms::ComboBox());
			this->FC_L_Resize = (gcnew System::Windows::Forms::Label());
			this->FC_L_Width = (gcnew System::Windows::Forms::Label());
			this->FC_E_Width = (gcnew System::Windows::Forms::TextBox());
			this->FC_L_Height = (gcnew System::Windows::Forms::Label());
			this->FC_E_Height = (gcnew System::Windows::Forms::TextBox());
			this->FC_D_Resize = (gcnew System::Windows::Forms::ComboBox());
			this->FC_L_BPP = (gcnew System::Windows::Forms::Label());
			this->FC_L_Compression = (gcnew System::Windows::Forms::Label());
			this->FC_D_Compress = (gcnew System::Windows::Forms::ComboBox());
			this->FC_L_FPS = (gcnew System::Windows::Forms::Label());
			this->FC_E_FPS = (gcnew System::Windows::Forms::TextBox());
			this->FC_D_BPP = (gcnew System::Windows::Forms::ComboBox());
			this->FC_L_Stat_FPS0 = (gcnew System::Windows::Forms::Label());
			this->FC_L_StatFPS1 = (gcnew System::Windows::Forms::Label());
			this->FC_L_Stat_BW0 = (gcnew System::Windows::Forms::Label());
			this->FC_Stat_BW1 = (gcnew System::Windows::Forms::Label());
			this->FC_B_Apply = (gcnew System::Windows::Forms::Button());
			this->FC_L_InclBorder = (gcnew System::Windows::Forms::Label());
			this->FC_C_BorderIncl = (gcnew System::Windows::Forms::CheckBox());
			this->FC_L_ARW0 = (gcnew System::Windows::Forms::Label());
			this->FC_L_ARW1 = (gcnew System::Windows::Forms::Label());
			this->FC_L_ARH0 = (gcnew System::Windows::Forms::Label());
			this->FC_L_ARH1 = (gcnew System::Windows::Forms::Label());
			this->FC_L_CPU0 = (gcnew System::Windows::Forms::Label());
			this->FC_L_CPU1 = (gcnew System::Windows::Forms::Label());
			this->FC_L_Port = (gcnew System::Windows::Forms::Label());
			this->FC_E_Port = (gcnew System::Windows::Forms::TextBox());
			this->FC_B_WindowRefresh = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// FC_D_WindowName
			// 
			this->FC_D_WindowName->FormattingEnabled = true;
			this->FC_D_WindowName->ImeMode = System::Windows::Forms::ImeMode::NoControl;
			this->FC_D_WindowName->Location = System::Drawing::Point(131, -1);
			this->FC_D_WindowName->MaxDropDownItems = 60;
			this->FC_D_WindowName->Name = L"FC_D_WindowName";
			this->FC_D_WindowName->Size = System::Drawing::Size(121, 21);
			this->FC_D_WindowName->TabIndex = 0;
			// 
			// FC_L_Resize
			// 
			this->FC_L_Resize->AutoSize = true;
			this->FC_L_Resize->Location = System::Drawing::Point(1, 53);
			this->FC_L_Resize->Name = L"FC_L_Resize";
			this->FC_L_Resize->Size = System::Drawing::Size(73, 13);
			this->FC_L_Resize->TabIndex = 3;
			this->FC_L_Resize->Text = L"Resize Speed";
			// 
			// FC_L_Width
			// 
			this->FC_L_Width->AutoSize = true;
			this->FC_L_Width->Location = System::Drawing::Point(1, 79);
			this->FC_L_Width->Name = L"FC_L_Width";
			this->FC_L_Width->Size = System::Drawing::Size(35, 13);
			this->FC_L_Width->TabIndex = 4;
			this->FC_L_Width->Text = L"Width";
			// 
			// FC_E_Width
			// 
			this->FC_E_Width->Location = System::Drawing::Point(131, 72);
			this->FC_E_Width->MaxLength = 6;
			this->FC_E_Width->Name = L"FC_E_Width";
			this->FC_E_Width->Size = System::Drawing::Size(121, 20);
			this->FC_E_Width->TabIndex = 5;
			this->FC_E_Width->Text = L"640";
			// 
			// FC_L_Height
			// 
			this->FC_L_Height->AutoSize = true;
			this->FC_L_Height->Location = System::Drawing::Point(1, 127);
			this->FC_L_Height->Name = L"FC_L_Height";
			this->FC_L_Height->Size = System::Drawing::Size(38, 13);
			this->FC_L_Height->TabIndex = 6;
			this->FC_L_Height->Text = L"Height";
			// 
			// FC_E_Height
			// 
			this->FC_E_Height->Location = System::Drawing::Point(131, 124);
			this->FC_E_Height->MaxLength = 6;
			this->FC_E_Height->Name = L"FC_E_Height";
			this->FC_E_Height->Size = System::Drawing::Size(121, 20);
			this->FC_E_Height->TabIndex = 7;
			this->FC_E_Height->Text = L"480";
			// 
			// FC_D_Resize
			// 
			this->FC_D_Resize->FormattingEnabled = true;
			this->FC_D_Resize->ImeMode = System::Windows::Forms::ImeMode::NoControl;
			this->FC_D_Resize->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"No Resize", L"Resize"});
			this->FC_D_Resize->Location = System::Drawing::Point(131, 45);
			this->FC_D_Resize->Name = L"FC_D_Resize";
			this->FC_D_Resize->Size = System::Drawing::Size(121, 21);
			this->FC_D_Resize->TabIndex = 8;
			this->FC_D_Resize->Text = L"1";
			// 
			// FC_L_BPP
			// 
			this->FC_L_BPP->AutoSize = true;
			this->FC_L_BPP->Location = System::Drawing::Point(1, 176);
			this->FC_L_BPP->Name = L"FC_L_BPP";
			this->FC_L_BPP->Size = System::Drawing::Size(62, 13);
			this->FC_L_BPP->TabIndex = 9;
			this->FC_L_BPP->Text = L"BitsPerPixel";
			// 
			// FC_L_Compression
			// 
			this->FC_L_Compression->AutoSize = true;
			this->FC_L_Compression->Location = System::Drawing::Point(1, 203);
			this->FC_L_Compression->Name = L"FC_L_Compression";
			this->FC_L_Compression->Size = System::Drawing::Size(110, 13);
			this->FC_L_Compression->TabIndex = 11;
			this->FC_L_Compression->Text = L"Network Compression";
			// 
			// FC_D_Compress
			// 
			this->FC_D_Compress->FormattingEnabled = true;
			this->FC_D_Compress->Items->AddRange(gcnew cli::array< System::Object^  >(11) {L"No Compression", L"Fastest", L"Compression 1", 
				L"Compression 2", L"Compression 3", L"Compression 4", L"Compression 5", L"Compression 6", L"Compression 7", L"Compression 8", 
				L"Compression 9"});
			this->FC_D_Compress->Location = System::Drawing::Point(131, 195);
			this->FC_D_Compress->Name = L"FC_D_Compress";
			this->FC_D_Compress->Size = System::Drawing::Size(121, 21);
			this->FC_D_Compress->TabIndex = 12;
			this->FC_D_Compress->Text = L"2";
			// 
			// FC_L_FPS
			// 
			this->FC_L_FPS->AutoSize = true;
			this->FC_L_FPS->Location = System::Drawing::Point(1, 229);
			this->FC_L_FPS->Name = L"FC_L_FPS";
			this->FC_L_FPS->Size = System::Drawing::Size(48, 13);
			this->FC_L_FPS->TabIndex = 13;
			this->FC_L_FPS->Text = L"FPSLimit";
			// 
			// FC_E_FPS
			// 
			this->FC_E_FPS->Location = System::Drawing::Point(131, 222);
			this->FC_E_FPS->MaxLength = 6;
			this->FC_E_FPS->Name = L"FC_E_FPS";
			this->FC_E_FPS->Size = System::Drawing::Size(121, 20);
			this->FC_E_FPS->TabIndex = 14;
			this->FC_E_FPS->Text = L"20";
			// 
			// FC_D_BPP
			// 
			this->FC_D_BPP->FormattingEnabled = true;
			this->FC_D_BPP->Items->AddRange(gcnew cli::array< System::Object^  >(5) {L"8", L"7", L"6", L"5", L"4"});
			this->FC_D_BPP->Location = System::Drawing::Point(131, 169);
			this->FC_D_BPP->Name = L"FC_D_BPP";
			this->FC_D_BPP->Size = System::Drawing::Size(121, 21);
			this->FC_D_BPP->TabIndex = 15;
			this->FC_D_BPP->Text = L"6";
			// 
			// FC_L_Stat_FPS0
			// 
			this->FC_L_Stat_FPS0->AutoSize = true;
			this->FC_L_Stat_FPS0->Location = System::Drawing::Point(1, 274);
			this->FC_L_Stat_FPS0->Name = L"FC_L_Stat_FPS0";
			this->FC_L_Stat_FPS0->Size = System::Drawing::Size(59, 13);
			this->FC_L_Stat_FPS0->TabIndex = 16;
			this->FC_L_Stat_FPS0->Text = L"FPS actual";
			// 
			// FC_L_StatFPS1
			// 
			this->FC_L_StatFPS1->AutoSize = true;
			this->FC_L_StatFPS1->Location = System::Drawing::Point(128, 274);
			this->FC_L_StatFPS1->Name = L"FC_L_StatFPS1";
			this->FC_L_StatFPS1->Size = System::Drawing::Size(13, 13);
			this->FC_L_StatFPS1->TabIndex = 17;
			this->FC_L_StatFPS1->Text = L"0";
			// 
			// FC_L_Stat_BW0
			// 
			this->FC_L_Stat_BW0->AutoSize = true;
			this->FC_L_Stat_BW0->Location = System::Drawing::Point(1, 296);
			this->FC_L_Stat_BW0->Name = L"FC_L_Stat_BW0";
			this->FC_L_Stat_BW0->Size = System::Drawing::Size(79, 13);
			this->FC_L_Stat_BW0->TabIndex = 18;
			this->FC_L_Stat_BW0->Text = L"Network usage";
			// 
			// FC_Stat_BW1
			// 
			this->FC_Stat_BW1->AutoSize = true;
			this->FC_Stat_BW1->Location = System::Drawing::Point(128, 296);
			this->FC_Stat_BW1->Name = L"FC_Stat_BW1";
			this->FC_Stat_BW1->Size = System::Drawing::Size(13, 13);
			this->FC_Stat_BW1->TabIndex = 19;
			this->FC_Stat_BW1->Text = L"0";
			// 
			// FC_B_Apply
			// 
			this->FC_B_Apply->Location = System::Drawing::Point(76, 366);
			this->FC_B_Apply->Name = L"FC_B_Apply";
			this->FC_B_Apply->Size = System::Drawing::Size(97, 27);
			this->FC_B_Apply->TabIndex = 20;
			this->FC_B_Apply->Text = L"Apply Settings";
			this->FC_B_Apply->UseVisualStyleBackColor = true;
			this->FC_B_Apply->Click += gcnew System::EventHandler(this, &Form1::FC_B_Apply_Click);
			// 
			// FC_L_InclBorder
			// 
			this->FC_L_InclBorder->AutoSize = true;
			this->FC_L_InclBorder->Location = System::Drawing::Point(1, 26);
			this->FC_L_InclBorder->Name = L"FC_L_InclBorder";
			this->FC_L_InclBorder->Size = System::Drawing::Size(75, 13);
			this->FC_L_InclBorder->TabIndex = 21;
			this->FC_L_InclBorder->Text = L"Include border";
			// 
			// FC_C_BorderIncl
			// 
			this->FC_C_BorderIncl->AutoSize = true;
			this->FC_C_BorderIncl->Location = System::Drawing::Point(131, 26);
			this->FC_C_BorderIncl->Name = L"FC_C_BorderIncl";
			this->FC_C_BorderIncl->Size = System::Drawing::Size(15, 14);
			this->FC_C_BorderIncl->TabIndex = 22;
			this->FC_C_BorderIncl->UseVisualStyleBackColor = true;
			// 
			// FC_L_ARW0
			// 
			this->FC_L_ARW0->AutoSize = true;
			this->FC_L_ARW0->Location = System::Drawing::Point(1, 103);
			this->FC_L_ARW0->Name = L"FC_L_ARW0";
			this->FC_L_ARW0->Size = System::Drawing::Size(93, 13);
			this->FC_L_ARW0->TabIndex = 23;
			this->FC_L_ARW0->Text = L"Width aspect ratio";
			// 
			// FC_L_ARW1
			// 
			this->FC_L_ARW1->AutoSize = true;
			this->FC_L_ARW1->Location = System::Drawing::Point(128, 103);
			this->FC_L_ARW1->Name = L"FC_L_ARW1";
			this->FC_L_ARW1->Size = System::Drawing::Size(13, 13);
			this->FC_L_ARW1->TabIndex = 24;
			this->FC_L_ARW1->Text = L"0";
			// 
			// FC_L_ARH0
			// 
			this->FC_L_ARH0->AutoSize = true;
			this->FC_L_ARH0->Location = System::Drawing::Point(1, 150);
			this->FC_L_ARH0->Name = L"FC_L_ARH0";
			this->FC_L_ARH0->Size = System::Drawing::Size(96, 13);
			this->FC_L_ARH0->TabIndex = 25;
			this->FC_L_ARH0->Text = L"Height aspect ratio";
			// 
			// FC_L_ARH1
			// 
			this->FC_L_ARH1->AutoSize = true;
			this->FC_L_ARH1->Location = System::Drawing::Point(128, 150);
			this->FC_L_ARH1->Name = L"FC_L_ARH1";
			this->FC_L_ARH1->Size = System::Drawing::Size(13, 13);
			this->FC_L_ARH1->TabIndex = 26;
			this->FC_L_ARH1->Text = L"0";
			// 
			// FC_L_CPU0
			// 
			this->FC_L_CPU0->AutoSize = true;
			this->FC_L_CPU0->Location = System::Drawing::Point(1, 318);
			this->FC_L_CPU0->Name = L"FC_L_CPU0";
			this->FC_L_CPU0->Size = System::Drawing::Size(61, 13);
			this->FC_L_CPU0->TabIndex = 27;
			this->FC_L_CPU0->Text = L"CPU usage";
			// 
			// FC_L_CPU1
			// 
			this->FC_L_CPU1->AutoSize = true;
			this->FC_L_CPU1->Location = System::Drawing::Point(128, 318);
			this->FC_L_CPU1->Name = L"FC_L_CPU1";
			this->FC_L_CPU1->Size = System::Drawing::Size(13, 13);
			this->FC_L_CPU1->TabIndex = 28;
			this->FC_L_CPU1->Text = L"0";
			// 
			// FC_L_Port
			// 
			this->FC_L_Port->AutoSize = true;
			this->FC_L_Port->Location = System::Drawing::Point(1, 252);
			this->FC_L_Port->Name = L"FC_L_Port";
			this->FC_L_Port->Size = System::Drawing::Size(69, 13);
			this->FC_L_Port->TabIndex = 29;
			this->FC_L_Port->Text = L"Network Port";
			// 
			// FC_E_Port
			// 
			this->FC_E_Port->Location = System::Drawing::Point(131, 249);
			this->FC_E_Port->Name = L"FC_E_Port";
			this->FC_E_Port->Size = System::Drawing::Size(121, 20);
			this->FC_E_Port->TabIndex = 30;
			this->FC_E_Port->Text = L"6888";
			// 
			// FC_B_WindowRefresh
			// 
			this->FC_B_WindowRefresh->Location = System::Drawing::Point(4, 0);
			this->FC_B_WindowRefresh->Name = L"FC_B_WindowRefresh";
			this->FC_B_WindowRefresh->Size = System::Drawing::Size(113, 23);
			this->FC_B_WindowRefresh->TabIndex = 31;
			this->FC_B_WindowRefresh->Text = L"Refresh Window List";
			this->FC_B_WindowRefresh->UseVisualStyleBackColor = true;
			this->FC_B_WindowRefresh->Click += gcnew System::EventHandler(this, &Form1::FC_B_WindowRefresh_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(332, 451);
			this->Controls->Add(this->FC_B_WindowRefresh);
			this->Controls->Add(this->FC_E_Port);
			this->Controls->Add(this->FC_L_Port);
			this->Controls->Add(this->FC_L_CPU1);
			this->Controls->Add(this->FC_L_CPU0);
			this->Controls->Add(this->FC_L_ARH1);
			this->Controls->Add(this->FC_L_ARH0);
			this->Controls->Add(this->FC_L_ARW1);
			this->Controls->Add(this->FC_L_ARW0);
			this->Controls->Add(this->FC_C_BorderIncl);
			this->Controls->Add(this->FC_L_InclBorder);
			this->Controls->Add(this->FC_B_Apply);
			this->Controls->Add(this->FC_Stat_BW1);
			this->Controls->Add(this->FC_L_Stat_BW0);
			this->Controls->Add(this->FC_L_StatFPS1);
			this->Controls->Add(this->FC_L_Stat_FPS0);
			this->Controls->Add(this->FC_D_BPP);
			this->Controls->Add(this->FC_E_FPS);
			this->Controls->Add(this->FC_L_FPS);
			this->Controls->Add(this->FC_D_Compress);
			this->Controls->Add(this->FC_L_Compression);
			this->Controls->Add(this->FC_L_BPP);
			this->Controls->Add(this->FC_D_Resize);
			this->Controls->Add(this->FC_E_Height);
			this->Controls->Add(this->FC_L_Height);
			this->Controls->Add(this->FC_E_Width);
			this->Controls->Add(this->FC_L_Width);
			this->Controls->Add(this->FC_L_Resize);
			this->Controls->Add(this->FC_D_WindowName);
			this->Name = L"Form1";
			this->ShowIcon = false;
			this->Text = L"GamerPC video collector";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: System::Void FC_B_Apply_Click(System::Object^  sender, System::EventArgs^  e) 
	{
//		MessageBoxW(NULL, L"Error msg", L"Error", MB_OK );
		char *stringPointer = (char*) Marshal::StringToHGlobalAnsi( this->FC_D_WindowName->Text ).ToPointer();
		GlobalData.WindowName = _strdup( stringPointer );
//		ShutDownAllDataProcessing();
//		InitDataProcessing();
//		StartDataProcessing();
	}

	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) 
	{

	}
	private: System::Void FC_B_WindowRefresh_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		this->FC_D_WindowName->Items->Clear();
		FC_D_WindowName->Items->Add( "Desktop" );
		FC_D_WindowName->SelectedIndex = 0;
		ProcessNameList ProcessList;
		GetProcessNameList( &ProcessList );
		for( std::list<char*>::iterator itr = ProcessList.Names.begin(); itr != ProcessList.Names.end(); itr++ )
		{
			char *Name = *itr;
			String^ clistr = gcnew String( Name );
			this->FC_D_WindowName->Items->Add( clistr );
		}
	}
};
}

