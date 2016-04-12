// PhotoPubDoc.cpp : CPhotoPubDoc 类的实现
//

#include "stdafx.h"
#include <io.h>
#include "PhotoPub.h"

#include "PhotoPubDoc.h"
#include "ImgLdrDll\\Imageloader.h"
#include "PgSelectDlg.h"
#include "CustomPageDlg.h"
#include "SizeDlg.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPhotoPubDoc

IMPLEMENT_DYNCREATE(CPhotoPubDoc, CDocument)

BEGIN_MESSAGE_MAP(CPhotoPubDoc, CDocument)
	ON_BN_CLICKED(ID_IMAGE_OPEN, &CPhotoPubDoc::OnOpenImage)
	ON_BN_CLICKED(ID_SELPAGE, &CPhotoPubDoc::OnBnClickedSelpage)
	ON_BN_CLICKED(ID_PREVIEW, &CPhotoPubDoc::OnBnClickedSavePreview)
	ON_BN_CLICKED(ID_BATCHPUB, &CPhotoPubDoc::OnBnClickedBatchPub)
	ON_BN_CLICKED(ID_CSTPAGE, &CPhotoPubDoc::OnBnClickedCstpage)
	ON_BN_CLICKED(IDC_THUMB, &CPhotoPubDoc::OnBnClickedThumb)
END_MESSAGE_MAP()


// CPhotoPubDoc 构造/析构

#define MAX_BUF_SIZE 1024

CPhotoPubDoc::CPhotoPubDoc()
: m_pOpenedImage(NULL),
m_pPreviewImage(NULL),
m_pPublishingImage(NULL),
m_pPageSetting(NULL),
m_Show(ORG)
{
	// TODO: 在此添加一次性构造代码
	ReadPreDefPages();

}

CPhotoPubDoc::~CPhotoPubDoc()
{

	for (VecPPage::iterator it=m_PreDefinedPages.begin();it!=m_PreDefinedPages.end();++it){
		delete (*it);
		*it=NULL;
	}
	cvReleaseImage(&m_pOpenedImage);
	cvReleaseImage(&m_pPreviewImage);
	cvReleaseImage(&m_pPublishingImage);
}

BOOL CPhotoPubDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	SetTitle("");

	return TRUE;
}




// CPhotoPubDoc 序列化

void CPhotoPubDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CPhotoPubDoc 诊断

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


// CPhotoPubDoc 命令

