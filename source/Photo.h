#pragma once

#include "cv.h"
#include <map>
using namespace std;

const int DPI=300;
//Math stuff
inline int Cm2Dot(double cm) {return int(DPI*cm/2.54+0.5);}
inline double Dot2Cm(int dot) {return 2.54*dot/DPI;}

class CImgBuf{
	map<CString, IplImage*> m_Buf;

	//no assignment allowed
	CImgBuf(const CImgBuf &);
	CImgBuf &operator=(const CImgBuf &);
public:
	CImgBuf();
	~CImgBuf();
	IplImage *Buffer(const CString &size) {return m_Buf.find(size)->second;}
};

extern  CImgBuf g_ImageBuf;

class CElement{
	//no assignment allowed
	CElement(const CElement &);
	CElement &operator=(const CElement &);
protected:
	IplImage *pImgBuf;
	CElement(){}
public:
	double x;
	double y;
	double width;
	double height;
	double right(){return x+width;}
	double bottom(){return y+height;}
	CvRect GetCvRect(){return cvRect(Cm2Dot(x),Cm2Dot(y),Cm2Dot(width),Cm2Dot(height));}
	virtual ~CElement(){}
};

class CPhoto : public CElement
{
	CPhoto();
	//no assignment allowed
	CPhoto(const CPhoto &);
	CPhoto &operator=(const CPhoto &);

	CString m_Size;
public:
	CPhoto(double _x,double _y, CString size):m_Size(size){
		x=_x,y=_y,Size2WH(m_Size,width,height);
		pImgBuf=g_ImageBuf.Buffer(size);
	}
	virtual ~CPhoto(void){}
	CString &GetSize(){return m_Size;}

	static void Size2WH(CString &size, double &wid,double &hi) {
		double wphoto=0;
		double hphoto=0;
		if (size[0]=='1') {
			wphoto=3.6;
			hphoto=2.6;
		}else if (size[0]=='2'){
			wphoto=5.2;
			hphoto=3.6;
		}else{
			ASSERT(FALSE);
			wphoto=hphoto=0;
		}
		if (size.Right(1)=="\'") {
			wid=hphoto;
			hi=wphoto;
		}else{
			wid=wphoto;
			hi=hphoto;
		}
	
	}
};

class CText:public CElement
{
	CText();
	//no assignment allowed
	CText(const CText &);
	CText &operator=(const CText &);
public:
	CText(double x0,double y0, double x1,double y1,IplImage *pBuf){
		x=x0;y=y0;width=x1-x0;height=y1-y0;
		pImgBuf=pBuf;
	}
	void SetBuffer(IplImage *pBuf){pImgBuf=pBuf;}
	virtual ~CText(){}
};
