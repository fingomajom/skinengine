/********************************************************************
	created:	2009/05/02
	created:	2:5:2009   12:30
	filename: 	d:\lidengwang\se\trunk\src\dw_explorer\DWWebWnd.h
	file path:	d:\lidengwang\se\trunk\src\dw_explorer
	file base:	DWWebWnd
	file ext:	h
	author:		lidengwang@s-zone.com
	
	purpose:	
*********************************************************************/

class CDWWebWnd : 
    public CAxWindow,
    public CMessageFilter
{
public:

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
            (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
            return FALSE;

        // give HTML page a chance to translate this message
        return (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);
    }

    //BEGIN_MSG_MAP(CDWWebWnd)

    //    MESSAGE_HANDLER(WM_CREATE , OnCreate )

    //END_MSG_MAP()

    CComPtr<IWebBrowser2>	m_spWebBrowser;


    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CMessageLoop* pLoop = _Module.GetMessageLoop();
        ATLASSERT(pLoop != NULL);
        pLoop->AddMessageFilter(this);

        HRESULT hr = S_OK;
        if ( m_spWebBrowser == NULL )
        {
            WCHAR wszCLSID[50] = { 0 };

            StringFromGUID2(CLSID_WebBrowser, wszCLSID, sizeof(wszCLSID) / sizeof(wszCLSID[0])); 

            hr = CreateControlEx( wszCLSID );
            if (FAILED(hr))
                return FALSE;

            hr = QueryControl( &m_spWebBrowser);
            if (FAILED(hr))
                return FALSE;

            m_spWebBrowser->Navigate( L"http://www.baidu.com",0,0,0,0 );
        }


        return 0;
    }

};