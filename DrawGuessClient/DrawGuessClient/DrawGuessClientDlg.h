
// DrawGuessClientDlg.h : ͷ�ļ�
//

#pragma once


// CDrawGuessClientDlg �Ի���
class CDrawGuessClientDlg : public CDialogEx
{

// ����
public:
	CDrawGuessClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DRAWGUESSCLIENT_DIALOG };

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
	LRESULT LoginMsg(WPARAM W,LPARAM L);

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnClose();
	afx_msg void OnBnClickedLoginbutton();
	afx_msg void OnBnClickedRegbutton1();
	CString m_usernamemailbox;
	CString m_userpwbox;
};
