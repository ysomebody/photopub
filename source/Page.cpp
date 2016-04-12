#include "StdAfx.h"
#include "Page.h"
#include "ProgressDlg.h"
#include "ImgLdr\ImgLdr.h"


#define MAX_BUF_SIZE 1024

CPage::CPage(CString size, VecPElement photos)
:m_Size(size),m_photos(photos),m_pTitle(NULL), m_bAddTitleEach(false)
{

	//生成页面尺寸、描述
	Size2WH(m_Size, m_width, m_height);
	m_SizeDiscription=m_Size+"寸";

	//生成照片种类统计、设置文字缓存
	for (VecPElement::iterator it=m_photos.begin();it!=m_photos.end();++it){
		if (CPhoto *pPhoto=dynamic_cast<CPhoto *>(*it)) {
			CString &photosize=pPhoto->GetSize();
			map<CString, int>::iterator findres=m_TypeNum.find(photosize);
			if (findres==m_TypeNum.end()) {
				m_TypeNum[photosize]=1;
			}else{
				findres->second++;
			}
		}else if(CText *pText=dynamic_cast<CText *>(*it)){
			m_pTitle=cvCreateImage(cvSize(Cm2Dot(pText->width),Cm2Dot(pText->height)),IPL_DEPTH_8U,3);
			pText->SetBuffer(m_pTitle);
		}
	}

	//统计尺寸，生成版面描述
	int typenum=0;
	char buf[MAX_BUF_SIZE];
	for (int i=1;i<=2;++i){
		int num=0;
		map<CString,int>::iterator it;
		_itoa_s(i,buf,10);
		it = m_TypeNum.find(buf);
		if (it!=m_TypeNum.end()) num+=it->second;
		buf[1]='\'';buf[2]='\0';
		it = m_TypeNum.find(buf);
		if (it!=m_TypeNum.end()) num+=it->second;
		if (num>0){
			typenum++;
			buf[1]='\0';
			m_PageDiscription+=CString(buf)+"寸";
			_itoa_s(num,buf,10);
			m_PageDiscription+=CString(buf)+"张";
		}
	}
	if (typenum>1) m_PageDiscription+="混排";
	if (m_bAddTitleEach) m_PageDiscription+="，照片加字";
	if (m_pTitle) m_PageDiscription+="，整版加字";
}
CPage::~CPage(void)
{
	for(VecPElement::iterator it=m_photos.begin();it!=m_photos.end();++it) {
		delete (*it);
		*it=NULL;
	}
	cvReleaseImage(&m_pTitle);
}

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

		//add photo num to TypeNum
		pair <CString, int> data(photosize,nphoto);
		pair< map<CString,int>::iterator, bool > res;
		res=m_TypeNum.insert(data);
		if (res.second==false)
			m_TypeNum[buf]+=nphoto;

		//Generate photos
		for (int j=0;j<nphoto;++j){
			double x,y;
			fscanf_s(fp,"%lf,%lf",&x,&y);
			CPhoto *pPhoto=new CPhoto(x,y,photosize);
			m_photos.push_back(pPhoto);
		}
	}

	//添加文字及其缓存
	int IsTitle;
	fscanf_s(fp,"%9s",buf,MAX_BUF_SIZE);	//AddTitle=
	fscanf_s(fp,"%d",&IsTitle);
	if (IsTitle) {
		double x0,y0,x1,y1;
		fscanf_s(fp,"%lf,%lf,%lf,%lf",&x0,&y0,&x1,&y1);
		m_pTitle=cvCreateImage(cvSize(Cm2Dot(x1-x0),Cm2Dot(y1-y0)),IPL_DEPTH_8U,3);
		CText *pText=new CText(x0,y0,x1,y1,m_pTitle);
		m_photos.push_back(pText);
	}else{
		m_pTitle=NULL;
	}

	fscanf_s(fp,"%13s",buf,MAX_BUF_SIZE);	//AddTitleEach=
	fscanf_s(fp,"%d",&m_bAddTitleEach);

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
	if (m_bAddTitleEach) m_PageDiscription+="，照片加字";
	if (m_pTitle) m_PageDiscription+="，整版加字";

	return true;
}

