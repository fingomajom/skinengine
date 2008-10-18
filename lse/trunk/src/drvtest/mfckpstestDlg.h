// mfckpstestDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include <vector>

#include "ProtectDevC.h"


// CmfckpstestDlg �Ի���
class CmfckpstestDlg : public CDialog
{
// ����
public:
	CmfckpstestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MFCKPSTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


public:

    void InitRuleList(CListCtrl& ListCtrl);

    void UpdateRuleList( CListCtrl& ListCtrl, DRIVER_RULE_INFO& RuleInfo, int idx );

    void AddRule( CListCtrl& ListCtrl, std::vector<DRIVER_RULE_INFO>& vtRule );
    void DelRule( CListCtrl& ListCtrl, std::vector<DRIVER_RULE_INFO>& vtRule , int idx );

    void EditRule( CListCtrl& ListCtrl, std::vector<DRIVER_RULE_INFO>& vtRule , int idx );

    std::vector<DRIVER_RULE_INFO> m_vtblack_rule_list;
    std::vector<DRIVER_RULE_INFO> m_vtwhite_rule_list;
    std::vector<DRIVER_RULE_INFO> m_vtprotect_rule_list;
    

    CProtectDevC m_devc;

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedInstall();
    afx_msg void OnBnClickedUninstall();
    afx_msg void OnBnClickedAddProtectFile();
    afx_msg void OnBnClickedAddBlackFile();



    CListCtrl m_black_rule_list;
    CListCtrl m_white_rule_list;
    CListCtrl m_protect_rule_list;
    afx_msg void OnBnClickedAddWhiteFile();
    afx_msg void OnBnClickedClearBlackFile();
    afx_msg void OnBnClickedClearWhiteFile();
    afx_msg void OnBnClickedClearProtectFile();
    afx_msg void OnNMDblclkBlackList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkWhiteList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkProtectList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedSetBlackFile();
    afx_msg void OnBnClickedSetWhiteFile();
    afx_msg void OnBnClickedSetProtectFile();
};
