// MainFrm.h : CMainFrame ��Ľӿ�
//


#pragma once
#include "afxext.h"
#include "TrueColorToolBar.h"

class CMainFrame : public CFrameWnd
{
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
	CStatusBar  m_wndStatusBar;
	//CDialogBar m_dlgMenu;
	CTrueColorToolBar m_wndToolBar;

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	void SetRatio(int val) 
	{
		CString str;
		str.Format("%d%%",val);
		m_wndStatusBar.SetPaneText(2,str,TRUE);
	}
	void SetWatching(bool bwatch)
	{
		if (bwatch) m_wndStatusBar.SetPaneText(1,"���ڼ����ļ���",TRUE);
		else m_wndStatusBar.SetPaneText(1,"",TRUE);
	}
};


