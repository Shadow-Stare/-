#include "stdafx.h"
#include "DrawPictrue.h"


CDrawPictrue::CDrawPictrue(void)
{
	m_bStart = false;
	m_iseraser = false;//��ǰ״̬�Ƿ�Ϊ��Ƥ
	pendegree = 2;
	pencolor = RGB(0,0,0);
}


CDrawPictrue::~CDrawPictrue(void)
{
}
BEGIN_MESSAGE_MAP(CDrawPictrue, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


void CDrawPictrue::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_iseraser == true){
		m_pen = CreatePen(PS_SOLID,20,RGB(255,255,255));
	}else{
		m_pen = CreatePen(PS_SOLID,pendegree,pencolor);
	}
	
	m_Point = point;

	if(theApp.all_identity == 1 && theApp.all_isGameOn == true){
		m_bStart = true;
	}
	
	CStatic::OnLButtonDown(nFlags, point);
}

void CDrawPictrue::OnLButtonUp(UINT nFlags, CPoint point)
{
	DeleteObject(m_pen);
	m_bStart = false;

	

	//����ͼƬ
	CRect rcPlot;
	theApp.m_pMainWnd->GetDlgItem(IDC_DRAWPIC)->GetWindowRect(&rcPlot); //��ȡIDC_VIEW��������Ļ�е�����λ��
	ScreenToClient(&rcPlot);//��ȡIDC_VIEW�ڸ������е�λ��
 
	HDC hDC= ::GetWindowDC(theApp.m_pMainWnd->GetDlgItem(IDC_DRAWPIC)->GetSafeHwnd());
	HDC hDCMem= ::CreateCompatibleDC(hDC);// ��������Ļ���ݵ�bitmap 
 
	HBITMAP hBitmap= ::CreateCompatibleBitmap(hDC,rcPlot.Width(),rcPlot.Height());
	HBITMAP hOldBitmap= (HBITMAP)::SelectObject(hDCMem,hBitmap);////��memBitmapѡ���ڴ�DC 
 
	::BitBlt(hDCMem,0,0,rcPlot.Width(),rcPlot.Height(),hDC,0,0,SRCCOPY);// ����߶ȿ�� 
	::SelectObject(hDCMem,hOldBitmap);
	CImage image;
	image.Attach(hBitmap); //��λͼ����ͼƬ����

	dpist.CreateMemoryStream(NULL);
	image.Save(dpist.GetStream(),Gdiplus::ImageFormatPNG);
	
	int len = dpist.GetLength();
	TRACE("%d\n\n",len);

	theApp.m_pTCPKernel->SendPicData(&dpist,len);

	CStatic::OnLButtonUp(nFlags, point);
}

void CDrawPictrue::OnMouseMove(UINT nFlags, CPoint point)
{
	CClientDC dc(this);
	if(m_bStart)
	{
		dc.SelectObject(m_pen);
		dc.MoveTo(m_Point.x,m_Point.y);
		dc.LineTo(point.x,point.y);
		m_Point = point;
	}

	CStatic::OnMouseMove(nFlags, point);
}

void CDrawPictrue::ClearScreen(){
	CClientDC dc(this);
	CRect rcPlot;
	GetWindowRect(&rcPlot); //��ȡIDC_VIEW��������Ļ�е�����λ��
	ScreenToClient(&rcPlot);
	m_pen = CreatePen(PS_SOLID,1,RGB(255,255,255));
	dc.SelectObject(m_pen);
	dc.Rectangle(rcPlot);
	DeleteObject(m_pen);
}