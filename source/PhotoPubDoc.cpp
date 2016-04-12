// PhotoPubDoc.cpp : CPhotoPubDoc ���ʵ��
//

#include "stdafx.h"
#include "PhotoPub.h"

#include "PhotoPubDoc.h"
#include "highgui.h"
#include "PgSelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPhotoPubDoc

IMPLEMENT_DYNCREATE(CPhotoPubDoc, CDocument)

BEGIN_MESSAGE_MAP(CPhotoPubDoc, CDocument)
	ON_BN_CLICKED(ID_IMAGE_OPEN, &CPhotoPubDoc::OnOpenImage)
	ON_BN_CLICKED(ID_SELPAGE, &CPhotoPubDoc::OnBnClickedSelpage)
	ON_BN_CLICKED(ID_PREVIEW, &CPhotoPubDoc::OnBnClickedSavePreview)
END_MESSAGE_MAP()


// CPhotoPubDoc ����/����

#define MAX_BUF_SIZE 1024

CPhotoPubDoc::CPhotoPubDoc()
: m_pOpenedImage(NULL),
m_pPreviewImage(NULL),
m_pPublishingImage(NULL),
m_pPageSetting(NULL),
m_Show(ORG)
{
	// TODO: �ڴ����һ���Թ������
	ReadPreDefPages();

}

CPhotoPubDoc::~CPhotoPubDoc()
{
	cvReleaseImage(&m_pOpenedImage);
	cvReleaseImage(&m_pPreviewImage);
	cvReleaseImage(&m_pPublishingImage);
}

BOOL CPhotoPubDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CPhotoPubDoc ���л�

void CPhotoPubDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CPhotoPubDoc ���

#ifdef _DEBUG
void CPhotoPubDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPhotoPubDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CPhotoPubDoc ����

void CPhotoPubDoc::OnOpenImage()
{
	CFileDialog dlg(TRUE, "*.bmp","",\
		OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,\
		"ͼ���ļ� (*.bmp; *.jpg; *.png; *.tiff) |*.bmp;*.jpg;*.png;*.tiff|All Files (*.*)|*.*||",NULL);

	char title[]= {"��ͼ��"};
	dlg.m_ofn.lpstrTitle= title;
	if (dlg.DoModal() == IDOK) 
	{
		CString path= dlg.GetPathName(); // contain the
										// selected filename
		cvReleaseImage(&m_pOpenedImage);
		cvReleaseImage(&m_pPreviewImage);
		cvReleaseImage(&m_pPublishingImage);
		m_pOpenedImage=cvLoadImage(path,-1);

		m_Show=ORG;
		UpdateAllViews(NULL);
	}
}

IplImage * CPhotoPubDoc::GetImage(void)
{
	if (m_Show==ORG)
		return m_pOpenedImage;
	else if (m_Show==SEL)
		return m_pPreviewImage;
	else if (m_Show==PRV)
		return m_pPublishingImage;
	return NULL;
}


void CPhotoPubDoc::OnBnClickedSelpage()
{
	// TODO: Add your control notification handler code here


/*	if (!m_pOpenedImage) {
		AfxMessageBox("���ȴ�һ����Ƭ��");
		return;
	}
*/
	CPgSelectDlg dlg(AfxGetApp()->GetMainWnd(),m_PreDefinedPages,m_pOpenedImage);
	if (IDOK==dlg.DoModal()) {
		m_pPageSetting=dlg.m_pCurrentPageSetting;
		if (dlg.m_bGenPreview) {
			cvReleaseImage(&m_pPublishingImage);
			m_pPageSetting->Publish(m_pOpenedImage,m_pPublishingImage);
			m_Show=PRV;
		}else{
			m_Show=ORG;
		}
		UpdateAllViews(NULL);
	}else{
		UpdateAllViews(NULL);
	};
	
	

}

bool CPhotoPubDoc::ReadPreDefPages(void)
{
	char buf[MAX_BUF_SIZE];
	FILE *fp;
	if( fopen_s( &fp, "pages.fmt", "r" ) )
		return false;
	while(1){
		fscanf_s(fp,"%s",buf,MAX_BUF_SIZE);
		if ( !strcmp(buf,"[End]") ) break;
		CPage page;
		page.LoadPage(fp);
		m_PreDefinedPages.push_back(page);
	}
	fclose(fp);
	return true;
}

void CPhotoPubDoc::OnBnClickedSavePreview()
{
	// TODO: Add your control notification handler code here
	if (!m_pPublishingImage) {
		AfxMessageBox("��δ����Ԥ������ѡ���ʽ������Ԥ����Ƭ��");
		return;
	}

	CFileDialog dlg(FALSE, "*.bmp","",\
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,\
		"ͼ���ļ� (*.bmp; *.jpg; *.png; *.tiff) |*.bmp;*.jpg;*.png;*.tiff|All Files (*.*)|*.*||",NULL);

	char title[]= {"�����Ű���Ƭ"};
	dlg.m_ofn.lpstrTitle= title;
	if (dlg.DoModal() == IDOK) 
	{
		CString path= dlg.GetPathName(); // contain the
										// selected filename
		cvSaveImage(path,m_pPublishingImage);
	}	
}
