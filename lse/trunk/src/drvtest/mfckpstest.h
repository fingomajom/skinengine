// mfckpstest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CmfckpstestApp:
// �йش����ʵ�֣������ mfckpstest.cpp
//

class CmfckpstestApp : public CWinApp
{
public:
	CmfckpstestApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CmfckpstestApp theApp;