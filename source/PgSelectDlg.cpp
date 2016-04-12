// PgSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PhotoPub.h"
#include "MainFrm.h"
#include "PhotoPubDoc.h"
#include "PgSelectDlg.h"
#include <set>
using namespace std;

// CPgSelectDlg dialog

IMPLEMENT_DYNAMIC(CPgSelectDlg, CDialog)

CPgSelectDlg::CPgSelectDlg(CWnd* pParent ,VecPPage &pages,bool bCanPreview)
	: CDialog(CPgSelectDlg::IDD, pParent), m_PageSettings(pages)
	, m_bGenPreview(TRUE), m_bCanPreview(bCanPreview)
{
}

CPgSelectDlg::~CPgSelectDlg()
{
}

void CPgSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_PageSizeList);
	DDX_Control(pDX, IDC_COMBO2, m_PageList);
	DDX_Check(pDX, IDC_GENPREV, m_bGenPreview);
}


BEGIN_MESSAGE_MAP(CPgSelectDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CPgSelectDlg::OnCbnSelchangeCombo1)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_COMBO2, &CPgSelectDlg::OnCbnSelchangeCombo2)
END_MESSAGE_MAP()


// CPgSelectDlg message handlers

BOOL CPgSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	//CPhotoPubDoc *pDoc=(CPhotoPubDoc *)((CMainFrame *)GetParent())->GetActiveDocument();
	//const vector<CPage> &pages=pDoc->m_PreDefinedPages;
	size_t size=m_PageSettings.size();
	set<CString> pagetypes;
	for (size_t i=0;i<size;++i){
		pagetypes.insert(m_PageSettings[i]->m_SizeDiscription);
	}
	set<CString>::iterator it;
	for (it=pagetypes.begin();it!=pagetypes.end();++it){
		m_PageSizeList.AddString(*it);
	}
	m_PageSizeList.SetCurSel(0);

	CString pagesize;
	m_PageSizeList.GetLBText(0,pagesize);
	for (size_t i=0;i<size;++i){
		if (m_PageSettings[i]->m_SizeDiscription==pagesize) 
			m_PageList.AddString(m_PageSettings[i]->m_PageDiscription);
	}
	m_PageList.SetCurSel(0);

	m_pCurrentPageSetting=GetCurrentPageSetting();

	if (!m_bCanPreview) {
		((CButton*)GetDlgItem(IDC_GENPREV))->SetCheck(false);
		((CButton*)GetDlgItem(IDC_GENPREV))->EnableWindow(false);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPgSelectDlg::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here
	m_PageList.ResetContent();
	CString pagesize;
	m_PageSizeList.GetLBText(m_PageSizeList.GetCurSel(),pagesize);
	size_t size=m_PageSettings.size();
	for (size_t i=0;i<size;++i){
		if (m_PageSettings[i]->m_SizeDiscription==pagesize) 
			m_PageList.AddString(m_PageSettings[i]->m_PageDiscription);
	}
	m_PageList.SetCurSel(0);

	m_pCurrentPageSetting=GetCurrentPageSetting();
	Invalidate();
}

void CPgSelectDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	CRect rct;
	GetDlgItem(IDC_PREVIEW)->GetWindowRect(&rct);
	ScreenToClient(&rct);

	m_pCurrentPageSetting->PreView(&dc,&rct,20);
}


CPage * CPgSelectDlg::GetCurrentPageSetting(void)
{
	CString pagesize;
	CString pageset;
	m_PageSizeList.GetLBText(m_PageSizeList.GetCurSel(),pagesize);
	m_PageList.GetLBText(m_PageList.GetCurSel(),pageset);
	
	size_t size=m_PageSettings.size();
	for (size_t i=0;i<size;++i){
		if (m_PageSettings[i]->m_SizeDiscription==pagesize
			&& m_PageSettings[i]->m_PageDiscription==pageset	) 
			return m_PageSettings[i];
	}

	return NULL;
}

void CPgSelectDlg::OnCbnSelchangeCombo2()
{
	// TODO: Add your control notification handler code here
	m_pCurrentPageSetting=GetCurrentPageSetting();
	Invalidate();
}
