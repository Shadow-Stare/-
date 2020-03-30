#pragma once
#include "afxwin.h"
#include "DrawPictrue.h"
#include "afxbutton.h"
#include "afxcolorbutton.h"
#include "afxcmn.h"

// Mainwin �Ի���



class Mainwin : public CDialogEx
{
	DECLARE_DYNAMIC(Mainwin)

public:
	Mainwin(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Mainwin();

// �Ի�������
	enum { IDD = IDD_MAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	COleStreamFile ist;
	DECLARE_MESSAGE_MAP()
public:
	CDrawPictrue m_drawpic;
	
	CMFCColorButton m_colorbtn;
	afx_msg void OnBnClickedColorbtn();
	CSliderCtrl m_pendegreebtn;
	afx_msg void OnTRBNThumbPosChangingPendegree(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedEraserbtn();
	afx_msg void OnBnClickedPenbtn();
	afx_msg void OnBnClickedClearpicbtn();
	afx_msg void OnBnClickedSendmessagebtn();
	CStatic m_hintstrtag;
	CFont FontA;
	CFont FontB;
	virtual BOOL OnInitDialog();
	CStatic m_PlayerInfoBox;
	afx_msg void OnBnClickedGamestartbtn();

public:
	LRESULT PlayerNumRenewMsg(WPARAM W,LPARAM L);
	LRESULT GetGoodInfoMsg(WPARAM W,LPARAM L);
	LRESULT CloseRoomMsg(WPARAM W,LPARAM L);
	LRESULT TimerEventMsg(WPARAM W,LPARAM L);

	CEdit m_chatEdit;
	CEdit m_messageEdit;
	afx_msg void OnClose();


	CStatic m_TimerTag;
	CButton m_GameStartBtn;
};
