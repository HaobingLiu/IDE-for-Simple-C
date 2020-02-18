
// tccDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "tcc.h"
#include "tccDlg.h"
#include "afxdialogex.h"
#include "main.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CtccDlg �Ի���




CtccDlg::CtccDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CtccDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtccDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_EDIT1, m_filetext);
	DDX_Control(pDX, IDC_EDIT2, m_filetext);
	DDX_Control(pDX, IDC_EDIT1, m_filename);
}

BEGIN_MESSAGE_MAP(CtccDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CtccDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CtccDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CtccDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CtccDlg ��Ϣ�������

BOOL CtccDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CtccDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CtccDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CtccDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CString filename;		//�ļ���
CString filepath;
void CtccDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CFileDialog fileDlg(TRUE, "�����ļ�", "",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"C Files(*.c)|*.c||");
	if(fileDlg.DoModal() == IDOK)
	{
		filename=fileDlg.GetPathName();
		int n = filename.ReverseFind('.');			//��ȡ�ļ�·��
		filepath = filename.Left(n+1);
		
		m_filename.SetWindowTextA(fileDlg.GetPathName()); 
	}

	CStringA strFile;
	CFile file;
	
	if(!file.Open(fileDlg.GetPathName(),CFile::modeRead))
	{
		AfxMessageBox( "can   not   open   file! ");
		exit(1);
	}
	file.Read(strFile.GetBufferSetLength(file.GetLength()),file.GetLength());
	file.Close();



	GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);


	m_filetext.SetWindowTextA(strFile);
}


void CtccDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	m_filetext.GetWindowTextA(str);
	CFile file;

	if(!file.Open(filename,CFile::modeWrite))
	{
		AfxMessageBox( "can   not   open   file! ");
		exit(1);
	}
	file.Write(str+"\r\n",str.GetLength()+2);
	file.Close();


}


void CtccDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	tcc_main(filename,filepath);
}
