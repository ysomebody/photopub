#pragma once

#include "page.h"
#include "afxwin.h"
// CCustomPageDlg dialog

class CCustomPageDlg : public CDialog
{
	DECLARE_DYNAMIC(CCustomPageDlg)

	enum {FACTOR=20};
    CPoint lastpoint;

	int m_width;	  //in dot
	int m_height;	  //in dot
	int m_PhotoType;
	CRect m_ClientRct;

public:
	CString m_Size;
	VecPElement m_photos;

	CCustomPageDlg(CWnd* pParent, CString size);   // standard constructor
	virtual ~CCustomPageDlg();

// Dialog Data
	enum { IDD = IDD_ADDPAGE };

protected:
	bool m_bCross;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedV1();
	afx_msg void OnBnClickedH2();
	afx_msg void OnBnClickedH1();
	afx_msg void OnBnClickedV2();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnBnClickedText();

	void Client2Valid(CPoint &point,const CRect &ValidRct);

	int m_save;


};
