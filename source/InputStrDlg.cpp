// InputStrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PhotoPub.h"
#include "InputStrDlg.h"


// CInputStrDlg dialog

IMPLEMENT_DYNAMIC(CInputStrDlg, CDialog)

CInputStrDlg::CInputStrDlg(CWnd* pParent , const CString &what, const CString &input)
	: CDialog(CInputStrDlg::IDD, pParent)
	, m_input(input)
{
	m_what=what;
}

CInputStrDlg::~CInputStrDlg()
{
}

void CInputStrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INP, m_input);
}


BEGIN_MESSAGE_MAP(CInputStrDlg, CDialog)
END_MESSAGE_MAP()


// CInputStrDlg message handlers
BOOL CInputStrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	GetDlgItem(IDC_TIP)->SetWindowText("«Î ‰»Î"+m_what+"£∫");
	SetWindowText(" ‰»Î"+m_what);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