bool CPage::SavePage(FILE *&fp)
{
	//char buf[MAX_BUF_SIZE];

	if (!fp) return false;
	
	//save page
	fprintf_s(fp,"PageSize=%s\n",m_Size);	//PageSize=

	//save photos on the page
	size_t typenum=m_TypeNum.size();
	fprintf_s(fp,"TypeNum=%d\n",typenum);	//TypeNum=

	for(map<CString,int>::iterator it=m_TypeNum.begin();it!=m_TypeNum.end();++it)
	{
		fprintf_s(fp,"PhotoSize=%s\n",it->first);	//PhotoSize=
		fprintf_s(fp,"PhotoNum=%d\n",it->second);	//PhotoNum=

		//save photos
		for (VecPElement::iterator ele=m_photos.begin();ele!=m_photos.end();++ele){
			if (CPhoto *pPhoto=dynamic_cast<CPhoto *>(*ele)) {
				if (pPhoto->GetSize()==it->first)
					fprintf_s(fp,"%.2f,%.2f\n",pPhoto->x,pPhoto->y);
			}
		}
	}

	
	//save text
	int IsTitle=(m_pTitle)?1:0;

	fprintf_s(fp,"AddTitle=%d\n",IsTitle);	//AddTitle=
	if (IsTitle) {
		double x0,y0,x1,y1;
		for (VecPElement::iterator ele=m_photos.begin();ele!=m_photos.end();++ele){
			if (CText *pText=dynamic_cast<CText *>(*ele)){
				x0=pText->x;y0=pText->y;
				x1=pText->x+pText->width;
				y1=pText->y+pText->height;
				break;
			}
		}
		fprintf_s(fp,"%.1f,%.1f,%.1f,%.1f\n",x0,y0,x1,y1);
	}

	fprintf_s(fp,"AddTitleEach=%d\n",m_bAddTitleEach);	//AddTitleEach=

	return true;
}

void CPage::PreView(CDC *pDC,CRect *pRect,int FACTOR)
{
	int width=int(m_width*FACTOR);
	int height=int(m_height*FACTOR);
	
	int left=abs(pRect->Width()-width)/2 + pRect->left;
	int right=left+width;
	int top=abs(pRect->Height()-height)/2 + pRect->top;
	int bot=top+height;

	pDC->Rectangle(left,top,right,bot);

	VecPElement::iterator it;
	for(it=m_photos.begin();it!=m_photos.end();++it) {
		CElement *pElement=*it;
		int l=int(pElement->x*FACTOR) + left;
		int r=int(pElement->right()*FACTOR) + left;
		int t=int(pElement->y*FACTOR) + top;
		int b=int(pElement->bottom()*FACTOR) + top;
		pDC->Rectangle(l,t,r,b);
		if (CPhoto *pPht=dynamic_cast<CPhoto *>(pElement)) {
			pDC->TextOut(l+1,t+1,pPht->GetSize().Left(1)+"寸");
			int theight=12;
			if (m_bAddTitleEach) {
				if (r-l<b-t) {
					//pDC->Rectangle(l,b-1,r,b+theight);
					CFont font;
					font.CreateFont(
						theight-2,                        // nHeight
						0,                         // nWidth
						0,                         // nEscapement
						0,                         // nOrientation
						FW_NORMAL,                 // nWeight
						FALSE,                     // bItalic
						FALSE,                     // bUnderline
						0,                         // cStrikeOut
						ANSI_CHARSET,              // nCharSet
						OUT_DEFAULT_PRECIS,        // nOutPrecision
						CLIP_DEFAULT_PRECIS,       // nClipPrecision
						DEFAULT_QUALITY,           // nQuality
						DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
						"宋体");                 // lpszFacename
					CFont *pof=pDC->SelectObject(&font);
					pDC->TextOut(l+1,b,"显示文件名");
					pDC->SelectObject(pof);							
				}else{
					//pDC->Rectangle(r-1,t,r+theight,b);
					CFont font;
					font.CreateFont(
						theight-2,                 // nHeight
						0,                         // nWidth
						900,                         // nEscapement
						900,                         // nOrientation
						FW_NORMAL,                 // nWeight
						FALSE,                     // bItalic
						FALSE,                     // bUnderline
						0,                         // cStrikeOut
						ANSI_CHARSET,              // nCharSet
						OUT_DEFAULT_PRECIS,        // nOutPrecision
						CLIP_DEFAULT_PRECIS,       // nClipPrecision
						DEFAULT_QUALITY,           // nQuality
						DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
						"宋体");                 // lpszFacename
					CFont *pof=pDC->SelectObject(&font);
					pDC->TextOut(r,b-1,"显示文件名");
					pDC->SelectObject(pof);					}

			}
		}else if(CText *pTxt=dynamic_cast<CText *>(pElement)){
			CFont font;
			font.CreateFont(
				b-t-2,                        // nHeight
				0,                         // nWidth
			   0,                         // nEscapement
			   0,                         // nOrientation
			   FW_NORMAL,                 // nWeight
			   FALSE,                     // bItalic
			   FALSE,                     // bUnderline
			   0,                         // cStrikeOut
			   ANSI_CHARSET,              // nCharSet
			   OUT_DEFAULT_PRECIS,        // nOutPrecision
			   CLIP_DEFAULT_PRECIS,       // nClipPrecision
			   DEFAULT_QUALITY,           // nQuality
			   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
			   "宋体");                 // lpszFacename
			CFont *pof=pDC->SelectObject(&font);
			pDC->TextOut(l+1,t+1,"（显示文件名）");
			pDC->SelectObject(pof);			
		}
	}
}

