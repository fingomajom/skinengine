

#pragma once


class CDWMessageLoop : public CMessageLoop
{
public:

    int Run()
    {
        BOOL bDoIdle = TRUE;
        int nIdleCount = 0;
        BOOL bRet;

        for(;;)
        {
            while(bDoIdle && !::PeekMessage(&m_msg, NULL, 0, 0, PM_NOREMOVE))
            {
                if(!OnIdle(nIdleCount++))
                    bDoIdle = FALSE;
            }

            bRet = ::GetMessage(&m_msg, NULL, 0, 0);

            if(bRet == -1)
            {
                ATLTRACE2(atlTraceUI, 0, _T("::GetMessage returned -1 (error)\n"));
                continue;   // error, don't process
            }
            else if(!bRet)
            {
                ATLTRACE2(atlTraceUI, 0, _T("CMessageLoop::Run - exiting\n"));
                break;   // WM_QUIT, exit message loop
            }

            __try
            {
                if(!PreTranslateMessage(&m_msg))
                {
                    ::TranslateMessage(&m_msg);
                    ::DispatchMessage(&m_msg);
                }
            }
            __except(1)
            {
            }

            if(IsIdleMessage(&m_msg))
            {
                bDoIdle = TRUE;
                nIdleCount = 0;
            }
        }

        return (int)m_msg.wParam;
    }

};