
// DrawGuessClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "TCPKernel.h"

// CDrawGuessClientApp:
// �йش����ʵ�֣������ DrawGuessClient.cpp
//

class CDrawGuessClientApp : public CWinApp
{
public:
	IKernel *m_pTCPKernel;


	CString all_username;//ȫ�֣��û���
	long long all_userid;//ȫ�֣��û�ID
	long long all_roomid;//ȫ�֣����ڷ���ID Ϊ-1ʱ����û������
	int all_identity;//ȫ�֣����0����δ֪��1��������2�������
	int all_isGuessSuccess;//ȫ�֣�����Ƿ��Ѳ��н���ı�־������0����δ���У�1������У�2����δ����Ϸ״̬
	int all_playerNum;//ȫ�֣���ǰ���������Ϊ-1����δ������Ϸ״̬
	goodinfo all_goodinfo;//ȫ�֣���ǰ��Ϸ����Ʒ��Ϣ
	int all_lefttime;//ȫ�֣�ʣ�µ�ʱ��
	bool all_isGameOn;//ȫ�֣���Ϸ�Ƿ�ʼ

public:
	CDrawGuessClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CDrawGuessClientApp theApp;