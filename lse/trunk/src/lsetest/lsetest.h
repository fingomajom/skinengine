// lsetest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

#include <atlhttp.h>


// ClsetestApp:
// �йش����ʵ�֣������ lsetest.cpp
//

class ClsetestApp : public CWinApp
{
public:
	ClsetestApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern ClsetestApp theApp;