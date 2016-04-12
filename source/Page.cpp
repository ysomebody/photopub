#include "StdAfx.h"
#include "Page.h"
#include <highgui.h>
#include "ImageLoader\ImageLoader.h"



#define MAX_BUF_SIZE 1024
bool CPage::LoadPage(FILE *&fp)
{
	char buf[MAX_BUF_SIZE];

	if (!fp) return false;

	
	//set up page
	fscanf_s(fp,"%9s",buf,MAX_BUF_SIZE);	//PageSize=
	fscanf_s(fp,"%s",buf,MAX_BUF_SIZE);
	m_Size=buf;

	Size2WH(m_Size, m_width, m_height);
	m_SizeDiscription=m_Size+"寸";

	
	//set up photos on the page
	int typenum=0;
	fscanf_s(fp,"%8s",buf,MAX_BUF_SIZE);	//TypeNum=
	fscanf_s(fp,"%d",&typenum);

	for( int i=0;i<typenum;++i)
	{
		fscanf_s(fp,"%10s",buf,MAX_BUF_SIZE);	//PhotoSize=
		fscanf_s(fp,"%s",buf,MAX_BUF_SIZE);
		CString photosize=buf;

		int nphoto=0;
		fscanf_s(fp,"%9s",buf,MAX_BUF_SIZE);	//PhotoNum=
		fscanf_s(fp,"%d",&nphoto);
		//_itoa_s(nphoto,buf,10);

		pair <CString, int> data(photosize,nphoto);
		pair< map<CString,int>::iterator, bool > res;
		res=m_TypeNum.insert(data);
		if (res.second==false)
			m_TypeNum[buf]+=nphoto;

		for (int j=0;j<nphoto;++j){
			double x,y;
			fscanf_s(fp,"%lf,%lf",&x,&y);
			CPhoto photo(x,y,photosize);
			m_photos.push_back(photo);
		}
		//alloc the memory for the type of photo
		int wid=Cm2Dot(m_photos.back().width);
		int hi=Cm2Dot(m_photos.back().height);
		IplImage *p=cvCreateImage(cvSize(wid,hi),IPL_DEPTH_8U,3);
		pair <CString, IplImage*> idata(photosize,p);
		pair< map<CString,IplImage*>::iterator, bool > ires;
		ires=m_TypeImage.insert(idata);
		if (ires.second==false) {
			cvReleaseImage(&p);
		}
	}

	//统计尺寸，生成版面描述
	int num;
	for (int i=1;i<=2;++i){
		num=0;
		map<CString,int>::iterator it;
		_itoa_s(i,buf,10);
		it = m_TypeNum.find(buf);
		if (it!=m_TypeNum.end()) num+=it->second;
		buf[1]='\'';buf[2]='\0';
		it = m_TypeNum.find(buf);
		if (it!=m_TypeNum.end()) num+=it->second;
		if (num>0){
			buf[1]='\0';
			m_PageDiscription+=CString(buf)+"寸";
			_itoa_s(num,buf,10);
			m_PageDiscription+=CString(buf)+"张";
		}
	}
	if (typenum>1) m_PageDiscription+="混排";
	return true;
}
CPage::CPage(CString size, vector<CPhoto> photos)
:m_Size(size),m_photos(photos)
{

	//生成页面尺寸、描述
	Size2WH(m_Size, m_width, m_height);
	m_SizeDiscription=m_Size+"寸";
	
	//生成照片种类统计、缓存
	size_t nphts=photos.size();
	for (size_t i=0;i<nphts;++i) {
		CString photosize=photos[i].GetSize();
		pair <CString, int> data(photosize,1);
		pair< map<CString,int>::iterator, bool > res;
		res=m_TypeNum.insert(data);
		if (res.second==false){
			m_TypeNum[photosize]++;
		}else{
			int wid=Cm2Dot(photos[i].width);
			int hi=Cm2Dot(photos[i].height);
			IplImage *p=cvCreateImage(cvSize(wid,hi),IPL_DEPTH_8U,3);
			pair <CString, IplImage*> idata(photosize,p);
			pair< map<CString,IplImage*>::iterator, bool > ires;
			ires=m_TypeImage.insert(idata);
			if (ires.second==false) {
				cvReleaseImage(&p);
			}
		}
	}

	//统计尺寸,生成版面描述
	char buf[MAX_BUF_SIZE];
	int num,tpnum=0;
	for (int i=1;i<=2;++i){
		num=0;
		map<CString,int>::iterator it;
		_itoa_s(i,buf,10);
		it = m_TypeNum.find(buf);
		if (it!=m_TypeNum.end()) num+=it->second;
		buf[1]='\'';buf[2]='\0';
		it = m_TypeNum.find(buf);
		if (it!=m_TypeNum.end()) num+=it->second;
		if (num>0){
			tpnum++;
			buf[1]='\0';
			m_PageDiscription+=CString(buf)+"寸";
			_itoa_s(num,buf,10);
			m_PageDiscription+=CString(buf)+"张";
		}
	}
	if (tpnum>1) m_PageDiscription+="混排";
}
CPage::~CPage(void)
{
	map<CString,IplImage *>::iterator it;
	for (it=m_TypeImage.begin();it!=m_TypeImage.end();++it)
		cvReleaseImage(&(it->second));
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
		pDC->TextOut(l+1,t+1,it->GetSize().Left(1)+"寸");
	}
}

