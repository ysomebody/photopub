#pragma once

#include "Page.h"
#include "afxcmn.h"
#include "resource.h"

extern volatile int g_progress;
extern volatile bool g_sigend;
UINT PublishDirThreadProc(LPVOID pParam);
UINT A4PreviewThreadProc(LPVOID pParam);


// CProgressDlg dialog

class CProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProgressDlg();

	CWinThread *pThread;
	CString srcPath;
	CString tarPath;
	CPage *page;
	CString title;
	CString photosize;

// Dialog Data
	enum { IDD = IDD_PROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnFinished(WPARAM wParam, LPARAM lParam); 



protected:
	CProgressCtrl m_progress;

	virtual void OnCancel();
};