void CPhotoPubDoc::OnOpenImage()
{
	CFileDialog dlg(TRUE, "*.jpg","",\
		OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,\
		"JPEG格式(*.jpg; *.jpeg) |*.jpg;*jpeg|BMP格式|*.bmp|All Files (*.*)|*.*||",NULL);

	char title[]= {"打开图像"};
	dlg.m_ofn.lpstrTitle= title;
	if (dlg.DoModal() == IDOK) 
	{
		CString path= dlg.GetPathName(); // contain the
										// selected filename
		m_filename=path.Right(path.GetLength()-path.ReverseFind('\\')-1);

		cvReleaseImage(&m_pOpenedImage);
		cvReleaseImage(&m_pPreviewImage);
		cvReleaseImage(&m_pPublishingImage);
		m_pOpenedImage=LoadFImage(path);

		m_Show=ORG;
		UpdateAllViews(NULL);

		SetTitle(path.Right(path.GetLength()-path.ReverseFind('\\')-1));
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
	if (!m_pOpenedImage) {
		AfxMessageBox("请先打开一张照片");
		return;
	}

	CPgSelectDlg dlg(AfxGetApp()->GetMainWnd(),m_PreDefinedPages,m_pOpenedImage!=NULL);
	if (IDOK==dlg.DoModal()) {
		m_pPageSetting=dlg.m_pCurrentPageSetting;
		if (dlg.m_bGenPreview) {
			cvReleaseImage(&m_pPublishingImage);
			m_pPageSetting->PublishSingle(m_pOpenedImage,m_pPublishingImage,m_filename.Left(m_filename.Find('.')));
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
	GetModuleFileName(NULL,buf,MAX_BUF_SIZE);
	CString pathname(buf);
	pathname=pathname.Left(pathname.ReverseFind('\\')+1);

	FILE *fp;
	if( fopen_s( &fp, pathname+"pages.fmt", "r" ) )
		return false;
	while(1){
		fscanf_s(fp,"%s",buf,MAX_BUF_SIZE);
		if ( !strcmp(buf,"[End]") ) break;
		CPage *pPage=new CPage;
		pPage->LoadPage(fp);
		m_PreDefinedPages.push_back(pPage);
	}
	fclose(fp);
	return true;
}

bool CPhotoPubDoc::WriteDefPages(void)
{
	char buf[MAX_BUF_SIZE];
	GetModuleFileName(NULL,buf,MAX_BUF_SIZE);
	CString pathname(buf);
	pathname=pathname.Left(pathname.ReverseFind('\\')+1);

	char tmpname[MAX_BUF_SIZE]="tmpXXXXXX";
	FILE *fptmp,*fpfmt;
	if (_mktemp_s(tmpname,MAX_BUF_SIZE))
		return false;
	if( fopen_s( &fptmp, pathname+tmpname, "w" ) )
		return false;
	if( fopen_s( &fpfmt, pathname+"pages.fmt", "r" ) )
		return false;
	size_t size=m_PreDefinedPages.size();
	for (size_t i=0;i<size;++i){
		fprintf_s(fptmp,"[%d]\n",i,MAX_BUF_SIZE);
		m_PreDefinedPages[i]->SavePage(fptmp);
	}
	fprintf_s(fptmp,"%s","[End]",MAX_BUF_SIZE);

	fclose(fptmp);
	fclose(fpfmt);

   if( remove( pathname+"pages.fmt" )  )
      return false;

   if (rename(pathname+tmpname,pathname+"pages.fmt"))
	   return false;


	return true;
}

void CPhotoPubDoc::OnBnClickedSavePreview()
{
	// TODO: Add your control notification handler code here
	if (!m_pPublishingImage) {
		AfxMessageBox("请先生成排版照片。");
		return;
	}

	CFileDialog dlg(FALSE, "*.jpg","",\
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,\
		"JPEG格式(*.jpg; *.jpeg) |*.jpg;*jpeg|BMP格式|*.bmp|All Files (*.*)|*.*||",NULL);

	char title[]= {"保存排版照片"};
	dlg.m_ofn.lpstrTitle= title;
	if (dlg.DoModal() == IDOK) 
	{
		CString path= dlg.GetPathName(); // contain the
										// selected filename
		SaveFImage(path,m_pPublishingImage);
	}	
}


void CPhotoPubDoc::OnBnClickedBatchPub()
{
	char szDir[MAX_PATH];
	BROWSEINFO bi;
	ITEMIDLIST *pidl;

	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDir;
	bi.lpszTitle = "请选择照片所在的文件夹:";
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;

	pidl = SHBrowseForFolder(&bi);
	if(pidl == NULL)
		return ;
	if(!SHGetPathFromIDList(pidl, szDir))
		return ;
	CString SrcPath(szDir);
	CString TarPath(SrcPath+"\\排版");

	int n=CountAllFolderFile(SrcPath,"*.jpg");
	if (!n) {
		AfxMessageBox("该文件夹中没有JPG文件！");
		return;
	}

	// TODO: Add your control notification handler code here
	CPgSelectDlg dlg(AfxGetApp()->GetMainWnd(),m_PreDefinedPages,m_pOpenedImage!=NULL);
	if (IDCANCEL==dlg.DoModal()) {
		return;
	}else{
		m_pPageSetting=dlg.m_pCurrentPageSetting;
	};

	CreateDirectory(TarPath, NULL);

	CProgressDlg ProgDlg;
	ProgDlg.page=m_pPageSetting;
	ProgDlg.srcPath=SrcPath;
	ProgDlg.tarPath=TarPath;
	ProgDlg.title="排版";
	ProgDlg.pThread =
		AfxBeginThread(PublishDirThreadProc, &ProgDlg,
			           THREAD_PRIORITY_NORMAL);
	ProgDlg.DoModal();

	

}

void CPhotoPubDoc::OnBnClickedCstpage()
{
	// TODO: Add your control notification handler code here
	CSizeDlg sizeDlg;
	if (IDCANCEL==sizeDlg.DoModal())
		return;
	CString size=sizeDlg.m_Size.Left(1);

	CCustomPageDlg dlg(NULL,size);
	if (IDCANCEL==dlg.DoModal())
		return;
	CPage *pPage=new CPage(dlg.m_Size,dlg.m_photos);
	m_PreDefinedPages.push_back(pPage);
	if (dlg.m_save) {
		WriteDefPages();
	}

}

void CPhotoPubDoc::OnBnClickedThumb()
{
	// TODO: Add your control notification handler code here
	char szDir[MAX_PATH];
	BROWSEINFO bi;
	ITEMIDLIST *pidl;

	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDir;
	bi.lpszTitle = "请选择照片所在的文件夹:";
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;

	pidl = SHBrowseForFolder(&bi);
	if(pidl == NULL)
		return ;
	if(!SHGetPathFromIDList(pidl, szDir))
		return ;
	CString SrcPath(szDir);
	CString TarPath(SrcPath+"\\A4缩略图");

	int n=CountAllFolderFile(SrcPath,"*.jpg");
	if (!n) {
		AfxMessageBox("该文件夹中没有JPG文件！");
		return;
	}

	CreateDirectory(TarPath, NULL);

	//CPage::A4PreView(SrcPath,TarPath);

	CProgressDlg ProgDlg;
	ProgDlg.page=m_pPageSetting;
	ProgDlg.srcPath=SrcPath;
	ProgDlg.tarPath=TarPath;
	ProgDlg.title="生成缩略图";
	ProgDlg.pThread =
		AfxBeginThread(A4PreviewThreadProc, &ProgDlg,
			           THREAD_PRIORITY_NORMAL);
	ProgDlg.DoModal();

}
