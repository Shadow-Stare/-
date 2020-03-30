
// DrawGuessClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DrawGuessClient.h"
#include "DrawGuessClientDlg.h"
#include "afxdialogex.h"
#include "Hallwin.h"
#include "Registerwin.h"

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


// CDrawGuessClientDlg �Ի���



CDrawGuessClientDlg::CDrawGuessClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDrawGuessClientDlg::IDD, pParent)
	, m_usernamemailbox(_T("hanmin"))
	, m_userpwbox(_T("512240272"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDrawGuessClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_usernamemailbox);
	DDX_Text(pDX, IDC_EDIT2, m_userpwbox);
}

BEGIN_MESSAGE_MAP(CDrawGuessClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_MESSAGE(UM_LOGINMSG,&CDrawGuessClientDlg::LoginMsg)
	ON_BN_CLICKED(IDC_LOGINBUTTON, &CDrawGuessClientDlg::OnBnClickedLoginbutton)
	ON_BN_CLICKED(IDC_REGBUTTON1, &CDrawGuessClientDlg::OnBnClickedRegbutton1)
END_MESSAGE_MAP()


// CDrawGuessClientDlg ��Ϣ�������

BOOL CDrawGuessClientDlg::OnInitDialog()
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
	if(!theApp.m_pTCPKernel->Open())
	{
		MessageBox(_T("����������ʧ��"));
	}
	

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CDrawGuessClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDrawGuessClientDlg::OnPaint()
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
HCURSOR CDrawGuessClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDrawGuessClientDlg::OnClose()
{
	theApp.m_pTCPKernel->Close();
	
	CDialogEx::OnClose();
}


LRESULT CDrawGuessClientDlg::LoginMsg(WPARAM W,LPARAM L){
	UpdateData(TRUE);
	Hallwin dlg;
	theApp.m_pMainWnd = &dlg;
	EndDialog(IDOK);
	dlg.DoModal();
	return 0;
}

void CDrawGuessClientDlg::OnBnClickedLoginbutton()
{
	UpdateData(TRUE); //��ȡ����
	STRU_LOGIN_RQ srr;
	srr.m_nType = _DEF_PROTOCOL_LOGIN_RQ;
	if(m_usernamemailbox != "" && m_userpwbox != ""){
		WideCharToMultiByte(CP_ACP,0,m_userpwbox,-1,srr.m_szPassWord,_DEF_SIZE,0,0);
		WideCharToMultiByte(CP_ACP,0,m_usernamemailbox,-1,srr.m_szUserName,_DEF_SIZE,0,0);
		WideCharToMultiByte(CP_ACP,0,m_usernamemailbox,-1,srr.m_szEmail,_DEF_SIZE,0,0);
		srr.m_szGender = 3;
		theApp.m_pTCPKernel->SendData((char*)&srr,sizeof(srr));
	}else{
		MessageBox(_T("��������д������Ϣ��"),_T("��¼ʧ��"),MB_OK);
	}

}


void CDrawGuessClientDlg::OnBnClickedRegbutton1()
{
	UpdateData(TRUE);  
	Registerwin *regdlg = new Registerwin();
	regdlg->Create(IDD_REGDIALOG); //����һ����ģ̬�Ի���
    regdlg->ShowWindow(SW_SHOWNORMAL); //��ʾ��ģ̬�Ի���
}
