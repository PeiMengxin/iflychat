
// SoundTest_MFC.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSoundTest_MFCApp: 
// �йش����ʵ�֣������ SoundTest_MFC.cpp
//

class CSoundTest_MFCApp : public CWinApp
{
public:
	CSoundTest_MFCApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSoundTest_MFCApp theApp;