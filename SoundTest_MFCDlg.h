
// SoundTest_MFCDlg.h : ͷ�ļ�
//

#pragma once

#include "xunfeiSDK.h"
#include "afxwin.h"
#include "afxeditbrowsectrl.h"

// CSoundTest_MFCDlg �Ի���
class CSoundTest_MFCDlg : public CDialogEx
{
// ����
public:
	CSoundTest_MFCDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SOUNDTEST_MFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	bool recording;
	CString des_path;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnPlayTts();

	SoundTest::XunFeiSDK xunfei;
	CEdit m_edit_txt_speak;
	afx_msg void OnBnClickedBtnSpack();
	afx_msg void OnBnClickedBtnSpchRecg();
	CMFCEditBrowseCtrl m_browse;
};
