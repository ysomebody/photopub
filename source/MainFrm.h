// MainFrm.h : CMainFrame ��Ľӿ�
//


#pragma once
#include "afxext.h"


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
	CDialogBar m_dlgMenu;

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:

	int GetRatio(void){	return m_dlgMenu.GetDlgItemInt(IDC_EDIT1);  }
	void SetRatio(int val) {m_dlgMenu.SetDlgItemInt(IDC_EDIT1,val);}
};


