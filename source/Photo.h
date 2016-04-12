#pragma once

#include "cv.h"

const int DPI=300;
//Math stuff
inline int Cm2Dot(double cm) {return int(DPI*cm/2.54+0.5);}

class CPhoto
{
public:
	double x;
	double y;
	double width;
	double height;

	CPhoto(double _x,double _y, double _width, double _height):x(_x),y(_y),width(_width),height(_height) {}
	~CPhoto(void){}
	double right(){return x+width;}
	double bottom(){return y+height;}
	CvRect GetCvRect(){return cvRect(Cm2Dot(x),Cm2Dot(y),Cm2Dot(width),Cm2Dot(height));}
};


