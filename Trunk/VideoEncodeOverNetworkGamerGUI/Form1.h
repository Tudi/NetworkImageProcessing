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
	private: System::Windows::Forms::Label^  FC_L_StartX;
	private: System::Windows::Forms::TextBox^  FC_E_StartX;
	private: System::Windows::Forms::TextBox^  FC_E_StartY;
	private: System::Windows::Forms::Label^  FC_L_StartY;
	private: System::Windows::Forms::TextBox^  FC_E_ForceWidth;
	private: System::Windows::Forms::Label^  FC_L_ForcedWidth;
	private: System::Windows::Forms::TextBox^  FC_E_ForceHeight;
	private: System::Windows::Forms::Label^  FC_L_ForceHeight;
	private: System::Windows::Forms::CheckBox^  FC_C_AudioEnable;
	private: System::Windows::Forms::Label^  FC_L_AudioEnable;
	private: System::Windows::Forms::TextBox^  FC_E_AudioPort;
	private: System::Windows::Forms::Label^  FC_L_AudioPort;
	float				TargetWindowAspectRatio;
	BackgroundWorker	^m_oWorker;
public: 
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
            m_oWorker = gcnew BackgroundWorker();
			m_oWorker->DoWork += gcnew DoWorkEventHandler( this, &VideoEncodeOverNetworkGamerGUI::Form1::m_oWorker_DoWork );
            m_oWorker->ProgressChanged += gcnew ProgressChangedEventHandler( this, &VideoEncodeOverNetworkGamerGUI::Form1::m_oWorker_ProgressChanged );
//            m_oWorker->RunWorkerCompleted += gcnew RunWorkerCompletedEventHandler( this, &VideoEncodeOverNetworkGamerGUI::Form1::m_oWorker_RunWorkerCompleted );
            m_oWorker->WorkerReportsProgress = true;
            m_oWorker->WorkerSupportsCancellation = true;

			this->FC_D_Resize->SelectedIndex = 1;
			this->FC_D_BPP->SelectedIndex = 2;
			this->FC_D_Compress->SelectedIndex = 4;

			TargetWindowAspectRatio = 640.0f / 480.0f;
			System::EventArgs ^NoArgs;
			FC_B_WindowRefresh_Click( nullptr, NoArgs );

//			LoadSettingsFromFile( "Config.txt" );
		}

//        void m_oWorker_RunWorkerCompleted(Object ^sender, RunWorkerCompletedEventArgs ^e){}
        void m_oWorker_ProgressChanged(Object ^sender, ProgressChangedEventArgs ^e)
        {
			char StatVal[50];
			String^ clistr;

			sprintf_s( StatVal, 50, "%d", GlobalData.StatAvgFPS );
			clistr = gcnew String( StatVal );
			this->FC_L_Stat_FPS1->Text = clistr;

			if( GlobalData.StatAvgNetworkUsage > 1024 )
				sprintf_s( StatVal, 50, "%.3f k", (float)GlobalData.StatAvgNetworkUsage / 1024.0f );
			else if( GlobalData.StatAvgNetworkUsage > 1024 * 1024 )
				sprintf_s( StatVal, 50, "%.3f", (float)GlobalData.StatAvgNetworkUsage / 1024.0f / 1024.0f );
			else
				sprintf_s( StatVal, 50, "%d", GlobalData.StatAvgNetworkUsage );
			clistr = gcnew String( StatVal );
			this->FC_L_Stat_BW1->Text = clistr;

			sprintf_s( StatVal, 50, "%.2f", GlobalData.StatLastCPUUsage );
			clistr = gcnew String( StatVal );
			this->FC_L_Stat_CPU1->Text = clistr;

        }
        void m_oWorker_DoWork(Object ^sender, DoWorkEventArgs ^e)
        {
            while( 1 )
            {
                Sleep( 1000 );
                if (m_oWorker->CancellationPending)
                {
                    e->Cancel = true;
                    m_oWorker->ReportProgress( 0 );
                    return;
                }
                m_oWorker->ReportProgress( 0 );
            }
        }

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			m_oWorker->CancelAsync();
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
	private: System::Windows::Forms::Label^  FC_L_Stat_FPS1;

	private: System::Windows::Forms::Label^  FC_L_Stat_BW0;
	private: System::Windows::Forms::Label^  FC_L_Stat_BW1;

	private: System::Windows::Forms::Button^  FC_B_Apply;

	private: System::Windows::Forms::Label^  FC_L_InclBorder;
	private: System::Windows::Forms::CheckBox^  FC_C_BorderIncl;
	private: System::Windows::Forms::Label^  FC_L_ARW0;


	private: System::Windows::Forms::Label^  FC_L_ARW1;
	private: System::Windows::Forms::Label^  FC_L_ARH0;
	private: System::Windows::Forms::Label^  FC_L_ARH1;
	private: System::Windows::Forms::Label^  FC_L_CPU0;
