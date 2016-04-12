#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )

#ifdef _WINDLL
	#define _DllSpec DllExport
#else 
	#define _DllSpec DllImport
#endif


#define IMG_LOAD_ERR -1
#define BUF_SIZE_SMALL -2
#define LOAD_SUC 0

_DllSpec IplImage* LoadFImage( const char* filename );
_DllSpec bool SaveFImage( const char* filename, const IplImage* arr );

_DllSpec int LoadImageToBuf(const char *filename, IplImage *img, int sizeofbuf);
