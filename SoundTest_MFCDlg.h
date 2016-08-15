
// SoundTest_MFCDlg.h : 头文件
//

#pragma once

#include "xunfeiSDK.h"
#include "afxwin.h"
#include "afxeditbrowsectrl.h"

// CSoundTest_MFCDlg 对话框
class CSoundTest_MFCDlg : public CDialogEx
{
// 构造
public:
	CSoundTest_MFCDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SOUNDTEST_MFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	bool recording;
	CString des_path;

	// 生成的消息映射函数
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
