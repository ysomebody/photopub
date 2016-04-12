// PhotoPubDoc.h : CPhotoPubDoc ��Ľӿ�
//


#pragma once

#include "cv.h"
#include "page.h"
#include <vector>
using namespace std;

class CPhotoPubDoc : public CDocument
{
protected: // �������л�����
	CPhotoPubDoc();
	DECLARE_DYNCREATE(CPhotoPubDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CPhotoPubDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
protected:
	IplImage *m_pOpenedImage;
	IplImage *m_pPreviewImage;
	IplImage *m_pPublishingImage;
public:
	IplImage * GetImage(void);
	vector<CPage> m_PreDefinedPages;
	CPage *m_pPageSetting;
public:
	afx_msg void OnOpenImage();
public:
	afx_msg void OnBnClickedSelpage();
protected:
	enum {ORG,PRV,SEL} m_Show;
	bool ReadPreDefPages(void);
public:
	afx_msg void OnBnClickedSavePreview();
};

