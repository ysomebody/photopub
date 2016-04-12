#pragma once

#include "cv.h"
#include "photo.h"
#include <vector>
#include <map>
#include <set>
using namespace std;


int CountAllFolderFile(const CString &path, const CString &name);
int GetAllFolderFile(const CString &path, const CString &name, set<CString> &files);
bool TextoutToImage(const CString &text,IplImage* pImg);

class CPage;

typedef vector<CElement *> VecPElement;
typedef vector<CPage *> VecPPage;

class CPage
{
	double m_width;	  //in cm
	double m_height;  //in cm
	CString m_Size;
	map<CString,int> m_TypeNum;
	VecPElement m_photos;
	IplImage *m_pTitle;

	//no assignment allowed
	CPage(const CPage &);
	CPage &operator=(const CPage &);
	
	bool m_bAddTitleEach;
public:
	CString m_SizeDiscription;
	CString m_PageDiscription;

	CPage(void) {}
	CPage(CString size, VecPElement photos);
	bool LoadPage(FILE *&fp);
	bool SavePage(FILE *&fp);
	~CPage();

	void PreView(CDC *pDC,CRect *pRect,int FACTOR);
	void PublishSingle(IplImage * pImg, IplImage * &pPub, const CString &text);
	int PublishDir(const CString &srcpath,const CString &tarpath);

	static void Size2WH(const CString &size, double &width, double &height);
	static void A4PreView(const CString &srcpath,const CString &tarpath, const CString &size);

private:
	void Publish(IplImage * pImg, IplImage * pPub, const CString& text);
	static void Make1Photo(IplImage *pImg,IplImage *pPub,int x,int y,const CString &size,const CString &text);
};
