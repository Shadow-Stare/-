// Mainwin.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DrawGuessClient.h"
#include "Mainwin.h"
#include "afxdialogex.h"
#include "Hallwin.h"

// Mainwin �Ի���

IMPLEMENT_DYNAMIC(Mainwin, CDialogEx)

Mainwin::Mainwin(CWnd* pParent /*=NULL*/)
	: CDialogEx(Mainwin::IDD, pParent)
{
	
	
}

Mainwin::~Mainwin()
{
}

void _stdcall TimerProc(HWND hWnd,UINT nMsg,UINT nTimerid,DWORD dwTime){
	if(nTimerid == 11){
		theApp.all_lefttime--;
		theApp.m_pMainWnd->PostMessage(UM_TIMEREVENTMSG);
		if(theApp.all_lefttime == 0){
			theApp.m_pMainWnd->KillTimer(11);
		}
	}
}


void Mainwin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DRAWPIC, m_drawpic);
	DDX_Control(pDX, IDC_COLORBTN, m_colorbtn);
	DDX_Control(pDX, IDC_PENDEGREE, m_pendegreebtn);
	DDX_Control(pDX, IDC_HINTSTR, m_hintstrtag);
	DDX_Control(pDX, IDC_PLAYERINFO, m_PlayerInfoBox);
	DDX_Control(pDX, IDC_CHATEDIT, m_chatEdit);
	DDX_Control(pDX, IDC_MESSAGEDIT, m_messageEdit);
	DDX_Control(pDX, IDC_TIMERSTR, m_TimerTag);
	DDX_Control(pDX, IDC_GAMESTARTBTN, m_GameStartBtn);
}


BEGIN_MESSAGE_MAP(Mainwin, CDialogEx)
	
	ON_BN_CLICKED(IDC_COLORBTN, &Mainwin::OnBnClickedColorbtn)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_PENDEGREE, &Mainwin::OnTRBNThumbPosChangingPendegree)
	
	ON_BN_CLICKED(IDC_ERASERBTN, &Mainwin::OnBnClickedEraserbtn)
	ON_BN_CLICKED(IDC_PENBTN, &Mainwin::OnBnClickedPenbtn)
	ON_BN_CLICKED(IDC_CLEARPICBTN, &Mainwin::OnBnClickedClearpicbtn)
	ON_BN_CLICKED(IDC_SENDMESSAGEBTN, &Mainwin::OnBnClickedSendmessagebtn)
	ON_BN_CLICKED(IDC_GAMESTARTBTN, &Mainwin::OnBnClickedGamestartbtn)

	ON_MESSAGE(UM_PLAYERNUMRENEWMSG,&Mainwin::PlayerNumRenewMsg)
	ON_MESSAGE(UM_GETGOODSINFOMSG,&Mainwin::GetGoodInfoMsg)
	ON_MESSAGE(UM_TIMEREVENTMSG,&Mainwin::TimerEventMsg)
	ON_MESSAGE(UM_CLOSEROOMMSG,&Mainwin::CloseRoomMsg)

	ON_WM_CLOSE()
END_MESSAGE_MAP()


// Mainwin ��Ϣ�������




void Mainwin::OnBnClickedColorbtn()
{
	m_drawpic.pencolor = m_colorbtn.GetColor();

}

void Mainwin::OnTRBNThumbPosChangingPendegree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// �˹���Ҫ�� Windows Vista ����߰汾��
	// _WIN32_WINNT ���ű��� >= 0x0600��
	NMTRBTHUMBPOSCHANGING *pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);
	m_drawpic.pendegree = m_pendegreebtn.GetPos();
	
	*pResult = 0;
}

void Mainwin::OnBnClickedEraserbtn()
{
	m_drawpic.m_iseraser = true;
}

void Mainwin::OnBnClickedPenbtn()
{
	m_drawpic.m_iseraser = false;
}

void Mainwin::OnBnClickedClearpicbtn()
{
	if(theApp.all_identity == 1 || theApp.all_isGameOn == false){
		m_drawpic.ClearScreen();
	}
}

