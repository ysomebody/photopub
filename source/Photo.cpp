#include "StdAfx.h"
#include "Photo.h"

CImgBuf g_ImageBuf;

#define SIZENUM 4



CImgBuf::CImgBuf()
{
	static CString sizes[SIZENUM]={
		"1",
		"1'",
		"2",
		"2'"
	};
	double width,height;
	for (int i=0;i<SIZENUM;++i) {
		CPhoto::Size2WH(sizes[i],width,height);
		IplImage *p=cvCreateImage(cvSize(Cm2Dot(width),Cm2Dot(height)),IPL_DEPTH_8U,3);
		m_Buf[sizes[i]]=p;
	}
}
CImgBuf::~CImgBuf()
{
	for (map<CString,IplImage*>::iterator it=m_Buf.begin();it!=m_Buf.end();++it) {
		cvReleaseImage(&(it->second));
	}
}