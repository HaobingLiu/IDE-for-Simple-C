#pragma once
#include "afxwin.h"


// showDlg 对话框

class showDlg : public CDialogEx
{
	DECLARE_DYNAMIC(showDlg)

public:
	showDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~showDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_showEdiCtl1;
	CEdit m_showEdiCtl2;
	CEdit m_showEdiCtl3;

	CString str1;///新加
	CString str2;
	CString str3;
	afx_msg void OnBnClickedOk();
};
