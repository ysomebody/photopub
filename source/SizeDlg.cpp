// SizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PhotoPub.h"
#include "SizeDlg.h"


// CSizeDlg dialog

IMPLEMENT_DYNAMIC(CSizeDlg, CDialog)

CSizeDlg::CSizeDlg(CWnd* pParent ,const CString &title, const CStringArray &sizelist)
	: CDialog(CSizeDlg::IDD, pParent),
	m_title(title),
	m_sizelist(sizelist)
{
}

CSizeDlg::~CSizeDlg()
{
}

void CSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO3, m_SizeBox);
	DDX_CBString(pDX, IDC_COMBO3, m_Size);
	DDX_Control(pDX, IDC_DSC, m_discSTAT);
}


BEGIN_MESSAGE_MAP(CSizeDlg, CDialog)
END_MESSAGE_MAP()


// CSizeDlg message handlers

BOOL CSizeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	INT_PTR size=m_sizelist.GetCount();
	for (INT_PTR i=0;i<size;++i) {
		m_SizeBox.AddString(m_sizelist.GetAt(i));
	}
	m_SizeBox.SetCurSel(0);
	this->SetWindowText(CString("«Î—°‘Ò")+m_title+"≥ﬂ¥Á");
	m_discSTAT.SetWindowText(m_title+"≥ﬂ¥Á");

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
