// PhotoPubView.h : CPhotoPubView 类的接口
//


#pragma once
#include "afxwin.h"
#include <set>

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
	double m_ratio;
	CString m_WatchingPath;
	CString m_InitStr;

// 生成的消息映射函数
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

#ifndef _DEBUG  // PhotoPubView.cpp 中的调试版本
inline CPhotoPubDoc* CPhotoPubView::GetDocument() const
   { return reinterpret_cast<CPhotoPubDoc*>(m_pDocument); }
#endif

