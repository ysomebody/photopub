// CustomPageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PhotoPub.h"
#include "CustomPageDlg.h"

static CString PhotoTypeMap[]={"1'","1","2'","2"};

// CCustomPageDlg dialog

IMPLEMENT_DYNAMIC(CCustomPageDlg, CDialog)

CCustomPageDlg::CCustomPageDlg(CWnd* pParent, CString size)
	: CDialog(CCustomPageDlg::IDD, pParent)
	, m_PhotoType(0),lastpoint(-1,-1)
{
	m_Size=size;
	double width,height;
	CPage::Size2WH(size,width,height);
	m_width=(int)(width*FACTOR);
	m_height=(int)(height*FACTOR);
}

CCustomPageDlg::~CCustomPageDlg()
{
}

void CCustomPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_V1, m_PhotoType);
}


BEGIN_MESSAGE_MAP(CCustomPageDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_V1, &CCustomPageDlg::OnBnClickedV1)
	ON_BN_CLICKED(IDC_H2, &CCustomPageDlg::OnBnClickedH2)
	ON_BN_CLICKED(IDC_H1, &CCustomPageDlg::OnBnClickedH1)
	ON_BN_CLICKED(IDC_V2, &CCustomPageDlg::OnBnClickedV2)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


// CCustomPageDlg message handlers

BOOL CCustomPageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_PAGESETTING)->GetWindowRect(&m_ClientRct);
	ScreenToClient(&m_ClientRct);
	int wdif=m_ClientRct.Width()-m_width;
	int hdif=m_ClientRct.Height()-m_height;
	m_ClientRct.right=m_ClientRct.left+m_width;
	m_ClientRct.bottom=m_ClientRct.top+m_height;
	m_ClientRct.OffsetRect(wdif/2,hdif/2);


	
	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCustomPageDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

	dc.Rectangle(&m_ClientRct);
	size_t size=m_photos.size();
	for (size_t i=0;i<size;++i) {
		CPhoto &pht=m_photos[i];
		int l=(int)(pht.x*FACTOR)+m_ClientRct.left;
		int t=(int)(pht.y*FACTOR)+m_ClientRct.top;
		int r=(int)((pht.x+pht.width)*FACTOR)+m_ClientRct.left;
		int b=(int)((pht.y+pht.height)*FACTOR)+m_ClientRct.top;
		dc.Rectangle(l,t,r,b);
		dc.TextOut(l+1,t+1,pht.GetSize().Left(1)+"´ç");
	}
}

void CCustomPageDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//CDialog::OnMouseMove(nFlags, point);

	UpdateData();
	if (m_PhotoType<0 || m_PhotoType>3 ) return;
	double fwidth,fheight;
	int width,height;
	CPhoto::Size2WH(PhotoTypeMap[m_PhotoType],fwidth,fheight);
	width=(int)(fwidth*FACTOR);
	height=(int)(fheight*FACTOR);
	
	CRect ValidRct(m_ClientRct);
	ValidRct.DeflateRect(0,0,width,height);
	Client2Valid(point,ValidRct);
	
	CRect rc(point.x,point.y,point.x,point.y);
	rc.InflateRect(0,0,width,height);

	CClientDC dc(this);
	dc.SetROP2(R2_NOTXORPEN);

	if (lastpoint!=point){
	   CPen BluePen(PS_DOT,1,RGB(0, 0, 255));
	   CPen* pOldPen = dc.SelectObject(&BluePen);
		if (lastpoint.x!=-1) {
			CRect lastrc(rc);
			lastrc.MoveToXY(lastpoint);
			dc.Rectangle(&lastrc);		
		}
		dc.Rectangle(&rc);
		lastpoint=point;	
		dc.SelectObject(pOldPen);
	}
}
void CCustomPageDlg::Client2Valid(CPoint &point, const CRect &ValidRct)
{
	if (ValidRct.PtInRect(point)) return;
	if(point.x<ValidRct.left) point.x=ValidRct.left; 
	else if(point.x>=ValidRct.right) point.x=ValidRct.right; 
	if(point.y<ValidRct.top) point.y=ValidRct.top; 
	else if(point.y>=ValidRct.bottom) point.y=ValidRct.bottom; 
}
void CCustomPageDlg::OnBnClickedV1()
{
	// TODO: Add your control notification handler code here
	InvalidateRect(&m_ClientRct);
	lastpoint.SetPoint(-1,-1);
}

void CCustomPageDlg::OnBnClickedH2()
{
	// TODO: Add your control notification handler code here
	InvalidateRect(&m_ClientRct);
	lastpoint.SetPoint(-1,-1);
}

void CCustomPageDlg::OnBnClickedH1()
{
	// TODO: Add your control notification handler code here
	InvalidateRect(&m_ClientRct);
	lastpoint.SetPoint(-1,-1);
}

void CCustomPageDlg::OnBnClickedV2()
{
	// TODO: Add your control notification handler code here
	InvalidateRect(&m_ClientRct);
	lastpoint.SetPoint(-1,-1);
}

void CCustomPageDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	UpdateData();
	if (m_PhotoType<0 || m_PhotoType>3 ) return;

	double fwidth,fheight;
	int width,height;
	CPhoto::Size2WH(PhotoTypeMap[m_PhotoType],fwidth,fheight);
	width=(int)(fwidth*FACTOR);
	height=(int)(fheight*FACTOR);
	
	CRect ValidRct(m_ClientRct);
	ValidRct.DeflateRect(0,0,width,height);
	Client2Valid(point,ValidRct);

	//¼ì²âÖØµþ
	CRect cur(point,CPoint(point.x+width,point.y+height));
	size_t size=m_photos.size();
	for (size_t i=0;i<size;++i) {
		CPhoto &pht=m_photos[i];
		int l=(int)(pht.x*FACTOR)+m_ClientRct.left;
		int t=(int)(pht.y*FACTOR)+m_ClientRct.top;
		CPoint p(l,t);
		CRect phtrct(p,CPoint(p.x+int(pht.width*FACTOR),p.y+int(pht.height*FACTOR)));
		if (IntersectRect(&phtrct,&phtrct,&cur)) {
			AfxMessageBox("ÕÕÆ¬ÖØµþ!");
			return;
		}
	}

	double cmx,cmy;
	cmx=(point.x-m_ClientRct.left)*1.0/FACTOR;
	cmy=(point.y-m_ClientRct.top)*1.0/FACTOR;

	CPhoto photo(cmx,cmy,PhotoTypeMap[m_PhotoType]);
	m_photos.push_back(photo);
	
	lastpoint.SetPoint(-1,-1);
	Invalidate();

}

void CCustomPageDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	//CDialog::OnRButtonDown(nFlags, point);
	lastpoint.SetPoint(-1,-1);
	if (m_photos.size()) m_photos.pop_back();
	Invalidate();

}
