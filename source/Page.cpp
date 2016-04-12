#include "StdAfx.h"
#include "Page.h"

#define MAX_BUF_SIZE 1024
bool CPage::LoadPage(FILE *&fp)
{
	char buf[MAX_BUF_SIZE];

	if (!fp) return false;

	fscanf_s(fp,"%9s",buf,MAX_BUF_SIZE);	//PageSize=
	fscanf_s(fp,"%s",buf,MAX_BUF_SIZE);
	m_size=buf;

	if (m_size=="7") {
		m_width=17.8;
		m_height=12.7;
	}else if (m_size=="6"){
		m_width=15.2;
		m_height=10.5;
	}else if (m_size=="5"){
		m_width=12.7;
		m_height=8.9;
	}
	m_SizeDiscription=m_size+"¥Á";

	int typenum=0;
	fscanf_s(fp,"%8s",buf,MAX_BUF_SIZE);	//TypeNum=
	fscanf_s(fp,"%d",&typenum);

	for( int i=0;i<typenum;++i)
	{
		fscanf_s(fp,"%10s",buf,MAX_BUF_SIZE);	//PhotoSize=
		fscanf_s(fp,"%s",buf,MAX_BUF_SIZE);
		m_photosizes.push_back(CString(buf[0]));

		double wphoto=0;
		double hphoto=0;
		if (!strcmp(buf,"1")) {
			wphoto=3.6;
			hphoto=2.6;
		}else if (!strcmp(buf,"2")){
			wphoto=5.2;
			hphoto=3.6;
		}else if (!strcmp(buf,"1'")){
			wphoto=2.6;
			hphoto=3.6;
		}else if (!strcmp(buf,"2'")){
			wphoto=3.6;
			hphoto=5.2;
		}	

		int nphoto=0;

		fscanf_s(fp,"%9s",buf,MAX_BUF_SIZE);	//PhotoNum=
		fscanf_s(fp,"%d",&nphoto);
		
		_itoa_s(nphoto,buf,10);
		m_PageDiscription+=m_photosizes.back()+"¥Á"+buf+"’≈";

		for (int j=0;j<nphoto;++j){
			double x,y;
			fscanf_s(fp,"%lf,%lf",&x,&y);
			CPhoto photo(x,y,wphoto,hphoto);
			m_photos.push_back(photo);
		}
		
	}
	if (typenum>1) m_PageDiscription+="ªÏ≈≈";

	return true;
}


void CPage::PreView(IplImage * &pImage)
{
	int width=int(m_width*10);
	int height=int(m_height*10);
	pImage=cvCreateImage(cvSize(width,height), IPL_DEPTH_8U,3);
	cvRectangle( pImage, cvPoint(0,0), cvPoint(width-1,height-1), cvScalar(0,0,0));

	vector<CPhoto>::iterator it;
	for(it=m_photos.begin();it!=m_photos.end();++it) {
		int left,right,top,bottom;
		left=int(it->x*10);
		right=int(it->right()*10);
		top=int(it->y*10);
		bottom=int(it->bottom()*10);
		cvRectangle( pImage, cvPoint(left,top), cvPoint(right,bottom), cvScalar(0,0,0));
	}
}

void CPage::PreView(CDC *pDC,CRect *pRect)
{
	int width=int(m_width*10);
	int height=int(m_height*10);
	
	int left=abs(pRect->Width()-width)/2 + pRect->left;
	int right=left+width;
	int top=abs(pRect->Height()-height)/2 + pRect->top;
	int bot=top+height;

	pDC->Rectangle(left,top,right,bot);

	vector<CPhoto>::iterator it;
	for(it=m_photos.begin();it!=m_photos.end();++it) {
		int l=int(it->x*10) + left;
		int r=int(it->right()*10) + left;
		int t=int(it->y*10) + top;
		int b=int(it->bottom()*10) + top;
		pDC->Rectangle(l,t,r,b);
	}
}

void CPage::Publish(IplImage * pImg, IplImage *& pPub)
{

	int pagewidth=Cm2Dot(m_width);
	int pageheight=Cm2Dot(m_height);

	//create a white page
	pPub=cvCreateImage(cvSize(pagewidth,pageheight), IPL_DEPTH_8U,3);
	memset(pPub->imageData,255,pPub->widthStep*pPub->height);

	vector<CPhoto>::iterator it;
	for(it=m_photos.begin();it!=m_photos.end();++it) {
		//get the target position
		CvMat subPub;
		cvGetSubRect(pPub,&subPub,it->GetCvRect());
		
		//scale the original image
		int photowidth=Cm2Dot(it->width);
		int photoheight=Cm2Dot(it->height);
		double x_scale=((double)pImg->width)/photowidth;
		double y_scale=((double)pImg->height)/photoheight;
		int newheight,newwidth;
		if (x_scale>y_scale) { // scale to y
			newheight=photoheight;
			newwidth=int(pImg->width/y_scale+0.5);
		}else{	//scale to x
			newwidth=photowidth;
			newheight=int(pImg->height/x_scale+0.5);
		}
		IplImage *tmp=cvCreateImage(cvSize(newwidth,newheight),IPL_DEPTH_8U,3);
		cvResize(pImg,tmp,CV_INTER_CUBIC);
		//crop the image
		CvMat subPht;
		cvGetSubRect(tmp,&subPht,cvRect(abs(photowidth-newwidth)/2,abs(photoheight-newheight)/2,photowidth,photoheight));

		//copy into the page
		cvCopy(&subPht,&subPub);

		cvReleaseImage(&tmp);
		//cvRectangle( pPub, cvPoint(left,top), cvPoint(right,bottom), cvScalar(0,0,0));
	}
	
}