void CPage::PublishSingle(IplImage * pImg, IplImage *& pPub)
{
	//create a white page
	int pagewidth=Cm2Dot(m_width);
	int pageheight=Cm2Dot(m_height);

	pPub=cvCreateImage(cvSize(pagewidth,pageheight), IPL_DEPTH_8U,3);
	memset(pPub->imageData,255,pPub->widthStep*pPub->height);

	Publish(pImg,pPub);
	
}
void CPage::Size2WH(CString &size, double &width, double &height)
{
	if (size=="7") {
		width=17.8;
		height=12.7;
	}else if (size=="6"){
		width=15.2;
		height=10.5;
	}else if (size=="5"){
		width=12.7;
		height=8.9;
	}else{
		ASSERT(FALSE);
		width=height=0;
	}
}

int CPage::PublishDir(const CString &srcpath,const CString &tarpath)
{
	int pagewidth=Cm2Dot(m_width);
	int pageheight=Cm2Dot(m_height);

	set<CString> Filenames;
	const int nFiles=GetAllFolderFile(srcpath, "*.jpg", Filenames);
	if (!nFiles) {
		return 0;
	}else{
		const int PreservedSize=6*1024*1024*3;
		set<CString>::iterator it;

		void *PreservedSpace=new char[PreservedSize];
		IplImage *pPub=cvCreateImage(cvSize(pagewidth,pageheight),IPL_DEPTH_8U,3);
		
		for (it=Filenames.begin();it!=Filenames.end();++it) {

			//read the file
			IplImage imghead;
			imghead.imageData=(char *)PreservedSpace;

			int res=LoadImageToBuf(srcpath+"\\"+*it,&imghead,PreservedSize);
			if( res!=LOAD_SUC)
			{
				AfxMessageBox(CString("读取文件")+*it+"失败");
				continue;
			}

			//here: publish
			memset(pPub->imageData,255,pPub->widthStep*pPub->height);
			Publish(&imghead,pPub);

			cvSaveImage(tarpath+"\\排版_"+*it,pPub);
		}

		cvReleaseImage(&pPub);
		delete []PreservedSpace;
		return nFiles;
	}

}

void CPage::Publish(IplImage *pImg, IplImage *pPub)
{

	//Preprocess the photos into required size
	map<CString,IplImage *>::iterator it;
	for (it=m_TypeImage.begin();it!=m_TypeImage.end();++it) {
		
		bool isT=(it->first.Right(1)!="'");

		CvMat subImg;
		if (isT) {
			int phtheight=it->second->width;
			int phtwidth=it->second->height;
			int imgwidth=pImg->width;
			int imgheight=pImg->height;
			double phtwtoh=((double)phtwidth)/phtheight;
			double imgwtoh=((double)imgwidth)/imgheight;
			if (imgwtoh>phtwtoh) {
				//crop on width
				int width=int(imgheight*phtwtoh);
				cvGetSubRect(pImg,&subImg,cvRect(abs(imgwidth-width)/2,0,width,imgheight));
			}else{
				//crop on height
				int height=int(imgwidth/phtwtoh);
				cvGetSubRect(pImg,&subImg,cvRect(0,abs(imgheight-height)/2,imgwidth,height));
			}
			//resize
			IplImage *tmp=cvCreateImage(cvSize(phtwidth,phtheight),IPL_DEPTH_8U,3);
			cvResize(&subImg,tmp);
			//transpose
			cvTranspose(tmp,it->second);
		}else{
			int phtwidth=it->second->width;
			int phtheight=it->second->height;
			int imgwidth=pImg->width;
			int imgheight=pImg->height;
			double phtwtoh=((double)phtwidth)/phtheight;
			double imgwtoh=((double)imgwidth)/imgheight;
			if (imgwtoh>phtwtoh) {
				//crop on width
				int width=int(imgheight*phtwtoh);
				cvGetSubRect(pImg,&subImg,cvRect(abs(imgwidth-width)/2,0,width,imgheight));
			}else{
				//crop on height
				int height=int(imgwidth/phtwtoh);
				cvGetSubRect(pImg,&subImg,cvRect(0,abs(imgheight-height)/2,imgwidth,height));
			}
			//resize
			cvResize(&subImg,it->second);
		}
	}

	size_t size=m_photos.size();
	for (size_t i=0;i<size;++i) {
		//get the target position
		CPhoto &photo=m_photos[i];
		CvMat subPub;
		cvGetSubRect(pPub,&subPub,photo.GetCvRect());
		cvCopy(m_TypeImage[photo.GetSize()],&subPub);
	}
	
}


int GetAllFolderFile(const CString &path, const CString &name, set<CString> &files)
{
	int nFileFound=0;
	HANDLE hFind;
	WIN32_FIND_DATA fd;
	CString SearchName=path+"\\*.jpg"; //search all jpg files

	if ((hFind = ::FindFirstFile (SearchName, &fd)) == INVALID_HANDLE_VALUE)
		return nFileFound;
	do 
	{
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // a file not subdir
		{
			nFileFound++;
			files.insert(fd.cFileName);

		}
	} while (::FindNextFile (hFind, &fd));
	::FindClose (hFind);
	return nFileFound;
}