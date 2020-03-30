#pragma once
#include "afxwin.h"


// Hallwin �Ի���

class Hallwin : public CDialogEx
{
	DECLARE_DYNAMIC(Hallwin)

public:
	Hallwin(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Hallwin();

// �Ի�������
	enum { IDD = IDD_HALLDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCreatroom();

	LRESULT CreateRoomMsg(WPARAM W,LPARAM L);
	LRESULT PlayerEnterRoomMsg(WPARAM W,LPARAM L);

	LRESULT PlayerEnterRoomFailMsg(WPARAM W,LPARAM L);
	afx_msg void OnClose();
	afx_msg void OnBnClickedEnterroom();
	CEdit m_RoomIDEdit;
	CFont fontA;
	virtual BOOL OnInitDialog();
};
