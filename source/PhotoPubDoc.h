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
	IplImage *m_pPreviewImage;
	IplImage *m_pPublishingImage;
	CString m_filename;
//	bool m_bWatching;
public:
	IplImage *m_pOpenedImage;
	IplImage * GetImage(void);
	VecPPage m_PreDefinedPages;
	CPage *m_pPageSetting;
	bool m_bWatching;
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
	afx_msg void OnBnClickedWatch();
public:
	afx_msg void OnUpdateWatch(CCmdUI *pCmdUI);
public:
	afx_msg void OnUpdateStop(CCmdUI *pCmdUI);
public:
	afx_msg void OnUpdateImageOpen(CCmdUI *pCmdUI);
public:
	afx_msg void OnUpdatePreview(CCmdUI *pCmdUI);
public:
	afx_msg void OnUpdateSelpage(CCmdUI *pCmdUI);

public:
	afx_msg void OnUpdateBatchpub(CCmdUI *pCmdUI);
public:
	afx_msg void OnUpdateThumb(CCmdUI *pCmdUI);
};


