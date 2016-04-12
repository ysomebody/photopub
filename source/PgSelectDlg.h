#pragma once
#include "afxwin.h"


// CPgSelectDlg dialog
#include <vector>
using namespace std;

class CPgSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CPgSelectDlg)

public:
	CPgSelectDlg(CWnd* pParent , VecPPage &pages, bool bCanPreview);   // standard constructor
	virtual ~CPgSelectDlg();

// Dialog Data
	enum { IDD = IDD_PGSEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CPage *m_pCurrentPageSetting;
protected:
	CComboBox m_PageSizeList;
	CComboBox m_PageList;
	VecPPage &m_PageSettings;
	bool m_bCanPreview;

public:
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnPaint();
	afx_msg void OnCbnSelchangeCombo2();
public:
	BOOL m_bGenPreview;
	CPage * GetCurrentPageSetting(void);
};