void Mainwin::OnBnClickedSendmessagebtn()
{
	UpdateData(TRUE); //��ȡ����
	CString message;
	m_messageEdit.GetWindowTextW(message);

	if(message != ""){
		USES_CONVERSION;
		if(message.Find(A2T(theApp.all_goodinfo.m_ThingName)) != -1 && theApp.all_isGuessSuccess == 1){
			CString historychat;
			m_chatEdit.GetWindowTextW(historychat);
			historychat = historychat + _T("��ϵͳ��\r\n");
			historychat = historychat + _T("���Ѿ�������ȷ���޷����Ͱ�����ȷ�𰸵����ݣ�\r\n\r\n");
			m_chatEdit.SetWindowTextW(historychat);
			
		}else{
			STRU_PLAYERMESSAG_RQ pmrq;
			pmrq.m_nType = _DEF_PROTOCOL_PLAYERMESSAG_RQ;
			pmrq.m_playeruid = theApp.all_userid;
			pmrq.m_roomid = theApp.all_roomid;
			WideCharToMultiByte(CP_ACP,0,theApp.all_username,-1,pmrq.m_playername,_DEF_SIZE,0,0);
			pmrq.m_GuessResult = -1;

			WideCharToMultiByte(CP_ACP,0,message,-1,pmrq.m_MessageContent,_DEF_PLAYERMESSAG_SIZE,0,0);
			
			theApp.m_pTCPKernel->SendData((char*)&pmrq,sizeof(pmrq));
		}

		m_messageEdit.SetWindowTextW(_T(""));
	}else{
		MessageBox(_T("���������������ٷ��ͣ�"),_T("��ʾ"),MB_OK);
	}
	
}


BOOL Mainwin::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_pendegreebtn.SetRange(1,25);

	FontA.CreatePointFont(160,L"����");
	FontB.CreatePointFont(100,L"����");
	m_hintstrtag.SetFont(&FontA);
	m_hintstrtag.SetWindowTextW(_T("��ʾ��"));

	theApp.all_isGuessSuccess = 2;
	m_PlayerInfoBox.SetFont(&FontB);
	m_chatEdit.SetFont(&FontB);
	m_TimerTag.SetFont(&FontB);

	m_TimerTag.SetWindowTextW(_T(""));

	CString infostr;
	infostr.Format(_T("�㻭�Ҳ�-�����%d"),theApp.all_roomid);
	this->SetWindowTextW(infostr);

	infostr.Format(_T("��ϵͳ��\r\n��ӭ���뷿��[%d]��\r\n\r\n"),theApp.all_roomid);
	m_chatEdit.SetWindowTextW(infostr);
	
	infostr.Format(_T("���� �������:%d"),theApp.all_playerNum);
	m_PlayerInfoBox.SetWindowTextW(infostr);

	return TRUE;
}


void Mainwin::OnBnClickedGamestartbtn()
{
	if(theApp.all_isGameOn == false){
		if(theApp.all_identity == 1 && theApp.all_playerNum >= 2){
			STRU_GAMESTART_RQ gsrq;
			gsrq.m_nType = _DEF_PROTOCOL_GAMESTART_RQ;
			gsrq.m_playernum = theApp.all_playerNum;
			gsrq.m_roomid = theApp.all_roomid;
			gsrq.m_uid = theApp.all_userid;

			theApp.m_pTCPKernel->SendData((char*)&gsrq,sizeof(gsrq));

		}else{
			MessageBox(_T("�����Ƿ����򷿼�����δ�����ˣ��޷���ʼ��Ϸ��"),_T("��ʾ"),MB_OK);
		}
	}
}

LRESULT Mainwin::PlayerNumRenewMsg(WPARAM W,LPARAM L){
	CString infostr;
	infostr.Format(_T("���� �������:%d"),theApp.all_playerNum);
	m_PlayerInfoBox.SetWindowTextW(infostr);
	UpdateData(TRUE); //��ȡ����

	CString historychat;
	m_chatEdit.GetWindowTextW(historychat);
	historychat = historychat + _T("��ϵͳ��\r\nһ����Ҽ���/�뿪�˷���\r\n\r\n");
	m_chatEdit.SetWindowTextW(historychat);

	return 0;
}

