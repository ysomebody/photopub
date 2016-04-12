// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PhotoPub.h"
#include "ProgressDlg.h"
#include "PhotoPubDoc.h"


#define UM_FINISHED (WM_USER+100) 


volatile int g_progress=0;
volatile bool g_sigend=false;

// CProgressDlg dialog

IMPLEMENT_DYNAMIC(CProgressDlg, CDialog)

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)

{

}

CProgressDlg::~CProgressDlg()
{
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	ON_WM_TIMER()
	ON_MESSAGE(UM_FINISHED, OnFinished) 
END_MESSAGE_MAP()


// CProgressDlg message handlers

BOOL CProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_progress.SetRange(0,100);
	SetTimer(1,100,NULL);
	GetDlgItem(IDC_TEXT)->SetWindowText("正在"+title+",请等待...");


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProgressDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	m_progress.SetPos(g_progress);
	CDialog::OnTimer(nIDEvent);
}
LRESULT CProgressDlg::OnFinished(WPARAM wParam, LPARAM lParam) 
{
	GetDlgItem(IDC_TEXT)->SetWindowText(title+"完成!");
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	return TRUE;
}

void CProgressDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	pThread->SuspendThread();
	if (IDNO==AfxMessageBox(title+"尚未完成，是否强行中止？",MB_YESNO)) {
		pThread->ResumeThread();
		return;
	}else{
		g_sigend=true;
		pThread->ResumeThread();
		CDialog::OnCancel();
		return;
	}
}


UINT PublishDirThreadProc(LPVOID pParam)
{
	g_progress=0;
	g_sigend=false;

	CProgressDlg* pDlg=(CProgressDlg*)pParam;

	CPage *pPageSetting=pDlg->page;
	//const int nFiles=CountAllFolderFile(pDlg->srcPath, "*.jpg");

	pPageSetting->PublishDir(pDlg->srcPath,pDlg->tarPath);

	if (!g_sigend) 
		pDlg->PostMessage(UM_FINISHED);
	return 0;
}

UINT A4PreviewThreadProc(LPVOID pParam)
{
	g_progress=0;
	g_sigend=false;

	CProgressDlg* pDlg=(CProgressDlg*)pParam;

	CPage::A4PreView(pDlg->srcPath,pDlg->tarPath,pDlg->photosize);

	if (!g_sigend) 
		pDlg->PostMessage(UM_FINISHED);
	return 0;
}
UINT MultiA4PreviewThreadProc(LPVOID pParam)
{
	g_progress=0;
	g_sigend=false;

	CProgressDlg* pDlg=(CProgressDlg*)pParam;

	CPage::MultiA4PreView(pDlg->srcPath,pDlg->tarPath,pDlg->photosize);

	if (!g_sigend) 
		pDlg->PostMessage(UM_FINISHED);
	return 0;
}
