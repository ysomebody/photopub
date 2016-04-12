#ifdef IMAGELDR_EXPORTS
#define IMAGELDR_API __declspec(dllexport)
#else
#define IMAGELDR_API __declspec(dllimport)
#endif

#define IMG_LOAD_ERR -1
#define BUF_SIZE_SMALL -2
#define LOAD_SUC 0

IMAGELDR_API IplImage* LoadFImage( const char* filename );
IMAGELDR_API bool SaveFImage( const char* filename, const IplImage* arr );

IMAGELDR_API int LoadImageToBuf(const char *filename, IplImage *img, int sizeofbuf);

IMAGELDR_API IplImage* CreateFImage(CvSize size, int depth, int channels );
IMAGELDR_API void ReleaseFImage(IplImage **img);
