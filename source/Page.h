#pragma once

#include "cv.h"
#include "photo.h"
#include <vector>
#include <map>
#include <set>
using namespace std;

int GetAllFolderFile(const CString &path, const CString &name, set<CString> &files);

class CPage
{
	double m_width;	  //in cm
	double m_height;  //in cm
	CString m_Size;
	vector<CPhoto> m_photos;
	map<CString,int> m_TypeNum;
	map<CString,IplImage *> m_TypeImage;
	

public:
	CString m_SizeDiscription;
	CString m_PageDiscription;

	CPage(void) {}
	CPage(CString size, vector<CPhoto> photos);
	bool LoadPage(FILE *&fp);
	~CPage();

	CPage(const CPage &pg):m_width(pg.m_width),m_height(pg.m_height),m_Size(pg.m_Size),
		m_SizeDiscription(pg.m_SizeDiscription),m_PageDiscription(pg.m_PageDiscription),
		m_photos(pg.m_photos),m_TypeNum(pg.m_TypeNum),m_TypeImage(pg.m_TypeImage) 
	{
		map<CString,IplImage *>::iterator it;
		for (it=m_TypeImage.begin();it!=m_TypeImage.end();++it){
			it->second=cvCreateImage(cvSize(it->second->width,it->second->height),IPL_DEPTH_8U,3);
		}
	}
	CPage &operator=(const CPage &pg)
	{
		m_width=pg.m_width;
		m_height=pg.m_height;
		m_Size=pg.m_Size;
		m_SizeDiscription=pg.m_SizeDiscription;
		m_PageDiscription=pg.m_PageDiscription;
		m_photos=pg.m_photos;
		m_TypeNum=pg.m_TypeNum;
		m_TypeImage=pg.m_TypeImage; 

		map<CString,IplImage *>::iterator it;
		for (it=m_TypeImage.begin();it!=m_TypeImage.end();++it){
			it->second=cvCreateImage(cvSize(it->second->width,it->second->height),IPL_DEPTH_8U,3);
		}

		return *this;
	}
	
	void PreView(CDC *pDC,CRect *pRect);
	void PublishSingle(IplImage * pImg, IplImage * &pPub);
	int PublishDir(const CString &srcpath,const CString &tarpath);

	static void Size2WH(CString &size, double &width, double &height);

private:
	void Publish(IplImage * pImg, IplImage * pPub);
};