void CPage::PublishSingle(IplImage * pImg, IplImage *& pPub, const CString& text)
{
	//create a white page
	int pagewidth=Cm2Dot(m_width);
	int pageheight=Cm2Dot(m_height);

	pPub=cvCreateImage(cvSize(pagewidth,pageheight), IPL_DEPTH_8U,3);
	memset(pPub->imageData,255,pPub->widthStep*pPub->height);

	Publish(pImg,pPub,text);
	
}
void CPage::Size2WH(const CString &size, double &width, double &height)
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
	int nProcessedFiles=0;
	if (!nFiles) {
		return 0;
	}else{
		const int PreservedSize=6*1024*1024*3;
		set<CString>::iterator it;

		void *PreservedSpace=new char[PreservedSize];
		IplImage *pPub=cvCreateImage(cvSize(pagewidth,pageheight),IPL_DEPTH_8U,3);
		
	for (it=Filenames.begin();!g_sigend && it!=Filenames.end();++it) {

			//read the file
			IplImage imghead;
			imghead.imageData=(char *)PreservedSpace;

			int res=LoadImageToBuf(srcpath+"\\"+*it+".jpg",&imghead,PreservedSize);
			if( res!=LOAD_SUC)
			{
				AfxMessageBox(CString("读取文件")+*it+".jpg失败,跳过...");
				continue;
			}

			//here: publish
			memset(pPub->imageData,255,pPub->widthStep*pPub->height);
			Publish(&imghead,pPub,*it);

			SaveFImage(tarpath+"\\"+*it+".jpg",pPub);

			nProcessedFiles++;
			g_progress=nProcessedFiles*100/nFiles;

		}

		cvReleaseImage(&pPub);
		delete []PreservedSpace;
		return nFiles;
		
	}

}