LRESULT Mainwin::GetGoodInfoMsg(WPARAM W,LPARAM L){
	CString infostr;

	m_GameStartBtn.EnableWindow(FALSE);

	if(theApp.all_identity == 1){
		USES_CONVERSION;
		infostr.Format(_T("�ֵ��������� ��Ŀ�ǡ�%s��"),A2T(theApp.all_goodinfo.m_ThingName));
		
	}else{
		USES_CONVERSION;
		infostr.Format(_T("�ֵ������� ��ʾһ��%s"),A2T(theApp.all_goodinfo.m_FirstHint));
		
	}
	m_hintstrtag.SetWindowTextW(infostr);
	
	//��ʼ��ʱ
	theApp.all_lefttime = theApp.all_goodinfo.m_GuessTime;
	infostr.Format(_T("ʣ��[%d]��"),theApp.all_lefttime);
	m_TimerTag.SetWindowTextW(infostr);
	SetTimer(11,1000,TimerProc);
	
	return 0;
}

void Mainwin::OnClose()
{
	STRU_LEAVEROOM_RQ lrrq;
	lrrq.m_nType = _DEF_PROTOCOL_LEAVEROOM_RQ;
	lrrq.m_playeruid = theApp.all_userid;
	lrrq.m_roomid = theApp.all_roomid;
	if(theApp.all_identity == 1){
		lrrq.m_isRoomOwner = true;
	}else{
		lrrq.m_isRoomOwner = false;
	}

	theApp.m_pTCPKernel->SendData((char*)&lrrq,sizeof(lrrq));


	theApp.all_isGameOn = false;
	KillTimer(11);

	UpdateData(TRUE);
	Hallwin dlg;
	theApp.m_pMainWnd = &dlg;
	EndDialog(IDOK);
	dlg.DoModal();

	CDialogEx::OnClose();
}

LRESULT Mainwin::TimerEventMsg(WPARAM W,LPARAM L){
	CString infostr;
	infostr.Format(_T("ʣ��[%d]��"),theApp.all_lefttime);
	m_TimerTag.SetWindowTextW(infostr);

	if(theApp.all_identity == 2){
		if(theApp.all_lefttime == (theApp.all_goodinfo.m_GuessTime / 3)*2){
			USES_CONVERSION;
			infostr.Format(_T("�ֵ������� ��ʾһ��%s ��ʾ����%s"),A2T(theApp.all_goodinfo.m_FirstHint),A2T(theApp.all_goodinfo.m_SecondHint));
			m_hintstrtag.SetWindowTextW(infostr);
		}else if(theApp.all_lefttime == (theApp.all_goodinfo.m_GuessTime / 3) ){
			USES_CONVERSION;
			infostr.Format(_T("�ֵ������� ��ʾһ��%s ��ʾ����%s ��ʾ����%s"),A2T(theApp.all_goodinfo.m_FirstHint),A2T(theApp.all_goodinfo.m_SecondHint),A2T(theApp.all_goodinfo.m_ThirdHint));
			m_hintstrtag.SetWindowTextW(infostr);
		}else if(theApp.all_lefttime == 0){
			m_TimerTag.SetWindowTextW(_T("���ֽ���"));
		}
	}

	return 0;
}

LRESULT Mainwin::CloseRoomMsg(WPARAM W,LPARAM L){
	MessageBox(_T("���ڷ����˳����䣬��Ϸ������"),_T("��ʾ"),MB_OK);
	theApp.all_isGameOn = false;
	KillTimer(11);

	UpdateData(TRUE);
	Hallwin dlg;
	theApp.m_pMainWnd = &dlg;
	EndDialog(IDOK);
	dlg.DoModal();
	CDialogEx::OnClose();
	return 0;
}