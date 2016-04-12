// PhotoPubView.h : CPhotoPubView 类的接口
//


#pragma once
#include "afxwin.h"


class CPhotoPubView : public CScrollView
{
protected: // 仅从序列化创建
	CPhotoPubView();
	DECLARE_DYNCREATE(CPhotoPubView)

// 属性
public:
	CPhotoPubDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
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
	

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // PhotoPubView.cpp 中的调试版本
inline CPhotoPubDoc* CPhotoPubView::GetDocument() const
   { return reinterpret_cast<CPhotoPubDoc*>(m_pDocument); }
#endif