void CPage::Publish(IplImage *pImg, IplImage *pPub, const CString &text)
{

	//Preprocess the photos into required size and copy to buffer
	for (map<CString,int>::iterator it=m_TypeNum.begin();it!=m_TypeNum.end();++it) {
		bool isT=(it->first.Right(1)!="'");
		IplImage *pBuf=g_ImageBuf.Buffer(it->first);
		CvMat subImg;
		if (isT) {
			int phtheight=pBuf->width;
			int phtwidth=pBuf->height;
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
				//cvGetSubRect(pImg,&subImg,cvRect(0,abs(imgheight-height)/2,imgwidth,height));
				cvGetSubRect(pImg,&subImg,cvRect(0,0,imgwidth,height));
			}
			//resize
			IplImage *tmp=cvCreateImage(cvSize(phtwidth,phtheight),IPL_DEPTH_8U,3);
			cvResize(&subImg,tmp);
			//transpose + flip = rot90
			cvTranspose(tmp,pBuf);
			cvFlip(pBuf);
			cvReleaseImage(&tmp);
		}else{
			int phtwidth=pBuf->width;
			int phtheight=pBuf->height;
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
				//cvGetSubRect(pImg,&subImg,cvRect(0,abs(imgheight-height)/2,imgwidth,height));
				cvGetSubRect(pImg,&subImg,cvRect(0,0,imgwidth,height));
			}
			//resize
			cvResize(&subImg,pBuf);
		}
		IplImage *pBufT=g_ImageBuf.Buffer(it->first+"T");
		if (!isT) {
			TextoutToImage(text,pBufT);
		}else{
			IplImage *tmp=cvCreateImage(cvSize(pBufT->height,pBufT->width),IPL_DEPTH_8U,3);
			TextoutToImage(text,tmp);
			cvTranspose(tmp,pBufT);
			cvReleaseImage(&tmp);
			cvFlip(pBufT);
		}
	}

	//Preprocess the Text
	if (m_pTitle) {
		TextoutToImage(text,m_pTitle);
	}

	//publish the photos
	size_t size=m_photos.size();
	for (size_t i=0;i<size;++i) {
		//get the target position
		CElement *pElement=m_photos[i];
		CvMat subPub;
		CvRect rct=pElement->GetCvRect();
		cvGetSubRect(pPub,&subPub,rct);
		IplImage *pBuf, *pBufT;
		if (CPhoto *pPht=dynamic_cast<CPhoto*>(pElement)) {
			pBuf=g_ImageBuf.Buffer(pPht->GetSize());
			cvCopy(pBuf,&subPub);
			//each title
			if (m_bAddTitleEach)//!m_pTitle) 
			{
				CString sizetext=pPht->GetSize()+"T";
				pBufT=g_ImageBuf.Buffer(sizetext);
				if (sizetext[1]=='\'')
					cvGetSubRect(pPub,&subPub,cvRect(rct.x,rct.y+rct.height,rct.width,pBufT->height));
				else
					cvGetSubRect(pPub,&subPub,cvRect(rct.x+rct.width,rct.y,pBufT->width,rct.height));
				cvCopy(pBufT,&subPub);	
			}
		}else{
			pBuf=m_pTitle;
			cvCopy(pBuf,&subPub);
		}
	}



}

bool TextoutToImage(const CString &text,IplImage* pImg)
{
	CDC DrawingDC;
	CBitmap DrawingBMP;

	DrawingDC.CreateCompatibleDC(CDC::FromHandle(::GetDC(AfxGetMainWnd()->m_hWnd)));
	DrawingBMP.CreateCompatibleBitmap(CDC::FromHandle(::GetDC(AfxGetMainWnd()->m_hWnd)),pImg->width,pImg->height);
	DrawingDC.SelectObject(DrawingBMP);

	CPen pen(PS_SOLID,1,RGB(255,255,255));
	CPen *pop=DrawingDC.SelectObject(&pen);
	DrawingDC.Rectangle(0,0,pImg->width,pImg->height);
	DrawingDC.SelectObject(pop);
	
	
	int w=min(pImg->width/text.GetLength(),pImg->height/2);

	CFont font;
	font.CreateFont(
		pImg->height,                        // nHeight
		w,                         // nWidth
	   0,                         // nEscapement
	   0,                         // nOrientation
	   FW_NORMAL,                 // nWeight
	   FALSE,                     // bItalic
	   FALSE,                     // bUnderline
	   0,                         // cStrikeOut
	   ANSI_CHARSET,              // nCharSet
	   OUT_DEFAULT_PRECIS,        // nOutPrecision
	   CLIP_DEFAULT_PRECIS,       // nClipPrecision
	   DEFAULT_QUALITY,           // nQuality
	   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
	   "宋体");                 // lpszFacename
	CFont *pof=DrawingDC.SelectObject(&font);
	//DrawingDC.TextOut(0,0,text,CString::StringLength(text));
	CRect txtrct(CPoint(0,0),CSize(pImg->width,pImg->height));
	DrawingDC.DrawText(text,-1,txtrct,DT_CENTER);
	DrawingDC.SelectObject(pof);

	BITMAP bmp;
	int j =	DrawingBMP.GetBitmap(&bmp);
	if (!j) return false;

	switch(bmp.bmBitsPixel){
		case 16:
			{
				BYTE* p=new BYTE[bmp.bmHeight*bmp.bmWidthBytes];
				DrawingBMP.GetBitmapBits(bmp.bmHeight*bmp.bmWidthBytes,p);
				char *data=pImg->imageData;
				for(int y=0;y<bmp.bmHeight;y++){
					int pt=y*bmp.bmWidthBytes;
					int pt1=y*pImg->widthStep;
					for(int x=0;x<bmp.bmWidth;x++){
						BYTE l=p[pt];
						BYTE h=p[pt+1];
						data[pt1+2]=(h>>3)*255/31;
						data[pt1+1]=(((h&0x07)<<3)|(l>>5))*255/63;
						data[pt1+0]=(l&0x1f)*255/31;						
						pt+=2;
						pt1+=3;
					}
				}
				delete []p;
			}
			break;
		case 24:
			DrawingBMP.GetBitmapBits(bmp.bmHeight*bmp.bmWidthBytes,pImg->imageData);
			break;
		case 32:
			{
				BYTE* p=new BYTE[bmp.bmHeight*bmp.bmWidthBytes];
				DrawingBMP.GetBitmapBits(bmp.bmHeight*bmp.bmWidthBytes,p);
				char *data=pImg->imageData;
				for(int y=0;y<bmp.bmHeight;y++){
					int pt=y*bmp.bmWidthBytes;
					int pt1=y*pImg->widthStep;
					for(int x=0;x<bmp.bmWidth;x++){
						BYTE r=p[pt+2];
						BYTE g=p[pt+1];
						BYTE b=p[pt];
						data[pt1+2]=r;
						data[pt1+1]=g;
						data[pt1+0]=b;
						pt+=4;
						pt1+=3;
					}
				}
				delete []p;
			}
			break;
		default:
			return false;
	}
				
	return true;

}


