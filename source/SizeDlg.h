#pragma once
#include "afxwin.h"


// CSizeDlg dialog

class CSizeDlg : public CDialog
{
	DECLARE_DYNAMIC(CSizeDlg)

public:
	CSizeDlg(CWnd* pParent,const CString &title, const CStringArray &sizelist);   // standard constructor
	virtual ~CSizeDlg();


// Dialog Data
	enum { IDD = IDD_PAGESIZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CComboBox m_SizeBox;

	const CString m_title;
	const CStringArray &m_sizelist;


public:
	virtual BOOL OnInitDialog();
public:
	CString m_Size;

public:
	CStatic m_discSTAT;
};
