// PhotoPubView.h : CPhotoPubView ��Ľӿ�
//


#pragma once
#include "afxwin.h"
#include <set>

class CPhotoPubView : public CScrollView
{
protected: // �������л�����
	CPhotoPubView();
	DECLARE_DYNCREATE(CPhotoPubView)

// ����
public:
	CPhotoPubDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CPhotoPubView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    uchar display_buffer[sizeof(BITMAPINFOHEADER) + 1024];
	BITMAPINFO* m_bmi_buffer;
    BITMAPINFOHEADER* m_bmih_buffer;
	double m_ratio;
	CString m_WatchingPath;
	CString m_InitStr;

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	//afx_msg void OnBnClickedImageOpen();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void ReleaseWatching();

	set<CString> m_CurFiles;
public:
	bool SetupWatching(const CString& path);
	void DrawImage(CDC *pDC,IplImage *pImg);
};

#ifndef _DEBUG  // PhotoPubView.cpp �еĵ��԰汾
inline CPhotoPubDoc* CPhotoPubView::GetDocument() const
   { return reinterpret_cast<CPhotoPubDoc*>(m_pDocument); }
#endif

