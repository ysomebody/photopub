// PhotoPubView.cpp : CPhotoPubView 类的实现
//

#include "stdafx.h"
#include "PhotoPub.h"

#include "MainFrm.h"
#include "PhotoPubDoc.h"
#include "PhotoPubView.h"
#include "InputStrDlg.h"
#include "StringNumber.h"
#include "ImgLdrDll/ImageLoader.h"
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
	ON_BN_CLICKED(ID_STOP, &CPhotoPubView::ReleaseWatching)
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CPhotoPubView 构造/析构

CPhotoPubView::CPhotoPubView()
:m_InitStr("330100198106240000")
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
void CPhotoPubView::DrawImage(CDC *pDC,IplImage *pImg)
{
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


void CPhotoPubView::OnDraw(CDC* pDC)
{

	// TODO: 在此处为本机数据添加绘制代码
	CPhotoPubDoc *pDoc=GetDocument();
	IplImage *pImg=pDoc->GetImage();

	DrawImage(pDC,pImg);
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

/*void CPhotoPubView::OnBnClickedImageOpen()
{
	m_ratio=1;
	((CMainFrame*)GetParent())->SetRatio(int(m_ratio*100));
	CPhotoPubDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->OnOpenImage();
	
	// TODO: Add your control notification handler code here
}
*/
bool CPhotoPubView::SetupWatching(const CString& path)
{
	m_CurFiles.clear();
	const int nFiles=GetAllFolderFile(path, "*.jpg", m_CurFiles);
	/*if (nFiles!=0) {
		AfxMessageBox("错误！\n该文件夹中已存在照片！");	
		return false;
	}*/
	m_WatchingPath=path;
	SetTimer(1,500,NULL);
	return true;
}
void CPhotoPubView::ReleaseWatching()
{
	KillTimer(1);
	CPhotoPubDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->m_bWatching=false;
	
	((CMainFrame*)GetParentFrame())->SetWatching(false);

}
void GetNewName(const CString &init,CString &newname)
{
	CInputStrDlg dlg(NULL,"照片名",init);
	dlg.DoModal();
	newname=dlg.m_input;
}
void CPhotoPubView::OnTimer(UINT_PTR nIDEvent)
{
	KillTimer(1);
	CPhotoPubDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: Add your message handler code here and/or call default
	int nFiles=m_CurFiles.size();
	set<CString> CurFiles1;
	const int nFiles1=GetAllFolderFile(m_WatchingPath, "*.jpg", CurFiles1);
	if (nFiles==nFiles1) {
		SetTimer(1,500,NULL);
		return;
	}else if (nFiles>nFiles1){
		AfxMessageBox("同步发生错误，无法恢复，请重新运行！\n");
		exit(1);
	}
	
	//Here Comes a new Photo
	//wait transfering
	Sleep(1000);
	//process
	set<CString>::iterator it1;
	for (it1=CurFiles1.begin();it1!=CurFiles1.end();++it1)
	{
		if ( m_CurFiles.find(*it1)==m_CurFiles.end() ) //not exist before
		{
			((CMainFrame*)GetParent())->ShowWindow(SW_SHOWMAXIMIZED);

			CClientDC cdc(this);
			CRect clrct;
			GetClientRect(&clrct);
			int width=clrct.Width();
			int height=clrct.Height();

			cvReleaseImage(&pDoc->m_pOpenedImage);
			IplImage *pImg=pDoc->m_pOpenedImage=LoadFImage(m_WatchingPath+"\\"+*it1+".jpg");
			m_ratio=min(double(width)/pImg->width,double(height)/pImg->height);
			((CMainFrame*)GetParent())->SetRatio(int(m_ratio*100));
			OnUpdate(NULL,NULL,NULL);
			DrawImage(&cdc,pImg);

			CString newname;
			int res;
			do {
				GetNewName(m_InitStr,newname);
				res=rename(m_WatchingPath+"\\"+*it1+".jpg",m_WatchingPath+"\\"+newname+".jpg");
			}while( 
				(res==0) ? false : (true,
					(errno==EEXIST) ? AfxMessageBox("文件名重复！请重试"): (
						(errno==EINVAL) ? AfxMessageBox("文件名含非法字符！请重试"):AfxMessageBox("未知错误！请重试")
				) ) );
			m_CurFiles.insert(newname);
			m_InitStr=newname;
			CStringNumber snum(m_InitStr);
			snum.Increase();
			snum.ToString(m_InitStr);
			//break;
		}
	}
	
	CScrollView::OnTimer(nIDEvent);

	SetTimer(1,500,NULL);
}
