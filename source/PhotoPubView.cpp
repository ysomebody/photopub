// PhotoPubView.cpp : CPhotoPubView ���ʵ��
//

#include "stdafx.h"
#include "PhotoPub.h"

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
END_MESSAGE_MAP()

// CPhotoPubView ����/����

CPhotoPubView::CPhotoPubView()
{
	// TODO: �ڴ˴���ӹ������
    m_bmi_buffer = (BITMAPINFO*)display_buffer;
    m_bmih_buffer = &(m_bmi_buffer->bmiHeader);
}

CPhotoPubView::~CPhotoPubView()
{
}

BOOL CPhotoPubView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CScrollView::PreCreateWindow(cs);
}

// CPhotoPubView ����

void CPhotoPubView::OnDraw(CDC* pDC)
{
	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
	CPhotoPubDoc *pDoc=GetDocument();
	IplImage *pImg=pDoc->GetImage();
	int wid=0,hi=0;

	CRect ClipBox;
	pDC->GetClipBox(&ClipBox);
	if (!pImg) {
		CBitmap bkg;
		bkg.LoadBitmap(IDB_BACKGROUND);
		CBrush brush(&bkg);
		pDC->FillRect(&ClipBox,&brush);	
		return;
	}

	wid=pImg->width;
	hi=pImg->height;

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
		ImageRect.left,ImageRect.top,ImageRect.Width(),ImageRect.Height(), 
		0, 0, 0, hi, pImg->imageData, m_bmi_buffer, DIB_RGB_COLORS 
	);


}

void CPhotoPubView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: �������ͼ�ĺϼƴ�С
	sizeTotal.cx = sizeTotal.cy = 0;
	SetScrollSizes(MM_TEXT, sizeTotal);
}


// CPhotoPubView ���

#ifdef _DEBUG
void CPhotoPubView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CPhotoPubView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CPhotoPubDoc* CPhotoPubView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPhotoPubDoc)));
	return (CPhotoPubDoc*)m_pDocument;
}
#endif //_DEBUG


// CPhotoPubView ��Ϣ�������

void CPhotoPubView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: Add your specialized code here and/or call the base class
	CPhotoPubDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CSize sizeTotal;
	// TODO: �������ͼ�ĺϼƴ�С
	IplImage *pImg=pDoc->GetImage();
	if (!pImg) return;
	sizeTotal.cx = pImg->width;
	sizeTotal.cy = pImg->height;
	SetScrollSizes(MM_TEXT, sizeTotal);

	Invalidate();

}

BOOL CPhotoPubView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;//CScrollView::OnEraseBkgnd(pDC);
}

