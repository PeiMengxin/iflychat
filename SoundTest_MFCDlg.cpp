
// SoundTest_MFCDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SoundTest_MFC.h"
#include "SoundTest_MFCDlg.h"
#include "afxdialogex.h"

#include "json/json.h"

#include <mmsystem.h>  
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSoundTest_MFCDlg 对话框



CSoundTest_MFCDlg::CSoundTest_MFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSoundTest_MFCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	recording = false;
}

void CSoundTest_MFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TXT_SPEAK, m_edit_txt_speak);
	DDX_Control(pDX, IDC_MFCEDITBROWSE, m_browse);
}

BEGIN_MESSAGE_MAP(CSoundTest_MFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_PLAY_TTS, &CSoundTest_MFCDlg::OnBnClickedBtnPlayTts)
	ON_BN_CLICKED(IDC_BTN_RECORD, &CSoundTest_MFCDlg::OnBnClickedBtnSpack)
	ON_BN_CLICKED(IDC_BTN_SPCH_RECG, &CSoundTest_MFCDlg::OnBnClickedBtnSpchRecg)
END_MESSAGE_MAP()


// CSoundTest_MFCDlg 消息处理程序

BOOL CSoundTest_MFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	if (-1 == xunfei.status())
	{
		MessageBox(_T("初始化失败"));
		return FALSE;
	}

	if (!(xunfei.Login()))
	{
		MessageBox(_T("登录失败"));
		return FALSE;
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSoundTest_MFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSoundTest_MFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSoundTest_MFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSoundTest_MFCDlg::OnBnClickedBtnPlayTts()
{
	// TODO:  在此添加控件通知处理程序代码

	CString text;
	m_edit_txt_speak.GetWindowText(text);
	// tts_status_lab->Text = "先转换，再播放语音";
	//set_xunfei_param();//参数设置
	// 
	SetDlgItemText(IDC_STATIC_STATE, CString("语音合成中，请稍等..."));

	std::string temp((LPCTSTR)text);

	if (-1 == xunfei.TextToSpeech(temp))
	{
		MessageBox(_T("转换失败"));
	}
	else
	{
		SetDlgItemText(IDC_STATIC_STATE, CString("语音播放中..."));
		xunfei.Play(xunfei.GetPcmName());
		//MessageBox(_T("bofang"));
	}

	SetDlgItemText(IDC_STATIC_STATE, CString("准备"));
}


void CSoundTest_MFCDlg::OnBnClickedBtnSpack()
{
	// TODO:  在此添加控件通知处理程序代码

	//CString text;
	recording = !recording;

	if (recording)
	{
		SetDlgItemText(IDC_BTN_RECORD, CString("结束说话"));
		SetDlgItemText(IDC_STATIC_STATE, CString("录音中..."));
		GetDlgItemText(IDC_MFCEDITBROWSE, des_path);
	}
	if (!recording)
	{
		SetDlgItemText(IDC_BTN_RECORD, CString("点击说话"));
		SetDlgItemText(IDC_STATIC_STATE, CString("识别中，请稍等..."));

		PlaySound(CString(des_path), NULL, SND_FILENAME | SND_ASYNC);
		
		string temp((LPCTSTR)des_path);

		string result = xunfei.Iat(temp);

		Json::Reader json_reader;
		Json::Value json_value;

		json_reader.parse(result, json_value);

		CString result_show("no data return ");

		if (!json_value["answer"].isNull())
		{
			result_show = json_value["answer"]["text"].asCString();
		}

		m_edit_txt_speak.SetWindowText(result_show);

		SetDlgItemText(IDC_STATIC_STATE, CString("准备"));

	}

}


void CSoundTest_MFCDlg::OnBnClickedBtnSpchRecg()
{
	// TODO:  在此添加控件通知处理程序代码

	//CString des_path;
	GetDlgItemText(IDC_MFCEDITBROWSE, des_path);

	SetDlgItemText(IDC_STATIC_STATE, CString("识别中，请稍等..."));

	string temp((LPCTSTR)des_path);

	string result = xunfei.SpeechToText(temp);

	m_edit_txt_speak.SetWindowText(CString(result.c_str()));

	SetDlgItemText(IDC_STATIC_STATE, CString("准备"));
}
