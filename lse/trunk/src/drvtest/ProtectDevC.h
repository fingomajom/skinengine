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

    virtual BOOL __stdcall AppendRule( LP_DRIVER_RULE_INFO RuleInfo );
    virtual BOOL __stdcall ClearRule( UINT uRuleType );

private:

    HANDLE m_hDevcHandle;

};