void CPage::A4PreView(const CString &srcpath,const CString &tarpath, const CString &size)
{
	//A4 210mm×297mm
	//10*12 in ==> 203 × 305 mm
	// for compact reason ==> 210 * 316
	int pagewidth=Cm2Dot(21.0);
	int pageheight=Cm2Dot(31.6);
	double w,h;
	CPhoto::Size2WH(size+"'",w,h);
	const int photow=Cm2Dot(w);
	const int photoh=Cm2Dot(h);

	int x,y;
	int LeftMargin=0;
	int HSpace = 0;
	int VSpace = 0;
	int TopMargin=0;
	int TextHeight=0;
	int PNHeight=0;
	int TitleHeight=0;

	if (size=="1"){
		LeftMargin = 5;
		TopMargin = 7;
		HSpace = 2;
		VSpace = 0;
		TextHeight = 35;
		PNHeight = 45;
		TitleHeight=photoh/2;
	}else if (size=="2"){
		LeftMargin = 125;
		TopMargin = 150;
		HSpace = 20;
		VSpace = 10;
		TextHeight = 50;
		PNHeight = 45;
		TitleHeight=photoh/5;
	}

	int lastSlashPos = 0;
	while(1) 
	{
		int pos = srcpath.Find('\\',lastSlashPos+1);
		if (pos<0)
			break;
		else
			lastSlashPos = pos;
	}

	CString FolderName = srcpath.Right(srcpath.GetLength()-lastSlashPos-1);


	set<CString> Filenames;
	const int nFiles=GetAllFolderFile(srcpath, "*.jpg", Filenames);

	if (!nFiles) {
		return;
	}else{

		const int PreservedSize=6*1024*1024*3; //for a 6M photo
		set<CString>::iterator it;


		void *PreservedSpace=new char[PreservedSize];
		IplImage *pPub=cvCreateImage(cvSize(pagewidth,pageheight),IPL_DEPTH_8U,3);

		IplImage imghead;
		imghead.imageData=(char *)PreservedSpace;

		int nProcessed=0;
		int pagecnt = 0;

		x=LeftMargin,y=TopMargin;
		
		bool bNewPage = true;
		memset(pPub->imageData,255,pPub->widthStep*pPub->height);
		pagecnt++;
		bNewPage = false;

		//add folder title
		CvMat PubRgn;
		cvGetSubRect(pPub,&PubRgn,cvRect(x,10+3*TitleHeight/5,pagewidth-2*LeftMargin,photoh/4));
		IplImage imghdr;
		imghdr.width=PubRgn.width;
		imghdr.height=PubRgn.height;
		imghdr.widthStep=PubRgn.step;
		imghdr.imageData=(char*)PubRgn.data.ptr;
		TextoutToImage(FolderName,&imghdr);
		y += TitleHeight*5/4;

		for (it=Filenames.begin();!g_sigend && it!=Filenames.end();++it) {

			//set a white page
			if (bNewPage) {
				memset(pPub->imageData,255,pPub->widthStep*pPub->height);
				pagecnt++;
				bNewPage = false;
			}

			//read the photos
			int res=LoadImageToBuf(srcpath+"\\"+*it+".jpg",&imghead,PreservedSize);
			if( res!=LOAD_SUC)
			{
				AfxMessageBox(CString("读取文件\"")+*it+".jpg\"失败,跳过...请在结束后检查该文件");
				continue;
			}

			
			//here: preview
			Make1Photo(&imghead,pPub,x,y,size,*it,TextHeight);

			//move to next photo point
			x = x + photow + HSpace ;
			if (x + photow >= pagewidth){
				x = LeftMargin;
				y = y +photoh + VSpace + TextHeight;
				if (y + photoh >= pageheight-PNHeight){
					//add page number
					CvMat PubRgn;
					cvGetSubRect(pPub,&PubRgn,cvRect(LeftMargin,pageheight-PNHeight-1,pagewidth-2*LeftMargin,PNHeight));
					IplImage imghdr;
					imghdr.width=PubRgn.width;
					imghdr.height=PubRgn.height;
					imghdr.widthStep=PubRgn.step;
					imghdr.imageData=(char*)PubRgn.data.ptr;
					char buf[20];
					sprintf(buf,"--- %d ---",pagecnt);
					TextoutToImage(buf,&imghdr);

					//save page
					CString flnm;
					flnm.Format("%d.jpg",pagecnt);
					SaveFImage(tarpath+"\\"+"预览"+flnm,pPub);
					y = TopMargin;

					bNewPage = true;
				}
			}
	
			g_progress=(++nProcessed)*100/nFiles;
		}

		if (!bNewPage)
		{
			//add page number
			CvMat PubRgn;
			cvGetSubRect(pPub,&PubRgn,cvRect(LeftMargin,pageheight-PNHeight-1,pagewidth-2*LeftMargin,PNHeight));
			IplImage imghdr;
			imghdr.width=PubRgn.width;
			imghdr.height=PubRgn.height;
			imghdr.widthStep=PubRgn.step;
			imghdr.imageData=(char*)PubRgn.data.ptr;
			char buf[20];
			sprintf(buf,"--- %d ---",pagecnt);
			TextoutToImage(buf,&imghdr);

			//save page
			CString flnm;
			flnm.Format("%d.jpg",pagecnt);
			SaveFImage(tarpath+"\\"+"预览"+flnm,pPub);
		}

		cvReleaseImage(&pPub);
		delete []PreservedSpace;
		return ;
		
	}

}

