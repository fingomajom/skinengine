#pragma once


// CConfigDlg dialog

class CConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigDlg();

// Dialog Data
	enum { IDD = IDD_CONFIG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
    
    DRIVER_CONFIG m_cfg;
    
    BOOL m_blog;
    BOOL m_bblock;
    BOOL m_bhide;
    virtual BOOL OnInitDialog();
protected:
    virtual void OnOK();
};
