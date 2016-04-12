// PhotoPubView.h : CPhotoPubView ��Ľӿ�
//


#pragma once
#include "afxwin.h"


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
	

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // PhotoPubView.cpp �еĵ��԰汾
inline CPhotoPubDoc* CPhotoPubView::GetDocument() const
   { return reinterpret_cast<CPhotoPubDoc*>(m_pDocument); }
#endif