void CPage::Make1Photo(IplImage *pImg,IplImage *pPub,int x,int y,const CString &size,const CString &text, int textheight)
{
	double w,h;
	CPhoto::Size2WH(size+"'",w,h);
	int phtwidth=Cm2Dot(w);
	int phtheight=Cm2Dot(h);


	//do the photo
	CvMat PubRgn;
	cvGetSubRect(pPub,&PubRgn,cvRect(x,y,phtwidth,phtheight));

	CvMat subImg;
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
		//cvGetSubRect(pImg,&subImg,cvRect(0,abs(imgheight-height)/2,imgwidth,height));
		cvGetSubRect(pImg,&subImg,cvRect(0,0,imgwidth,height));
	}
	//resize
	cvResize(&subImg,&PubRgn);

	//do the Text
	cvGetSubRect(pPub,&PubRgn,cvRect(x,y+phtheight+1,phtwidth,textheight));
	IplImage imghdr;
	imghdr.width=PubRgn.width;
	imghdr.height=PubRgn.height;
	imghdr.widthStep=PubRgn.step;
	imghdr.imageData=(char*)PubRgn.data.ptr;
	TextoutToImage(text,&imghdr);

}



int GetAllFolderFile(const CString &path, const CString &name, set<CString> &files)
{
	int nFileFound=0;
	HANDLE hFind;
	WIN32_FIND_DATA fd;
	CString SearchName=path+"\\"+name; //search all jpg files

	if ((hFind = ::FindFirstFile (SearchName, &fd)) == INVALID_HANDLE_VALUE)
		return nFileFound;
	do 
	{
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // a file not subdir
		{
			nFileFound++;
			CString fn(fd.cFileName);
			files.insert(fn.Left(fn.Find('.')));
		}
	} while (::FindNextFile (hFind, &fd));
	::FindClose (hFind);
	return nFileFound;
}

