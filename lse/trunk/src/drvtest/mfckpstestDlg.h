// mfckpstestDlg.h : ͷ�ļ�
//

#pragma once


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
};