private: System::Windows::Forms::Label^  FC_L_Stat_CPU1;


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
			this->FC_L_Stat_FPS1 = (gcnew System::Windows::Forms::Label());
			this->FC_L_Stat_BW0 = (gcnew System::Windows::Forms::Label());
			this->FC_L_Stat_BW1 = (gcnew System::Windows::Forms::Label());
			this->FC_B_Apply = (gcnew System::Windows::Forms::Button());
			this->FC_L_InclBorder = (gcnew System::Windows::Forms::Label());
			this->FC_C_BorderIncl = (gcnew System::Windows::Forms::CheckBox());
			this->FC_L_ARW0 = (gcnew System::Windows::Forms::Label());
			this->FC_L_ARW1 = (gcnew System::Windows::Forms::Label());
			this->FC_L_ARH0 = (gcnew System::Windows::Forms::Label());
			this->FC_L_ARH1 = (gcnew System::Windows::Forms::Label());
			this->FC_L_CPU0 = (gcnew System::Windows::Forms::Label());
			this->FC_L_Stat_CPU1 = (gcnew System::Windows::Forms::Label());
			this->FC_L_Port = (gcnew System::Windows::Forms::Label());
			this->FC_E_Port = (gcnew System::Windows::Forms::TextBox());
			this->FC_B_WindowRefresh = (gcnew System::Windows::Forms::Button());
			this->FC_L_StartX = (gcnew System::Windows::Forms::Label());
			this->FC_E_StartX = (gcnew System::Windows::Forms::TextBox());
			this->FC_E_StartY = (gcnew System::Windows::Forms::TextBox());
			this->FC_L_StartY = (gcnew System::Windows::Forms::Label());
			this->FC_E_ForceWidth = (gcnew System::Windows::Forms::TextBox());
			this->FC_L_ForcedWidth = (gcnew System::Windows::Forms::Label());
			this->FC_E_ForceHeight = (gcnew System::Windows::Forms::TextBox());
			this->FC_L_ForceHeight = (gcnew System::Windows::Forms::Label());
			this->FC_C_AudioEnable = (gcnew System::Windows::Forms::CheckBox());
			this->FC_L_AudioEnable = (gcnew System::Windows::Forms::Label());
			this->FC_E_AudioPort = (gcnew System::Windows::Forms::TextBox());
			this->FC_L_AudioPort = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// FC_D_WindowName
			// 
			this->FC_D_WindowName->FormattingEnabled = true;
			this->FC_D_WindowName->ImeMode = System::Windows::Forms::ImeMode::NoControl;
			this->FC_D_WindowName->Location = System::Drawing::Point(131, -1);
			this->FC_D_WindowName->MaxDropDownItems = 60;
			this->FC_D_WindowName->Name = L"FC_D_WindowName";
			this->FC_D_WindowName->Size = System::Drawing::Size(189, 21);
			this->FC_D_WindowName->TabIndex = 0;
			this->FC_D_WindowName->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::FC_D_WindowName_SelectedIndexChanged);
			// 
			// FC_L_Resize
			// 
			this->FC_L_Resize->AutoSize = true;
			this->FC_L_Resize->Location = System::Drawing::Point(1, 159);
			this->FC_L_Resize->Name = L"FC_L_Resize";
			this->FC_L_Resize->Size = System::Drawing::Size(73, 13);
			this->FC_L_Resize->TabIndex = 3;
			this->FC_L_Resize->Text = L"Resize Speed";
			// 
			// FC_L_Width
			// 
			this->FC_L_Width->AutoSize = true;
			this->FC_L_Width->Location = System::Drawing::Point(1, 185);
			this->FC_L_Width->Name = L"FC_L_Width";
			this->FC_L_Width->Size = System::Drawing::Size(35, 13);
			this->FC_L_Width->TabIndex = 4;
			this->FC_L_Width->Text = L"Width";
			// 
			// FC_E_Width
			// 
			this->FC_E_Width->Location = System::Drawing::Point(131, 178);
			this->FC_E_Width->MaxLength = 6;
			this->FC_E_Width->Name = L"FC_E_Width";
			this->FC_E_Width->Size = System::Drawing::Size(121, 20);
			this->FC_E_Width->TabIndex = 5;
			this->FC_E_Width->Text = L"640";
			this->FC_E_Width->TextChanged += gcnew System::EventHandler(this, &Form1::FC_E_Width_TextChanged);
			// 
			// FC_L_Height
			// 
			this->FC_L_Height->AutoSize = true;
			this->FC_L_Height->Location = System::Drawing::Point(1, 233);
			this->FC_L_Height->Name = L"FC_L_Height";
			this->FC_L_Height->Size = System::Drawing::Size(38, 13);
			this->FC_L_Height->TabIndex = 6;
			this->FC_L_Height->Text = L"Height";
			// 
			// FC_E_Height
			// 
			this->FC_E_Height->Location = System::Drawing::Point(131, 230);
			this->FC_E_Height->MaxLength = 6;
			this->FC_E_Height->Name = L"FC_E_Height";
			this->FC_E_Height->Size = System::Drawing::Size(121, 20);
			this->FC_E_Height->TabIndex = 7;
			this->FC_E_Height->Text = L"480";
			this->FC_E_Height->TextChanged += gcnew System::EventHandler(this, &Form1::FC_E_Height_TextChanged);
			// 
			// FC_D_Resize
			// 
			this->FC_D_Resize->FormattingEnabled = true;
			this->FC_D_Resize->ImeMode = System::Windows::Forms::ImeMode::NoControl;
			this->FC_D_Resize->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"No Resize", L"Resize"});
			this->FC_D_Resize->Location = System::Drawing::Point(131, 151);
			this->FC_D_Resize->Name = L"FC_D_Resize";
			this->FC_D_Resize->Size = System::Drawing::Size(121, 21);
			this->FC_D_Resize->TabIndex = 8;
			// 
			// FC_L_BPP
			// 
			this->FC_L_BPP->AutoSize = true;
			this->FC_L_BPP->Location = System::Drawing::Point(1, 282);
			this->FC_L_BPP->Name = L"FC_L_BPP";
			this->FC_L_BPP->Size = System::Drawing::Size(62, 13);
			this->FC_L_BPP->TabIndex = 9;
			this->FC_L_BPP->Text = L"BitsPerPixel";
			// 
			// FC_L_Compression
			// 
			this->FC_L_Compression->AutoSize = true;
			this->FC_L_Compression->Location = System::Drawing::Point(1, 309);
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
			this->FC_D_Compress->Location = System::Drawing::Point(131, 301);
			this->FC_D_Compress->Name = L"FC_D_Compress";
			this->FC_D_Compress->Size = System::Drawing::Size(121, 21);
			this->FC_D_Compress->TabIndex = 12;
			// 
			// FC_L_FPS
			// 
			this->FC_L_FPS->AutoSize = true;
			this->FC_L_FPS->Location = System::Drawing::Point(1, 335);
			this->FC_L_FPS->Name = L"FC_L_FPS";
			this->FC_L_FPS->Size = System::Drawing::Size(48, 13);
			this->FC_L_FPS->TabIndex = 13;
			this->FC_L_FPS->Text = L"FPSLimit";
			// 
			// FC_E_FPS
			// 
			this->FC_E_FPS->Location = System::Drawing::Point(131, 328);
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
			this->FC_D_BPP->Location = System::Drawing::Point(131, 275);
			this->FC_D_BPP->Name = L"FC_D_BPP";
			this->FC_D_BPP->Size = System::Drawing::Size(121, 21);
			this->FC_D_BPP->TabIndex = 15;
			// 
			// FC_L_Stat_FPS0
			// 
			this->FC_L_Stat_FPS0->AutoSize = true;
			this->FC_L_Stat_FPS0->Location = System::Drawing::Point(1, 430);
			this->FC_L_Stat_FPS0->Name = L"FC_L_Stat_FPS0";
			this->FC_L_Stat_FPS0->Size = System::Drawing::Size(59, 13);
			this->FC_L_Stat_FPS0->TabIndex = 16;
			this->FC_L_Stat_FPS0->Text = L"FPS actual";
			// 
			// FC_L_Stat_FPS1
			// 
			this->FC_L_Stat_FPS1->AutoSize = true;
			this->FC_L_Stat_FPS1->Location = System::Drawing::Point(128, 430);
			this->FC_L_Stat_FPS1->Name = L"FC_L_Stat_FPS1";
			this->FC_L_Stat_FPS1->Size = System::Drawing::Size(13, 13);
			this->FC_L_Stat_FPS1->TabIndex = 17;
			this->FC_L_Stat_FPS1->Text = L"0";
			// 
			// FC_L_Stat_BW0
			// 
			this->FC_L_Stat_BW0->AutoSize = true;
			this->FC_L_Stat_BW0->Location = System::Drawing::Point(1, 452);
			this->FC_L_Stat_BW0->Name = L"FC_L_Stat_BW0";
			this->FC_L_Stat_BW0->Size = System::Drawing::Size(79, 13);
			this->FC_L_Stat_BW0->TabIndex = 18;
			this->FC_L_Stat_BW0->Text = L"Network usage";
			// 
			// FC_L_Stat_BW1
			// 
			this->FC_L_Stat_BW1->AutoSize = true;
			this->FC_L_Stat_BW1->Location = System::Drawing::Point(128, 452);
			this->FC_L_Stat_BW1->Name = L"FC_L_Stat_BW1";
			this->FC_L_Stat_BW1->Size = System::Drawing::Size(13, 13);
			this->FC_L_Stat_BW1->TabIndex = 19;
			this->FC_L_Stat_BW1->Text = L"0";
			// 
			// FC_B_Apply
			// 
			this->FC_B_Apply->Location = System::Drawing::Point(94, 497);
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
			this->FC_L_InclBorder->Size = System::Drawing::Size(114, 13);
			this->FC_L_InclBorder->TabIndex = 21;
			this->FC_L_InclBorder->Text = L"Include window border";
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
			this->FC_L_ARW0->Location = System::Drawing::Point(1, 209);
			this->FC_L_ARW0->Name = L"FC_L_ARW0";
			this->FC_L_ARW0->Size = System::Drawing::Size(93, 13);
			this->FC_L_ARW0->TabIndex = 23;
			this->FC_L_ARW0->Text = L"Width aspect ratio";
			// 
			// FC_L_ARW1
			// 
			this->FC_L_ARW1->AutoSize = true;
			this->FC_L_ARW1->Location = System::Drawing::Point(128, 209);
			this->FC_L_ARW1->Name = L"FC_L_ARW1";
			this->FC_L_ARW1->Size = System::Drawing::Size(13, 13);
			this->FC_L_ARW1->TabIndex = 24;
			this->FC_L_ARW1->Text = L"0";
			// 
			// FC_L_ARH0
			// 
			this->FC_L_ARH0->AutoSize = true;
			this->FC_L_ARH0->Location = System::Drawing::Point(1, 256);
			this->FC_L_ARH0->Name = L"FC_L_ARH0";
			this->FC_L_ARH0->Size = System::Drawing::Size(96, 13);
			this->FC_L_ARH0->TabIndex = 25;
			this->FC_L_ARH0->Text = L"Height aspect ratio";
			// 
			// FC_L_ARH1
			// 
			this->FC_L_ARH1->AutoSize = true;
			this->FC_L_ARH1->Location = System::Drawing::Point(128, 256);
			this->FC_L_ARH1->Name = L"FC_L_ARH1";
			this->FC_L_ARH1->Size = System::Drawing::Size(13, 13);
			this->FC_L_ARH1->TabIndex = 26;
			this->FC_L_ARH1->Text = L"0";
			// 
			// FC_L_CPU0
			// 
			this->FC_L_CPU0->AutoSize = true;
			this->FC_L_CPU0->Location = System::Drawing::Point(1, 474);
			this->FC_L_CPU0->Name = L"FC_L_CPU0";
			this->FC_L_CPU0->Size = System::Drawing::Size(61, 13);
			this->FC_L_CPU0->TabIndex = 27;
			this->FC_L_CPU0->Text = L"CPU usage";
			// 
			// FC_L_Stat_CPU1
			// 
			this->FC_L_Stat_CPU1->AutoSize = true;
			this->FC_L_Stat_CPU1->Location = System::Drawing::Point(128, 474);
			this->FC_L_Stat_CPU1->Name = L"FC_L_Stat_CPU1";
			this->FC_L_Stat_CPU1->Size = System::Drawing::Size(13, 13);
			this->FC_L_Stat_CPU1->TabIndex = 28;
			this->FC_L_Stat_CPU1->Text = L"0";
			// 
			// FC_L_Port
			// 
			this->FC_L_Port->AutoSize = true;
			this->FC_L_Port->Location = System::Drawing::Point(1, 358);
			this->FC_L_Port->Name = L"FC_L_Port";
			this->FC_L_Port->Size = System::Drawing::Size(69, 13);
			this->FC_L_Port->TabIndex = 29;
			this->FC_L_Port->Text = L"Network Port";
			// 
			// FC_E_Port
			// 
			this->FC_E_Port->Location = System::Drawing::Point(131, 355);
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
			// FC_L_StartX
			// 
			this->FC_L_StartX->AutoSize = true;
			this->FC_L_StartX->Location = System::Drawing::Point(1, 53);
			this->FC_L_StartX->Name = L"FC_L_StartX";
			this->FC_L_StartX->Size = System::Drawing::Size(77, 13);
			this->FC_L_StartX->TabIndex = 32;
			this->FC_L_StartX->Text = L"Custom Start X";
			// 
			// FC_E_StartX
			// 
			this->FC_E_StartX->Location = System::Drawing::Point(131, 46);
			this->FC_E_StartX->Name = L"FC_E_StartX";
			this->FC_E_StartX->Size = System::Drawing::Size(100, 20);
			this->FC_E_StartX->TabIndex = 33;
			this->FC_E_StartX->Text = L"0";
			// 
			// FC_E_StartY
			// 
			this->FC_E_StartY->Location = System::Drawing::Point(131, 72);
			this->FC_E_StartY->Name = L"FC_E_StartY";
			this->FC_E_StartY->Size = System::Drawing::Size(100, 20);
			this->FC_E_StartY->TabIndex = 34;
			this->FC_E_StartY->Text = L"0";
			// 
			// FC_L_StartY
			// 
			this->FC_L_StartY->AutoSize = true;
			this->FC_L_StartY->Location = System::Drawing::Point(1, 79);
			this->FC_L_StartY->Name = L"FC_L_StartY";
			this->FC_L_StartY->Size = System::Drawing::Size(77, 13);
			this->FC_L_StartY->TabIndex = 35;
			this->FC_L_StartY->Text = L"Custom Start Y";
			// 
			// FC_E_ForceWidth
			// 
			this->FC_E_ForceWidth->Location = System::Drawing::Point(131, 98);
			this->FC_E_ForceWidth->Name = L"FC_E_ForceWidth";
			this->FC_E_ForceWidth->Size = System::Drawing::Size(100, 20);
			this->FC_E_ForceWidth->TabIndex = 36;
			this->FC_E_ForceWidth->Text = L"0";
			// 
			// FC_L_ForcedWidth
			// 
			this->FC_L_ForcedWidth->AutoSize = true;
			this->FC_L_ForcedWidth->Location = System::Drawing::Point(1, 105);
			this->FC_L_ForcedWidth->Name = L"FC_L_ForcedWidth";
			this->FC_L_ForcedWidth->Size = System::Drawing::Size(73, 13);
			this->FC_L_ForcedWidth->TabIndex = 37;
			this->FC_L_ForcedWidth->Text = L"Custom Width";
			// 
			// FC_E_ForceHeight
			// 
			this->FC_E_ForceHeight->Location = System::Drawing::Point(131, 124);
			this->FC_E_ForceHeight->Name = L"FC_E_ForceHeight";
			this->FC_E_ForceHeight->Size = System::Drawing::Size(100, 20);
			this->FC_E_ForceHeight->TabIndex = 38;
			this->FC_E_ForceHeight->Text = L"0";
			// 
			// FC_L_ForceHeight
			// 
			this->FC_L_ForceHeight->AutoSize = true;
			this->FC_L_ForceHeight->Location = System::Drawing::Point(1, 127);
			this->FC_L_ForceHeight->Name = L"FC_L_ForceHeight";
			this->FC_L_ForceHeight->Size = System::Drawing::Size(76, 13);
			this->FC_L_ForceHeight->TabIndex = 39;
			this->FC_L_ForceHeight->Text = L"Custom Height";
			// 
			// FC_C_AudioEnable
			// 
			this->FC_C_AudioEnable->AutoSize = true;
			this->FC_C_AudioEnable->Checked = true;
			this->FC_C_AudioEnable->CheckState = System::Windows::Forms::CheckState::Checked;
			this->FC_C_AudioEnable->Location = System::Drawing::Point(131, 381);
			this->FC_C_AudioEnable->Name = L"FC_C_AudioEnable";
			this->FC_C_AudioEnable->Size = System::Drawing::Size(15, 14);
			this->FC_C_AudioEnable->TabIndex = 40;
			this->FC_C_AudioEnable->UseVisualStyleBackColor = true;
			// 
			// FC_L_AudioEnable
			// 
			this->FC_L_AudioEnable->AutoSize = true;
			this->FC_L_AudioEnable->Location = System::Drawing::Point(1, 382);
			this->FC_L_AudioEnable->Name = L"FC_L_AudioEnable";
			this->FC_L_AudioEnable->Size = System::Drawing::Size(70, 13);
			this->FC_L_AudioEnable->TabIndex = 41;
			this->FC_L_AudioEnable->Text = L"Stream Audio";
			// 
			// FC_E_AudioPort
			// 
			this->FC_E_AudioPort->Location = System::Drawing::Point(131, 401);
			this->FC_E_AudioPort->Name = L"FC_E_AudioPort";
			this->FC_E_AudioPort->Size = System::Drawing::Size(121, 20);
			this->FC_E_AudioPort->TabIndex = 42;
			this->FC_E_AudioPort->Text = L"6889";
			// 
			// FC_L_AudioPort
			// 
			this->FC_L_AudioPort->AutoSize = true;
			this->FC_L_AudioPort->Location = System::Drawing::Point(1, 404);
			this->FC_L_AudioPort->Name = L"FC_L_AudioPort";
			this->FC_L_AudioPort->Size = System::Drawing::Size(56, 13);
			this->FC_L_AudioPort->TabIndex = 43;
			this->FC_L_AudioPort->Text = L"Audio Port";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(324, 527);
			this->Controls->Add(this->FC_L_AudioPort);
			this->Controls->Add(this->FC_E_AudioPort);
			this->Controls->Add(this->FC_L_AudioEnable);
			this->Controls->Add(this->FC_C_AudioEnable);
			this->Controls->Add(this->FC_L_ForceHeight);
			this->Controls->Add(this->FC_E_ForceHeight);
			this->Controls->Add(this->FC_L_ForcedWidth);
			this->Controls->Add(this->FC_E_ForceWidth);
			this->Controls->Add(this->FC_L_StartY);
			this->Controls->Add(this->FC_E_StartY);
			this->Controls->Add(this->FC_E_StartX);
			this->Controls->Add(this->FC_L_StartX);
			this->Controls->Add(this->FC_B_WindowRefresh);
			this->Controls->Add(this->FC_E_Port);
			this->Controls->Add(this->FC_L_Port);
			this->Controls->Add(this->FC_L_Stat_CPU1);
			this->Controls->Add(this->FC_L_CPU0);
			this->Controls->Add(this->FC_L_ARH1);
			this->Controls->Add(this->FC_L_ARH0);
			this->Controls->Add(this->FC_L_ARW1);
			this->Controls->Add(this->FC_L_ARW0);
			this->Controls->Add(this->FC_C_BorderIncl);
			this->Controls->Add(this->FC_L_InclBorder);
			this->Controls->Add(this->FC_B_Apply);
			this->Controls->Add(this->FC_L_Stat_BW1);
			this->Controls->Add(this->FC_L_Stat_BW0);
			this->Controls->Add(this->FC_L_Stat_FPS1);
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
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: System::Void FC_B_Apply_Click(System::Object^  sender, System::EventArgs^  e) 
	{
//		MessageBoxW(NULL, L"Error msg", L"Error", MB_OK );

		ShutDownAllDataProcessing();
		AudioShutDownAllDataProcessing();

		char *stringPointer = (char*) Marshal::StringToHGlobalAnsi( this->FC_D_WindowName->Text ).ToPointer();
		GlobalData.WindowName = _strdup( stringPointer );	//this can be anything and it's ok even if it is null. We will capture desktop

		stringPointer = (char*) Marshal::StringToHGlobalAnsi( this->FC_E_StartX->Text ).ToPointer();
		GlobalData.StartX = atoi( stringPointer );
		stringPointer = (char*) Marshal::StringToHGlobalAnsi( this->FC_E_StartY->Text ).ToPointer();
		GlobalData.StartY = atoi( stringPointer );
		stringPointer = (char*) Marshal::StringToHGlobalAnsi( this->FC_E_ForceWidth->Text ).ToPointer();
		GlobalData.ForcedWidth = atoi( stringPointer );
		stringPointer = (char*) Marshal::StringToHGlobalAnsi( this->FC_E_ForceHeight->Text ).ToPointer();
		GlobalData.ForcedHeight = atoi( stringPointer );

		if( this->FC_D_Resize->SelectedIndex == 1 )	//fast resize
		{
			GlobalData.ResizeMethod = 1;	//only one is implemented anyway
			stringPointer = (char*) Marshal::StringToHGlobalAnsi( this->FC_E_Width->Text ).ToPointer();
			GlobalData.ResizeWidth = atoi( stringPointer );
			stringPointer = (char*) Marshal::StringToHGlobalAnsi( this->FC_E_Height->Text ).ToPointer();
			GlobalData.ResizeHeight = atoi( stringPointer );
		}
		else 
			//if( this->FC_D_Resize->SelectedIndex == 0 )	//no resize
		{
			GlobalData.ResizeHeight = -1;
			GlobalData.ResizeWidth = -1;
			GlobalData.ResizeMethod = -1;
		}

		if( this->FC_D_BPP->SelectedIndex == 0 )
			GlobalData.ColorFilterMask = 0x00FFFFFF; //8 bpp
		else if( this->FC_D_BPP->SelectedIndex == 1 )
			GlobalData.ColorFilterMask = 0x00FEFEFE; //7 bpp
		else if( this->FC_D_BPP->SelectedIndex == 2 )
			GlobalData.ColorFilterMask = 0x00FCFCFC; //6 bpp
		else if( this->FC_D_BPP->SelectedIndex == 3 )
			GlobalData.ColorFilterMask = 0x00F8F8F8; //5 bpp
		else if( this->FC_D_BPP->SelectedIndex == 4 )
			GlobalData.ColorFilterMask = 0x00F0F0F0; //4 bpp
		else
			GlobalData.ColorFilterMask = 0x00FFFFFF; //8 bpp

		if( this->FC_D_Compress->SelectedIndex == 0 )
			GlobalData.CompressionStrength = 0;	//no compression
		else if( this->FC_D_Compress->SelectedIndex == 1 )
			GlobalData.CompressionStrength = 1;	
		else if( this->FC_D_Compress->SelectedIndex == 2 )
			GlobalData.CompressionStrength = 2;	
		else if( this->FC_D_Compress->SelectedIndex == 3 )
			GlobalData.CompressionStrength = 3;	
		else if( this->FC_D_Compress->SelectedIndex == 4 )
			GlobalData.CompressionStrength = 4;	
		else if( this->FC_D_Compress->SelectedIndex == 5 )
			GlobalData.CompressionStrength = 5;	
		else if( this->FC_D_Compress->SelectedIndex == 6 )
			GlobalData.CompressionStrength = 6;	
		else if( this->FC_D_Compress->SelectedIndex == 7 )
			GlobalData.CompressionStrength = 7;	
		else if( this->FC_D_Compress->SelectedIndex == 8 )
			GlobalData.CompressionStrength = 8;	
		else if( this->FC_D_Compress->SelectedIndex == 9 )
			GlobalData.CompressionStrength = 9;	
		else
			GlobalData.CompressionStrength = 3;		//meh compression

		stringPointer = (char*) Marshal::StringToHGlobalAnsi( this->FC_E_FPS->Text ).ToPointer();
		GlobalData.FPSLimit = atoi( stringPointer );
		if( GlobalData.FPSLimit <= 0 )
			GlobalData.FPSLimit = 15;
		else if( GlobalData.FPSLimit > 60 ) //be realistic, 24 fps is a realistic limit
			GlobalData.FPSLimit = 15;

		stringPointer = (char*) Marshal::StringToHGlobalAnsi( this->FC_E_Port->Text ).ToPointer();
		if( GlobalData.VideoNetworkPort == NULL )
		{
			this->FC_E_Port->ReadOnly = true;
			//start statistics update thread ( only once )
//			_beginthread( ListenAndAcceptNewClients, 0, (void*)NULL );
//			TimerCallback^ tcb = gcnew TimerCallback( statusChecker, &StatusChecker::CheckStatus);
			m_oWorker->RunWorkerAsync();
		}
		GlobalData.VideoNetworkPort = _strdup( stringPointer );

		if( this->FC_C_BorderIncl->Checked )
			GlobalData.IncludeWindowBorder = 1;
		else
			GlobalData.IncludeWindowBorder = 0;

		if( this->FC_C_AudioEnable->Checked )
		{
			stringPointer = (char*) Marshal::StringToHGlobalAnsi( this->FC_E_AudioPort->Text ).ToPointer();
			GlobalData.AudioNetworkPort = _strdup( stringPointer );
		}
		else
		{
			GlobalData.AudioNetworkIP = NULL;
			GlobalData.AudioNetworkPort = NULL;
		}

		InitDataProcessing();
		AudioInitDataProcessing();

		StartDataProcessing();
		AudioStartDataProcessing();
	}

	private: System::Void FC_B_WindowRefresh_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		FC_D_WindowName->Items->Clear();
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
	private: System::Void FC_D_WindowName_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
	{
		char *stringPointer = (char*) Marshal::StringToHGlobalAnsi( this->FC_D_WindowName->Text ).ToPointer();
		HWND SelectedHWND = FindWindowWithNameNonThreaded( stringPointer );
		if( SelectedHWND == NULL )
			SelectedHWND = GetDesktopWindow();
		CRect rect;
		::GetWindowRect( SelectedHWND, rect );		
		TargetWindowAspectRatio = (float)rect.Width() / (float)rect.Height();
		if( TargetWindowAspectRatio == 0.0 )
			TargetWindowAspectRatio = 1;
	}
	private: System::Void FC_E_Width_TextChanged(System::Object^  sender, System::EventArgs^  e) 
	{
		char *stringPointer = (char*) Marshal::StringToHGlobalAnsi( this->FC_E_Width->Text ).ToPointer();
		int WidthEntered = atoi( stringPointer );
		int SuggestedHeight = (int)( (float)WidthEntered / TargetWindowAspectRatio );

		char StringBuffer[50];
		sprintf_s( StringBuffer, 50, "%d", SuggestedHeight );
		String^ clistr = gcnew String( StringBuffer );

		this->FC_L_ARH1->Text = clistr;
	}
	private: System::Void FC_E_Height_TextChanged(System::Object^  sender, System::EventArgs^  e) 
	{
		char *stringPointer = (char*) Marshal::StringToHGlobalAnsi( this->FC_E_Height->Text ).ToPointer();
		int HeightEntered = atoi( stringPointer );
		int SuggestedWidth = (int)( HeightEntered * TargetWindowAspectRatio );

		char StringBuffer[50];
		sprintf_s( StringBuffer, 50, "%d", SuggestedWidth );
		String^ clistr = gcnew String( StringBuffer );

		this->FC_L_ARW1->Text = clistr;
	}
};
}

