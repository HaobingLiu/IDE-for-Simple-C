// showDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "tcc.h"
#include "showDlg.h"
#include "afxdialogex.h"


// showDlg �Ի���

IMPLEMENT_DYNAMIC(showDlg, CDialogEx)

showDlg::showDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

showDlg::~showDlg()
{
}

void showDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_showEdiCtl1);
	DDX_Control(pDX, IDC_EDIT2, m_showEdiCtl2);
	DDX_Control(pDX, IDC_EDIT3, m_showEdiCtl3);

	m_showEdiCtl1.SetWindowTextA(str1);///�¼�
	m_showEdiCtl2.SetWindowTextA(str2);
	m_showEdiCtl3.SetWindowTextA(str3);
}


BEGIN_MESSAGE_MAP(showDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &showDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// showDlg ��Ϣ�������


void showDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
}
