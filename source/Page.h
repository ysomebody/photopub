#pragma once

#include "cv.h"
#include "photo.h"
#include <vector>
using namespace std;

class CPage
{
	double m_width;	  //in cm
	double m_height;  //in cm
	CString m_size;
	vector<CString> m_photosizes;
	vector<CPhoto> m_photos;

public:
	CString m_SizeDiscription;
	CString m_PageDiscription;
	CPage(void) {m_width=0;m_height=0;}
	CPage(double width, double height) {m_width=width;m_height=height;}
	~CPage(void){}

	void SetPageSize(double width,double height) {m_width=width;m_height=height;}
	bool LoadPage(FILE *&fp);
	void PreView(IplImage *&pImage);
	void PreView(CDC *pDC,CRect *pRect);

public:
	void Publish(IplImage * pImg, IplImage *& pPub);
};