int GetAllSubFolderFiles(const CString &path, const CString &name, map<CString,set<CString>> &folderFiles)
{
	HANDLE hFind;
	WIN32_FIND_DATA fd;
	CString SearchFolderName=path+"\\*.*"; //search all jpg files
	int nFiles=0;

	if ((hFind = ::FindFirstFile (SearchFolderName, &fd)) == INVALID_HANDLE_VALUE)
		return nFiles;
	do 
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // find a subdir
		{
			CString folder(fd.cFileName);
			if (folder=="." || folder==".." ||folder.Find("缩略图")!=-1)
				continue;
			CString SearchName=path+"\\"+folder+"\\"+name;
			int nFileFound=0;
			HANDLE hFileFind;
			if ((hFileFind = ::FindFirstFile (SearchName, &fd)) == INVALID_HANDLE_VALUE)
				continue;
			do 
			{
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // a file not subdir
				{
					nFileFound++;
					CString fn(fd.cFileName);
					folderFiles[folder].insert(fn.Left(fn.Find('.')));
				}
				if (!nFileFound)
				{
					folderFiles.erase(folder);
				}
			} while (::FindNextFile (hFileFind, &fd));
			nFiles+=nFileFound;
			::FindClose(hFileFind);
		}
	} while (::FindNextFile (hFind, &fd));
	::FindClose (hFind);
	return nFiles;
}


int CountAllFolderFile(const CString &path, const CString &name)
{
	int nFileFound=0;
	HANDLE hFind;
	WIN32_FIND_DATA fd;
	CString SearchName=path+"\\"+name; //search all jpg files

	if ((hFind = ::FindFirstFile (SearchName, &fd)) == INVALID_HANDLE_VALUE)
		return nFileFound;
	do 
	{
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // a file not subdir
		{
			nFileFound++;
		}
	} while (::FindNextFile (hFind, &fd));
	::FindClose (hFind);
	return nFileFound;
}


