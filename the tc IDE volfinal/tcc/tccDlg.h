
// tccDlg.h : ͷ�ļ�
//

#pragma once


// CtccDlg �Ի���
class CtccDlg : public CDialogEx
{
// ����
public:
	CtccDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TCC_DIALOG };

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
	afx_msg void OnBnClickedButton1();
	CEdit m_filetext;
	CEdit m_filename;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};
