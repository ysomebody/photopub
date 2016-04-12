// PhotoPubDoc.h : CPhotoPubDoc 类的接口
//


#pragma once

#include "cv.h"
#include "page.h"
#include <vector>
using namespace std;



class CPhotoPubDoc : public CDocument
{
protected: // 仅从序列化创建
	CPhotoPubDoc();
	DECLARE_DYNCREATE(CPhotoPubDoc)

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CPhotoPubDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
protected:
	IplImage *m_pOpenedImage;
	IplImage *m_pPreviewImage;
	IplImage *m_pPublishingImage;
	CString m_filename;
public:
	IplImage * GetImage(void);
	VecPPage m_PreDefinedPages;
	CPage *m_pPageSetting;
public:
	afx_msg void OnOpenImage();
	afx_msg void OnBnClickedSelpage();
	afx_msg void OnBnClickedSavePreview();
	afx_msg void OnBnClickedBatchPub();
	afx_msg void OnBnClickedCstpage();

protected:
	enum {ORG,PRV,SEL} m_Show;
	bool ReadPreDefPages(void);
	bool WriteDefPages(void);

public:
	afx_msg void OnBnClickedThumb();
};


