#pragma once
#include "afxwin.h"


// showDlg �Ի���

class showDlg : public CDialogEx
{
	DECLARE_DYNAMIC(showDlg)

public:
	showDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~showDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_showEdiCtl1;
	CEdit m_showEdiCtl2;
	CEdit m_showEdiCtl3;

	CString str1;///�¼�
	CString str2;
	CString str3;
	afx_msg void OnBnClickedOk();
};
