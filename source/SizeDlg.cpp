// SizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PhotoPub.h"
#include "SizeDlg.h"


// CSizeDlg dialog

IMPLEMENT_DYNAMIC(CSizeDlg, CDialog)

CSizeDlg::CSizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSizeDlg::IDD, pParent)
	, m_Size(_T(""))
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
}


BEGIN_MESSAGE_MAP(CSizeDlg, CDialog)
END_MESSAGE_MAP()


// CSizeDlg message handlers

BOOL CSizeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_SizeBox.AddString("5´ç");
	m_SizeBox.AddString("6´ç");
	m_SizeBox.AddString("7´ç");
	m_SizeBox.SetCurSel(0);

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