void CPage::MultiA4PreView(const CString &srcpath,const CString &tarpath, const CString &size)
{
	//A4 210mm×297mm
	//10*12 in ==> 203 × 305 mm
	// for compact reason ==> 210 * 316
	int pagewidth=Cm2Dot(21.0);
	int pageheight=Cm2Dot(31.6);
	double w,h;
	CPhoto::Size2WH(size+"'",w,h);
	const int photow = Cm2Dot(w);
	const int photoh = Cm2Dot(h);

	int x,y;
	int LeftMargin=0;
	int HSpace = 0;
	int VSpace = 0;
	int TopMargin=0;
	int TextHeight=0;
	int PNHeight=0;
	int TitleHeight=0;

	if (size=="1"){
		LeftMargin = 5;
		TopMargin = 7;
		HSpace = 2;
		VSpace = 0;
		TextHeight = 35;
		PNHeight = 45;
		TitleHeight=photoh/2;
	}else if (size=="2"){
		LeftMargin = 125;
		TopMargin = 70;
		HSpace = 20;
		VSpace = 10;
		TextHeight = 50;
		PNHeight = 45;
		TitleHeight=photoh/5;
	}
	x=LeftMargin,y=TopMargin;

	map<CString, set<CString> > folderFiles;
	map<CString, set<CString> >::iterator itFolder;
	int nFiles = GetAllSubFolderFiles(srcpath,"*.jpg",folderFiles);
	if (!nFiles) return;
	bool bNewPage=true;

	const int PreservedSize=6*1024*1024*3; //for a 6M photo

	void *PreservedSpace=new char[PreservedSize];
	IplImage *pPub=cvCreateImage(cvSize(pagewidth,pageheight),IPL_DEPTH_8U,3);

	IplImage imghead;
	imghead.imageData=(char *)PreservedSpace;

	int pagecnt = 0;
	int nProcessed = 0;

	for (itFolder=folderFiles.begin();itFolder!=folderFiles.end();++itFolder) {
		if (x!=LeftMargin) 
		{
			x = LeftMargin;
			y = y + photoh + VSpace + TextHeight;
			if (y + TitleHeight >= pageheight-PNHeight){
				//add page number
				CvMat PubRgn;
				cvGetSubRect(pPub,&PubRgn,cvRect(LeftMargin,pageheight-PNHeight-1,pagewidth-2*LeftMargin,PNHeight));
				IplImage imghdr;
				imghdr.width=PubRgn.width;
				imghdr.height=PubRgn.height;
				imghdr.widthStep=PubRgn.step;
				imghdr.imageData=(char*)PubRgn.data.ptr;
				char buf[20];
				sprintf(buf,"--- %d ---",pagecnt);
				TextoutToImage(buf,&imghdr);

				//save page
				CString flnm;
				flnm.Format("%d.jpg",pagecnt);
				SaveFImage(tarpath+"\\"+"预览"+flnm,pPub);
				y = TopMargin;

				bNewPage = true;
			}
		}
		if (bNewPage)
		{
			x=LeftMargin,y=TopMargin;
			memset(pPub->imageData,255,pPub->widthStep*pPub->height);
			pagecnt++;
			bNewPage = false;
		}
		
		CvMat PubRgn;
		cvGetSubRect(pPub,&PubRgn,cvRect(x,y+10+3*TitleHeight/5,pagewidth-2*LeftMargin,TitleHeight/2));
		IplImage imghdr;
		imghdr.width=PubRgn.width;
		imghdr.height=PubRgn.height;
		imghdr.widthStep=PubRgn.step;
		imghdr.imageData=(char*)PubRgn.data.ptr;
		TextoutToImage(itFolder->first,&imghdr);
		y += TitleHeight*5/4;

		set<CString>::iterator it;
		for (it=itFolder->second.begin();!g_sigend && it!=itFolder->second.end();++it) {
			if (bNewPage)
			{
				x=LeftMargin,y=TopMargin+75;
				memset(pPub->imageData,255,pPub->widthStep*pPub->height);
				pagecnt++;
				bNewPage = false;
			}
			//read the photos
			int res=LoadImageToBuf(srcpath+"\\"+itFolder->first+"\\"+*it+".jpg",&imghead,PreservedSize);
			if( res!=LOAD_SUC)
			{
				AfxMessageBox(CString("读取文件\"")+*it+".jpg\"失败,跳过...请在结束后检查该文件");
				continue;
			}

			
			//here: preview
			Make1Photo(&imghead,pPub,x,y,size,*it,TextHeight);

			//move to next photo point
			x = x + photow + HSpace ;
			if (x + photow >= pagewidth){
				x = LeftMargin;
				y = y +photoh + VSpace + TextHeight;
				if (y + photoh >= pageheight-PNHeight){
					//add page number
					CvMat PubRgn;
					cvGetSubRect(pPub,&PubRgn,cvRect(LeftMargin,pageheight-PNHeight-1,pagewidth-2*LeftMargin,PNHeight));
					IplImage imghdr;
					imghdr.width=PubRgn.width;
					imghdr.height=PubRgn.height;
					imghdr.widthStep=PubRgn.step;
					imghdr.imageData=(char*)PubRgn.data.ptr;
					char buf[20];
					sprintf(buf,"--- %d ---",pagecnt);
					TextoutToImage(buf,&imghdr);

					//save page
					CString flnm;
					flnm.Format("%d.jpg",pagecnt);
					SaveFImage(tarpath+"\\"+"预览"+flnm,pPub);
					y = TopMargin;

					bNewPage = true;
				}
			}

			g_progress=(++nProcessed)*100/nFiles;
		}

	}

	if (!bNewPage)
	{
		//add page number
		CvMat PubRgn;
		cvGetSubRect(pPub,&PubRgn,cvRect(LeftMargin,pageheight-PNHeight-1,pagewidth-2*LeftMargin,PNHeight));
		IplImage imghdr;
		imghdr.width=PubRgn.width;
		imghdr.height=PubRgn.height;
		imghdr.widthStep=PubRgn.step;
		imghdr.imageData=(char*)PubRgn.data.ptr;
		char buf[20];
		sprintf(buf,"--- %d ---",pagecnt);
		TextoutToImage(buf,&imghdr);

		//save page
		CString flnm;
		flnm.Format("%d.jpg",pagecnt);
		SaveFImage(tarpath+"\\"+"预览"+flnm,pPub);
	}

	cvReleaseImage(&pPub);
	delete []PreservedSpace;
	return ;
		
}
