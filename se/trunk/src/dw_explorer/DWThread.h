
/********************************************************************
	created:	2009/05/13
	created:	13:5:2009   22:32
	filename: 	d:\lidengwang\se\trunk\src\dw_explorer\DWThread.h
	file path:	d:\lidengwang\se\trunk\src\dw_explorer
	file base:	DWThread
	file ext:	h
	author:		lidengwang@s-zone.com
	
	purpose:	
*********************************************************************/

#pragma once


class CDWThread
{
public:
    CDWThread()
    {
        m_dwThreadId = 0;
        m_hThread    = NULL;
    }

    ~CDWThread()
    {
        m_dwThreadId = 0;
        if ( m_hThread != NULL )
            CloseHandle(m_hThread);
        m_hThread = NULL;
    }

    HANDLE CreateThread( 
        LPSECURITY_ATTRIBUTES lpThreadAttributes = 0, 
        SIZE_T  dwStackSize = 0,
        WORD    dwCreationFlags = 0 )
    {
        m_hThread = ::CreateThread( lpThreadAttributes,
            dwStackSize, 
            _DWThreadFunc,
            this,
            dwCreationFlags, 
            &m_dwThreadId);

        return m_hThread;
    }

    DWORD ResumeThread()
    {
        return ::ResumeThread(m_hThread);
    }

    DWORD SuspendThread()
    {
        return ::SuspendThread(m_hThread);
    }


protected:

    virtual DWORD _RunThreadFunc()
    {
        return 0;
    }

    static DWORD WINAPI _DWThreadFunc( LPVOID p )
    {
        CDWThread* pthis = (CDWThread*)p;
        return pthis->_RunThreadFunc();
    }
    
    DWORD  m_dwThreadId;
    HANDLE m_hThread;

};