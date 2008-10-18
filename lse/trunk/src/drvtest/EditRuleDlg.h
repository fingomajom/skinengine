#pragma once
#include "afxwin.h"


// CEditRuleDlg dialog

class CEditRuleDlg : public CDialog
{
	DECLARE_DYNAMIC(CEditRuleDlg)

public:
	CEditRuleDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditRuleDlg();

// Dialog Data
	enum { IDD = IDD_EDIT_RULE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CString m_strTarget;
    int m_ctype;


public:

    DRIVER_RULE_INFO m_RuleInfo;

    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedBButton();
};
