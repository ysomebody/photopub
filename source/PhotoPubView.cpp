// PhotoPubView.cpp : CPhotoPubView 类的实现
//

#include "stdafx.h"
#include "PhotoPub.h"

#include "MainFrm.h"
#include "PhotoPubDoc.h"
#include "PhotoPubView.h"

#include "cv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPhotoPubView

IMPLEMENT_DYNCREATE(CPhotoPubView, CScrollView)

BEGIN_MESSAGE_MAP(CPhotoPubView, CScrollView)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON1, &CPhotoPubView::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CPhotoPubView::OnBnClickedButton2)
	ON_BN_CLICKED(ID_IMAGE_OPEN, &CPhotoPubView::OnBnClickedImageOpen)
END_MESSAGE_MAP()

// CPhotoPubView 构造/析构

CPhotoPubView::CPhotoPubView()
{
	// TODO: 在此处添加构造代码
    m_bmi_buffer = (BITMAPINFO*)display_buffer;
    m_bmih_buffer = &(m_bmi_buffer->bmiHeader);
	m_ratio=1;
}

CPhotoPubView::~CPhotoPubView()
{
}

BOOL CPhotoPubView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CScrollView::PreCreateWindow(cs);
}

// CPhotoPubView 绘制

void CPhotoPubView::OnDraw(CDC* pDC)
{

	// TODO: 在此处为本机数据添加绘制代码
	CPhotoPubDoc *pDoc=GetDocument();
	IplImage *pImg=pDoc->GetImage();
	int wid=0,hi=0;

	CRect ClipBox;
	GetClientRect(&ClipBox);
	if (!pImg) {
		CBitmap bkg;
		bkg.LoadBitmap(IDB_BACKGROUND);
		CBrush brush(&bkg);
		pDC->FillRect(&ClipBox,&brush);	
		return;
	}

	wid=int(pImg->width*m_ratio);
	hi=int(pImg->height*m_ratio);

	IplImage *pStub=pImg;
	IplImage *pTmp=NULL;
	if (wid!=pImg->width) {
		pTmp=cvCreateImage(cvSize(wid,hi),IPL_DEPTH_8U,3);
		cvResize(pImg,pTmp);
		pStub=pTmp;
	}
		

	CRgn WindowRgn;
	WindowRgn.CreateRectRgnIndirect(&ClipBox);
	CRect ImageRect(0,0,wid,hi);
	int top=max((ClipBox.Height()-hi)/2,0);
	int left=max((ClipBox.Width()-wid)/2,0);
	ImageRect.MoveToXY(left,top);
	CRgn ImageRgn;
	ImageRgn.CreateRectRgnIndirect(&ImageRect);
	WindowRgn.CombineRgn(&WindowRgn,&ImageRgn,RGN_DIFF);

	CBitmap bkg;
	bkg.LoadBitmap(IDB_BACKGROUND);
	CBrush brush(&bkg);
	pDC->FillRgn(&WindowRgn,&brush);	
	
	memset(m_bmih_buffer, 0, sizeof(*m_bmih_buffer));
    m_bmih_buffer->biSize = sizeof(BITMAPINFOHEADER);
    m_bmih_buffer->biWidth = wid;
    m_bmih_buffer->biHeight = -hi;
    m_bmih_buffer->biPlanes = 1;
    m_bmih_buffer->biBitCount = 24;
    m_bmih_buffer->biCompression = BI_RGB;


	SetDIBitsToDevice(
		pDC->m_hDC, 
		ImageRect.left,ImageRect.top,pStub->width,pStub->height, 
		0, 0, 0, hi, pStub->imageData, m_bmi_buffer, DIB_RGB_COLORS 
	);

	cvReleaseImage(&pTmp);

}

void CPhotoPubView::OnInitialUpdate()
{
	((CMainFrame*)GetParent())->SetRatio(int(m_ratio*100));

	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 计算此视图的合计大小
	sizeTotal.cx = sizeTotal.cy = 0;
	SetScrollSizes(MM_TEXT, sizeTotal);
}


// CPhotoPubView 诊断

#ifdef _DEBUG
void CPhotoPubView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CPhotoPubView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CPhotoPubDoc* CPhotoPubView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPhotoPubDoc)));
	return (CPhotoPubDoc*)m_pDocument;
}
#endif //_DEBUG


// CPhotoPubView 消息处理程序

void CPhotoPubView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: Add your specialized code here and/or call the base class
	CPhotoPubDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CSize sizeTotal;
	// TODO: 计算此视图的合计大小
	IplImage *pImg=pDoc->GetImage();
	if (!pImg) return;
	sizeTotal.cx = long(pImg->width*m_ratio);
	sizeTotal.cy = long(pImg->height*m_ratio);
	SetScrollSizes(MM_TEXT, sizeTotal);

	Invalidate();

}

BOOL CPhotoPubView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;//CScrollView::OnEraseBkgnd(pDC);
}


void CPhotoPubView::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	m_ratio/=1.2;
	((CMainFrame*)GetParent())->SetRatio(int(m_ratio*100));
	OnUpdate(NULL,NULL,NULL);

}

void CPhotoPubView::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	m_ratio*=1.2;
	((CMainFrame*)GetParent())->SetRatio(int(m_ratio*100));
	OnUpdate(NULL,NULL,NULL);
}

void CPhotoPubView::OnBnClickedImageOpen()
{
	m_ratio=1;
	((CMainFrame*)GetParent())->SetRatio(int(m_ratio*100));
	CPhotoPubDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->OnOpenImage();
	
	// TODO: Add your control notification handler code here
}
