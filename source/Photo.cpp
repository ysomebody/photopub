#include "StdAfx.h"
#include "Photo.h"

CImgBuf g_ImageBuf;




CImgBuf::CImgBuf()
{
	static CString sizes[]={
		"1",
		"1'",
		"2",
		"2'",
		"1T",
		"1'T",
		"2T",
		"2'T",
	};
	double width,height;
	for (int i=0;i<4;++i) {
		CPhoto::Size2WH(sizes[i],width,height);
		IplImage *p=cvCreateImage(cvSize(Cm2Dot(width),Cm2Dot(height)),IPL_DEPTH_8U,3);
		m_Buf[sizes[i]]=p;
	}
	for (int i=4;i<8;++i) {
		CPhoto::Size2WH(sizes[i].Left(sizes[i].GetLength()-1),width,height);
		double wordheight=0.45;
		if (width>height) {
			IplImage *p=cvCreateImage(cvSize(Cm2Dot(wordheight),Cm2Dot(height)),IPL_DEPTH_8U,3);
			m_Buf[sizes[i]]=p;
		}else{
			IplImage *p=cvCreateImage(cvSize(Cm2Dot(width),Cm2Dot(wordheight)),IPL_DEPTH_8U,3);
			m_Buf[sizes[i]]=p;
		}
	}
}
CImgBuf::~CImgBuf()
{
	for (map<CString,IplImage*>::iterator it=m_Buf.begin();it!=m_Buf.end();++it) {
		cvReleaseImage(&(it->second));
	}
}