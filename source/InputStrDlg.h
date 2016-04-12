#pragma once


// CInputStrDlg dialog

class CInputStrDlg : public CDialog
{
	DECLARE_DYNAMIC(CInputStrDlg)

	CString m_what;

public:

	CInputStrDlg(CWnd* pParent, const CString &what, const CString &input=_T(""));   // standard constructor
	virtual ~CInputStrDlg();

// Dialog Data
	enum { IDD = IDD_InputStrDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CString m_input;
};
