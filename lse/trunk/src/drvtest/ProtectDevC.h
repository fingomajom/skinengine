/********************************************************************
	created:	2008/10/18
	created:	18:10:2008   21:07
	filename: 	d:\lidengwang\lse\trunk\src\drvtest\ProtectDriverCtrl.h
	file path:	d:\lidengwang\lse\trunk\src\drvtest
	file base:	ProtectDriverCtrl
	file ext:	h
	author:		lidengwang
	
	purpose:	
*********************************************************************/

#pragma once


class IProtectDevCallback
{
public:
    virtual void ReportLog( LP_DRIVER_EVENT_INFO EventInfo ) = 0;
};


class CProtectDevC
{
public:
    CProtectDevC(void);
    ~CProtectDevC(void);
    

    virtual BOOL __stdcall InitDevC();
    virtual void __stdcall UninitDevC();

    virtual unsigned int __stdcall GetDriverVersion();
    virtual unsigned int __stdcall GetDllVersion();

    virtual int  __stdcall GetStatus();
    virtual BOOL __stdcall SetStatus(int nStatus);

    virtual BOOL __stdcall GetConfig(LP_DRIVER_CONFIG pconfig);
    virtual BOOL __stdcall SetConfig(LP_DRIVER_CONFIG pconfig);

    virtual BOOL __stdcall AppendRule( LP_DRIVER_RULE_INFO RuleInfo );
    virtual BOOL __stdcall ClearRule( UINT uRuleType );

    virtual BOOL __stdcall GetReportLog( LP_DRIVER_EVENT_INFO EventInfo );

    virtual void __stdcall SetCallback(IProtectDevCallback* piCallback);

protected:

    virtual BOOL __stdcall SetReportEvent( HANDLE hReportEvent );


protected:

    BOOL StartReadEventThread();
    

    static DWORD WINAPI ThreadReadEventFunc(LPVOID lpParam);

    void RunReadEventFunc();


    HANDLE m_hReadEventThread;
    HANDLE m_hExitEvent;

private:

    HANDLE m_hDevcHandle;
    HANDLE m_hReportEvent;

    IProtectDevCallback* m_piCallback;

};
