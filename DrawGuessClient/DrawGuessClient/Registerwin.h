#pragma once


// Registerwin �Ի���

class Registerwin : public CDialogEx
{
	DECLARE_DYNAMIC(Registerwin)

public:
	Registerwin(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Registerwin();

// �Ի�������
	enum { IDD = IDD_REGDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRegbutton2();
	virtual void PostNcDestroy();
	CString m_Genderbox;
	CString m_usernamebox;
	CString m_passwordbox;
	CString m_mailbox;
};
