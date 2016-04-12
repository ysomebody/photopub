#pragma once

#include "cv.h"

const int DPI=300;
//Math stuff
inline int Cm2Dot(double cm) {return int(DPI*cm/2.54+0.5);}

class CPhoto
{
	CString m_Size;
public:
	double x;
	double y;
	double width;
	double height;

	CPhoto(double _x,double _y, CString size):x(_x),y(_y),m_Size(size){
		Size2WH(m_Size,width,height);
	}
	~CPhoto(void){}
	double right(){return x+width;}
	double bottom(){return y+height;}
	CString &GetSize(){return m_Size;}
	CvRect GetCvRect(){return cvRect(Cm2Dot(x),Cm2Dot(y),Cm2Dot(width),Cm2Dot(height));}

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


