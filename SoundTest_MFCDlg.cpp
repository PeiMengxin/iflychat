
// SoundTest_MFCDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
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


// CSoundTest_MFCDlg �Ի���



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


// CSoundTest_MFCDlg ��Ϣ�������

BOOL CSoundTest_MFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	if (-1 == xunfei.status())
	{
		MessageBox(_T("��ʼ��ʧ��"));
		return FALSE;
	}

	if (!(xunfei.Login()))
	{
		MessageBox(_T("��¼ʧ��"));
		return FALSE;
	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSoundTest_MFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSoundTest_MFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSoundTest_MFCDlg::OnBnClickedBtnPlayTts()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	CString text;
	m_edit_txt_speak.GetWindowText(text);
	// tts_status_lab->Text = "��ת�����ٲ�������";
	//set_xunfei_param();//��������
	// 
	SetDlgItemText(IDC_STATIC_STATE, CString("�����ϳ��У����Ե�..."));

	std::string temp((LPCTSTR)text);

	if (-1 == xunfei.TextToSpeech(temp))
	{
		MessageBox(_T("ת��ʧ��"));
	}
	else
	{
		SetDlgItemText(IDC_STATIC_STATE, CString("����������..."));
		xunfei.Play(xunfei.GetPcmName());
		//MessageBox(_T("bofang"));
	}

	SetDlgItemText(IDC_STATIC_STATE, CString("׼��"));
}


void CSoundTest_MFCDlg::OnBnClickedBtnSpack()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	//CString text;
	recording = !recording;

	if (recording)
	{
		SetDlgItemText(IDC_BTN_RECORD, CString("����˵��"));
		SetDlgItemText(IDC_STATIC_STATE, CString("¼����..."));
		GetDlgItemText(IDC_MFCEDITBROWSE, des_path);
	}
	if (!recording)
	{
		SetDlgItemText(IDC_BTN_RECORD, CString("���˵��"));
		SetDlgItemText(IDC_STATIC_STATE, CString("ʶ���У����Ե�..."));

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

		SetDlgItemText(IDC_STATIC_STATE, CString("׼��"));

	}

}


void CSoundTest_MFCDlg::OnBnClickedBtnSpchRecg()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	//CString des_path;
	GetDlgItemText(IDC_MFCEDITBROWSE, des_path);

	SetDlgItemText(IDC_STATIC_STATE, CString("ʶ���У����Ե�..."));

	string temp((LPCTSTR)des_path);

	string result = xunfei.SpeechToText(temp);

	m_edit_txt_speak.SetWindowText(CString(result.c_str()));

	SetDlgItemText(IDC_STATIC_STATE, CString("׼��"));
}
